/*
 * Copyright (C) 2010-2012 Freescale Semiconductor, Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/mx6.h>
#include <asm/arch/mx6_pins.h>
#include <asm/arch/mx6dl_pins.h>
#if defined(CONFIG_SECURE_BOOT)
#include <asm/arch/mx6_secure.h>
#endif
#include <asm/arch/iomux-v3.h>
#include <asm/arch/regs-anadig.h>
#include <asm/errno.h>
#include <imx_wdog.h>
#ifdef CONFIG_MXC_FEC
#include <miiphy.h>
#endif

#if defined(CONFIG_MXC_EPDC)
#include <lcd.h>
#endif

#ifdef CONFIG_IMX_ECSPI
#include <imx_spi.h>
#endif

#ifdef CONFIG_CMD_MMC
#include <mmc.h>
#include <fsl_esdhc.h>
#endif

#ifdef CONFIG_MXC_GPIO
#include <asm/gpio.h>
#include <asm/arch/gpio.h>
#endif

#ifdef CONFIG_ANDROID_RECOVERY
#include <recovery.h>
#endif

#if CONFIG_I2C_MXC
#include <i2c.h>
#endif

#ifdef CONFIG_MXC_EPDC //[
#include "../../../drivers/video/mxc_epdc_fb.h"
#endif //]CONFIG_MXC_EPDC

#include "ntx_comm.h"
#include "ntx_comm.c"


#include "ntx_hwconfig.h"
extern volatile NTX_HWCONFIG *gptNtxHwCfg ;

#include "ntx_hw.h"
#include "ntx_hw.c"

//#include "epdfb_dc.h"


unsigned int gdwBusNum=2;
I2C_PLATFORM_DATA gtI2C_platform_dataA[] = {
	{I2C1_BASE_ADDR,100000,0xfe,0},
	{I2C2_BASE_ADDR,100000,0xfe,0},
	{I2C3_BASE_ADDR,100000,0xfe,0},
};

DECLARE_GLOBAL_DATA_PTR;

static u32 system_rev;
static enum boot_device boot_dev;

#define USB_OTG_PWR IMX_GPIO_NR(4, 0)
#define USB_H1_PWR IMX_GPIO_NR(4, 2)

//volatile unsigned GPIO_POWER_KEY=IMX_GPIO_NR(4, 25);
//#define GPIO_POWER_KEY gMX6SL_POWER_KEY



static inline void setup_boot_device(void)
{
	uint soc_sbmr = readl(SRC_BASE_ADDR + 0x4);
	uint bt_mem_ctl = (soc_sbmr & 0x000000FF) >> 4 ;
	uint bt_mem_type = (soc_sbmr & 0x00000008) >> 3;

	switch (bt_mem_ctl) {
	case 0x0:
		if (bt_mem_type)
			boot_dev = ONE_NAND_BOOT;
		else
			boot_dev = WEIM_NOR_BOOT;
		break;
	case 0x2:
			boot_dev = SATA_BOOT;
		break;
	case 0x3:
		if (bt_mem_type)
			boot_dev = I2C_BOOT;
		else
			boot_dev = SPI_NOR_BOOT;
		break;
	case 0x4:
	case 0x5:
		boot_dev = SD_BOOT;
		break;
	case 0x6:
	case 0x7:
		boot_dev = MMC_BOOT;
		break;
	case 0x8 ... 0xf:
		boot_dev = NAND_BOOT;
		break;
	default:
		boot_dev = UNKNOWN_BOOT;
		break;
	}
}

enum boot_device get_boot_device(void)
{
	return boot_dev;
}

u32 get_board_rev(void)
{
	return fsl_system_rev;
}

#ifdef CONFIG_ARCH_MMU
void board_mmu_init(void)
{
	unsigned long ttb_base = PHYS_SDRAM_1 + 0x4000;
	unsigned long i;

	/*
	* Set the TTB register
	*/
	asm volatile ("mcr  p15,0,%0,c2,c0,0" : : "r"(ttb_base) /*:*/);

	/*
	* Set the Domain Access Control Register
	*/
	i = ARM_ACCESS_DACR_DEFAULT;
	asm volatile ("mcr  p15,0,%0,c3,c0,0" : : "r"(i) /*:*/);

	/*
	* First clear all TT entries - ie Set them to Faulting
	*/
	memset((void *)ttb_base, 0, ARM_FIRST_LEVEL_PAGE_TABLE_SIZE);
	/* Actual   Virtual  Size   Attributes          Function */
	/* Base     Base     MB     cached? buffered?  access permissions */
	/* xxx00000 xxx00000 */
	X_ARM_MMU_SECTION(0x000, 0x000, 0x001,
			ARM_UNCACHEABLE, ARM_UNBUFFERABLE,
			ARM_ACCESS_PERM_RW_RW); /* ROM, 1M */
	X_ARM_MMU_SECTION(0x001, 0x001, 0x008,
			ARM_UNCACHEABLE, ARM_UNBUFFERABLE,
			ARM_ACCESS_PERM_RW_RW); /* 8M */
	X_ARM_MMU_SECTION(0x009, 0x009, 0x001,
			ARM_UNCACHEABLE, ARM_UNBUFFERABLE,
			ARM_ACCESS_PERM_RW_RW); /* IRAM */
	X_ARM_MMU_SECTION(0x00A, 0x00A, 0x0F6,
			ARM_UNCACHEABLE, ARM_UNBUFFERABLE,
			ARM_ACCESS_PERM_RW_RW); /* 246M */
	/*
	 * Phys		Virtual		Size		Property
	 * ----------	----------	--------	----------
	 * 0x10000000	0x10000000	256M		cacheable
	 * 0x80000000	0x20000000	16M		uncacheable
	 * 0x81000000	0x21000000	240M		cacheable
	 */
	/* Reserve the first 256MB of bank 1 as cacheable memory */
	X_ARM_MMU_SECTION(0x100, 0x100, 0x100,
			ARM_CACHEABLE, ARM_BUFFERABLE,
			ARM_ACCESS_PERM_RW_RW);

	/* Reserve the first 16MB of bank 2 uncachable memory*/
	X_ARM_MMU_SECTION(0x800, 0x200, 0x010,
			ARM_UNCACHEABLE, ARM_UNBUFFERABLE,
			ARM_ACCESS_PERM_RW_RW);

	/* Reserve the remaining 240MB of bank 2 as cacheable memory */
	X_ARM_MMU_SECTION(0x810, 0x210, 0x0F0,
			ARM_CACHEABLE, ARM_BUFFERABLE,
			ARM_ACCESS_PERM_RW_RW);

	/* Enable MMU */
	MMU_ON();
}
#endif

int dram_init(void)
{
	/*
	 * Switch PL301_FAST2 to DDR Dual-channel mapping
	 * however this block the boot up, temperory redraw
	 */
	/*
	 * u32 reg = 1;
	 * writel(reg, GPV0_BASE_ADDR);
	 */

	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;
#ifdef SDRAM_NO_BANK_2//[
#else //][!SDRAM_NO_BANK_2
	gd->bd->bi_dram[1].start = PHYS_SDRAM_2;
	gd->bd->bi_dram[1].size = PHYS_SDRAM_2_SIZE;
#endif //] SDRAM_NO_BANK_2

	return 0;
}

static void setup_uart(void)
{
	/* UART1 TXD */
	mxc_iomux_v3_setup_pad(MX6DL_PAD_CSI0_DAT10__UART1_TXD);

	/* UART1 RXD */
	mxc_iomux_v3_setup_pad(MX6DL_PAD_CSI0_DAT11__UART1_RXD);
}

#ifdef CONFIG_NET_MULTI
int board_eth_init(bd_t *bis)
{
	int rc = -ENODEV;

	return rc;
}
#endif

#ifdef CONFIG_CMD_MMC

/* On this board, only SD3 can support 1.8V signalling
 * that is required for UHS-I mode of operation.
 * Last element in struct is used to indicate 1.8V support.
 */
struct fsl_esdhc_cfg usdhc_cfg[4] = {
	{USDHC1_BASE_ADDR, 1, 1, 1, 0},
	{USDHC2_BASE_ADDR, 1, 1, 1, 0},
	{USDHC3_BASE_ADDR, 1, 1, 1, 0},
	{USDHC4_BASE_ADDR, 1, 1, 1, 0},
};

#ifdef CONFIG_DYNAMIC_MMC_DEVNO
int get_mmc_env_devno(void)
{
#if 0
	uint soc_sbmr = readl(SRC_BASE_ADDR + 0x4);

	if (SD_BOOT == boot_dev || MMC_BOOT == boot_dev) {
		/* BOOT_CFG2[3] and BOOT_CFG2[4] */
		return (soc_sbmr & 0x00001800) >> 11;
	} else
		return -1;
#else
	return 0;	// always return 0 for internal SD.
#endif
}
#endif
iomux_v3_cfg_t usdhc1_4bits_pads[] = {
	MX6DL_PAD_SD1_CLK__USDHC1_CLK,
	MX6DL_PAD_SD1_CMD__USDHC1_CMD,
	MX6DL_PAD_SD1_DAT0__USDHC1_DAT0,
	MX6DL_PAD_SD1_DAT1__USDHC1_DAT1,
	MX6DL_PAD_SD1_DAT2__USDHC1_DAT2,
	MX6DL_PAD_SD1_DAT3__USDHC1_DAT3,
};

