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

#define LOG_TAG "Sensors_Socket"

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

#include "sensors_thread.h"

/*****************************************************************************/

void add_sock_inf_to_list(socket_thread_t* thread, socket_inf_t* inf)
{
	socket_inf_t* temp;

	if(thread->sock_count == 0)
	{
		thread->sock_inf_head = inf;
		inf->prev = NULL;
	} else {
		for(temp = thread->sock_inf_head; temp->next != NULL; temp = temp->next) {
			;
		}
		temp->next = inf;
		inf->prev = temp;
	}

	thread->sock_count++;
}

void remove_sock_inf_from_list(socket_thread_t* thread, socket_inf_t* inf)
{
	if(thread->sock_count == 1) {
		thread->sock_inf_head = NULL;
	} else {
		if(inf->prev != NULL) {
			(inf->prev)->next = inf->next;
		}
		if(inf->next != NULL) {
			(inf->next)->prev = inf->prev;
		}
		if(thread->sock_inf_head == inf) {
			thread->sock_inf_head = inf->next;
		}
	}
	close(inf->fd);
	free(inf);

	thread->sock_count--;
}

static int add_socket(socket_thread_t* thread, sock_command_t* msg)
{
	socket_inf_t* inf;
	socket_inf_t* temp;
	int sock;

	LOGD_IF(DEBUG, "add_socket: pid = %d, name = %s\n", msg->pid, msg->name.sun_path);

	inf = (socket_inf_t*)malloc(sizeof(socket_inf_t));
	if(inf == NULL) {
		LOGE("memory allocate\n");
		return -1;
	}

	inf->pid = msg->pid;
	strcpy(inf->name.sun_path, msg->name.sun_path);
	inf->name.sun_family = msg->name.sun_family; 
	inf->next = NULL;
	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if(sock < 0) {
		LOGE("Couldn't create socket for application socket\n");
		free(inf);
		return -1;
	}
	inf->fd = sock;

	add_sock_inf_to_list(thread, inf);

	LOGD_IF(DEBUG, "socket count = %d\n", thread->sock_count);

	return 0;
}

static int remove_socket(socket_thread_t* thread, sock_command_t* msg)
{
	socket_inf_t* temp;
	char find = 0;

	LOGD_IF(DEBUG, "remove_socket: pid = %d\n", msg->pid);
	for(temp = thread->sock_inf_head; temp != NULL; temp = temp->next) {
		if(temp->pid == msg->pid) {
			find = 1;
			break;
		}
	}

	if(find == 0) {
		/* no match */
		LOGD_IF(DEBUG, "no match socket\n");
		return 0;
	}
	/* remove socket from socket list */
	remove_sock_inf_from_list(thread, temp);

	LOGD_IF(DEBUG, "socket count = %d\n", thread->sock_count);

	return 0;
}


static int duplicate_event(socket_thread_t* thread, sensors_data_t* data)
{
	socket_inf_t* temp;

	LOGD_IF(0, "duplicate_event\n");

	for(temp = thread->sock_inf_head; temp != NULL; temp = temp->next) {
		if (sendto(temp->fd, data, sizeof(sensors_data_t), 0,
					(const struct sockaddr*)&temp->name, sizeof(struct sockaddr_un)) < 0) {
			LOGE("sending datagram message - %s\n", strerror(errno));
			/* remove err socket from socket list */
			remove_sock_inf_from_list(thread, temp);
		}
	}

	return 0;
}

