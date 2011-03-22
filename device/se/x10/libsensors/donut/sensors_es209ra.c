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

#define LOG_TAG "Sensors"

#include <hardware/sensors.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <math.h>
#include <poll.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <linux/input.h>

#include <cutils/log.h>
#include <cutils/atomic.h>
#include <cutils/native_handle.h>

#include "akm8973.h"
#include "sensors_thread.h"

/*****************************************************************************/

#define SENSORS_ORIENTATION 			0x01
#define SENSORS_ACCELERATION			0x02
#define SENSORS_TEMPERATURE 			0x04
#define SENSORS_MAGNETIC_FIELD			0x08
#define SENSORS_PROXIMITY				0x20
#define SENSORS_ORIENTATION_RAW			0x80

#define SENSORS_ORIENTATION_HANDLE		0
#define SENSORS_ACCELERATION_HANDLE 	1
#define SENSORS_TEMPERATURE_HANDLE		2
#define SENSORS_MAGNETIC_FIELD_HANDLE	3
#define SENSORS_PROXIMITY_HANDLE		5
#define SENSORS_ORIENTATION_RAW_HANDLE	7

/*****************************************************************************/

static int sensors_control_device_close(struct hw_device_t *dev);
static native_handle_t* sensors_control_open_data_source(struct sensors_control_device_t *dev);
static int sensors_control_activate(struct sensors_control_device_t *dev, 
            int handle, int enabled);
static int sensors_control_wake(struct sensors_control_device_t *dev);

static int sensors_data_device_close(struct hw_device_t *dev);
static int sensors_data_data_open(struct sensors_data_device_t *dev, native_handle_t* nh);
static int sensors_data_data_close(struct sensors_data_device_t *dev);
static int sensors_data_poll(struct sensors_data_device_t *dev, 
            sensors_data_t* data);

static int sensors_get_sensors_list(struct sensors_module_t* module,
	struct sensor_t const**);

static int sensors_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device);

/*****************************************************************************/

static volatile int sSockFD = -1;
static uint32_t sActiveSensors = 0;
static uint32_t sDelayTime;

static pthread_t sProximity_thread = -1;
static int sProximityThSock = -1;

static pthread_t sAccelerometer_thread = -1;
static int sAccelerometerThSock = -1;

static pthread_t sMagnetic_thread = -1;
static int sMagneticThSock = -1;

static pthread_t sSock_thread = -1;
static int sSocketThSockControl = -1;
static int sSocketThSockData = -1;
static int sSensorsSock = -1;

/*****************************************************************************/

static native_handle_t* sensors_control_open_data_source(struct sensors_control_device_t *dev)
{
	native_handle_t* handle = NULL;
	LOGD_IF(DEBUG, "sensors_control_open_data_source\n");

	if(sSensorsSock != -1) {
		handle = native_handle_create(1, 1);
		handle->data[0] = dup(sSensorsSock);
	}
	LOGD_IF(DEBUG, "return data socket = %d\n", handle->data[0]);
	return handle;
}