iomux_v3_cfg_t usdhc2_pads[] = {
	MX6DL_PAD_SD2_CLK__USDHC2_CLK,
	MX6DL_PAD_SD2_CMD__USDHC2_CMD,
	MX6DL_PAD_SD2_DAT0__USDHC2_DAT0,
	MX6DL_PAD_SD2_DAT1__USDHC2_DAT1,
	MX6DL_PAD_SD2_DAT2__USDHC2_DAT2,
	MX6DL_PAD_SD2_DAT3__USDHC2_DAT3,
};

iomux_v3_cfg_t usdhc3_pads[] = {
	MX6DL_PAD_SD3_CLK__USDHC3_CLK,
	MX6DL_PAD_SD3_CMD__USDHC3_CMD,
	MX6DL_PAD_SD3_DAT0__USDHC3_DAT0,
	MX6DL_PAD_SD3_DAT1__USDHC3_DAT1,
	MX6DL_PAD_SD3_DAT2__USDHC3_DAT2,
	MX6DL_PAD_SD3_DAT3__USDHC3_DAT3,
	MX6DL_PAD_SD3_DAT4__USDHC3_DAT4,
	MX6DL_PAD_SD3_DAT5__USDHC3_DAT5,
	MX6DL_PAD_SD3_DAT6__USDHC3_DAT6,
	MX6DL_PAD_SD3_DAT7__USDHC3_DAT7,
};

iomux_v3_cfg_t usdhc4_pads[] = {
	MX6DL_PAD_SD4_CLK__USDHC4_CLK,
	MX6DL_PAD_SD4_CMD__USDHC4_CMD,
	MX6DL_PAD_SD4_DAT0__USDHC4_DAT0,
	MX6DL_PAD_SD4_DAT1__USDHC4_DAT1,
	MX6DL_PAD_SD4_DAT2__USDHC4_DAT2,
	MX6DL_PAD_SD4_DAT3__USDHC4_DAT3,
/*
	MX6DL_PAD_SD4_DAT4__USDHC4_DAT4,
	MX6DL_PAD_SD4_DAT5__USDHC4_DAT5,
	MX6DL_PAD_SD4_DAT6__USDHC4_DAT6,
	MX6DL_PAD_SD4_DAT7__USDHC4_DAT7,
*/
};

int usdhc_gpio_init(bd_t *bis)
{
	s32 status = 0;
	u32 index = 0;

	int iISD_IDX=_get_boot_sd_number();

	if(!gptNtxHwCfg) {

		switch (iISD_IDX) {
		case 0:
			mxc_iomux_v3_setup_multiple_pads(usdhc1_4bits_pads,
				sizeof(usdhc1_4bits_pads) /
				sizeof(usdhc1_4bits_pads[0]));
			break;
		case 1:
			mxc_iomux_v3_setup_multiple_pads(usdhc2_pads,
				sizeof(usdhc2_pads) /
				sizeof(usdhc2_pads[0]));
			break;
		case 2:
			mxc_iomux_v3_setup_multiple_pads(usdhc3_pads,
				sizeof(usdhc3_pads) /
				sizeof(usdhc3_pads[0]));
		case 3:
			mxc_iomux_v3_setup_multiple_pads(usdhc4_pads,
				sizeof(usdhc4_pads) /
				sizeof(usdhc4_pads[0]));
			break;
		default:
			break;
		}

		printf("%s(%d):ISD @ SD%d is mmc0\n",__FUNCTION__,__LINE__,iISD_IDX+1);
		status |= fsl_esdhc_initialize(bis, &usdhc_cfg[iISD_IDX]);

		//printf("loading ISD hwconfig @ %s()\n",__FUNCTION__);
		_load_isd_hwconfig();

	}


			// SD1 is ESD .
			// SD2 is GPIO .
			// SD3 is EMMC .
			// SD4 is WIFI SDIO .
			if (2==iISD_IDX) {
				// boot from EMMC@SD4 || EMMC@SD1 ...
				//
				// SD2 config as mmcblk1 ...
				mxc_iomux_v3_setup_multiple_pads(usdhc1_4bits_pads,
					ARRAY_SIZE(usdhc1_4bits_pads));
				status |= fsl_esdhc_initialize(bis, &usdhc_cfg[0]);
			}
			else {
				// boot from ESD ...
				printf("HW switch boot from ESD\n");

				if(NTXHWCFG_TST_FLAG(gptNtxHwCfg->m_val.bPCB_Flags2,0)) {
					printf("eMMC@SD1\n");
					// eMMC@SD1 config as mmcblk1 .
					mxc_iomux_v3_setup_multiple_pads(usdhc1_4bits_pads,
						ARRAY_SIZE(usdhc1_4bits_pads));
					status |= fsl_esdhc_initialize(bis, &usdhc_cfg[0]);
				}
				else {
					printf("eMMC@SD3\n");
					// eMMC@SD3 config as mmcblk1 .
					mxc_iomux_v3_setup_multiple_pads(usdhc3_pads,
						ARRAY_SIZE(usdhc3_pads));
					status |= fsl_esdhc_initialize(bis, &usdhc_cfg[2]);
				}
			}

		mxc_iomux_v3_setup_multiple_pads(usdhc4_pads,
			ARRAY_SIZE(usdhc4_pads));
		status |= fsl_esdhc_initialize(bis, &usdhc_cfg[3]);


	//_led_G(0);
	//printf("%s() end\n",__FUNCTION__);
	return status;
}

int board_mmc_init(bd_t *bis)
{
	if (!usdhc_gpio_init(bis))
		return 0;
	else
		return -1;
}

#ifdef CONFIG_MXC_EPDC //[
#define NTX_EPDC_FB
#ifdef CONFIG_SPLASH_SCREEN//[
int setup_splash_img(void)
{
	//int iTest=0;
#ifdef NTX_EPDC_FB //[
	if(!gpbWaveform) {
		printf("%s() skip because of uboot waveform not ready \n",__FUNCTION__);
		return -1;
	}


	if(gpbLogo) 
	//if(0==iTest)
	{
#if 0
		EPDFB_DC *pDC,*pLogoDC ;
		EPDFB_DC_RET tRet;

		printf("%s(): logo 0x%x to framebuffer 0x%x\n",__FUNCTION__,
				gpbLogo,CONFIG_FB_BASE);
		pDC = epdfbdc_create_ex2(panel_info.vl_col,panel_info.vl_row,\
						panel_info.vl_col,panel_info.vl_row,8,
						CONFIG_FB_BASE,EPDFB_DC_FLAG_DEFAUT);
	
		pLogoDC = epdfbdc_create_ex2(panel_info.vl_col,panel_info.vl_row,\
						panel_info.vl_col,panel_info.vl_row,4,
						gpbLogo,EPDFB_DC_FLAG_DEFAUT);


		printf("%s(): drawing logo ...",__FUNCTION__);
		tRet = epdfbdc_put_dcimg(pDC,pLogoDC,EPDFB_R_0,0,0,panel_info.vl_col,panel_info.vl_row,0,0);
		printf("ok\n",__FUNCTION__);

		tRet = epdfbdc_delete(pDC);
		tRet = epdfbdc_delete(pLogoDC);
#else
		int x,y;
		unsigned char *pbFB = CONFIG_FB_BASE;
		unsigned char *pbImg = gpbLogo;
		printf("drawing logo ...\n");
		for(y=0;y<panel_info.vl_row;y++) {
			for(x=0;x<panel_info.vl_col;x++) {
				if(x&1) {
					*pbFB++=(unsigned char)((*pbImg)&0xf0);
					++pbImg;
				}
				else {
					*pbFB++=(unsigned char)(((*pbImg)<<4)&0xf0);
				}
			}
		}
		printf("logo ok\n");
#endif

		// disable pass logo to kernel ...
		gtNtxHiddenMem_logo2.iIsEnabled = 0;
		gtNtxHiddenMem_logo.iIsEnabled = 0;
	}
	else 
	{
		int i;
		int w,h;
		unsigned char *pbFB = CONFIG_FB_BASE;

		w=panel_info.vl_col;
		h=panel_info.vl_row;
		//w=800;
		//h=600;
		printf("Draw black border around framebuffer ...\n");
		/* Draw black border around framebuffer*/
		memset(pbFB, 0xFF, w * h);
		memset(pbFB, 0x0, 24 * w);
		for (i = 24; i < (h - 24); i++) {
			memset((u8 *)pbFB + i * w, 0x00, 24);
			memset((u8 *)pbFB + i * w
				+ w - 24, 0x00, 24);
		}
		memset((u8 *)pbFB + w * (h - 24),
			0x00, 24 * w);
		printf("Draw black border around framebuffer ...ok\n");
	}

	return 0;

#else //][!NTX_EPDC_FB
	#ifdef CONFIG_SPLASH_IS_IN_MMC//[
	int mmc_dev = get_mmc_env_devno();
	ulong offset = CONFIG_SPLASH_IMG_OFFSET;
	ulong size = CONFIG_SPLASH_IMG_SIZE;
	ulong addr = 0;
	char *s = NULL;
	struct mmc *mmc = find_mmc_device(mmc_dev);
	uint blk_start, blk_cnt, n;

	s = getenv("splashimage");

	if (NULL == s) {
		puts("env splashimage not found!\n");
		return -1;
	}
	addr = simple_strtoul(s, NULL, 16);

	if (!mmc) {
		printf("MMC Device %d not found\n", mmc_dev);
		return -1;
	}

	if (mmc_init(mmc)) {
		puts("MMC init failed\n");
		return -1;
	}

	blk_start = ALIGN(offset, mmc->read_bl_len) / mmc->read_bl_len;
	blk_cnt = ALIGN(size, mmc->read_bl_len) / mmc->read_bl_len;
	n = mmc->block_dev.block_read(mmc_dev, blk_start,
				      blk_cnt, (u_char *) addr);
	flush_cache((ulong) addr, blk_cnt * mmc->read_bl_len);

	return (n == blk_cnt) ? 0 : -1;
	#else //][!CONFIG_SPLASH_IS_IN_MMC
	return 0;
	#endif//]CONFIG_SPLASH_IS_IN_MMC
#endif//]NTX_EPDC_FB
}


