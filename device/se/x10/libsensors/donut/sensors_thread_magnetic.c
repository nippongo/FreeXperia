/*
 * Copyright 2009, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "Sensors_magnetic"

#include <hardware/sensors.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <math.h>
#include <poll.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

#include <linux/input.h>

#include <cutils/log.h>
#include <cutils/atomic.h>

#include "akm8973.h"
#include "sensors_thread.h"

/*****************************************************************************/

#define SENSORS_MGNT_CTRL_DEVICE	"/dev/akm8973_aot"

// sensor IDs must be a power of two and
// must match values in SensorManager.java
#define EVENT_TYPE_ACCEL_X			ABS_Y
#define EVENT_TYPE_ACCEL_Y			ABS_X
#define EVENT_TYPE_ACCEL_Z			ABS_Z
#define EVENT_TYPE_ACCEL_STATUS 	ABS_WHEEL

#define EVENT_TYPE_YAW				ABS_RX
#define EVENT_TYPE_PITCH			ABS_RY
#define EVENT_TYPE_ROLL 			ABS_RZ
#define EVENT_TYPE_ORIENT_STATUS	ABS_RUDDER

/*** AKEMD ATTENTION! To adjust Android ***/
/*** cordination, SWAP X and Y axis.	***/
#define EVENT_TYPE_MAGV_X			ABS_HAT0X
#define EVENT_TYPE_MAGV_Y			ABS_HAT0Y
#define EVENT_TYPE_MAGV_Z			ABS_BRAKE

#define EVENT_TYPE_TEMPERATURE		ABS_THROTTLE
#define EVENT_TYPE_STEP_COUNT		ABS_GAS

// 720 LSG = 1G
#define LSG 						(720.0f)

// conversion of acceleration data to SI units (m/s^2)
#define CONVERT_A					(GRAVITY_EARTH / LSG)
#define CONVERT_A_X 				(CONVERT_A)
#define CONVERT_A_Y 				(-CONVERT_A)
#define CONVERT_A_Z 				(-CONVERT_A)

// conversion of magnetic data to uT units
#define CONVERT_M					(1.0f/16.0f)
#define CONVERT_M_X 				(CONVERT_M)
#define CONVERT_M_Y 				(CONVERT_M)
#define CONVERT_M_Z 				(CONVERT_M)

#define CONVERT_O					(1.0f/64.0f)
#define CONVERT_O_Y 				(CONVERT_O)
#define CONVERT_O_P 				(CONVERT_O)
#define CONVERT_O_R 				(-CONVERT_O)

#define SENSORS_MASK				0x8f
#define SENSORS_ORIENTATION 		0x01
#define SENSORS_ACCELERATION		0x02
#define SENSORS_TEMPERATURE 		0x04
#define SENSORS_MAGNETIC_FIELD		0x08
#define SENSORS_ORIENTATION_RAW		0x80

#define SENSOR_STATE_MASK           (0x7FFF)

#define MAX_NUM_SENSORS 8

/*****************************************************************************/

static int sAkmFd = -1;
static int sActiveSensors = 0;
static int sNewSensor = 0;
static int sInputFD = -1;
static const int ID_O  = 0;
static const int ID_A  = 1;
static const int ID_T  = 2;
static const int ID_M  = 3;
static const int ID_OR = 7; // orientation raw
static sensors_data_t sSensors[MAX_NUM_SENSORS];
static uint32_t sPendingSensors;


/*****************************************************************************/

/*
 * We use a Least Mean Squares filter to smooth out the output of the yaw
 * sensor.
 *
 * The goal is to estimate the output of the sensor based on previous acquired
 * samples.
 *
 * We approximate the input by a line with the equation:
 *      Z(t) = a * t + b
 *
 * We use the Least Mean Squares method to calculate a and b so that the
 * distance between the line and the measured COUNT inputs Z(t) is minimal.
 *
 * In practice we only need to compute b, which is the value we're looking for
 * (it's the estimated Z at t=0). However, to improve the latency a little bit,
 * we're going to discard a certain number of samples that are too far from
 * the estimated line and compute b again with the new (trimmed down) samples.
 *
 * notes:
 * 'a' is the slope of the line, and physicaly represent how fast the input
 * is changing. In our case, how fast the yaw is changing, that is, how fast the
 * user is spinning the device (in degre / nanosecond). This value should be
 * zero when the device is not moving.
 *
 * The minimum distance between the line and the samples (which we are not
 * explicitely computing here), is an indication of how bad the samples are
 * and gives an idea of the "quality" of the estimation (well, really of the
 * sensor values).
 *
 */