static int sensors_control_activate(struct sensors_control_device_t *dev, 
            int handle, int enabled)
{
	uint32_t active = sActiveSensors;
	uint32_t sensor = 1 << (handle - SENSORS_HANDLE_BASE);
	int sock;
	sensors_command_t msg;
	struct sockaddr_un name;
	int state = 0;

	LOGD_IF(DEBUG, "sensors_control_activate: handle = %d, enabled = %d\n", handle, enabled);
	LOGD_IF(DEBUG, "%s active=%08x sensor=%08x\n", __FUNCTION__, active, sensor);

	if(sensor & SENSORS_PROXIMITY) {
		if (enabled && (!(sActiveSensors & SENSORS_PROXIMITY))
			|| (!enabled && (sActiveSensors & SENSORS_PROXIMITY))) {

			/* activate/deactivate proximity sensor */
			msg.command = enabled?SENSORS_THREAD_ENABLE_SENSOR:SENSORS_THREAD_DISABLE_SENSOR;

			/* Construct name of socket to send to. */
			name.sun_family = AF_UNIX; 
			strcpy(name.sun_path, SENSORS_PRXY_SOCK_NAME);

			LOGD_IF(DEBUG, "send command to sock(%s) = %08x\n", name.sun_path, msg.command);
			/* Send message. */
			if (sendto(sProximityThSock, &msg, sizeof(sensors_command_t), 0, (const struct sockaddr*)&name, sizeof(struct sockaddr_un)) < 0) {
				LOGE("sending thread message(%s)\n", strerror(errno));
				state = -1;
			}
			if(enabled) {
				sActiveSensors |= SENSORS_PROXIMITY;
			} else {
				sActiveSensors &= ~SENSORS_PROXIMITY;
			}
		}
	}

	if(sensor & SENSORS_ACCELERATION) {
		if (enabled && (!(sActiveSensors & SENSORS_ACCELERATION))
			|| (!enabled && (sActiveSensors & SENSORS_ACCELERATION))) {

			/* activate/deactivate accelerometer sensor */
			msg.command = enabled?SENSORS_THREAD_ENABLE_SENSOR:SENSORS_THREAD_DISABLE_SENSOR;

			/* Construct name of socket to send to. */
			name.sun_family = AF_UNIX; 
			strcpy(name.sun_path, SENSORS_ACC_SOCK_NAME);

			LOGD_IF(DEBUG, "send command to sock(%s) = %08x\n", name.sun_path, msg.command);
			/* Send message. */
			if (sendto(sAccelerometerThSock, &msg, sizeof(sensors_command_t), 0, (const struct sockaddr*)&name, sizeof(struct sockaddr_un)) < 0) {
				LOGE("sending thread message(%s)\n", strerror(errno));
				state = -1;
			}
			if(enabled) {
				sActiveSensors |= SENSORS_ACCELERATION;
			} else {
				sActiveSensors &= ~SENSORS_ACCELERATION;
			}
		}
	}

	if(sensor & SENSORS_ORIENTATION) {
		if (enabled && (!(sActiveSensors & SENSORS_ORIENTATION))
			|| (!enabled && (sActiveSensors & SENSORS_ORIENTATION))) {

			/* activate/deactivate orientation sensor */
			msg.command = enabled?SENSORS_THREAD_ENABLE_SENSOR:SENSORS_THREAD_DISABLE_SENSOR;
			msg.param[0] = SENSORS_ORIENTATION;

			/* Construct name of socket to send to. */
			name.sun_family = AF_UNIX; 
			strcpy(name.sun_path, SENSORS_MGNT_SOCK_NAME);

			LOGD_IF(DEBUG, "send command to sock(%s) = %08x\n", name.sun_path, msg.command);
			/* Send message. */
			if (sendto(sMagneticThSock, &msg, sizeof(sensors_command_t), 0, (const struct sockaddr*)&name, sizeof(struct sockaddr_un)) < 0) {
				LOGE("sending thread message(%s)\n", strerror(errno));
				state = -1;
			}
			if(enabled) {
				sActiveSensors |= SENSORS_ORIENTATION;
			} else {
				sActiveSensors &= ~SENSORS_ORIENTATION;
			}
		}
	}

#if ENABLE_ORIENTATION_RAW_SENSOR
	if(sensor & SENSORS_ORIENTATION_RAW) {
		if (enabled && (!(sActiveSensors & SENSORS_ORIENTATION_RAW))
			|| (!enabled && (sActiveSensors & SENSORS_ORIENTATION_RAW))) {

			/* activate/deactivate orientation raw sensor */
			msg.command = enabled?SENSORS_THREAD_ENABLE_SENSOR:SENSORS_THREAD_DISABLE_SENSOR;
			msg.param[0] = SENSORS_ORIENTATION_RAW;

			/* Construct name of socket to send to. */
			name.sun_family = AF_UNIX; 
			strcpy(name.sun_path, SENSORS_MGNT_SOCK_NAME);

			LOGD_IF(DEBUG, "send command to sock(%s) = %08x\n", name.sun_path, msg.command);
			/* Send message. */
			if (sendto(sMagneticThSock, &msg, sizeof(sensors_command_t), 0, (const struct sockaddr*)&name, sizeof(struct sockaddr_un)) < 0) {
				LOGE("sending thread message(%s)\n", strerror(errno));
				state = -1;
			}
			if(enabled) {
				sActiveSensors |= SENSORS_ORIENTATION_RAW;
			} else {
				sActiveSensors &= ~SENSORS_ORIENTATION_RAW;
			}
		}
	}
#endif

	if(sensor & SENSORS_MAGNETIC_FIELD) {
		if (enabled && (!(sActiveSensors & SENSORS_MAGNETIC_FIELD))
			|| (!enabled && (sActiveSensors & SENSORS_MAGNETIC_FIELD))) {

			/* activate/deactivate magnetic sensor */
			msg.command = enabled?SENSORS_THREAD_ENABLE_SENSOR:SENSORS_THREAD_DISABLE_SENSOR;
			msg.param[0] = SENSORS_MAGNETIC_FIELD;

			/* Construct name of socket to send to. */
			name.sun_family = AF_UNIX; 
			strcpy(name.sun_path, SENSORS_MGNT_SOCK_NAME);

			LOGD_IF(DEBUG, "send command to sock(%s) = %08x\n", name.sun_path, msg.command);
			/* Send message. */
			if (sendto(sMagneticThSock, &msg, sizeof(sensors_command_t), 0, (const struct sockaddr*)&name, sizeof(struct sockaddr_un)) < 0) {
				LOGE("sending thread message(%s)\n", strerror(errno));
				state = -1;
			}
			if(enabled) {
				sActiveSensors |= SENSORS_MAGNETIC_FIELD;
			} else {
				sActiveSensors &= ~SENSORS_MAGNETIC_FIELD;
			}
		}
	}


#if ENABLE_TEMPERATURE_SENSOR
	if(sensor & SENSORS_TEMPERATURE) {
		if (enabled && (!(sActiveSensors & SENSORS_TEMPERATURE))
			|| (!enabled && (sActiveSensors & SENSORS_TEMPERATURE))) {

			/* activate/deactivate temperature sensor */
			msg.command = enabled?SENSORS_THREAD_ENABLE_SENSOR:SENSORS_THREAD_DISABLE_SENSOR;
			msg.param[0] = SENSORS_TEMPERATURE;

			/* Construct name of socket to send to. */
			name.sun_family = AF_UNIX; 
			strcpy(name.sun_path, SENSORS_MGNT_SOCK_NAME);

			LOGD_IF(DEBUG, "send command to sock(%s) = %08x\n", name.sun_path, msg.command);
			/* Send message. */
			if (sendto(sMagneticThSock, &msg, sizeof(sensors_command_t), 0, (const struct sockaddr*)&name, sizeof(struct sockaddr_un)) < 0) {
				LOGE("sending thread message(%s)\n", strerror(errno));
				state = -1;
			}
			if(enabled) {
				sActiveSensors |= SENSORS_TEMPERATURE;
			} else {
				sActiveSensors &= ~SENSORS_TEMPERATURE;
			}
		}
	}
#endif

	LOGD_IF(DEBUG, "sensors=%08x\n", sActiveSensors);
	return state;
}