#else //][!CONFIG_SPLASH_SCREEN
int setup_splash_img(void) {return 0;}
#endif//]CONFIG_SPLASH_SCREEN

#ifdef NTX_EPDC_FB//[

vidinfo_t E60SC8_panel_info = {
	.vl_refresh = 85,
	.vl_col = 800,
	.vl_row = 600,
	.vl_pixclock = 30000000,
	.vl_left_margin = 8,
	.vl_right_margin = 164,
	.vl_upper_margin = 4,
	.vl_lower_margin = 18,
	.vl_hsync = 4,
	.vl_vsync = 1,
	.vl_sync = 0,
	.vl_mode = 0,
	.vl_flag = 0,
	.vl_bpix = 3,
	.cmap = 0,
};
struct epdc_timing_params E60SC8_panel_timings = {
	.vscan_holdoff = 4,
	.sdoed_width = 10,
	.sdoed_delay = 20,
	.sdoez_width = 10,
	.sdoez_delay = 20,
	.gdclk_hp_offs = 465,
	.gdsp_offs = 250,
	.gdoe_offs = 0,
	.gdclk_offs = 8,
	.num_ce = 1,
};

vidinfo_t E60XC5_panel_info = {
	.vl_refresh = 85,
	.vl_col = 1024,
	.vl_row = 758,
	.vl_pixclock = 40000000,
	.vl_left_margin = 12,
	.vl_right_margin = 76,
	.vl_upper_margin = 4,
	.vl_lower_margin = 5,
	.vl_hsync = 12,
	.vl_vsync = 2,
	.vl_sync = 0,
	.vl_mode = 0,
	.vl_flag = 0,
	.vl_bpix = 3,
	.cmap = 0,
};
struct epdc_timing_params E60XC5_panel_timings = {
	.vscan_holdoff = 4,
	.sdoed_width = 10,
	.sdoed_delay = 20,
	.sdoez_width = 10,
	.sdoez_delay = 20,
	.gdclk_hp_offs = 524,
	.gdsp_offs = 25,
	.gdoe_offs = 0,
	.gdclk_offs = 19,
	.num_ce = 1,
};

vidinfo_t ED068TG1_panel_info = {
	.vl_refresh = 85,
	.vl_col = 1440,
	.vl_row = 1080,
	.vl_pixclock = 96000000,
	.vl_left_margin = 24,
	.vl_right_margin = 267,
	.vl_upper_margin = 4,
	.vl_lower_margin = 5,
	.vl_hsync = 24,
	.vl_vsync = 2,
	.vl_sync = 0,
	.vl_mode = 0,
	.vl_flag = 0,
	.vl_bpix = 3,
	.cmap = 0,
};
struct epdc_timing_params ED068TG1_panel_timings = {
	.vscan_holdoff = 4,
	.sdoed_width = 10,
	.sdoed_delay = 20,
	.sdoez_width = 10,
	.sdoez_delay = 20,
	.gdclk_hp_offs = 665,
	.gdsp_offs = 718,
	.gdoe_offs = 0,
	.gdclk_offs = 199,
	.num_ce = 1,
};

vidinfo_t PENG060D_panel_info = {
	.vl_refresh = 85,
	.vl_col = 1448,
	.vl_row = 1072,
	.vl_pixclock = 80000000,
	.vl_left_margin = 16,
	.vl_right_margin = 102,
	.vl_upper_margin = 4,
	.vl_lower_margin = 4,
	.vl_hsync = 28,
	.vl_vsync = 2,
	.vl_sync = 0,
	.vl_mode = 0,
	.vl_flag = 0,
	.vl_bpix = 3,
	.cmap = 0,
};
struct epdc_timing_params PENG060D_panel_timings = {
	.vscan_holdoff = 4,
	.sdoed_width = 10,
	.sdoed_delay = 20,
	.sdoez_width = 10,
	.sdoez_delay = 20,
	.gdclk_hp_offs = 562,
	.gdsp_offs = 664,
	.gdoe_offs = 0,
	.gdclk_offs = 227,
	.num_ce = 3,
};


vidinfo_t EF133UT1SCE_panel_info = {
	.vl_refresh = 65,
	.vl_col = 1600,
	.vl_row = 1200,
	.vl_pixclock = 72222223,
	.vl_left_margin = 8,
	.vl_right_margin = 97,
	.vl_upper_margin = 4,
	.vl_lower_margin = 7,
	.vl_hsync = 12,
	.vl_vsync = 1,
	.vl_sync = 12,
	.vl_mode = 1,
	.vl_flag = 0,
	.vl_bpix = 3,
	.cmap = 0,
};
struct epdc_timing_params EF133UT1SCE_panel_timings = {
	.vscan_holdoff = 4,
	.sdoed_width = 10,
	.sdoed_delay = 20,
	.sdoez_width = 10,
	.sdoez_delay = 20,
	.gdclk_hp_offs = 743,
	.gdsp_offs = 475,
	.gdoe_offs = 0,
	.gdclk_offs = 15,
	.num_ce = 1,
};



#endif//]CONFIG_MXC_EPDC


vidinfo_t panel_info = {
	.vl_refresh = 85,
	.vl_col = 800,
	.vl_row = 600,
	.vl_pixclock = 26666667,
	.vl_left_margin = 8,
	.vl_right_margin = 100,
	.vl_upper_margin = 4,
	.vl_lower_margin = 8,
	.vl_hsync = 4,
	.vl_vsync = 1,
	.vl_sync = 0,
	.vl_mode = 0,
	.vl_flag = 0,
	.vl_bpix = 3,
	.cmap = 0,
};

struct epdc_timing_params panel_timings = {
	.vscan_holdoff = 4,
	.sdoed_width = 10,
	.sdoed_delay = 20,
	.sdoez_width = 10,
	.sdoez_delay = 20,
	.gdclk_hp_offs = 419,
	.gdsp_offs = 20,
	.gdoe_offs = 0,
	.gdclk_offs = 5,
	.num_ce = 1,
};

static void setup_epdc_power(void)
{
	unsigned int reg;

	/* Setup epdc voltage */

	/* EPDC_PWRSTAT - GPIO2[13] for PWR_GOOD status */
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_PWRSTAT__GPIO_2_13);

	/* EPDC_VCOM0 - GPIO2[3] for VCOM control */
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_VCOM0__GPIO_2_3);

	/* Set as output */
	reg = readl(GPIO2_BASE_ADDR + GPIO_GDIR);
	reg |= (1 << 3);
	writel(reg, GPIO2_BASE_ADDR + GPIO_GDIR);

	/* EPDC_PWRWAKEUP - GPIO2[14] for EPD PMIC WAKEUP */
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_PWRWAKEUP__GPIO_2_14);
	/* Set as output */
	reg = readl(GPIO2_BASE_ADDR + GPIO_GDIR);
	reg |= (1 << 14);
	writel(reg, GPIO2_BASE_ADDR + GPIO_GDIR);

	/* EPDC_PWRCTRL0 - GPIO2[7] for EPD PWR CTL0 */
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_PWRCTRL0__GPIO_2_7);
	/* Set as output */
	reg = readl(GPIO2_BASE_ADDR + GPIO_GDIR);
	reg |= (1 << 7);
	writel(reg, GPIO2_BASE_ADDR + GPIO_GDIR);
}

void epdc_power_on()
{
#ifdef NTX_EPDC_FB //[
	int iCnt;

	EPDPMIC_power_on(1);
	udelay(2000);

	if(8==gptNtxHwCfg->m_val.bDisplayCtrl) {
		FP9928_rail_power_onoff(1);
	}
	else
	if(7==gptNtxHwCfg->m_val.bDisplayCtrl) {
		tps65185_rail_power_onoff(1);
	}
	
	printf("%s : waiting for epd power on ...",__FUNCTION__);
	iCnt = 0;
	while(1) {
		++iCnt;
		if(EPDPMIC_isPowerGood()) {
			break;
		}
		udelay(100);
	}
	printf("ok. cnt=%d\n",iCnt);
	udelay(2000);
	EPDPMIC_vcom_onoff(1);
	udelay(2000);
#else//][!NTX_EPDC_FB
	unsigned int reg;

	/* Set EPD_PWR_CTL0 to high - enable EINK_VDD (3.15) */
	reg = readl(GPIO2_BASE_ADDR + GPIO_DR);
	reg |= (1 << 7);
	writel(reg, GPIO2_BASE_ADDR + GPIO_DR);

	/* Set PMIC Wakeup to high - enable Display power */
	reg = readl(GPIO2_BASE_ADDR + GPIO_DR);
	reg |= (1 << 14);
	writel(reg, GPIO2_BASE_ADDR + GPIO_DR);

	/* Wait for PWRGOOD == 1 */
	while (1) {
		reg = readl(GPIO2_BASE_ADDR + GPIO_DR);
		if (!(reg & (1 << 13)))
			break;

		udelay(100);
	}

	/* Enable VCOM */
	reg = readl(GPIO2_BASE_ADDR + GPIO_DR);
	reg |= (1 << 3);
	writel(reg, GPIO2_BASE_ADDR + GPIO_DR);

	reg = readl(GPIO2_BASE_ADDR + GPIO_DR);

	udelay(500);
#endif//]NTX_EPDC_FB
}

