/*
 * Copyright (C) 2010-2012 Freescale Semiconductor, Inc.
 *
 * Configuration settings for the MX6Q Armadillo2 Freescale board.
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

#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/arch/mx6.h>

//#define CONFIG_MFG

 /* High Level Configuration Options */
#define CONFIG_ARMV7	/* This is armv7 Cortex-A9 CPU core */
#define CONFIG_MXC
#define CONFIG_MX6DL
#define CONFIG_MX6DL_LPDDR2
#define CONFIG_MX6Q_ARM2
#define CONFIG_DDR_32BIT /* For 32bit DDR, comment it out for 64bit */
#define CONFIG_FLASH_HEADER
#define CONFIG_FLASH_HEADER_OFFSET 0x400
#define CONFIG_MX6_CLK32	   32768

#define CONFIG_MX6DL_NTX

//#define SDRAM_NO_BANK_2		1

/*
 * #define CONFIG_SECURE_BOOT
 *	Enable Secure Boot. DO NOT TURN ON IT until you know what you are doing
 */

#define CONFIG_SKIP_RELOCATE_UBOOT

#define CONFIG_ARCH_CPU_INIT
#undef CONFIG_ARCH_MMU /* disable MMU first */
#define CONFIG_L2_OFF  /* disable L2 cache first*/

/*
 * #define CONFIG_FLASH_PLUG_IN
 */

#define CONFIG_MX6_HCLK_FREQ	24000000

#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

#define CONFIG_SYS_64BIT_VSPRINTF

#define BOARD_EARLY_INIT	// board early initial timing after emmc/sd/env loaded . 
#define BOARD_LATE_INIT

#define CONFIG_CMDLINE_TAG	/* enable passing of ATAGs */
#define CONFIG_SERIAL_TAG
#define CONFIG_REVISION_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN		(2 * 1024 * 1024)
/* size in bytes reserved for initial data */
#define CONFIG_SYS_GBL_DATA_SIZE	128

/*
 * Hardware drivers
 */
#define CONFIG_MXC_UART
#define CONFIG_UART_BASE_ADDR   UART1_BASE_ADDR

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_CONS_INDEX		1
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{9600, 19200, 38400, 57600, 115200}

/***********************************************************
 * Command definition
 ***********************************************************/

#include <config_cmd_default.h>

#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_MII
#define CONFIG_CMD_NET
#define CONFIG_NET_RETRY_COUNT  100
#define CONFIG_NET_MULTI 1
#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_DNS

#define CONFIG_CMD_SPI
#define CONFIG_CMD_I2C
#define CONFIG_CMD_IMXOTP

/* Enable below configure when supporting nand */
#define CONFIG_CMD_SF
#define CONFIG_CMD_MMC
#define CONFIG_CMD_ENV
#define CONFIG_CMD_DATE

#define CONFIG_CMD_CLOCK
#define CONFIG_REF_CLK_FREQ CONFIG_MX6_HCLK_FREQ

#undef CONFIG_CMD_IMLS

#define CONFIG_CMD_IMX_DOWNLOAD_MODE

#ifdef CONFIG_MFG
#define CONFIG_BOOTDELAY 0
#else
#define CONFIG_BOOTDELAY 1
#endif

#define CONFIG_PRIME	"FEC0"

#define CONFIG_LOADADDR		0x10800000	/* loadaddr env var */
#define CONFIG_RD_LOADADDR	0x10B00000 //
#define NTX_HWCFG_PRELOAD_ADDR	0x10aff800 // reserved 2KB Bytes between kernel and initrd .

#if 0 //[