static int sensors_control_delay(struct sensors_control_device_t *dev, int32_t ms)
{
	sensors_command_t msg;
	struct sockaddr_un name;

	LOGD_IF(DEBUG, "sensors_control_delay = %d ms\n",ms);

	msg.command  = SENSORS_THREAD_CHANGE_POLLING_INTERVAL;
	msg.param[0] = ms;
	name.sun_family = AF_UNIX;

	if (sActiveSensors & SENSORS_ACCELERATION) {
		strcpy(name.sun_path, SENSORS_ACC_SOCK_NAME);
		LOGD_IF(DEBUG,
			"send command (SENSORS_THREAD_CHANGE_POLLING_INTERVAL) to sock(%s) = %08x\n",
			name.sun_path, msg.command);
		if (sendto(sAccelerometerThSock,
			   &msg,
			   sizeof(sensors_command_t),
			   0,
			   (const struct sockaddr*)&name,
			   sizeof(struct sockaddr_un)) < 0) {
			LOGE("sending thread message(%s)\n", strerror(errno));
		}
	}
	return 0;
}


/*****************************************************************************/

struct hw_module_methods_t sensors_module_methods = {
	open: sensors_device_open
};

const struct sensors_module_t HAL_MODULE_INFO_SYM = {
	common: {
		tag: HARDWARE_MODULE_TAG,
		version_major: 1,
		version_minor: 0,
		id: SENSORS_HARDWARE_MODULE_ID,
		name : "ES209 Sensor module",
		author : "SEMC",
		methods: &sensors_module_methods,
	},
	get_sensors_list: sensors_get_sensors_list,
};