void epdc_power_off()
{
#ifdef NTX_EPDC_FB //[
	EPDPMIC_vcom_onoff(0);
	udelay(100);

	if(8==gptNtxHwCfg->m_val.bDisplayCtrl) {
		FP9928_rail_power_onoff(0);
	}
	else
	if(7==gptNtxHwCfg->m_val.bDisplayCtrl) {
		tps65185_rail_power_onoff(0);
	}
	
	printf("%s : waiting for epd power off ...",__FUNCTION__);
	while(1) {
		if(!EPDPMIC_isPowerGood()) {
			break;
		}
		udelay(100);
	}
	printf("ok\n");
	udelay(100);

	EPDPMIC_power_on(0);

#else //][!NTX_EPDC_FB
	unsigned int reg;
	/* Set PMIC Wakeup to low - disable Display power */
	reg = readl(GPIO2_BASE_ADDR + GPIO_DR);
	reg &= ~(1 << 14);
	writel(reg, GPIO2_BASE_ADDR + GPIO_DR);

	/* Disable VCOM */
	reg = readl(GPIO2_BASE_ADDR + GPIO_DR);
	reg &= ~(1 << 3);
	writel(reg, GPIO2_BASE_ADDR + GPIO_DR);

	/* Set EPD_PWR_CTL0 to low - disable EINK_VDD (3.15) */
	reg = readl(GPIO2_BASE_ADDR + GPIO_DR);
	reg &= ~(1 << 7);
	writel(reg, GPIO2_BASE_ADDR + GPIO_DR);
#endif //]NTX_EPDC_FB
}

int setup_waveform_file(int *O_piBufPixFmt)
{
#ifdef NTX_EPDC_FB //[

	//setup_splash_img();

	if(gpbWaveform) {
		printf("%s() : waveform is ready @ 0x%p size=%d!!\n",
				__FUNCTION__,gpbWaveform,(int)gdwWaveformSize);
		if(O_piBufPixFmt) {
			#ifdef ADVANCE_WAVEFORM_FILE//[
			struct mxcfb_waveform_data_file *wv_file =\
				(struct mxcfb_waveform_data_file *)gpbWaveform;
			#if 1
			if ((wv_file->wdh.luts & 0xC) == 0x4) 
			{
				*O_piBufPixFmt = EPDC_FORMAT_BUF_PIXEL_FORMAT_P5N;
			}
			else 
			#endif
			{
				*O_piBufPixFmt = EPDC_FORMAT_BUF_PIXEL_FORMAT_P4N;
			}
			#else//][!ADVANCE_WAVEFORM_FILE
			if(5==gptNtxHwCfg->m_val.bDisplayResolution) 
			{
				*O_piBufPixFmt = EPDC_FORMAT_BUF_PIXEL_FORMAT_P5N;
			}
			else 
			{
				*O_piBufPixFmt = EPDC_FORMAT_BUF_PIXEL_FORMAT_P4N;
			}
			#endif //]ADVANCE_WAVEFORM_FILE
		}
		return 0;
	}
	else {
		printf("%s() : waveform not ready !!\n",__FUNCTION__);
		return -1;
	}
#else //][!NTX_EPDC_FB
#ifdef CONFIG_WAVEFORM_FILE_IN_MMC
	int mmc_dev = get_mmc_env_devno();
	ulong offset = CONFIG_WAVEFORM_FILE_OFFSET;
	ulong size = CONFIG_WAVEFORM_FILE_SIZE;
	ulong addr = CONFIG_WAVEFORM_BUF_ADDR;
	char *s = NULL;
	struct mmc *mmc = find_mmc_device(mmc_dev);
	uint blk_start, blk_cnt, n;

	if (!mmc) {
		printf("MMC Device %d not found\n", mmc_dev);
		return -1;
	}

	if (mmc_init(mmc)) {
		puts("MMC init failed\n");
		return -1;
	}

	blk_start = ALIGN(offset, mmc->read_bl_len) / mmc->read_bl_len;
	blk_cnt = ALIGN(size, mmc->read_bl_len) / mmc->read_bl_len;
	n = mmc->block_dev.block_read(mmc_dev, blk_start,
				      blk_cnt, (u_char *) addr);
	flush_cache((ulong) addr, blk_cnt * mmc->read_bl_len);

	return (n == blk_cnt) ? 0 : -1;
#else
	return -1;
#endif
#endif //]NTX_EPDC_FB

}


static void setup_epdc()
{
	unsigned int reg;

#ifdef ADVANCE_WAVEFORM_FILE//[
	struct mxcfb_waveform_data_file *wv_file;
	int wv_data_offs;
#endif //]ADVANCE_WAVEFORM_FILE	

	/* epdc iomux settings */
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_D0__EPDC_SDDO_0);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_D1__EPDC_SDDO_1);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_D2__EPDC_SDDO_2);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_D3__EPDC_SDDO_3);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_D4__EPDC_SDDO_4);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_D5__EPDC_SDDO_5);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_D6__EPDC_SDDO_6);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_D7__EPDC_SDDO_7);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_D8__EPDC_SDDO_8);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_D9__EPDC_SDDO_9);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_D10__EPDC_SDDO_10);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_D11__EPDC_SDDO_11);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_D12__EPDC_SDDO_12);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_D13__EPDC_SDDO_13);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_D14__EPDC_SDDO_14);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_D15__EPDC_SDDO_15);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_GDCLK__EPDC_GDCLK);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_GDSP__EPDC_GDSP);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_GDOE__EPDC_GDOE);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_GDRL__EPDC_GDRL);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_SDCLK__EPDC_SDCLK);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_SDOE__EPDC_SDOE);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_SDLE__EPDC_SDLE);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_SDSHR__EPDC_SDSHR);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_BDR0__EPDC_BDR_0);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_SDCE0__EPDC_SDCE_0);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_SDCE1__EPDC_SDCE_1);
	//mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_SDCE2__EPDC_SDCE_2);

	/*** epdc Maxim PMIC settings ***/

	/* EPDC PWRSTAT - GPIO2[13] for PWR_GOOD status */
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_PWRSTAT__GPIO_2_13);

	/* EPDC VCOM0 - GPIO2[3] for VCOM control */
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_VCOM0__GPIO_2_3);

	/* UART4 TXD - GPIO2[14] for EPD PMIC WAKEUP */
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_PWRWAKEUP__GPIO_2_14);

	/* EIM_A18 - GPIO2[7] for EPD PWR CTL0 */
	mxc_iomux_v3_setup_pad(MX6SL_PAD_EPDC_PWRCTRL0__GPIO_2_7);

	/*** Set pixel clock rates for EPDC ***/

	/* EPDC AXI clk from PFD_400M, set to 396/2 = 198MHz */
	reg = readl(CCM_BASE_ADDR + CLKCTL_CHSCCDR);
	reg &= ~0x3F000;
	reg |= (0x4 << 15) | (1 << 12);
	writel(reg, CCM_BASE_ADDR + CLKCTL_CHSCCDR);

	/* EPDC AXI clk enable */
	reg = readl(CCM_BASE_ADDR + CLKCTL_CCGR3);
	reg |= 0x0030;
	writel(reg, CCM_BASE_ADDR + CLKCTL_CCGR3);

	/* EPDC PIX clk from PFD_540M, set to 540/4/5 = 27MHz */
	reg = readl(CCM_BASE_ADDR + CLKCTL_CSCDR2);
	reg &= ~0x03F000;
	reg |= (0x5 << 15) | (4 << 12);
	writel(reg, CCM_BASE_ADDR + CLKCTL_CSCDR2);

	reg = readl(CCM_BASE_ADDR + CLKCTL_CBCMR);
	reg &= ~0x03800000;
	reg |= (0x3 << 23);
	writel(reg, CCM_BASE_ADDR + CLKCTL_CBCMR);

	/* EPDC PIX clk enable */
	reg = readl(CCM_BASE_ADDR + CLKCTL_CCGR3);
	reg |= 0x0C00;
	writel(reg, CCM_BASE_ADDR + CLKCTL_CCGR3);

#if 0
	printf("force 800x600\n");
	gptNtxHwCfg->m_val.bDisplayResolution=0;
#endif

	if(1==gptNtxHwCfg->m_val.bDisplayResolution) {
		// 1024x758 .
		printf("%s() 1024x758\n",__FUNCTION__);
		memcpy(&panel_info,&E60XC5_panel_info,sizeof(panel_info));
		memcpy(&panel_timings,&E60XC5_panel_timings,sizeof(panel_timings));
	}
	else if(3==gptNtxHwCfg->m_val.bDisplayResolution) {
		// 1440x1080 .
		printf("%s() 1440x1080\n",__FUNCTION__);
		memcpy(&panel_info,&ED068TG1_panel_info,sizeof(panel_info));
		memcpy(&panel_timings,&ED068TG1_panel_timings,sizeof(panel_timings));
	}
	else if(5==gptNtxHwCfg->m_val.bDisplayResolution) {
		// 1448x1072 .
		printf("%s() 1448x1072\n",__FUNCTION__);
		memcpy(&panel_info,&PENG060D_panel_info,sizeof(panel_info));
		memcpy(&panel_timings,&PENG060D_panel_timings,sizeof(panel_timings));
	}
	else if(6==gptNtxHwCfg->m_val.bDisplayResolution) {
		// 1600x1200 .
		printf("%s() 1600x1200\n",__FUNCTION__);
		memcpy(&panel_info,&EF133UT1SCE_panel_info,sizeof(panel_info));
		memcpy(&panel_timings,&EF133UT1SCE_panel_timings,sizeof(panel_timings));
	}
	else {
		// 800x600 .
		printf("%s() 800x600\n",__FUNCTION__);
		memcpy(&panel_info,&E60SC8_panel_info,sizeof(panel_info));
		memcpy(&panel_timings,&E60SC8_panel_timings,sizeof(panel_timings));
	}
	
	panel_info.epdc_data.working_buf_addr = CONFIG_WORKING_BUF_ADDR;