/* sensor rate in me */
#define SENSORS_RATE_MS     20
/* timeout (constant value) in ms */
#define SENSORS_TIMEOUT_MS  100000
/* # of samples to look at in the past for filtering */
#define COUNT               24
/* prediction ratio */
#define PREDICTION_RATIO    (1.0f/3.0f)
/* prediction time in seconds (>=0) */
#define PREDICTION_TIME     ((SENSORS_RATE_MS*COUNT/1000.0f)*PREDICTION_RATIO)

static float mV[COUNT*2];
static float mT[COUNT*2];
static int mIndex;

static inline
float normalize(float x)
{
	x *= (1.0f / 360.0f);
	if (fabsf(x) >= 0.5f)
		x = x - ceilf(x + 0.5f) + 1.0f;
	if (x < 0)
		x += 1.0f;
	x *= 360.0f;
	return x;
}

static void LMSInit(void)
{
	memset(mV, 0, sizeof(mV));
	memset(mT, 0, sizeof(mT));
	mIndex = COUNT;
}

static float LMSFilter(int64_t time, int v)
{
	const float ns = 1.0f / 1000000000.0f;
	const float t = time*ns;
	float v1 = mV[mIndex];
	if ((v-v1) > 180) {
		v -= 360;
	} else if ((v1-v) > 180) {
		v += 360;
	}
	/* Manage the circular buffer, we write the data twice spaced by COUNT
	 * values, so that we don't have to memcpy() the array when it's full */
	mIndex++;
	if (mIndex >= COUNT*2)
		mIndex = COUNT;
	mV[mIndex] = v;
	mT[mIndex] = t;
	mV[mIndex-COUNT] = v;
	mT[mIndex-COUNT] = t;

	float A, B, C, D, E;
	float a, b;
	int i;

	A = B = C = D = E = 0;
	for (i=0 ; i<COUNT-1 ; i++) {
		const int j = mIndex - 1 - i;
		const float Z = mV[j];
		const float T = 0.5f*(mT[j] + mT[j+1]) - t;
		float dT = mT[j] - mT[j+1];
		dT *= dT;
		A += Z*dT;
		B += T*(T*dT);
		C +=   (T*dT);
		D += Z*(T*dT);
		E += dT;
	}
	b = (A*B + C*D) / (E*B + C*C);
	a = (E*b - A) / C;
	float f = b + PREDICTION_TIME*a;

	//LOGD_IF(DEBUG, "A=%f, B=%f, C=%f, D=%f, E=%f", A,B,C,D,E);
	//LOGD_IF(DEBUG, "%lld  %d  %f  %f", time, v, f, a);

	f = normalize(f);
	return f;
}

/*****************************************************************************/

static int sensors_thread_open_device(sensors_thread_t* thread)
{
	/* scan all input drivers and look for "compass" */
	int fd = -1;
	const char *dirname = "/dev/input";
	char devname[PATH_MAX];
	char *filename;
	DIR *dir;
	struct dirent *de;
	dir = opendir(dirname);
	if(dir == NULL)
		return -1;
	strcpy(devname, dirname);
	filename = devname + strlen(devname);
	*filename++ = '/';
	while((de = readdir(dir))) {
		if(de->d_name[0] == '.' &&
			(de->d_name[1] == '\0' ||
			(de->d_name[1] == '.' && de->d_name[2] == '\0')))
			continue;
		strcpy(filename, de->d_name);
		fd = open(devname, O_RDONLY);
		if (fd>=0) {
			char name[80];
			if (ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) < 1) {
			    name[0] = '\0';
			}
			if (!strcmp(name, "compass")) {
			    LOGD_IF(DEBUG, "using %s (name=%s)", devname, name);
			    break;
			}
			close(fd);
			fd = -1;
		}
	}
	closedir(dir);

	if (fd < 0) {
		LOGE("Couldn't find or open compass input (%s)", strerror(errno));
		return -1;
	}

	thread->device_input_fd = fd;

	return 0;
}

static int sensors_thread_close_device(sensors_thread_t* thread)
{
	close(thread->device_input_fd);
	thread->device_input_fd = -1;
	return 0;
}

