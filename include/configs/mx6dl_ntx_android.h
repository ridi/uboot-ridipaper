/*
 * Copyright (C) 2012 Freescale Semiconductor, Inc.
 *
 * Configuration settings for the MX6Q Sabre Lite2 Freescale board.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef MX6SL_NTX_ANDROID_H
#define MX6SL_NTX_ANDROID_H

#include <configs/mx6dl_ntx.h>
#include <asm/mxc_key_defs.h>

#define CONFIG_USB_DEVICE
#define CONFIG_IMX_UDC		       1
#define CONFIG_FASTBOOT		       1
#define CONFIG_FASTBOOT_STORAGE_EMMC_SATA
#define CONFIG_FASTBOOT_VENDOR_ID      0x18d1
#define CONFIG_FASTBOOT_PRODUCT_ID     0x0d02
#define CONFIG_FASTBOOT_BCD_DEVICE     0x311
#define CONFIG_FASTBOOT_MANUFACTURER_STR  "Freescale"
#define CONFIG_FASTBOOT_PRODUCT_NAME_STR "i.mx6sl NTX Smart Device"
#define CONFIG_FASTBOOT_INTERFACE_STR	 "Android fastboot"
#define CONFIG_FASTBOOT_CONFIGURATION_STR  "Android fastboot"

#define CONFIG_FASTBOOT_NTX_SN	1

#ifdef CONFIG_FASTBOOT_NTX_SN //[
#define CONFIG_FASTBOOT_SERIAL_NUM	gpszNTX_SN
#else //][!CONFIG_FASTBOOT_NTX_SN
#define CONFIG_FASTBOOT_SERIAL_NUM	"12345"
#endif //] CONFIG_FASTBOOT_NTX_SN

#define CONFIG_FASTBOOT_SATA_NO		 0

/*  mx6sl ddr address starts from 0x80000000, not like mx6dl and mx6q
*   which start from 0x10000000
*   For system.img growing up more than 256MB, more buffer needs
*   to receive the system.img*/
#define CONFIG_FASTBOOT_TRANSFER_BUF    0x80000000
#define CONFIG_FASTBOOT_TRANSFER_BUF_SIZE 0x40000000 /* 1G byte */


#define CONFIG_CMD_BOOTI
#define CONFIG_ANDROID_RECOVERY

#define CONFIG_VOL_DOWN_KEY     KEY_4
#define CONFIG_POWER_KEY	KEY_F2
#define CONFIG_HOME_KEY	KEY_MENU

#define CONFIG_MXC_KPD
#define CONFIG_MXC_KEYMAPPING \
	{       \
		KEY_SELECT, KEY_BACK, KEY_1,     KEY_2, \
		KEY_3,      KEY_4,    KEY_5,     KEY_MENU, \
		KEY_6,      KEY_7,    KEY_8,     KEY_9, \
		KEY_UP,     KEY_LEFT, KEY_RIGHT, KEY_DOWN, \
	}
#define CONFIG_MXC_KPD_COLMAX 4
#define CONFIG_MXC_KPD_ROWMAX 4


/* which mmc bus is your main storage ? */
#define CONFIG_ANDROID_MAIN_MMC_BUS 0
//#define CONFIG_ANDROID_BOOT_PARTITION_MMC 1
//#define CONFIG_ANDROID_SYSTEM_PARTITION_MMC 5
//#define CONFIG_ANDROID_RECOVERY_PARTITION_MMC 2
//#define CONFIG_ANDROID_CACHE_PARTITION_MMC 6
//#define CONFIG_ANDROID_DATA_PARTITION_MMC 7
//#define CONFIG_ANDROID_VENDOR_PARTITION_MMC 8
//#define CONFIG_ANDROID_MISC_PARTITION_MMC 9
//#define CONFIG_ANDROID_MEDIA_PARTITION_MMC 4
//#define CONFIG_ANDROID_EMERGENCY_PARTITION_MMC 10


#define CONFIG_ANDROID_RECOVERY_CMD_FILE "/recovery/command"
#define CONFIG_ANDROID_RECOVERY_BOOTARGS_MMC NULL
#define CONFIG_ANDROID_RECOVERY_BOOTCMD_MMC  \
	"booti ${fastboot_dev} recovery"

#if 0 //[
#define CONFIG_INITRD_TAG

#undef CONFIG_LOADADDR
#undef CONFIG_RD_LOADADDR
#undef CONFIG_EXTRA_ENV_SETTINGS

#define CONFIG_LOADADDR		0x80800000	/* loadaddr env var */
#define CONFIG_RD_LOADADDR	0x81000000


#define	CONFIG_EXTRA_ENV_SETTINGS					\
		"netdev=eth0\0"						\
		"ethprime=FEC0\0"					\
		"fastboot_dev=mmc0\0"					\
		"bootcmd=booti ${fastboot_dev}\0"

#endif //]

#endif