struct sensor_t sensors_descs[] = {
	{
		name : "AK8973 Compass",
		vendor : "Asahi Kasei Corp.",
		version : 1,
		handle : SENSORS_ORIENTATION_HANDLE,
		type : SENSOR_TYPE_ORIENTATION,
		maxRange : 360,
		resolution : 100,
		power : 0.8,
	},
#if ENABLE_ORIENTATION_RAW_SENSOR
	{
		name : "AK8973 Compass Raw",
		vendor : "Asahi Kasei Corp.",
		version : 1,
		handle : SENSORS_ORIENTATION_RAW_HANDLE,
		type : SENSOR_TYPE_ORIENTATION,
		maxRange : 23040,
		resolution : 100,
		power : 0.8,
	},
#endif
	{
		name : "AK8973 Magnetic Field",
		vendor : "Asahi Kasei Corp.",
		version : 1,
		handle : SENSORS_MAGNETIC_FIELD_HANDLE,
		type : SENSOR_TYPE_MAGNETIC_FIELD,
		maxRange : 2000,
		resolution : 100,
		power : 0.8,
	},
#if ENABLE_TEMPERATURE_SENSOR
	{
		name : "AK8973 Temperature",
		vendor : "Asahi Kasei Corp.",
		version : 1,
		handle : SENSORS_TEMPERATURE_HANDLE,
		type : SENSOR_TYPE_TEMPERATURE,
		maxRange : 358.15,
		resolution : 100,
		power : 0.8,
	},
#endif
	{
		name: "BMA150 accelerometer",
		vendor: "Bosch Sensortec GmbH",
		version: 1,
		handle: SENSORS_ACCELERATION_HANDLE,
		type: SENSOR_TYPE_ACCELEROMETER,
		maxRange: 9.81,
		resolution: 20,
		power: 0.13,
	},
	{
		name : "ES209RA Proximity",
		vendor : "Unknown",
		version : 1,
		handle : SENSORS_PROXIMITY_HANDLE,
		type : SENSOR_TYPE_PROXIMITY,
		maxRange : 1.0,
		resolution : 500,
		power : 0.6,
	}
};

/*****************************************************************************/

struct sensors_control_context_t {
	struct sensors_control_device_t device;
};

struct sensors_data_context_t {
	struct sensors_data_device_t device;
};

static int sensors_get_sensors_list(struct sensors_module_t* module,
		struct sensor_t const** plist){
	*plist = sensors_descs;
	return sizeof(sensors_descs)/sizeof(struct sensor_t);
}

