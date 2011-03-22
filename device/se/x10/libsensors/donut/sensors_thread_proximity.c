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

#define LOG_TAG "Sensors_proximity"

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

#include "es209ra_proximity.h"
#include "sensors_thread.h"

/*****************************************************************************/

#define SESNORS_PRXY_DEVICE_NAME	"/dev/es209ra_proximity"

#define USEC_PER_MSEC				1000L

#define NSEC_PER_SEC				1000000000L

#define SENSORS_PROXIMITY			0x20

/*****************************************************************************/

static char sState = 0xff;

/*****************************************************************************/

static inline int64_t timespec_to_ns(const struct timespec *ts)
{
	return ((int64_t) ts->tv_sec * NSEC_PER_SEC) + ts->tv_nsec;
}

static int sensors_thread_open_device(sensors_thread_t* thread)
{
	return 0;
}

static int sensors_thread_close_device(sensors_thread_t* thread)
{
	return 0;
}

static int enable_proximity_sensor(sensors_thread_t* thread)
{
	int fd;

	if(thread->device_input_fd == -1) {
		fd = open(SESNORS_PRXY_DEVICE_NAME, O_RDWR);

		/* open device file */
		if (fd < 0) {
			LOGE("Couldn't find or open proximity driver (%s)\n", strerror(errno));
			return -1;
		}
		thread->device_input_fd = fd;

		sState = 0xff;
	}

	LOGD_IF(DEBUG, "proximity sensor enabled: fd = %d\n", thread->device_input_fd);
	return 0;
}

static int disable_proximity_sensor(sensors_thread_t* thread)
{
	LOGD_IF(DEBUG, "proximity sensor disabled: fd = %d\n", thread->device_input_fd);
	close(thread->device_input_fd);
	thread->device_input_fd = -1;
	return 0;
}

static int proximity_thread_init(sensors_thread_t* thread) {
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
	thread->timer_value = PROXIMITY_BURST_OFF_TIME_DEFAULT;

	unlink(SENSORS_PRXY_SOCK_NAME);

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
	strcpy(name.sun_path, SENSORS_PRXY_SOCK_NAME);
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

static int proximity_thread_exit(sensors_thread_t* thread)
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

	unlink(SENSORS_PRXY_SOCK_NAME);

	/* close device driver */
	sensors_thread_close_device(thread);

	return 0;

}

void* proximity_thread_main( void* args ) {
	int err = 0;
	sensors_thread_t thread_info;
	fd_set fds, readfds;
	int maxfd = 0;
	char state;
	sensors_data_t data;
	sensors_command_t msg;
	struct timeval tv;
	int break_flag = 0;
    struct timespec t;

	LOGD_IF(DEBUG, "Proximity thread start\n");

	/* inisialize sensors thread */
	memset(&thread_info, 0x00, sizeof(sensors_thread_t));
	err = proximity_thread_init(&thread_info);
	if(err < 0) {
		return -1;
	}

	/* prepare for select */
	FD_ZERO(&readfds);
	FD_SET(thread_info.sock_hal_fd, &readfds);

	maxfd = thread_info.sock_hal_fd;

	while(1) {
		/* select */
		memcpy(&fds, &readfds, sizeof(fd_set));
		tv.tv_sec = 0;
		tv.tv_usec = thread_info.timer_value;
		err = select(maxfd+1, &fds, NULL, NULL, (thread_info.device_input_fd == -1)?NULL:&tv);
		if(err < 0) {
			LOGE("Couldn't select %d\n", err);
			break;
		} else if(err == 0) {	/* time out */
			/* sensing */
			err = ioctl(thread_info.device_input_fd, PROXIMITY_DO_SENSING, &state);
			if(err < 0) {
				LOGE("ioctl(PROXIMITY_DO_SENSING) = %x(%s)\n", err, strerror(errno));
				break;
			}
			/* convert state value for android value */
			state = (state==PROXIMITY_GPIO_DOUT_ON)?0:1;
			if(sState == state) {
				/* do not report if sensor status is not changed. */
				continue;
			}

			/* send data to sensor input */
			memset(&data, 0x00, sizeof(sensors_data_t));
			sState = data.vector.x = state;
			clock_gettime(CLOCK_REALTIME, &t);
			data.time = timespec_to_ns(&t);
			data.acceleration.status = SENSOR_STATUS_ACCURACY_HIGH;
			data.sensor = SENSORS_PROXIMITY;
			LOGD_IF(0, "sensor data sensing state = %x\n", state);
			if (sendto(thread_info.sock_input_fd, &data, sizeof(sensors_data_t), 0,
						(const struct sockaddr*)&thread_info.sock_input_addr, sizeof(struct sockaddr_un)) < 0) {
				LOGE("sending datagram message\n");
			}
		} else {				/* receive signal from sensors HAL */
			/* socket event */
			if (FD_ISSET(thread_info.sock_hal_fd, &fds)) {
				memset(&msg, 0x00, sizeof(sensors_command_t));
				read(thread_info.sock_hal_fd, &msg, sizeof(sensors_command_t));

				switch(msg.command)
				{
					case SENSORS_THREAD_ENABLE_SENSOR:
						LOGD_IF(DEBUG, "SENSORS_THREAD_ENABLE_SENSOR received\n");
						err = enable_proximity_sensor(&thread_info);
						if(err) {
							LOGE("Couldn't enable sensor\n");
							break_flag = 1;
						}
						break;

					case SENSORS_THREAD_DISABLE_SENSOR:
						LOGD_IF(DEBUG, "SENSORS_THREAD_DISABLE_SENSOR received\n");
						err = disable_proximity_sensor(&thread_info);
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
	err = proximity_thread_exit(&thread_info);
	LOGD_IF(DEBUG, "Proximity thread exit\n");

	return 0;
}