#ifdef ADVANCE_WAVEFORM_FILE//[
	// advance waveform file with header and temperature table .
	wv_file = CONFIG_WAVEFORM_BUF_ADDR;
	wv_data_offs = sizeof(wv_file->wdh) + (wv_file->wdh.trc + 1) + 1;
	panel_info.epdc_data.waveform_buf_addr = (unsigned char *)wv_file + wv_data_offs;
	printf("%s() wf buf addr=%p,skip header %d bytes\n",
			__FUNCTION__,panel_info.epdc_data.waveform_buf_addr,wv_data_offs);
#else//][!ADVANCE_WAVEFORM_FILE
	panel_info.epdc_data.waveform_buf_addr = CONFIG_WAVEFORM_BUF_ADDR;
	printf("%s() wf buf addr=%p\n",
			__FUNCTION__,panel_info.epdc_data.waveform_buf_addr);
#endif//] ADVANCE_WAVEFORM_FILE

	panel_info.epdc_data.wv_modes.mode_init = 0;
	panel_info.epdc_data.wv_modes.mode_du = 1;
	panel_info.epdc_data.wv_modes.mode_gc4 = 3;
	panel_info.epdc_data.wv_modes.mode_gc8 = 2;
	panel_info.epdc_data.wv_modes.mode_gc16 = 2;
	panel_info.epdc_data.wv_modes.mode_gc32 = 2;

	panel_info.epdc_data.epdc_timings = panel_timings;

	setup_epdc_power();

	/* Assign fb_base */
	gd->fb_base = CONFIG_FB_BASE;
}
#endif

/* For DDR mode operation, provide target delay parameter for each SD port.
 * Use cfg->esdhc_base to distinguish the SD port #. The delay for each port
 * is dependent on signal layout for that particular port.  If the following
 * CONFIG is not defined, then the default target delay value will be used.
 */
#ifdef CONFIG_GET_DDR_TARGET_DELAY
u32 get_ddr_delay(struct fsl_esdhc_cfg *cfg)
{
	/* No delay required on NTX board SD ports */
	return 0;
}
#endif
#endif

#ifdef CONFIG_IMX_ECSPI
s32 spi_get_cfg(struct imx_spi_dev_t *dev)
{
	if(NTXHWCFG_TST_FLAG(gptNtxHwCfg->m_val.bPCB_Flags2,2)) {
		// eMMC@SD2 , IOs@SPI
		return 0;
	}

	switch (dev->slave.cs) {
	case 0:
		/* SPI-NOR */
		dev->base = ECSPI1_BASE_ADDR;
		dev->freq = 25000000;
		dev->ss_pol = IMX_SPI_ACTIVE_LOW;
		dev->ss = 0;
		dev->fifo_sz = 64 * 4;
		dev->us_delay = 0;
		break;
	default:
		printf("Invalid Bus ID!\n");
		break;
	}

	return 0;
}

void spi_io_init(struct imx_spi_dev_t *dev)
{
	u32 reg;

	switch (dev->base) {
	case ECSPI1_BASE_ADDR:
#if 0
		/* Enable clock */
		reg = readl(CCM_BASE_ADDR + CLKCTL_CCGR1);
		reg |= 0x3;
		writel(reg, CCM_BASE_ADDR + CLKCTL_CCGR1);
		/* SCLK */
		mxc_iomux_v3_setup_pad(MX6SL_PAD_ECSPI1_SCLK__ECSPI1_SCLK);

		/* MISO */
		mxc_iomux_v3_setup_pad(MX6SL_PAD_ECSPI1_MISO__ECSPI1_MISO);

		/* MOSI */
		mxc_iomux_v3_setup_pad(MX6SL_PAD_ECSPI1_MOSI__ECSPI1_MOSI);

		if (dev->ss == 0)
			mxc_iomux_v3_setup_pad
			    (MX6SL_PAD_ECSPI1_SS0__ECSPI1_SS0);
#endif
		break;
	case ECSPI2_BASE_ADDR:
	case ECSPI3_BASE_ADDR:
		/* ecspi2-3 fall through */
		break;
	default:
		break;
	}
}
#endif

#ifdef CONFIG_MXC_FEC
iomux_v3_cfg_t enet_pads[] = {
	/* LAN8720A */
	MX6SL_PAD_FEC_MDIO__FEC_MDIO,
	MX6SL_PAD_FEC_MDC__FEC_MDC,
	MX6SL_PAD_FEC_RXD0__FEC_RDATA_0,
	MX6SL_PAD_FEC_RXD1__FEC_RDATA_1,
	MX6SL_PAD_FEC_CRS_DV__FEC_RX_DV,
	MX6SL_PAD_FEC_TXD0__FEC_TDATA_0,
	MX6SL_PAD_FEC_TXD1__FEC_TDATA_1,
	MX6SL_PAD_FEC_TX_EN__FEC_TX_EN,
#ifdef CONFIG_FEC_CLOCK_FROM_ANATOP
	MX6SL_PAD_FEC_REF_CLK__FEC_REF_OUT,	/* clock from anatop */
#else
	MX6SL_PAD_FEC_REF_CLK__GPIO_4_26,	/* clock from OSC */
#endif

	/*
	 * Since FEC_RX_ER is not connected with PHY(LAN8720A), we need
	 * either configure FEC_RX_ER PAD to other mode than FEC_RX_ER,
	 * or configure FEC_RX_ER PAD to FEC_RX_ER but need pull it down,
	 * otherwise, FEC MAC will report CRC error always. We configure
	 * FEC_RX_ER PAD to GPIO mode here.
	 */

	MX6SL_PAD_FEC_RX_ER__GPIO_4_19,
	MX6SL_PAD_FEC_TX_CLK__GPIO_4_21,	/* Phy power enable */
};

void enet_board_init(void)
{
	unsigned int reg;
	mxc_iomux_v3_setup_multiple_pads(enet_pads, ARRAY_SIZE(enet_pads));

	/*set GPIO4_26 input as FEC clock */
	reg = readl(GPIO4_BASE_ADDR + 0x04);
	reg &= ~(1 << 26);
	writel(reg, GPIO4_BASE_ADDR + 0x4);

	/* phy power enable and reset: gpio4_21 */
	/* DR: High Level on: Power ON */
	reg = readl(GPIO4_BASE_ADDR + 0x0);
	reg |= (1 << 21);
	writel(reg, GPIO4_BASE_ADDR + 0x0);

	/* DIR: output */
	reg = readl(GPIO4_BASE_ADDR + 0x4);
	reg |= (1 << 21);
	writel(reg, GPIO4_BASE_ADDR + 0x4);

	/* wait RC ms for hw reset */
	udelay(500);
}

#define ANATOP_PLL_LOCK                 0x80000000
#define ANATOP_PLL_PWDN_MASK            0x00001000
#define ANATOP_PLL_BYPASS_MASK          0x00010000
#define ANATOP_FEC_PLL_ENABLE_MASK      0x00002000

static int setup_fec(void)
{
	u32 reg = 0;
	s32 timeout = 100000;

	/* get enet tx reference clk from internal clock from anatop
	 * GPR1[14] = 0, GPR1[18:17] = 00
	 */
	reg = readl(IOMUXC_BASE_ADDR + 0x4);
	reg &= ~(0x3 << 17);
	reg &= ~(0x1 << 14);
	writel(reg, IOMUXC_BASE_ADDR + 0x4);

#ifdef CONFIG_FEC_CLOCK_FROM_ANATOP
	/* Enable PLLs */
	reg = readl(ANATOP_BASE_ADDR + 0xe0);	/* ENET PLL */
	if ((reg & ANATOP_PLL_PWDN_MASK) || (!(reg & ANATOP_PLL_LOCK))) {
		reg &= ~ANATOP_PLL_PWDN_MASK;
		writel(reg, ANATOP_BASE_ADDR + 0xe0);
		while (timeout--) {
			if (readl(ANATOP_BASE_ADDR + 0xe0) & ANATOP_PLL_LOCK)
				break;
		}
		if (timeout <= 0)
			return -1;
	}

	/* Enable FEC clock */
	reg |= ANATOP_FEC_PLL_ENABLE_MASK;
	reg &= ~ANATOP_PLL_BYPASS_MASK;
	writel(reg, ANATOP_BASE_ADDR + 0xe0);
#endif
	return 0;
}
#endif

#ifdef CONFIG_I2C_MXC //[
#define I2C1_SDA_GPIO5_26_BIT_MASK  (1 << 26)
#define I2C1_SCL_GPIO5_27_BIT_MASK  (1 << 27)
#define I2C2_SCL_GPIO4_12_BIT_MASK  (1 << 12)
#define I2C2_SDA_GPIO4_13_BIT_MASK  (1 << 13)
#define I2C3_SCL_GPIO1_3_BIT_MASK   (1 << 3)
#define I2C3_SDA_GPIO1_6_BIT_MASK   (1 << 6)