#define	CONFIG_EXTRA_ENV_SETTINGS					\
		"netdev=eth0\0"						\
		"ethprime=FEC0\0"					\
		"uboot=u-boot.bin\0"			\
		"kernel=uImage\0"				\
		"nfsroot=/opt/eldk/arm\0"				\
		"bootargs_base=setenv bootargs console=ttymxc0,115200\0"\
		"bootargs_nfs=setenv bootargs ${bootargs} root=/dev/nfs "\
			"ip=dhcp nfsroot=${serverip}:${nfsroot},v3,tcp\0"\
		"bootcmd_net=run bootargs_base bootargs_nfs; "		\
			"tftpboot ${loadaddr} ${kernel}; bootm\0"	\
		"bootargs_mmc=setenv bootargs ${bootargs} "     \
			"root=/dev/mmcblk0p1 rootwait\0"                \
		"bootcmd_mmc=run bootargs_base bootargs_mmc; "   \
		"mmc dev 0; "	\
		"mmc read ${loadaddr} 0x800 0x2000; bootm\0"	\
		"bootcmd=run bootcmd_mmc\0"                             \


#else //][!

#ifdef CONFIG_MFG //[

#ifdef CONFIG_MT42L128M32D1 //[
	#ifdef CONFIG_MFG_ADB //[
		#define CONFIG_BOOTARGS         "console=ttymxc0,115200 hwcfg_p=0x9ffffe00 hwcfg_sz=110 waveform_p=0x9F800000 waveform_sz=6609376"
	#else //][! CONFIG_MFG_ADB
		#define CONFIG_BOOTARGS         "console=ttymxc0,115200 rdinit=/linuxrc nosmp hwcfg_p=0x9ffffe00 hwcfg_sz=110"
	#endif //] CONFIG_MFG_ADB
#else //][! CONFIG_MT42L128M32D1
	#define CONFIG_BOOTARGS         "console=ttymxc0,115200 rdinit=/linuxrc nosmp hwcfg_p=0x9ffffe00 hwcfg_sz=110"
#endif //] CONFIG_MT42L128M32D1

#define CONFIG_BOOTCOMMAND      "bootm 0x10800000 0x10B00000"
#define CONFIG_EXTRA_ENV_SETTINGS                                       \
		"netdev=eth0\0"                                         \
		"ethprime=FEC0\0"                                       \
		"uboot=u-boot.bin\0"                    \
		"kernel=uImage\0"                               \

#else //][!CONFIG_MFG
#define	CONFIG_EXTRA_ENV_SETTINGS					\
		"uboot=u-boot.bin\0"			\
		"kernel=uImage\0"				\
		"bootargs_base=setenv bootargs console=ttymxc0,115200 rootwait rw no_console_suspend\0"\
		"bootargs_mmc=setenv bootargs ${bootargs}\0"  \
		"bootcmd_mmc=run bootargs_base bootargs_mmc;load_ntxkernel; bootm\0" \
		"bootargs_SD=setenv bootargs ${bootargs}\0" \
		"bootcmd_SD=run bootargs_base bootargs_SD;load_ntxkernel; bootm\0"   \
		"bootargs_recovery=setenv bootargs ${bootargs}\0" \
		"bootcmd_recovery=run bootargs_base bootargs_recovery;load_ntxkernel; bootm\0"   \
		"bootcmd=run bootcmd_mmc\0" \
		"KRN_SDNUM_SD=1\0" \
		"KRN_SDNUM_Recovery=0\0" \
		"verify=no" 
#endif//] CONFIG_MFG


#endif //]

#define CONFIG_ARP_TIMEOUT	200UL

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
//#define CONFIG_SYS_PROMPT		"MX6Sl NTX U-Boot > "
#define CONFIG_SYS_PROMPT		"eBR-1A # "
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE		1024	/* Console I/O Buffer Size */
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS	32	/* max number of command args */
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE /* Boot Argument Buffer Size */

#define CONFIG_SYS_MEMTEST_START	0x10000000	/* memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x10010000

#undef	CONFIG_SYS_CLKS_IN_HZ		/* everything, incl board info, in Hz */

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR

#define CONFIG_SYS_HZ			1000

#define CONFIG_CMDLINE_EDITING

