/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __SENSOR_THREAD_H__
#define __SENSOR_THREAD_H__

/* sensors thread information */
typedef struct {
	int					device_input_fd;
	int					device_fd;
	int					sock_input_fd;
	struct sockaddr_un	sock_input_addr;
	int					sock_hal_fd;
	int					sensing_time;
	int					timer_fd;
	int					timer_value;
} sensors_thread_t;

/* socket information */
typedef struct socket_inf {
	pid_t				pid;
	int					fd;
	struct sockaddr_un	name;
	struct socket_inf*	next;
	struct socket_inf*	prev;
} socket_inf_t;

/* socket thread information */
typedef struct {
	int					sock_input_fd;
	int					sock_hal_fd;
	int					sock_count;
	socket_inf_t*		sock_inf_head;
} socket_thread_t;

/* control commands */
#define SENSORS_THREAD_ENABLE_SENSOR		0x1001
#define SENSORS_THREAD_DISABLE_SENSOR		0x1002
#define SENSORS_THREAD_EXIT_THREAD			0x1003
#define SENSORS_THREAD_ADD_SOCKET			0x1004
#define SENSORS_THREAD_REMOVE_SOCKET		0x1005
#define SENSORS_THREAD_CHANGE_POLLING_INTERVAL		0x1006

/* sensors socket names */
#define SENSORS_INPUT_SOCK_NAME				"/dev/socket/sensors/sock_input"
#define SENSORS_SOCK_SOCK_NAME				"/dev/socket/sensors/sock_sock_thread"
#define SENSORS_ACC_SOCK_NAME				"/dev/socket/sensors/sock_acc_thread"
#define SENSORS_MGNT_SOCK_NAME				"/dev/socket/sensors/sock_mngt_thread"
#define SENSORS_PRXY_SOCK_NAME				"/dev/socket/sensors/sock_prxy_thread"

/* sensors thread message */
typedef struct {
	int					command;
	int					param[4];
} sensors_command_t;

/* sensors thread message */
typedef struct {
	int					command;
	pid_t				pid;
	struct sockaddr_un	name;
} sock_command_t;

/* sensors thread */
extern void* proximity_thread_main( void* args );
extern void* accelerometer_thread_main( void* args );
extern void* magnetic_thread_main( void* args );

/* sensors socket thread */
extern void* socket_thread_main( void* args );

/* flags */
#define ENABLE_TEMPERATURE_SENSOR 0
#define ENABLE_ORIENTATION_RAW_SENSOR 0

#define DEBUG 0

#endif