static int sensors_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device)
{
	int status = -EINVAL;
	if (!strcmp(name, SENSORS_HARDWARE_CONTROL)) {
		LOGD_IF(DEBUG, "sensors_device_open:SENSORS_HARDWARE_CONTROL\n");
		struct sensors_control_context_t *dev;
		dev = (struct sensors_control_context_t*)malloc(sizeof(*dev));

		/* initialize our state here */
		memset(dev, 0, sizeof(*dev));

		/* initialize the procs */
		dev->device.common.tag = HARDWARE_DEVICE_TAG;
		dev->device.common.version = 0;
		dev->device.common.module = (struct hw_module_t*)module;
		dev->device.common.close = sensors_control_device_close;
		dev->device.open_data_source = sensors_control_open_data_source;
		dev->device.activate = sensors_control_activate;
		dev->device.set_delay = sensors_control_delay;
		dev->device.wake = sensors_control_wake;

		*device = &dev->device.common;

		/* create sensors socket */
		if(sSensorsSock == -1) {
			/* delete sock file if exist */
			unlink(SENSORS_INPUT_SOCK_NAME);

			const int one = 1;
			struct sockaddr_un name;
			int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
			if(sock < 0) {
				LOGE("Couldn't open sensors socket %d\n", sock);
				return -1;
			}
			setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
			name.sun_family = AF_UNIX;
			strcpy(name.sun_path, SENSORS_INPUT_SOCK_NAME);
			if (bind(sock, (const struct sockaddr*)&name, strlen(name.sun_path) + sizeof (name.sun_family))) { 
				LOGE("binding name to sensors socket %s\n", strerror(errno));
				return -1;
			}
			sSensorsSock = sock;
		}

		/* create sensors socket thread */
		if(sSock_thread == -1) {
			int err = pthread_create(&sSock_thread, NULL, socket_thread_main, (void*)sSensorsSock);
			if (err < 0)
			{
				LOGE("Couldn't create sensors socket thread\n");
				return -1;
			}
		}

		/* start sensor threads */
		if(sProximity_thread == -1) {
			int err = pthread_create(&sProximity_thread, NULL, proximity_thread_main, NULL);
			if (err < 0)
			{
				LOGE("Couldn't create proximity sensor thread\n");
				return -1;
			}
		}
		if(sAccelerometer_thread == -1) {
			int err = pthread_create(&sAccelerometer_thread, NULL, accelerometer_thread_main, NULL);
			if (err < 0)
			{
				LOGE("Couldn't create accelerometer sensor thread\n");
				return -1;
			}
		}
		if(sMagnetic_thread == -1) {
			int err = pthread_create(&sMagnetic_thread, NULL, magnetic_thread_main, NULL);
			if (err < 0)
			{
				LOGE("Couldn't create magnetic sensor thread\n");
				return -1;
			}
		}

		/* create socket */
		if(sProximityThSock == -1) {
			int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
			if (sock < 0) {
				LOGE("opening datagram socket for proximity sensor\n");
				return -1;
			}
			sProximityThSock = sock;
		}
		if(sAccelerometerThSock == -1) {
			int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
			if (sock < 0) {
				LOGE("opening datagram socket for accelerometer sensor\n");
				return -1;
			}
			sAccelerometerThSock = sock;
		}
		if(sMagneticThSock == -1) {
			int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
			if (sock < 0) {
				LOGE("opening datagram socket for magnetic sensor\n");
				return -1;
			}
			sMagneticThSock = sock;
		}
		if(sSocketThSockControl == -1) {
			int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
			if (sock < 0) {
				LOGE("opening datagram socket for sensors socket thread in control side\n");
				return -1;
			}
			sSocketThSockControl = sock;
		}
		status = 0;
	}
	if (!strcmp(name, SENSORS_HARDWARE_DATA)) {
		LOGD_IF(DEBUG, "sensors_device_open:SENSORS_HARDWARE_DATA\n");
		struct sensors_data_context_t *dev;
		dev = (struct sensors_data_context_t*)malloc(sizeof(*dev));

		/* initialize our state here */
		memset(dev, 0, sizeof(*dev));

		/* initialize the procs */
		dev->device.common.tag = HARDWARE_DEVICE_TAG;
		dev->device.common.version = 0;
		dev->device.common.module = (struct hw_module_t*)module;
		dev->device.common.close = sensors_data_device_close;

		dev->device.data_open = sensors_data_data_open;
		dev->device.data_close = sensors_data_data_close;
		dev->device.poll = sensors_data_poll;

		*device = &dev->device.common;

		if(sSocketThSockData == -1) {
			int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
			if (sock < 0) {
				LOGE("opening datagram socket for sensors socket thread in data side\n");
				return -1;
			}
			sSocketThSockData = sock;
		}
		status = 0;
	}
	return status;
}