static uint32_t read_sensors_state(int fd)
{
	if (fd<0) return 0;
	short flags;
	uint32_t sensors = 0;

	// read the actual value of all sensors
	if (!ioctl(fd, ECS_IOCTL_APP_GET_MFLAG, &flags)) {
		if (flags)	sensors |= SENSORS_ORIENTATION;
		else		sensors &= ~SENSORS_ORIENTATION;
	}
	if (!ioctl(fd, ECS_IOCTL_APP_GET_AFLAG, &flags)) {
		if (flags)	sensors |= SENSORS_ACCELERATION;
		else		sensors &= ~SENSORS_ACCELERATION;
	}
	if (!ioctl(fd, ECS_IOCTL_APP_GET_TFLAG, &flags)) {
		if (flags)	sensors |= SENSORS_TEMPERATURE;
		else		sensors &= ~SENSORS_TEMPERATURE;
	}
#ifdef ECS_IOCTL_APP_SET_MVFLAG
	if (!ioctl(fd, ECS_IOCTL_APP_GET_MVFLAG, &flags)) {
		if (flags)	sensors |= SENSORS_MAGNETIC_FIELD;
		else		sensors &= ~SENSORS_MAGNETIC_FIELD;
	}
#endif
    return sensors;
}

static int enable_magnetic_sensor(sensors_thread_t* thread)
{
    short flags = 0;

	/* open device file */
	if(thread->device_fd == -1) {
		int fd = open(SENSORS_MGNT_CTRL_DEVICE, O_RDWR);
		if (fd < 0) {
			LOGE("Couldn't find or open magnetic driver (%s)\n", strerror(errno));
			return -1;
		}

		thread->device_fd = fd;
		LMSInit();
		memset(&sSensors, 0, sizeof(sSensors));
		int i;
		for (i=0 ; i<MAX_NUM_SENSORS ; i++) {
			// by default all sensors have high accuracy
			// (we do this because we don't get an update if the value doesn't
			// change).
			sSensors[i].vector.status = SENSOR_STATUS_ACCURACY_HIGH;
		}
		LOGD_IF(DEBUG, "magnetic sensor opened: fd = %d\n", thread->device_fd);
	}

	if (!(sActiveSensors & SENSORS_ORIENTATION) && !(sActiveSensors & SENSORS_ORIENTATION_RAW)) {
		if ((sNewSensor & SENSORS_ORIENTATION) | (sNewSensor & SENSORS_ORIENTATION_RAW)) {
			flags = 1;
		}
		if (ioctl(thread->device_fd, ECS_IOCTL_APP_SET_MFLAG, &flags) < 0) {
			LOGE("ECS_IOCTL_APP_SET_MFLAG error (%s)", strerror(errno));
			goto err_ioctl;
		}
	}

	flags = 0;
	if (!(sActiveSensors & SENSORS_ACCELERATION)) {
		if (sNewSensor & SENSORS_ACCELERATION) {
			flags = 1;
		}
		if (ioctl(thread->device_fd, ECS_IOCTL_APP_SET_AFLAG, &flags) < 0) {
			LOGE("ECS_IOCTL_APP_SET_AFLAG error (%s)", strerror(errno));
			goto err_ioctl;
		}
	}

	flags = 0;
	if (!(sActiveSensors & SENSORS_TEMPERATURE)) {
		if (sNewSensor & SENSORS_TEMPERATURE) {
			flags = 1;
		}
		if (ioctl(thread->device_fd, ECS_IOCTL_APP_SET_TFLAG, &flags) < 0) {
			LOGE("ECS_IOCTL_APP_SET_TFLAG error (%s)", strerror(errno));
			goto err_ioctl;
		}
	}

#ifdef ECS_IOCTL_APP_SET_MVFLAG
	flags = 0;
	if (!(sActiveSensors & SENSORS_MAGNETIC_FIELD)) {
		if (sNewSensor & SENSORS_MAGNETIC_FIELD) {
			flags = 1;
		}
		if (ioctl(thread->device_fd, ECS_IOCTL_APP_SET_MVFLAG, &flags) < 0) {
			LOGE("ECS_IOCTL_APP_SET_MVFLAG error (%s)", strerror(errno));
			goto err_ioctl;
		}
	}
#endif

	sActiveSensors |= sNewSensor;

	LOGD_IF(DEBUG, "magnetic sensor enabled: active = %x, changed = %x, real = %x\n",
		sActiveSensors, sNewSensor, read_sensors_state(thread->device_fd));
	return 0;

err_ioctl:
	return -1;
}