static void setup_i2c(unsigned int module_base)
{
	unsigned int reg;

	switch (module_base) {
	case I2C1_BASE_ADDR:
		/* i2c1 SDA */
		mxc_iomux_v3_setup_pad(MX6DL_PAD_CSI0_DAT8__I2C1_SDA);
		/* i2c1 SCL */
		mxc_iomux_v3_setup_pad(MX6DL_PAD_CSI0_DAT9__I2C1_SCL);

		/* Enable i2c clock */
		reg = readl(CCM_BASE_ADDR + CLKCTL_CCGR2);
		reg |= 0xC0;
		writel(reg, CCM_BASE_ADDR + CLKCTL_CCGR2);

		break;
	case I2C2_BASE_ADDR:
		/* i2c2 SDA */
		mxc_iomux_v3_setup_pad(MX6DL_PAD_KEY_ROW3__I2C2_SDA);

		/* i2c2 SCL */
		mxc_iomux_v3_setup_pad(MX6DL_PAD_KEY_COL3__I2C2_SCL);

		/* Enable i2c clock */
		reg = readl(CCM_BASE_ADDR + CLKCTL_CCGR2);
		reg |= 0x300;
		writel(reg, CCM_BASE_ADDR + CLKCTL_CCGR2);

		break;
	case I2C3_BASE_ADDR:
		/* GPIO_3 for I2C3_SCL */
		mxc_iomux_v3_setup_pad(MX6DL_PAD_GPIO_3__I2C3_SCL);
		/* GPIO_6 for I2C3_SDA */
		mxc_iomux_v3_setup_pad(MX6DL_PAD_GPIO_6__I2C3_SDA);
		/* Enable i2c clock */
		reg = readl(CCM_BASE_ADDR + CLKCTL_CCGR2);
		reg |= 0xC00;
		writel(reg, CCM_BASE_ADDR + CLKCTL_CCGR2);

		break;
	default:
		printf("Invalid I2C base: 0x%x\n", module_base);
		break;
	}
}

#ifdef CONFIG_I2C_MULTI_BUS//[

unsigned int i2c_get_bus_num(void) 
{
	return gdwBusNum;
}

int i2c_set_bus_num(unsigned int dwBusNum)
{
	if(dwBusNum<sizeof(gtI2C_platform_dataA)/sizeof(gtI2C_platform_dataA[0])) {
		
		gdwBusNum=dwBusNum;

		if(0==gtI2C_platform_dataA[dwBusNum].iIsInited) {
			setup_i2c (gtI2C_platform_dataA[dwBusNum].dwPort);
			i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
			gtI2C_platform_dataA[dwBusNum].iIsInited = 1;
		}

		return 0;
	}
	else {
		printf("%s(%d) : parameter error !!\n",__FUNCTION__,(int)dwBusNum);
		return -1;
	}
}

#endif //CONFIG_I2C_MULTI_BUS

/* Note: udelay() is not accurate for i2c timing */
static void __udelay(int time)
{
	int i, j;

	for (i = 0; i < time; i++) {
		for (j = 0; j < 200; j++) {
			asm("nop");
			asm("nop");
		}
	}
}

static void mx6q_i2c_gpio_scl_direction(int bus, int output)
{
	u32 reg;

	switch (bus) {
	case 1:
#if defined CONFIG_MX6Q
		mxc_iomux_v3_setup_pad(MX6Q_PAD_CSI0_DAT9__GPIO_5_27);
#elif defined CONFIG_MX6DL
		mxc_iomux_v3_setup_pad(MX6DL_PAD_CSI0_DAT9__GPIO_5_27);
#endif
		reg = readl(GPIO5_BASE_ADDR + GPIO_GDIR);
		if (output)
			reg |= I2C1_SCL_GPIO5_27_BIT_MASK;
		else
			reg &= ~I2C1_SCL_GPIO5_27_BIT_MASK;
		writel(reg, GPIO5_BASE_ADDR + GPIO_GDIR);
		break;
	case 2:
#if defined CONFIG_MX6Q
		mxc_iomux_v3_setup_pad(MX6Q_PAD_KEY_COL3__GPIO_4_12);
#elif defined CONFIG_MX6DL
		mxc_iomux_v3_setup_pad(MX6DL_PAD_KEY_COL3__GPIO_4_12);
#endif
		reg = readl(GPIO4_BASE_ADDR + GPIO_GDIR);
		if (output)
			reg |= I2C2_SCL_GPIO4_12_BIT_MASK;
		else
			reg &= ~I2C2_SCL_GPIO4_12_BIT_MASK;
		writel(reg, GPIO4_BASE_ADDR + GPIO_GDIR);
		break;
	case 3:
#if defined CONFIG_MX6Q
		mxc_iomux_v3_setup_pad(MX6Q_PAD_GPIO_3__GPIO_1_3);
#elif defined CONFIG_MX6DL
		mxc_iomux_v3_setup_pad(MX6DL_PAD_GPIO_3__GPIO_1_3);
#endif
		reg = readl(GPIO1_BASE_ADDR + GPIO_GDIR);
		if (output)
			reg |= I2C3_SCL_GPIO1_3_BIT_MASK;
		else
			reg &= I2C3_SCL_GPIO1_3_BIT_MASK;
		writel(reg, GPIO1_BASE_ADDR + GPIO_GDIR);
		break;
	}
}

/* set 1 to output, sent 0 to input */
static void mx6q_i2c_gpio_sda_direction(int bus, int output)
{
	u32 reg;

	switch (bus) {
	case 1:
#if defined CONFIG_MX6Q
		mxc_iomux_v3_setup_pad(MX6Q_PAD_CSI0_DAT8__GPIO_5_26);
#elif defined CONFIG_MX6DL
		mxc_iomux_v3_setup_pad(MX6DL_PAD_CSI0_DAT8__GPIO_5_26);
#endif
		reg = readl(GPIO5_BASE_ADDR + GPIO_GDIR);
		if (output)
			reg |= I2C1_SDA_GPIO5_26_BIT_MASK;
		else
			reg &= ~I2C1_SDA_GPIO5_26_BIT_MASK;
		writel(reg, GPIO5_BASE_ADDR + GPIO_GDIR);
		break;
	case 2:
#if defined CONFIG_MX6Q
		mxc_iomux_v3_setup_pad(MX6Q_PAD_KEY_ROW3__GPIO_4_13);
#elif defined CONFIG_MX6DL
		mxc_iomux_v3_setup_pad(MX6DL_PAD_KEY_ROW3__GPIO_4_13);
#endif
		reg = readl(GPIO4_BASE_ADDR + GPIO_GDIR);
		if (output)
			reg |= I2C2_SDA_GPIO4_13_BIT_MASK;
		else
			reg &= ~I2C2_SDA_GPIO4_13_BIT_MASK;
		writel(reg, GPIO4_BASE_ADDR + GPIO_GDIR);
	case 3:
#if defined CONFIG_MX6Q
		mxc_iomux_v3_setup_pad(MX6Q_PAD_GPIO_6__GPIO_1_6);
#elif defined CONFIG_MX6DL
		mxc_iomux_v3_setup_pad(MX6DL_PAD_GPIO_6__GPIO_1_6);
#endif
		reg = readl(GPIO1_BASE_ADDR + GPIO_GDIR);
		if (output)
			reg |= I2C3_SDA_GPIO1_6_BIT_MASK;
		else
			reg &= ~I2C3_SDA_GPIO1_6_BIT_MASK;
		writel(reg, GPIO1_BASE_ADDR + GPIO_GDIR);
	default:
		break;
	}
}

/* set 1 to high 0 to low */
static void mx6q_i2c_gpio_scl_set_level(int bus, int high)
{
	u32 reg;

	switch (bus) {
	case 1:
		reg = readl(GPIO5_BASE_ADDR + GPIO_DR);
		if (high)
			reg |= I2C1_SCL_GPIO5_27_BIT_MASK;
		else
			reg &= ~I2C1_SCL_GPIO5_27_BIT_MASK;
		writel(reg, GPIO5_BASE_ADDR + GPIO_DR);
		break;
	case 2:
		reg = readl(GPIO4_BASE_ADDR + GPIO_DR);
		if (high)
			reg |= I2C2_SCL_GPIO4_12_BIT_MASK;
		else
			reg &= ~I2C2_SCL_GPIO4_12_BIT_MASK;
		writel(reg, GPIO4_BASE_ADDR + GPIO_DR);
		break;
	case 3:
		reg = readl(GPIO1_BASE_ADDR + GPIO_DR);
		if (high)
			reg |= I2C3_SCL_GPIO1_3_BIT_MASK;
		else
			reg &= ~I2C3_SCL_GPIO1_3_BIT_MASK;
		writel(reg, GPIO1_BASE_ADDR + GPIO_DR);
		break;
	}
}

/* set 1 to high 0 to low */
static void mx6q_i2c_gpio_sda_set_level(int bus, int high)
{
	u32 reg;

	switch (bus) {
	case 1:
		reg = readl(GPIO5_BASE_ADDR + GPIO_DR);
		if (high)
			reg |= I2C1_SDA_GPIO5_26_BIT_MASK;
		else
			reg &= ~I2C1_SDA_GPIO5_26_BIT_MASK;
		writel(reg, GPIO5_BASE_ADDR + GPIO_DR);
		break;
	case 2:
		reg = readl(GPIO4_BASE_ADDR + GPIO_DR);
		if (high)
			reg |= I2C2_SDA_GPIO4_13_BIT_MASK;
		else
			reg &= ~I2C2_SDA_GPIO4_13_BIT_MASK;
		writel(reg, GPIO4_BASE_ADDR + GPIO_DR);
		break;
	case 3:
		reg = readl(GPIO1_BASE_ADDR + GPIO_DR);
		if (high)
			reg |= I2C3_SDA_GPIO1_6_BIT_MASK;
		else
			reg &= ~I2C3_SDA_GPIO1_6_BIT_MASK;
		writel(reg, GPIO1_BASE_ADDR + GPIO_DR);
		break;
	}
}