static int sensors_control_device_close(struct hw_device_t *dev)
{
	struct sensors_control_context_t* ctx = (struct sensors_control_context_t*)dev;
	struct sockaddr_un name;

	LOGD_IF(DEBUG, "sensors_control_device_close\n");
	if(sProximity_thread != -1) {
		/* finish proximity sensor thread */
		int command = SENSORS_THREAD_EXIT_THREAD;

		/* Construct name of socket to send to. */
		name.sun_family = AF_UNIX; 
		strcpy(name.sun_path, SENSORS_PRXY_SOCK_NAME);

		LOGD_IF(DEBUG, "send command to sock(%s) = %08x\n", name.sun_path, command);
		/* Send message. */
		if (sendto(sProximityThSock, &command, sizeof(command), 0, (const struct sockaddr*)&name, sizeof(struct sockaddr_un)) < 0) {
			LOGE("sending thread message(%s)\n", strerror(errno));
		}
		pthread_detach(sProximity_thread);
		sProximity_thread = -1;
	}

	if(sProximityThSock != -1) {
		close(sProximityThSock);
		sProximityThSock = -1;
	}

	if(sAccelerometer_thread != -1) {
		/* finish accelerometer sensor thread */
		int command = SENSORS_THREAD_EXIT_THREAD;

		/* Construct name of socket to send to. */
		name.sun_family = AF_UNIX; 
		strcpy(name.sun_path, SENSORS_ACC_SOCK_NAME);

		LOGD_IF(DEBUG, "send command to sock(%s) = %08x\n", name.sun_path, command);
		/* Send message. */
		if (sendto(sAccelerometerThSock, &command, sizeof(command), 0, (const struct sockaddr*)&name, sizeof(struct sockaddr_un)) < 0) {
			LOGE("sending thread message(%s)\n", strerror(errno));
		}
		pthread_detach(sAccelerometer_thread);
		sAccelerometer_thread = -1;
	}

	if(sAccelerometerThSock != -1) {
		close(sAccelerometerThSock);
		sAccelerometerThSock = -1;
	}

	if(sMagnetic_thread != -1) {
		/* finish magnetic field sensor thread */
		int command = SENSORS_THREAD_EXIT_THREAD;

		/* Construct name of socket to send to. */
		name.sun_family = AF_UNIX; 
		strcpy(name.sun_path, SENSORS_MGNT_SOCK_NAME);

		LOGD_IF(DEBUG, "send command to sock(%s) = %08x\n", name.sun_path, command);
		/* Send message. */
		if (sendto(sMagneticThSock, &command, sizeof(command), 0, (const struct sockaddr*)&name, sizeof(struct sockaddr_un)) < 0) {
			LOGE("sending thread message(%s)\n", strerror(errno));
		}
		pthread_detach(sMagnetic_thread);
		sMagnetic_thread = -1;
	}

	if(sMagneticThSock != -1) {
		close(sMagneticThSock);
		sMagneticThSock = -1;
	}

	if(sSock_thread != -1) {
		/* finish sensors socket thread */
		int command = SENSORS_THREAD_EXIT_THREAD;

		/* Construct name of socket to send to. */
		name.sun_family = AF_UNIX; 
		strcpy(name.sun_path, SENSORS_SOCK_SOCK_NAME);

		LOGD_IF(DEBUG, "send command to sock(%s) = %08x\n", name.sun_path, command);
		/* Send message. */
		if (sendto(sSocketThSockControl, &command, sizeof(command), 0, (const struct sockaddr*)&name, sizeof(struct sockaddr_un)) < 0) {
			LOGE("sending thread message(%s)\n", strerror(errno));
		}
		pthread_detach(sSock_thread);
		sSock_thread = -1;
	}

	if(sSocketThSockControl != -1) {
		close(sSocketThSockControl);
		sSocketThSockControl = -1;
	}

	if(sSensorsSock != -1) {
		close(sSensorsSock);
		sSensorsSock = -1;
	}

	free(ctx);

	return 0;
}

static int sensors_data_device_close(struct hw_device_t *dev)
{
	struct sensors_data_context_t* ctx = (struct sensors_data_context_t*)dev;
	LOGD_IF(DEBUG, "sensors_data_device_close\n");

	if(sSocketThSockData != -1) {
		close(sSocketThSockData);
		sSocketThSockData = -1;
	}

	free(ctx);
	return 0;
}

static int sensors_control_wake(struct sensors_control_device_t *dev)
{
	int sock;
	struct sockaddr_un name;
	sensors_data_t sensor_data;

	LOGD_IF(DEBUG, "sensors_control_wake\n");
	/* Create socket on which to send. */
	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock < 0) {
		LOGE("opening datagram socket\n");
		return -1;
	}
	/* Construct name of socket to send to. */
	name.sun_family = AF_UNIX; 
	strcpy(name.sun_path, SENSORS_INPUT_SOCK_NAME);

	/* Prepare the data for wake */
	memset(&sensor_data, 0x00, sizeof(sensors_data_t));
	sensor_data.reserved = EWOULDBLOCK;

	/* Send message. */
	if (sendto(sock, &sensor_data, sizeof(sensors_data_t), 0, (const struct sockaddr*)&name, sizeof(struct sockaddr_un)) < 0) {
		LOGE("sending datagram message(%s)\n", strerror(errno));
	}
	close(sock);
	return 0;
}

/*****************************************************************************/