static int disable_magnetic_sensor(sensors_thread_t* thread)
{
    short flags = 0;

	if (sNewSensor & SENSORS_ORIENTATION) {
		if(!(sActiveSensors & SENSORS_ORIENTATION_RAW)) {
			if (ioctl(thread->device_fd, ECS_IOCTL_APP_SET_MFLAG, &flags) < 0) {
				LOGE("ECS_IOCTL_APP_SET_MFLAG error (%s)", strerror(errno));
				goto err_ioctl;
			}
		}
	}
	if (sNewSensor & SENSORS_ORIENTATION_RAW) {
		if(!(sActiveSensors & SENSORS_ORIENTATION)) {
			if (ioctl(thread->device_fd, ECS_IOCTL_APP_SET_MFLAG, &flags) < 0) {
				LOGE("ECS_IOCTL_APP_SET_MFLAG error (%s)", strerror(errno));
				goto err_ioctl;
			}
		}
	}
	if (sNewSensor & SENSORS_TEMPERATURE) {
		if (ioctl(thread->device_fd, ECS_IOCTL_APP_SET_TFLAG, &flags) < 0) {
			LOGE("ECS_IOCTL_APP_SET_TFLAG error (%s)", strerror(errno));
			goto err_ioctl;
		}
	}
	if (sNewSensor & SENSORS_MAGNETIC_FIELD) {
		if (ioctl(thread->device_fd, ECS_IOCTL_APP_SET_MVFLAG, &flags) < 0) {
			LOGE("ECS_IOCTL_APP_SET_MVFLAG error (%s)", strerror(errno));
			goto err_ioctl;
		}
	}

	sActiveSensors ^= sNewSensor;

	LOGD_IF(DEBUG, "magnetic sensor disabled: active = %x, changed = %x, real = %x\n",
			sActiveSensors, sNewSensor, read_sensors_state(thread->device_fd));

	/* close device file */
	if(sActiveSensors == 0) {
		LOGD_IF(DEBUG, "magnetic sensor closed: fd = %d\n", thread->device_fd);
		close(thread->device_fd);
		thread->device_fd = -1;
	}
	return 0;

err_ioctl:
	return -1;
}

static int magnetic_thread_init(sensors_thread_t* thread) {
	int err = -1;
	int sock, timer;
	struct sockaddr_un name;
	const int one = 1;

	/* initialize sensors thread info */
	thread->device_input_fd = -1;
	thread->device_fd = -1;
	thread->sock_input_fd = -1;
	thread->sock_hal_fd = -1;
	thread->timer_fd = -1;
	thread->timer_value = SENSORS_TIMEOUT_MS;

	unlink(SENSORS_MGNT_SOCK_NAME);

	/* create socket for sensors input */
	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if(sock < 0) {
		LOGE("Couldn't create socket for sensors input\n");
		err = -1;
		goto err_create_sock_input;
	}
	thread->sock_input_fd = sock;

	/* create socket addr for sensors input */ 
	thread->sock_input_addr.sun_family = AF_UNIX;
	strcpy(thread->sock_input_addr.sun_path, SENSORS_INPUT_SOCK_NAME);

	/* create socket for sensors HAL */
	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if(sock < 0) {
		LOGE("Couldn't create socket for sensors HAL\n");
		goto err_create_sock_hal;
	}
	thread->sock_hal_fd = sock;

	/* bind to sensors HAL */ 
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
	name.sun_family = AF_UNIX;
	strcpy(name.sun_path, SENSORS_MGNT_SOCK_NAME);
	err = bind(sock, (const struct sockaddr*)&name, strlen(name.sun_path) + sizeof (name.sun_family));
	if(err < 0) {
		LOGE("Couldn't bind to sensors HAL(%s) - %s\n", name.sun_path, strerror(errno));
		goto err_bind_sock_hal;
	}

	/* open device driver */
	err = sensors_thread_open_device(thread);
	if(err < 0) {
		LOGE("Couldn't open device\n");
		goto err_device_open;
	}
	return 0;


err_device_open:
err_bind_sock_hal:
	close(thread->sock_hal_fd);
err_create_sock_hal:
	close(thread->sock_input_fd);
err_create_sock_input:
	return err;
}

static int magnetic_thread_exit(sensors_thread_t* thread)
{
	/* close socket for sensors input */
	if(thread->sock_input_fd != -1) {
		close(thread->sock_input_fd);
		thread->sock_input_fd = -1;
	}

	/* close socket for sensors HAL */
	if(thread->sock_hal_fd != -1) {
		close(thread->sock_hal_fd);
		thread->sock_hal_fd = -1;
	}

	unlink(SENSORS_MGNT_SOCK_NAME);

	/* close device driver */
	sensors_thread_close_device(thread);

	return 0;

}