static int mx6q_i2c_gpio_check_sda(int bus)
{
	u32 reg;
	int result = 0;

	switch (bus) {
	case 1:
		reg = readl(GPIO5_BASE_ADDR + GPIO_PSR);
		result = !!(reg & I2C1_SDA_GPIO5_26_BIT_MASK);
		break;
	case 2:
		reg = readl(GPIO4_BASE_ADDR + GPIO_PSR);
		result = !!(reg & I2C2_SDA_GPIO4_13_BIT_MASK);
		break;
	case 3:
		reg = readl(GPIO1_BASE_ADDR + GPIO_PSR);
		result = !!(reg & I2C3_SDA_GPIO1_6_BIT_MASK);
		break;
	}

	return result;
}

 /* Random reboot cause i2c SDA low issue:
  * the i2c bus busy because some device pull down the I2C SDA
  * line. This happens when Host is reading some byte from slave, and
  * then host is reset/reboot. Since in this case, device is
  * controlling i2c SDA line, the only thing host can do this give the
  * clock on SCL and sending NAK, and STOP to finish this
  * transaction.
  *
  * How to fix this issue:
  * detect if the SDA was low on bus send 8 dummy clock, and 1
  * clock + NAK, and STOP to finish i2c transaction the pending
  * transfer.
  */
int i2c_bus_recovery(void)
{
	int i, bus, result = 0;

	for (bus = 1; bus <= 3; bus++) {
		mx6q_i2c_gpio_sda_direction(bus, 0);

		if (mx6q_i2c_gpio_check_sda(bus) == 0) {
			printf("i2c: I2C%d SDA is low, start i2c recovery...\n", bus);
			mx6q_i2c_gpio_scl_direction(bus, 1);
			mx6q_i2c_gpio_scl_set_level(bus, 1);
			__udelay(10000);

			for (i = 0; i < 9; i++) {
				mx6q_i2c_gpio_scl_set_level(bus, 1);
				__udelay(5);
				mx6q_i2c_gpio_scl_set_level(bus, 0);
				__udelay(5);
			}

			/* 9th clock here, the slave should already
			   release the SDA, we can set SDA as high to
			   a NAK.*/
			mx6q_i2c_gpio_sda_direction(bus, 1);
			mx6q_i2c_gpio_sda_set_level(bus, 1);
			__udelay(1); /* Pull up SDA first */
			mx6q_i2c_gpio_scl_set_level(bus, 1);
			__udelay(5); /* plus pervious 1 us */
			mx6q_i2c_gpio_scl_set_level(bus, 0);
			__udelay(5);
			mx6q_i2c_gpio_sda_set_level(bus, 0);
			__udelay(5);
			mx6q_i2c_gpio_scl_set_level(bus, 1);
			__udelay(5);
			/* Here: SCL is high, and SDA from low to high, it's a
			 * stop condition */
			mx6q_i2c_gpio_sda_set_level(bus, 1);
			__udelay(5);

			mx6q_i2c_gpio_sda_direction(bus, 0);
			if (mx6q_i2c_gpio_check_sda(bus) == 1)
				printf("I2C%d Recovery success\n", bus);
			else {
				printf("I2C%d Recovery failed, I2C1 SDA still low!!!\n", bus);
				result |= 1 << bus;
			}
		}

		/* configure back to i2c */
		switch (bus) {
		case 1:
			setup_i2c(I2C1_BASE_ADDR);
			break;
		case 2:
			setup_i2c(I2C2_BASE_ADDR);
			break;
		case 3:
			setup_i2c(I2C3_BASE_ADDR);
			break;
		}
	}


	return result;
}

static int setup_pmic_voltages(void)
{
	unsigned char value, rev_id = 0;
#if CONFIG_MX6_INTER_LDO_BYPASS
	unsigned int val = 0;
#endif

#if 0
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	if (!i2c_probe(0x8)) {
		if (i2c_read(0x8, 0, 1, &value, 1)) {
			printf("Read device ID error!\n");
			return -1;
		}
		if (i2c_read(0x8, 3, 1, &rev_id, 1)) {
			printf("Read Rev ID error!\n");
			return -1;
		}
		printf("Found PFUZE100! deviceid=%x,revid=%x\n", value, rev_id);
#else
	{
#endif

#if CONFIG_MX6_INTER_LDO_BYPASS

#if 0
		/*VDDCORE 1.1V@800Mhz: SW1AB */
		value = 0x20;
		if (i2c_write(0x8, 0x20, 1, &value, 1)) {
			printf("VDDCORE set voltage error!\n");
			return -1;
		}

		/*VDDSOC 1.2V : SW1C*/
		value = 0x24;
		if (i2c_write(0x8, 0x2e, 1, &value, 1)) {
			printf("VDDSOC set voltage error!\n");
			return -1;
		}
#endif

		/* Bypass the VDDSOC from Anatop */
		val = REG_RD(ANATOP_BASE_ADDR, HW_ANADIG_REG_CORE);
		val &= ~BM_ANADIG_REG_CORE_REG2_TRG;
		val |= BF_ANADIG_REG_CORE_REG2_TRG(0x1f);
		REG_WR(ANATOP_BASE_ADDR, HW_ANADIG_REG_CORE, val);

		/* Bypass the VDDCORE from Anatop */
		val = REG_RD(ANATOP_BASE_ADDR, HW_ANADIG_REG_CORE);
		val &= ~BM_ANADIG_REG_CORE_REG0_TRG;
		val |= BF_ANADIG_REG_CORE_REG0_TRG(0x1f);
		REG_WR(ANATOP_BASE_ADDR, HW_ANADIG_REG_CORE, val);

		/* Bypass the VDDPU from Anatop */
		val = REG_RD(ANATOP_BASE_ADDR, HW_ANADIG_REG_CORE);
		val &= ~BM_ANADIG_REG_CORE_REG1_TRG;
		val |= BF_ANADIG_REG_CORE_REG1_TRG(0x1f);
		REG_WR(ANATOP_BASE_ADDR, HW_ANADIG_REG_CORE, val);

		printf("hw_anadig_reg_core=%x\n",
		       REG_RD(ANATOP_BASE_ADDR, HW_ANADIG_REG_CORE));
#endif
	}
	return 0;

}
#endif//]


#ifdef CONFIG_MXC_KPD
int setup_mxc_kpd(void)
{
#if 0
	mxc_iomux_v3_setup_pad(MX6SL_PAD_KEY_COL0__KPP_COL_0);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_KEY_COL1__KPP_COL_1);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_KEY_COL2__KPP_COL_2);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_KEY_COL3__KPP_COL_3);

	mxc_iomux_v3_setup_pad(MX6SL_PAD_KEY_ROW0__KPP_ROW_0);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_KEY_ROW1__KPP_ROW_1);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_KEY_ROW2__KPP_ROW_2);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_KEY_ROW3__KPP_ROW_3);
#endif

	return 0;
}

int check_powerkey_pressed(void)
{
	return _power_key_status();
}

#endif


int board_early_init(void)
{

	ntx_parse_syspart_type();

	//printf("\n\n%s()\n\n",__FUNCTION__);
	EPDPMIC_power_on(0);

#ifdef CONFIG_I2C_MXC
	setup_i2c(CONFIG_SYS_I2C_PORT);
	i2c_bus_recovery();
	RC5T619_disable_NOE();
	setup_pmic_voltages();
#endif

	if(8==gptNtxHwCfg->m_val.bDisplayCtrl) {
		FP9928_rail_power_onoff(0);
		EPDPMIC_vcom_onoff(0);
	}
	else
	if(7==gptNtxHwCfg->m_val.bDisplayCtrl) {
		_init_tps65185_power(0,0);
	}

	{
		gptNtxGpioWifiPwr = &gt_ntx_gpio_WIFI_3V3_ON;
		gptNtxGpioESDIN = &gt_ntx_gpio_esdin;
	}

	//_set_EP_3V3_ON(1);

	_set_ISD_3V3_ON(0);
	_set_ESD_3V3_ON(1);
	_set_WIFI_3V3_ON(0);

	_set_EP_3V3_ON(1);

	EPDPMIC_power_on(1);
	_set_TP_3V3_ON(1);
	_set_TP_RST(1);


	ntx_hw_early_init();

#ifndef CONFIG_MFG
	_load_boot_waveform();
	gpbLogo = NtxHiddenMem_load_ntxbin(&gtNtxHiddenMem_logo2,&gdwLogoSize);
	if(!gpbLogo) {
		NtxHiddenMem_load_ntxbin(&gtNtxHiddenMem_logo,&gdwLogoSize);
	}
#endif

#ifdef CONFIG_MXC_EPDC
	setup_epdc();
#endif

	return 0;
}

void mx6dl_ntx_reset (void)
{
	printf ("ricoh reboot...\n");
	RC5T619_write_reg(0x0f, 0x35);
	RC5T619_write_reg(0x0E, 0x1);
}

int board_init(void)
{
	int iChk;
	mxc_iomux_v3_init((void *)IOMUXC_BASE_ADDR);
	setup_boot_device();
	/* board id for linux */
	gd->bd->bi_arch_number = MACH_TYPE_MX6DL_NTX;

	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;

	setup_uart();

	return 0;
}

#if defined(CONFIG_ARCH_MISC_INIT)
/* miscellaneous arch dependent initialisations */
void arch_misc_init (void)
{
	char cBufA[32];
#ifdef CONFIG_LOADADDR //[
	sprintf(cBufA,"0x%08x",CONFIG_LOADADDR);
	setenv("loadaddr",cBufA);// overwrite loadaddr of boot environment loaded by user .
#endif //]CONFIG_LOADADDR

#ifdef CONFIG_RD_LOADADDR //[
	sprintf(cBufA,"0x%08x",CONFIG_RD_LOADADDR);
	setenv("rd_loadaddr",cBufA); // overwrite rd_loadaddr of boot environment loaded by user .
#endif //] CONFIG_RD_LOADADDR
}
#endif