static int sensors_data_data_open(struct sensors_data_device_t *dev, native_handle_t* nh)
{
	sock_command_t msg;
	struct sockaddr_un name;
	int state = 0;
	int sock;
	char sock_name[PATH_MAX];
	const int one = 1;

	LOGD_IF(DEBUG, "sensors_data_data_open: user = %d\n", getpid());

	/* create socket for this caller */
	memset(&sock_name[0], 0x00, sizeof(sock_name));
	sprintf(sock_name, "%s_%d", SENSORS_INPUT_SOCK_NAME, getpid());
	unlink(sock_name);
	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if(sock < 0) {
		LOGE("Couldn't open sensors socket %d\n", sock);
		return -1;
	}
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
	name.sun_family = AF_UNIX;
	strcpy(name.sun_path, sock_name);
	if (bind(sock, (const struct sockaddr*)&name, strlen(name.sun_path) + sizeof (name.sun_family))) { 
		LOGE("binding name to sensors socket %s\n", strerror(errno));
		close(sock);
		return -1;
	}

	/* register this caller's socket to sensors socket thread */
	msg.command = SENSORS_THREAD_ADD_SOCKET;
	msg.pid = getpid();
	strcpy(msg.name.sun_path, sock_name);
	msg.name.sun_family = AF_UNIX;

	/* Construct name of socket to send to. */
	name.sun_family = AF_UNIX; 
	strcpy(name.sun_path, SENSORS_SOCK_SOCK_NAME);

	LOGD_IF(DEBUG, "send command to sock(%s) = %08x\n", name.sun_path, msg.command);
	/* Send message. */
	if (sendto(sSocketThSockData, &msg, sizeof(sock_command_t), 0, (const struct sockaddr*)&name, sizeof(struct sockaddr_un)) < 0) {
		LOGE("sending thread message(%s)\n", strerror(errno));
		close(sock);
		return -1;
	}

	sSockFD = sock;

	LOGD_IF(DEBUG, "sensors data open: name = %s, fd = %d\n", sock_name, sock);
	return 0;
}

static int sensors_data_data_close(struct sensors_data_device_t *dev)
{
	sock_command_t msg;
	struct sockaddr_un name;

	LOGD_IF(DEBUG, "sensors_data_data_close: fd = %d, user = %d\n", sSockFD, getpid());

	/* unregister this caller's socket from sensors socket thread */
	msg.command = SENSORS_THREAD_REMOVE_SOCKET;
	msg.pid = getpid();

	/* Construct name of socket to send to. */
	name.sun_family = AF_UNIX; 
	strcpy(name.sun_path, SENSORS_SOCK_SOCK_NAME);

	LOGD_IF(DEBUG, "send command to sock(%s) = %08x\n", name.sun_path, msg.command);
	/* Send message. */
	if (sendto(sSocketThSockData, &msg, sizeof(sock_command_t), 0, (const struct sockaddr*)&name, sizeof(struct sockaddr_un)) < 0) {
		LOGE("sending thread message(%s)\n", strerror(errno));
	}

	close(sSockFD);
	sSockFD = -1;

	return 0;
}

static int conv_sensor_type_to_handle(int type)
{
	switch(type)
	{
		case SENSORS_ACCELERATION:
			return SENSORS_ACCELERATION_HANDLE;
		case SENSORS_MAGNETIC_FIELD:
			return SENSORS_MAGNETIC_FIELD_HANDLE;
		case SENSORS_ORIENTATION:
			return SENSORS_ORIENTATION_HANDLE;
#if ENABLE_TEMPERATURE_SENSOR
		case SENSORS_TEMPERATURE:
			return SENSORS_TEMPERATURE_HANDLE;
#endif
		case SENSORS_PROXIMITY:
			return SENSORS_PROXIMITY_HANDLE;
#if ENABLE_ORIENTATION_RAW_SENSOR
		case SENSORS_ORIENTATION_RAW:
			return SENSORS_ORIENTATION_RAW_HANDLE;
#endif
		default:
			break;
	}
	return -1;
}

static int sensors_data_poll(struct sensors_data_device_t *dev, sensors_data_t* data)
{
	int sock = sSockFD;
	char state;
	int size;

	if((sock == -1) || (data == NULL)) {
		return -1;
	}

	/* read socket */
	size = read(sock, data, sizeof(sensors_data_t));
	if (size < 0) {
		LOGE("receiving datagram packet\n");
		return -1;
	} if(size == 0) {
		LOGE("socket disconnected\n");
		return -1;
	}

	/* wake call? */
	if((data->sensor) == 0 && (data->reserved == EWOULDBLOCK)) {
		LOGD_IF(DEBUG, "wake call received\n");
		return -EWOULDBLOCK;
	}
//	LOGD_IF(0, "sensors_data_poll(%02x)\n", data->sensor);

	return conv_sensor_type_to_handle(data->sensor);
}