static int socket_thread_init(socket_thread_t* thread)
{
	int err = -1;
	int sock;
	struct sockaddr_un name;
	const int one = 1;

	/* initialize sensors thread info */
	thread->sock_count = 0;
	thread->sock_hal_fd = -1;
	thread->sock_inf_head = NULL;

	/* create socket for sensors HAL */
	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if(sock < 0) {
		LOGE("Couldn't create socket for sensors HAL\n");
		goto err_create_sock_hal;
	}
	thread->sock_hal_fd = sock;

	/* delete sock file if exist */
	unlink(SENSORS_SOCK_SOCK_NAME);

	/* bind to sensors HAL */ 
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
	name.sun_family = AF_UNIX;
	strcpy(name.sun_path, SENSORS_SOCK_SOCK_NAME);
	err = bind(sock, (const struct sockaddr*)&name, strlen(name.sun_path) + sizeof (name.sun_family));
	if(err < 0) {
		LOGE("Couldn't bind to sensors HAL(%s) - %s\n", name.sun_path, strerror(errno));
		goto err_bind_sock_hal;
	}
	return 0;


err_bind_sock_hal:
	close(thread->sock_hal_fd);
err_create_sock_hal:
	return err;
}

static int socket_thread_exit(socket_thread_t* thread)
{
	socket_inf_t* temp;
	struct socket_inf* temp_free;

	/* close socket for sensors HAL */
	if(thread->sock_hal_fd != -1) {
		close(thread->sock_hal_fd);
		thread->sock_hal_fd = -1;
	}
	unlink(SENSORS_SOCK_SOCK_NAME);

	/* free all socket info */
	if(thread->sock_inf_head != NULL) {
		for(temp = thread->sock_inf_head; temp != NULL;) {
			temp_free = temp;
			temp = temp->next;
			close(temp_free->fd);
			free(temp_free);
		}
	}

	return 0;
}

void* socket_thread_main( void* args )
{
	int err = 0;
	socket_thread_t thread_info;
	fd_set fds, readfds;
	int maxfd = 0;
	char state;
	sensors_data_t data;
	sock_command_t msg;
	int break_flag = 0;

	LOGD_IF(DEBUG, "Sensors socket thread start\n");

	/* inisialize sensors thread */
	memset(&thread_info, 0x00, sizeof(sensors_thread_t));
	err = socket_thread_init(&thread_info);
	thread_info.sock_input_fd = (int)args;	/* sensors socket FD */

	/* prepare for select */
	FD_ZERO(&readfds);
	FD_SET(thread_info.sock_hal_fd, &readfds);
	FD_SET(thread_info.sock_input_fd, &readfds);
	if (thread_info.sock_hal_fd > thread_info.sock_input_fd) {
		maxfd = thread_info.sock_hal_fd;
	} else {
		maxfd = thread_info.sock_input_fd;
	}

	while(1) {
		/* select */
		memcpy(&fds, &readfds, sizeof(fd_set));
		err = select(maxfd+1, &fds, NULL, NULL, NULL);
		if(err < 0) {
			LOGE("Couldn't select %d\n", err);
			break;
		} else {				/* receive signal from sensors HAL */
			/* input event */
			if (FD_ISSET(thread_info.sock_input_fd, &fds)) {
				memset(&data, 0x00, sizeof(sensors_data_t));
				read(thread_info.sock_input_fd, &data, sizeof(sensors_data_t));

				/* duplicate sensors data to all client sockets */
				duplicate_event(&thread_info, &data);
			}
			/* socket event */
			if (FD_ISSET(thread_info.sock_hal_fd, &fds)) {
				memset(&msg, 0x00, sizeof(sock_command_t));
				read(thread_info.sock_hal_fd, &msg, sizeof(sock_command_t));

				switch(msg.command)
				{
					case SENSORS_THREAD_ADD_SOCKET:
						LOGD_IF(DEBUG, "SENSORS_THREAD_ADD_SOCKET received\n");
						err = add_socket(&thread_info, &msg);
						if(err) {
							LOGE("Couldn't add socket\n");
							break_flag = 1;
						}
						break;

					case SENSORS_THREAD_REMOVE_SOCKET:
						LOGD_IF(DEBUG, "SENSORS_THREAD_REMOVE_SOCKET received\n");
						err = remove_socket(&thread_info, &msg);
						if(err) {
							LOGE("Couldn't del socket\n");
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
	err = socket_thread_exit(&thread_info);
	LOGD_IF(DEBUG, "Sensors socket thread exit\n");

	return 0;
}

