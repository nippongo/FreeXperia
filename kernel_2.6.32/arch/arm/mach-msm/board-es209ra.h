/* SEMC:modified */
/* 
   ES209RA board specific file.   
   Copyright (C) 2009 Sony Ericsson Mobile Communications Japan, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2, as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef _BOARD_ES209RA_H
#define _BOARD_ES209RA_H

#define MSM_PMEM_MDP_SIZE	0x1C91000

#define SMEM_SPINLOCK_I2C	"S:6"

#define MSM_PMEM_ADSP_SIZE	0x2196000
#define MSM_FB_SIZE		0x500000
#define MSM_AUDIO_SIZE		0x80000
#define MSM_GPU_PHYS_SIZE 	SZ_2M

#define MSM_SMI_BASE		0x00000000

#define MSM_SHARED_RAM_PHYS	(MSM_SMI_BASE + 0x00100000)

#define MSM_PMEM_SMI_BASE	(MSM_SMI_BASE + 0x02B00000)
#define MSM_PMEM_SMI_SIZE	0x01500000

#define MSM_FB_BASE		MSM_PMEM_SMI_BASE
#define MSM_GPU_PHYS_BASE 	(MSM_FB_BASE + MSM_FB_SIZE)
#define MSM_PMEM_SMIPOOL_BASE	(MSM_GPU_PHYS_BASE + MSM_GPU_PHYS_SIZE)
#define MSM_PMEM_SMIPOOL_SIZE	(MSM_PMEM_SMI_SIZE - MSM_FB_SIZE - MSM_GPU_PHYS_SIZE)

#define PMEM_KERNEL_EBI1_SIZE	0x28000

#define PMIC_VREG_WLAN_LEVEL	2600
#define PMIC_VREG_GP6_LEVEL	2850

#define FPGA_SDCC_STATUS	0x70000280


#define INT_ES209RA_GPIO_BATLOW			MSM_GPIO_TO_INT(145)
#define INT_ES209RA_GPIO_LEDC			MSM_GPIO_TO_INT(144)
#define INT_ES209RA_GPIO_UART2DM_RXD		MSM_GPIO_TO_INT(139)
#define INT_ES209RA_GPIO_PROXS			MSM_GPIO_TO_INT(108)
#define INT_ES209RA_GPIO_ETHER			MSM_GPIO_TO_INT(107)
#define INT_ES209RA_GPIO_VSYNC			MSM_GPIO_TO_INT(98)
#define INT_ES209RA_GPIO_ECOMPASS		MSM_GPIO_TO_INT(3)
#define INT_ES209RA_GPIO_ACCEL			MSM_GPIO_TO_INT(82)
#define INT_ES209RA_GPIO_OCV			MSM_GPIO_TO_INT(44)
#define INT_ES209RA_GPIO_TOUCHPAD		MSM_GPIO_TO_INT(37)
#define INT_ES209RA_GPIO_PM				MSM_GPIO_TO_INT(24)
#define INT_ES209RA_GPIO_CARD_INS_N		MSM_GPIO_TO_INT(23)
#define INT_ES209RA_GPIO_BT_HOSTWAKE	MSM_GPIO_TO_INT(21)
#define INT_ES209RA_GPIO_CAM_VSYNC		MSM_GPIO_TO_INT(14)
#define INT_ES209RA_GPIO_CAM_ISP		MSM_GPIO_TO_INT(1) 

/* SEMC: add bit definition for startup_reason - start */
/* PMIC startup reason */
#define STARTUP_REASON_POWER_KEY		(1 << 0)
#define STARTUP_REASON_RTC_ALARM		(1 << 1)
#define STARTUP_REASON_CABLE_POWER_ON	(1 << 2)
#define STARTUP_REASON_SMPL				(1 << 3)
#define STARTUP_REASON_WDOG_RESTART		(1 << 4)
#define STARTUP_REASON_USB_CHARGER		(1 << 5)
#define STARTUP_REASON_WALL_CHARGER		(1 << 6)
/* software startup reason */
#define STARTUP_REASON_FOTA_IU			(1 << 16)
#define STARTUP_REASON_FOTA_FI			(1 << 17)
#define STARTUP_REASON_MR				(1 << 18)
#define STARTUP_REASON_CMZ				(1 << 19)
/* software mode */
#define STARTUP_REASON_TYPE_APPROVAL	(1 << 28)
/* SEMC: add bit definition for startup_reason - end */

#define AKM8973_GPIO_RESET_PIN 2
#define AKM8973_GPIO_RESET_ON 0
#define AKM8973_GPIO_RESET_OFF 1


extern struct max17040_device_data max17040_dev_data;

#endif