static int pick_sensor(sensors_thread_t* thread)
{
	uint32_t mask = SENSORS_MASK;
	sensors_data_t data;

	memset(&data, 0x00, sizeof(sensors_data_t));
	while(mask) {
		uint32_t i = 31 - __builtin_clz(mask);
		mask &= ~(1<<i);
		if (sPendingSensors & (1<<i)) {
			sPendingSensors &= ~(1<<i);
			memcpy(&data, &sSensors[i], sizeof(sensors_data_t));
			data.sensor = (1<<i);
			LOGD_IF(0, "%d [%f, %f, %f]", (1<<i),
					data.vector.x,
					data.vector.y,
					data.vector.z);

			if (sendto(thread->sock_input_fd, &data, sizeof(sensors_data_t), 0,
						(const struct sockaddr*)&thread->sock_input_addr, sizeof(struct sockaddr_un)) < 0) {
				LOGE("sending datagram message\n");
			}
			return 0;
		}
	}
	LOGD_IF(DEBUG, "No sensor to return!!! sPendingSensors=%08x", sPendingSensors);
	// we may end-up in a busy loop, slow things down, just in case.
	usleep(100000);
	return 0;
}

void* magnetic_thread_main( void* args ) {
	int err = 0;
	sensors_thread_t thread_info;
	fd_set fds, readfds;
	int maxfd = 0;
	sensors_command_t msg;
	int break_flag = 0;
	struct input_event event;
	uint32_t new_sensors = 0;
	int nread = 0;
    int64_t t;

	LOGD_IF(DEBUG, "Magnetic thread start\n");

	/* inisialize sensors thread */
	memset(&thread_info, 0x00, sizeof(sensors_thread_t));
	err = magnetic_thread_init(&thread_info);
	if(err < 0) {
		return -1;
	}

	/* prepare for select */
	FD_ZERO(&readfds);
	FD_SET(thread_info.sock_hal_fd, &readfds);
	FD_SET(thread_info.device_input_fd, &readfds);
	if (thread_info.sock_hal_fd > thread_info.device_input_fd) {
		maxfd = thread_info.sock_hal_fd;
	} else {
		maxfd = thread_info.device_input_fd;
	}

	while(1) {
		/* select */
		if (sPendingSensors) {
			LOGD_IF(0, "sPending");
			pick_sensor(&thread_info);
			continue;
		}

		memcpy(&fds, &readfds, sizeof(fd_set));
		err = select(maxfd+1, &fds, NULL, NULL, NULL);
		if(err < 0) {
			LOGE("Couldn't select %d\n", err);
			break;
		} else if(err == 0) {	/* time out */
			/* nothing to do */
			continue;
		} else {				/* receive signal from sensors HAL */
			/* input event */
			if (FD_ISSET(thread_info.device_input_fd, &fds)) {
				while(1) {
					nread = read(thread_info.device_input_fd, &event, sizeof(event));
					if (nread == sizeof(event)) {
						uint32_t v;
						if (event.type == EV_ABS) {
							switch (event.code) {

								case EVENT_TYPE_ACCEL_X:
									new_sensors |= SENSORS_ACCELERATION;
									sSensors[ID_A].acceleration.x = event.value * CONVERT_A_X;
									break;
								case EVENT_TYPE_ACCEL_Y:
									new_sensors |= SENSORS_ACCELERATION;
									sSensors[ID_A].acceleration.y = event.value * CONVERT_A_Y;
									break;
								case EVENT_TYPE_ACCEL_Z:
									new_sensors |= SENSORS_ACCELERATION;
									sSensors[ID_A].acceleration.z = event.value * CONVERT_A_Z;
									break;

								case EVENT_TYPE_MAGV_X:
									new_sensors |= SENSORS_MAGNETIC_FIELD;
									sSensors[ID_M].magnetic.x = event.value * CONVERT_M_X;
									break;
								case EVENT_TYPE_MAGV_Y:
									new_sensors |= SENSORS_MAGNETIC_FIELD;
									sSensors[ID_M].magnetic.y = event.value * CONVERT_M_Y;
									break;
								case EVENT_TYPE_MAGV_Z:
									new_sensors |= SENSORS_MAGNETIC_FIELD;
									sSensors[ID_M].magnetic.z = event.value * CONVERT_M_Z;
									break;

								case EVENT_TYPE_YAW:
									if(sActiveSensors & SENSORS_ORIENTATION) {
										new_sensors |= SENSORS_ORIENTATION;
										sSensors[ID_O].orientation.azimuth = event.value * CONVERT_O_Y;
									}
									if(sActiveSensors & SENSORS_ORIENTATION_RAW) {
										new_sensors |= SENSORS_ORIENTATION_RAW;
										sSensors[ID_OR].orientation.azimuth = event.value;
									}
									break;
								case EVENT_TYPE_PITCH:
									if(sActiveSensors & SENSORS_ORIENTATION) {
										new_sensors |= SENSORS_ORIENTATION;
										sSensors[ID_O].orientation.pitch = event.value * CONVERT_O_P;
									}
									if(sActiveSensors & SENSORS_ORIENTATION_RAW) {
										new_sensors |= SENSORS_ORIENTATION_RAW;
										sSensors[ID_OR].orientation.pitch = event.value;
									}
									break;
								case EVENT_TYPE_ROLL:
									if(sActiveSensors & SENSORS_ORIENTATION) {
										new_sensors |= SENSORS_ORIENTATION;
										sSensors[ID_O].orientation.roll = event.value * CONVERT_O_R;
									}
									if(sActiveSensors & SENSORS_ORIENTATION_RAW) {
										new_sensors |= SENSORS_ORIENTATION_RAW;
										sSensors[ID_OR].orientation.roll = event.value;
									}
									break;

								case EVENT_TYPE_TEMPERATURE:
									new_sensors |= SENSORS_TEMPERATURE;
									sSensors[ID_T].temperature = event.value;
									break;

								case EVENT_TYPE_STEP_COUNT:
									// step count (only reported in MODE_FFD)
									// we do nothing with it for now.
									break;
								case EVENT_TYPE_ACCEL_STATUS:
									// accuracy of the calibration (never returned!)
									//LOGD_IF(DEBUG, "G-Sensor status %d", event.value);
									break;
								case EVENT_TYPE_ORIENT_STATUS:
									// accuracy of the calibration
									v = (uint32_t)(event.value & SENSOR_STATE_MASK);
									LOGD_IF(sSensors[ID_O].orientation.status != (uint8_t)v,
											"M-Sensor status %d", v);
									sSensors[ID_O].orientation.status = (uint8_t)v;
									sSensors[ID_OR].orientation.status = (uint8_t)v;
									break;
							}
						} else if (event.type == EV_SYN) {
							if (new_sensors) {
								sPendingSensors = new_sensors;
								t = event.time.tv_sec * 1000000000LL + event.time.tv_usec * 1000;
								while (new_sensors) {
									uint32_t i = 31 - __builtin_clz(new_sensors);
									new_sensors &= ~(1<<i);
									sSensors[i].time = t;
								}
								pick_sensor(&thread_info);
								break;
							}
						}
					}
				}
			}
			/* socket event */
			if (FD_ISSET(thread_info.sock_hal_fd, &fds)) {
				memset(&msg, 0x00, sizeof(sensors_command_t));
				read(thread_info.sock_hal_fd, &msg, sizeof(sensors_command_t));

				switch(msg.command)
				{
					case SENSORS_THREAD_ENABLE_SENSOR:
						LOGD_IF(DEBUG, "SENSORS_THREAD_ENABLE_SENSOR(%x) received\n", msg.param[0]);
						sNewSensor = msg.param[0];
						err = enable_magnetic_sensor(&thread_info);
						if(err) {
							LOGE("Couldn't enable sensor\n");
							break_flag = 1;
						}
						break;

					case SENSORS_THREAD_DISABLE_SENSOR:
						LOGD_IF(DEBUG, "SENSORS_THREAD_DISABLE_SENSOR(%x) received\n", msg.param[0]);
						sNewSensor = msg.param[0];
						err = disable_magnetic_sensor(&thread_info);
						if(err) {
							LOGE("Couldn't disable sensor\n");
							break_flag = 1;
						}
						break;

					case SENSORS_THREAD_EXIT_THREAD:
						LOGD_IF(DEBUG, "SENSORS_THREAD_EXIT_THREAD received\n");
						break_flag = 1;
						break;
					default:
						break;
				}
			}
		}
		if(break_flag) {
			break;
		}
	}

	/* close sensors thread */
	err = magnetic_thread_exit(&thread_info);
	LOGD_IF(DEBUG, "Magnetic thread exit\n");

	return 0;
}