int board_late_init(void)
{
	ntx_hw_late_init();

	{
		char cBufA[32];
		sprintf(cBufA,"mmc%d",0);
		setenv("fastboot_dev",cBufA);
	}

#ifdef CONFIG_MFG //[
	#if defined(CONFIG_MFG_FASTBOOT) || defined(CONFIG_MFG_ADB) //[
	setenv("bootcmd","booti ${fastboot_dev}");
	#endif//]
#else
	if(2==gptNtxHwCfg->m_val.bUIStyle) 
	{
		setenv("bootcmd","booti ${fastboot_dev}");
	}
	
	run_command("load_ntxbins", 0);//
#endif //]CONFIG_MFG

	ntx_config_fastboot_layout();
	//printf("%s(%d)\n",__FUNCTION__,__LINE__);
	//EPDPMIC_power_on(0);

	if (1 == gptNtxHwCfg->m_val.bPMIC) {
		RC5T619_write_reg(0x0B, 0x03);		// RICOH61x_PWR_WD, disable watchdog.
	}

	return 0;
}

int checkboard(void)
{
	printf("Board: MX6DualLite-NTX:[ ");

	switch (__REG(SRC_BASE_ADDR + 0x8)) {
	case 0x0001:
		printf("POR");
		break;
	case 0x0009:
		printf("RST");
		break;
	case 0x0010:
	case 0x0011:
		printf("WDOG");
		break;
	default:
		printf("unknown");
	}
	printf(" ]\n");

	printf("Boot Device: ");
	switch (get_boot_device()) {
	case WEIM_NOR_BOOT:
		printf("NOR\n");
		break;
	case ONE_NAND_BOOT:
		printf("ONE NAND\n");
		break;
	case I2C_BOOT:
		printf("I2C\n");
		break;
	case SPI_NOR_BOOT:
		printf("SPI NOR\n");
		break;
	case SD_BOOT:
		printf("SD\n");
		break;
	case MMC_BOOT:
		printf("MMC\n");
		break;
	case UNKNOWN_BOOT:
	default:
		printf("UNKNOWN\n");
		break;
	}

#ifdef CONFIG_SECURE_BOOT
	if (check_hab_enable() == 1)
		get_hab_status();
#endif

	return 0;
}

#ifdef CONFIG_ANDROID_RECOVERY
static int get_mmc_no(char *env_str)
{
	int digit = 0;
	unsigned char a;

	if (env_str && (strlen(env_str) >= 4) &&
	    !strncmp(env_str, "mmc", 3)) {
		a = env_str[3];
		if (a >= '0' && a <= '9')
			digit = a - '0';
	}

	return digit;
}

int ntx_check_droid_fastboot(void)
{

	int iPwr_Key;

#ifdef CONFIG_MFG_FASTBOOT //[
	return 1;
#endif
	if(2!=gptNtxHwCfg->m_val.bUIStyle) {
		// Not Android platform .
		// 統一交由 _detect_bootmode 決定。
		return 0;
	}

	iPwr_Key=_power_key_status();


	if (12==gptNtxHwCfg->m_val.bKeyPad) {
		int iUSB_in = ntx_detect_usb_plugin(0);

		printf("Detecting conditions for fastboot ... usb=%d,pwrkey=%d\n",
				iUSB_in,iPwr_Key);

		if( (1==iPwr_Key) && (1==iUSB_in) ) {
			fastboot_connection_timeout_us_set(5*1000*1000);
			fastboot_connection_abort_at_usb_remove_chk_setup(&ntx_is_fastboot_abort_inusbremove);
			return 1;
		}

	}
	else {
		int iFastbootKeyVal = ntx_gpio_key_is_fastboot_down();

		printf("Detecting Magic keys for fastboot ...%d %d\n", iFastbootKeyVal,iPwr_Key);
		if(( iFastbootKeyVal==1) && (iPwr_Key==1) )
		{
			return 1;
		}
	}
	return 0;
}

int ntx_check_droid_recovery(void)
{
	int iPwr_Key;

	if(2!=gptNtxHwCfg->m_val.bUIStyle) {
		// Not Android platform .
		return 0;
	}

	iPwr_Key=_power_key_status();

	if (12==gptNtxHwCfg->m_val.bKeyPad) {
		int iUSB_in = ntx_detect_usb_plugin(0);

		printf("Detecting conditions for recovery ... usb=%d,pwrkey=%d\n",
				iUSB_in,iPwr_Key);

		if( (1==iPwr_Key) && (1==iUSB_in) ) {
			if(0==ntx_wait_powerkey(10,1,1)) {
				return 1;
			}
		}
		
	}
	else {

		int iRecoveryKeyVal = ntxup_wait_key_esdupg();

		printf("Detecting Magic keys for recovery ...%d %d\n",iRecoveryKeyVal,iPwr_Key);
		if((iRecoveryKeyVal==1) && (iPwr_Key!=1)) { //disallow power key
			return 1;
		}
	}

	return 0;
}
int ntx_check_recovery_cmd_file(void)
{
	disk_partition_t info;
	ulong part_length;
	int filelen;
	char *env;

	block_dev_desc_t *dev_desc = NULL;
	struct mmc *mmc = NULL;
	int mmc_no;
	char *fastboot_env = getenv("fastboot_dev");


	if(2!=gptNtxHwCfg->m_val.bUIStyle) {
		// Not Android platform .
		return (-1);
	}


	printf("Checking for recovery command file...\n");
	switch (get_boot_device()) {
	case MMC_BOOT:
	case SD_BOOT:
		mmc_no = get_mmc_no(fastboot_env);
		mmc = find_mmc_device(mmc_no);
		dev_desc = get_dev("mmc", mmc_no);
		if (NULL == dev_desc) {
			printf("** Block device MMC %d not supported\n", mmc_no);
			return (-2);
		}

		mmc_init(mmc);

		if (get_partition_info(dev_desc,
			giNTX_Cache_partNO,
			&info)) 
		{
			printf("** Bad partition %d **\n",
				giNTX_Cache_partNO);
			return (-3);
		}

			
#ifdef CONFIG_CMD_EXT4//[
		{
			ext4_dev_desc = dev_desc;
			if ((part_length = ext4fs_set_blk_dev(dev_desc, giNTX_Cache_partNO)) == 0) {
				printf ("** Bad partition - mmc %d:%d **\n",  mmc_no, giNTX_Cache_partNO);

				return (-9);
			}
			if (init_fs(dev_desc)) {
				return (-10);
			}
			
			if (!ext4fs_mount(part_length)) {
				printf("** mount ext4 partition failed %d:%d **\n",
		      mmc_no,giNTX_Cache_partNO );
				deinit_fs(dev_desc);
				return (-11);
			}

			filelen = ext4fs_open(CONFIG_ANDROID_RECOVERY_CMD_FILE);
		
			ext4fs_close();
			if(dev_desc) {
				deinit_fs(dev_desc);dev_desc=0;
			}
		}
#else //][!CONFIG_CMD_EXT4
		{
			part_length = ext2fs_set_blk_dev(dev_desc,
					giNTX_Cache_partNO);
			if (part_length == 0) {
				printf("** Bad partition - mmc 0:%d **\n",
					giNTX_Cache_partNO);
				ext2fs_close();
				return (-4);
			}

			if (!ext2fs_mount(part_length)) {
				printf("** Bad ext2 partition or "
					"disk - mmc 0:%d **\n",
					giNTX_Cache_partNO);
				ext2fs_close();
				return (-5);
			}

			filelen = ext2fs_open(CONFIG_ANDROID_RECOVERY_CMD_FILE);

			ext2fs_close();
		}
#endif//]CONFIG_CMD_EXT4
		break;
	case NAND_BOOT:
		return (-6);
		break;
	case SPI_NOR_BOOT:
		return (-7);
		break;
	case UNKNOWN_BOOT:
	default:
		return (-8);
		break;
	}

	return (filelen > 0) ? 1 : 0;

}

int check_recovery_cmd_file(void)
{
	char *env;

#ifndef CONFIG_MFG//[
	if(2!=gptNtxHwCfg->m_val.bUIStyle) 
#endif //]CONFIG_MFG
	{
		// not Android .
		return 0;
	}

	/* For test only */
	/* When detecting android_recovery_switch,
	 * enter recovery mode directly */
	env = getenv("android_recovery_switch");
	if (!strcmp(env, "1")) {
		printf("Env recovery detected!\nEnter recovery mode!\n");
		return 1;
	}
	
	if (ntx_check_recovery_cmd_file()>0) {
		return 1;
	}
	if (check_and_clean_recovery_flag()) {
		return 1;
	}
	if (ntx_check_and_increase_boot_count()) {
		return 1;
	}
	return 0;
}

#endif

#ifdef CONFIG_IMX_UDC
void udc_pins_setting(void)
{
#if 0
	/* USB_OTG_PWR */
	mxc_iomux_v3_setup_pad(MX6SL_PAD_KEY_COL4__GPIO_4_0);
	mxc_iomux_v3_setup_pad(MX6SL_PAD_KEY_COL5__GPIO_4_2);
	/* USB_OTG_PWR = 0 */
	gpio_direction_output(USB_OTG_PWR, 0);
	/* USB_H1_POWER = 1 */
	gpio_direction_output(USB_H1_PWR, 1);
#endif
}
#endif