#if 0
#define CONFIG_FEC_CLOCK_FROM_ANATOP
#define CONFIG_FEC0_IOBASE	ENET_BASE_ADDR
#define CONFIG_FEC0_PINMUX	-1
#define CONFIG_FEC0_MIIBASE	-1
#define CONFIG_GET_FEC_MAC_ADDR_FROM_IIM
#define CONFIG_MXC_FEC
#define CONFIG_MII_GASKET
#define CONFIG_FEC0_PHY_ADDR		0
#define CONFIG_ETH_PRIME
#define CONFIG_MII
#define CONFIG_IPADDR			192.168.1.103
#define CONFIG_SERVERIP			192.168.1.101
#define CONFIG_NETMASK			255.255.255.0
#endif

#define CONFIG_MXC_GPIO

/*
 * OCOTP Configs
 */
#ifdef CONFIG_CMD_IMXOTP
	#define CONFIG_IMX_OTP
	#define IMX_OTP_BASE			OCOTP_BASE_ADDR
	#define IMX_OTP_ADDR_MAX		0x7F
	#define IMX_OTP_DATA_ERROR_VAL	0xBADABADA
#endif

/*
 * SPI Configs
 */
#ifdef CONFIG_CMD_SF
	#define CONFIG_FSL_SF		1
	#define CONFIG_SPI_FLASH_IMX_M25PXX	1
	#define CONFIG_SPI_FLASH_CS	0
	#define CONFIG_IMX_ECSPI
	#define IMX_CSPI_VER_2_3	1
	#define MAX_SPI_BYTES		(64 * 4)
#endif

/*
 * MMC Configs
 */
#ifdef CONFIG_CMD_MMC
	#define CONFIG_MMC
	#define CONFIG_GENERIC_MMC
	#define CONFIG_IMX_MMC
	#define CONFIG_SYS_FSL_USDHC_NUM        4
	#define CONFIG_SYS_FSL_ESDHC_ADDR       0
	#define CONFIG_SYS_MMC_ENV_DEV  0
	#define CONFIG_DOS_PARTITION	1
	#define CONFIG_CMD_FAT		1
	#define CONFIG_CMD_EXT2		1
	#define CONFIG_CMD_EXT4		1

	/* detect whether SD1, 2, or 3 is boot device */
	#define CONFIG_DYNAMIC_MMC_DEVNO

	/* SD1 is 8 bit */
	#define CONFIG_MMC_8BIT_PORTS   0x1
	/* Setup target delay in DDR mode for each SD port */
	#define CONFIG_GET_DDR_TARGET_DELAY
#endif

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128 * 1024)	/* regular stack */

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	2
#define PHYS_SDRAM_1		CSD0_DDR_BASE_ADDR
#define PHYS_SDRAM_1_SIZE	(1024 * 1024 * 1024)
#define PHYS_SDRAM_2		CSD1_DDR_BASE_ADDR
#define PHYS_SDRAM_2_SIZE	(1024 * 1024 * 1024)
#define iomem_valid_addr(addr, size) \
 ((addr >= PHYS_SDRAM_1 && addr <= (PHYS_SDRAM_1 + PHYS_SDRAM_1_SIZE)) || \
 (addr >= PHYS_SDRAM_2 && addr <= (PHYS_SDRAM_2 + PHYS_SDRAM_2_SIZE)))

/*-----------------------------------------------------------------------
 * IRAM Memory Map
 */
/* #define IRAM_FREE_START		0x00907000 */

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CONFIG_SYS_NO_FLASH

/* Monitor at beginning of flash */
#ifndef CONFIG_MFG
#define CONFIG_FSL_ENV_IN_MMC
#endif
/* #define CONFIG_FSL_ENV_IN_NAND */
/* #define CONFIG_FSL_ENV_IN_SATA */

#define CONFIG_ENV_SECT_SIZE    (8 * 1024)
#define CONFIG_ENV_SIZE         CONFIG_ENV_SECT_SIZE

#if defined(CONFIG_FSL_ENV_IN_MMC)
	#define CONFIG_ENV_IS_IN_MMC	1
	#define CONFIG_ENV_OFFSET	(768 * 1024)
#elif defined(CONFIG_FSL_ENV_IN_SF)
	#define CONFIG_ENV_IS_IN_SPI_FLASH	1
	#define CONFIG_ENV_SPI_CS		1
	#define CONFIG_ENV_OFFSET       (768 * 1024)
