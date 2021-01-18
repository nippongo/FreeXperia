/* include/linux/capella_cm3602.h
 *
 * Copyright (C) 2009 Google, Inc.
 * Author: Iliyan Malchev <malchev@google.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __LINUX_AVAGO_APDS9120_H
#define __LINUX_AVAGO_APDS9120_H

#include <linux/types.h>
#include <linux/ioctl.h>

#define AVAGO_APDS9120_IOCTL_MAGIC 'A'
#define PROXIMITY_GET_POWER_STATE 		_IOR(PROXIMITY_IOC_MAGIC, 0x00, unsigned char)
#define PROXIMITY_SET_POWER_STATE 		_IOW(PROXIMITY_IOC_MAGIC, 0x01, unsigned char)

#ifdef __KERNEL__
#define AVAGO_APDS9120 "avago_apds9120"
struct avago_apds9120_platform_data {
	int (*power)(int); /* power to the chip */
	int p_en; /* proximity-sensor enable */
	int p_out; /* proximity-sensor outpuCAPELLA_CM3602_IOCTL_ENABLE,t */
};
#endif /* __KERNEL__ */

#endif