#else
	#define CONFIG_ENV_IS_NOWHERE	1
#endif


/*
 * I2C Configs
 */
#ifdef CONFIG_CMD_I2C
	#define CONFIG_HARD_I2C         1
	#define CONFIG_I2C_MXC          1

	#define GET_I2C_CHN_NEONODE()		1
	#define GET_I2C_CHN_TPS65185()	2
	#define GET_I2C_CHN_MSP430()		3
	
	#define CONFIG_I2C_MULTI_BUS
	#ifdef CONFIG_I2C_MULTI_BUS//[
	
	#ifndef __ASSEMBLY__//[
	typedef struct tagI2C_PLATFORM_DATA {
		unsigned int dwPort;
		unsigned int dwSpeed;
		unsigned int dwSlave;
		int iIsInited;
	} I2C_PLATFORM_DATA;
	extern unsigned int gdwBusNum;
	extern I2C_PLATFORM_DATA gtI2C_platform_dataA[];
	#endif //]__ASSEMBLY__

	#define CONFIG_SYS_I2C_PORT             gtI2C_platform_dataA[gdwBusNum].dwPort
	#define CONFIG_SYS_I2C_SPEED            gtI2C_platform_dataA[gdwBusNum].dwSpeed
	#define CONFIG_SYS_I2C_SLAVE            gtI2C_platform_dataA[gdwBusNum].dwSlave
	#else//][!CONFIG_I2C_MULTI_BUS
//	#define CONFIG_SYS_I2C_PORT             I2C1_BASE_ADDR
//	#define CONFIG_SYS_I2C_SPEED            100000
//	#define CONFIG_SYS_I2C_SLAVE            0x8
	#define CONFIG_SYS_I2C_PORT             I2C3_BASE_ADDR
	#define CONFIG_SYS_I2C_SPEED            100000
	#define CONFIG_SYS_I2C_SLAVE            0xfe
	#endif//]CONFIG_I2C_MULTI_BUS
	
#endif

//#define CONFIG_SPLASH_SCREEN

/*
 * SPLASH SCREEN Configs
 */
#ifdef CONFIG_SPLASH_SCREEN
	/*
	 * Framebuffer and LCD
	 */
	#define CONFIG_MXC_EPDC				1
	#define CONFIG_LCD
	#define CONFIG_FB_BASE				(TEXT_BASE + 0x1000000)
	#define CONFIG_SYS_CONSOLE_IS_IN_ENV
#ifdef CONFIG_MXC_EPDC

	//#define ADVANCE_WAVEFORM_FILE 1

	#undef LCD_TEST_PATTERN
	/* #define CONFIG_SPLASH_IS_IN_MMC			1 */
	#define LCD_BPP					LCD_MONOCHROME
	/* #define CONFIG_SPLASH_SCREEN_ALIGN		1 */

	#define CONFIG_WORKING_BUF_ADDR			(TEXT_BASE + 0x100000)

	#ifdef ADVANCE_WAVEFORM_FILE//[
		#define CONFIG_WAVEFORM_BUF_ADDR		gpbWaveform
	#else//][!ADVANCE_WAVEFORM_FILE
		//	#define CONFIG_WAVEFORM_BUF_ADDR		(TEXT_BASE + 0x200000)
		#define CONFIG_WAVEFORM_BUF_ADDR		(TEXT_BASE + 0x800000)
	#endif//]ADVANCE_WAVEFORM_FILE

//	#define CONFIG_WAVEFORM_FILE_OFFSET		0x600000
//	#define CONFIG_WAVEFORM_FILE_SIZE		0xF0A00
	#define CONFIG_WAVEFORM_FILE_IN_MMC

#ifdef CONFIG_SPLASH_IS_IN_MMC
	#define CONFIG_SPLASH_IMG_OFFSET		0x4c000
	#define CONFIG_SPLASH_IMG_SIZE			0x19000
#endif
#endif
#endif /* CONFIG_SPLASH_SCREEN */

#ifndef __ASSEMBLY__//[
extern char *gpszNTX_SN;
#endif //]__ASSEMBLY__

#endif				/* __CONFIG_H */
