/*
 * ntx_hw.c : 將所有硬體相關的控制放置於此,由board.c引入.
 *
 *
 *
 */ 

//#define DEBUG_I2C_CHN	1
//
#include <usb/imx_udc.h>


#define PMIC_RC5T619 1
static const NTX_GPIO gt_ntx_gpio_home_key= {
	MX6DL_PAD_SD2_DAT3__GPIO_1_12_KEYIPU,  // pin pad/mux control .
	1, // gpio group .
	12, // gpio number .
	0, // key down value .
	0, // not inited .
	"[HOME]", // name .
	2, // 1:input ; 0:output ; 2:btn .
};

//#define HAVE_FL_KEY
#ifdef HAVE_FL_KEY  //[
static const NTX_GPIO gt_ntx_gpio_fl_key= {
	MX6SL_PAD_KEY_ROW0__GPIO_3_25,  // pin pad/mux control  .
	3, // gpio group .
	25, // gpio number .
	0, // key down value .
	0, // not inited .
	"[FL]", // name .
	2, // 1:input ; 0:output ; 2:btn .
};
#endif //] HAVE_FL_KEY

static const NTX_GPIO gt_ntx_gpio_return_key= {
	MX6DL_PAD_GPIO_2__GPIO_1_2_KEYIPU,  // pin pad/mux control  .
	1, // gpio group .
	2, // gpio number .
	0, // key down value .
	0, // not inited .
	"[RETURN]", // name .
	2, // 1:input ; 0:output ; 2:btn .
};
static const NTX_GPIO gt_ntx_gpio_menu_key= {
	MX6DL_PAD_SD2_CLK__GPIO_1_10_KEYIPU,  // pin pad/mux control  .
	1, // gpio group .
	10, // gpio number .
	0, // key down value .
	0, // not inited .
	"[MENU]", // name .
	2, // 1:input ; 0:output ; 2:btn .
};

static const NTX_GPIO gt_ntx_gpio_hallsensor_key= {
	MX6DL_PAD_KEY_COL0__GPIO_4_6_KEYIPU,  // pin pad/mux control .
	4, // gpio group .
	6, // gpio number .
	0, // key down value .
	0, // not inited .
	"[HALL]", // name .
	2, // 1:input ; 0:output ; 2:btn .
};

static const NTX_GPIO gt_ntx_gpio_power_key= {
	MX6DL_PAD_KEY_ROW1__GPIO_4_9,  // pin pad/mux control .
	4, // gpio group .
	9, // gpio number .
	0, // key down value .
	0, // not inited .
	"[POWER]", // name .
	2, // 1:input ; 0:output ; 2:btn .
};

static const NTX_GPIO gt_ntx_gpio_ACIN= {
	MX6DL_PAD_SD2_CMD__GPIO_1_11,  // pin pad/mux control .
	1, // gpio group .
	11, // gpio number .
	0, // NC .
	0, // not inited .
	"ACIN", // name .
	1, // 1:input ; 0:output ; 2:btn .
};

static const NTX_GPIO gt_ntx_gpio_esdin= {
	MX6DL_PAD_GPIO_1__GPIO_1_1_INTIPU,  // pin pad/mux control .
	1, // gpio group .
	1, // gpio number .
	0, // NC .
	0, // not inited .
	"ESDIN", // name .
	1, // 1:input ; 0:output ; 2:btn .
};
static const NTX_GPIO gt_ntx_gpio_chg= {
	MX6DL_PAD_KEY_COL1__GPIO_4_8_INTIPU,  // pin pad/mux control .
	4, // gpio group .
	8, // gpio number .
	0, // NC .
	0, // not inited .
	"CHG", // name .
	1, // 1:input ; 0:output ; 2:btn .
};

static const NTX_GPIO gt_ntx_gpio_EPDPMIC_PWRGOOD= {
	MX6DL_PAD_EIM_A17__GPIO_2_21_PUINT,  // pin pad/mux control .
	2, // gpio group .
	21, // gpio number .
	0, // NC .
	0, // not inited .
	"EPDPMIC_PWRGOOD", // name .
	1, // 1:input ; 0:output ; 2:btn .
};
static const NTX_GPIO gt_ntx_gpio_EPDPMIC_VCOM= {
	MX6DL_PAD_EIM_D17__GPIO_3_17,  // pin pad/mux control  .
	3, // gpio group .
	17, // gpio number .
	0, // default output value .
	0, // not inited .
	"EPDPMIC_VCOM", // name .
	0, // 1:input ; 0:output ; 2:btn .
};

//#define HAVE_EPDCPMIC_VIN	1
#ifdef HAVE_EPDCPMIC_VIN//[
static const NTX_GPIO gt_ntx_gpio_EPDPMIC_VIN= {
	MX6SL_PAD_EPDC_PWRWAKEUP__GPIO_2_14,  // pin pad/mux control  .
	2, // gpio group .
	14, // gpio number .
	1, // default output value .
	0, // not inited .
	"EPDPMIC_VIN", // name .
	0, // 1:input ; 0:output ; 2:btn .
};
#endif //]HAVE_EPDCPMIC_VIN

static const NTX_GPIO gt_ntx_gpio_65185_WAKEUP= {
	MX6DL_PAD_EIM_A18__GPIO_2_20,  // pin pad/mux control  .
	2, // gpio group .
	20, // gpio number .
	0, // default output value .
	0, // not inited .
	"65185_WAKEUP", // name .
	0, // 1:input ; 0:output ; 2:btn .
};
static const NTX_GPIO gt_ntx_gpio_65185_PWRUP= {
	MX6DL_PAD_EIM_A19__GPIO_2_19,  // pin pad/mux control  .
	2, // gpio group .
	19, // gpio number .
	0, // default output value .
	0, // not inited .
	"65185_PWRUP", // name .
	0, // 1:input ; 0:output ; 2:btn .
};

//#define HAVE_ON_LED		1
#ifdef HAVE_ON_LED //[
static const NTX_GPIO gt_ntx_gpio_ON_LED= {
	MX6SL_PAD_FEC_REF_CLK__GPIO_4_26,  // pin pad/mux control .
	4, // gpio group .
	26, // gpio number .
	0, // default output value .
	0, // not inited .
	"ON_LED", // name .
	0, // 1:input ; 0:output ; 2:btn .
};
#endif //] HAVE_ON_LED

static const NTX_GPIO gt_ntx_gpio_ACTION_LED= {
	MX6DL_PAD_SD2_DAT2__GPIO_1_13,  // pin pad/mux control .
	1, // gpio group .
	13, // gpio number .
	0, // default output value .
	0, // not inited .
	"ACTION_LED", // name .
	0, // 1:input ; 0:output ; 2:btn .
};

//#define HAVE_CHG_LED	1
#ifdef HAVE_CHG_LED //[
static const NTX_GPIO gt_ntx_gpio_Charge_LED= {
	MX6SL_PAD_FEC_TXD1__GPIO_4_16,  // pin pad/mux control .
	4, // gpio group .
	16, // gpio number .
	0, // default output value .
	0, // not inited .
	"Charge_LED", // name .
	0, // 1:input ; 0:output ; 2:btn .
};
#endif //] HAVE_CHG_LED

//#define HAVE_ISD_3V3_ON	1
#ifdef HAVE_ISD_3V3_ON //[
static const NTX_GPIO gt_ntx_gpio_ISD_3V3_ON= {
	MX6SL_PAD_KEY_ROW3__GPIO_3_31,  // pin pad/mux control .
	3, // gpio group .
	31, // gpio number .
	0, // default output value .
	0, // not inited .
	"ISD_3V3", // name .
	0, // 1:input ; 0:output ; 2:btn .
};
#endif //HAVE_ISD_3V3_ON


//#define HAVE_ESD_3V3_ON		1
#ifdef HAVE_ESD_3V3_ON //[
static const NTX_GPIO gt_ntx_gpio_ESD_3V3_ON= {
	MX6SL_PAD_KEY_ROW2__GPIO_3_29,  // pin pad/mux control .
	3, // gpio group .
	29, // gpio number .
	0, // default output value .
	0, // not inited .
	"ESD_3V3", // name .
	0, // 1:input ; 0:output ; 2:btn .
};
#endif //]HAVE_ESD_3V3_ON


//#define HAVE_TP_3V3_ON		1
#ifdef HAVE_TP_3V3_ON //[
static const NTX_GPIO gt_ntx_gpio_TP_3V3_ON= {
	MX6SL_PAD_KEY_ROW4__GPIO_4_1,  // pin pad/mux control .
	4, // gpio group .
	1, // gpio number .
	0, // default output value .
	0, // not inited .
	"TP_3V3", // name .
	0, // 1:input ; 0:output ; 2:btn .
};
#endif //] HAVE_TP_3V3_ON


static const NTX_GPIO gt_ntx_gpio_TP_RST= {
	MX6DL_PAD_SD2_DAT0__GPIO_1_15,  // pin pad/mux control .
	1, // gpio group .
	15, // gpio number .
	0, // default output value .
	0, // not inited .
	"TP_RST", // name .
	0, // 1:input ; 0:output ; 2:btn .
};


static const NTX_GPIO gt_ntx_gpio_WIFI_3V3_ON= {
	MX6DL_PAD_SD4_DAT5__GPIO_2_13,  // pin pad/mux control .
	2, // gpio group .
	13, // gpio number .
	0, // default output value .
	0, // not inited .
	"WIFI_3V3", // name .
	0, // 1:input ; 0:output ; 2:btn .
};

NTX_GPIO * ntx_gpio_keysA[NTX_GPIO_KEYS] = {
//	&gt_ntx_gpio_home_key,
//	&gt_ntx_gpio_fl_key,
//	&gt_ntx_gpio_hallsensor_key,
	0,
};

//int gi_ntx_gpio_keys=sizeof(ntx_gpio_keysA)/sizeof(ntx_gpio_keysA[0]);
int gi_ntx_gpio_keys=0;

NTX_GPIO *gptNtxGpioKey_Home,*gptNtxGpioKey_FL,*gptNtxGpioKey_Power;
NTX_GPIO *gptNtxGpioKey_Menu,*gptNtxGpioKey_Return,*gptNtxGpioSW_HallSensor;
NTX_GPIO *gptNtxGpioKey_Left,*gptNtxGpioKey_Right;
NTX_GPIO *gptNtxGpioKey_earphone_detector,*gptNtxGpioKey_Right;
NTX_GPIO *gptNtxGpioKey_TPSW,*gptNtxGpioKey_PGUP,*gptNtxGpioKey_PGDN;
NTX_GPIO *gptNtxGpioKey_R1,*gptNtxGpioKey_R2,*gptNtxGpioKey_L1,*gptNtxGpioKey_L2;
NTX_GPIO *gptNtxGpioWifiPwr;
NTX_GPIO *gptNtxGpioESDIN;

void _led_R(int iIsTurnON)
{
	if(!gptNtxHwCfg) {
		printf("%s(%d) : cannot work without ntx hwconfig !\n",__FUNCTION__,iIsTurnON);
		return ;
	}

#ifdef HAVE_CHG_LED //[
	ntx_gpio_set_value(&gt_ntx_gpio_Charge_LED,iIsTurnON);
#endif //]HAVE_CHG_LED
}
/*
 * For models without RGB LED, default goes _led_G()
 */
void _led_G(int iIsTurnON)
{

	int iChk;

	//printf("%s(%d)\n",__FUNCTION__,iIsTurnON);
	if(!gptNtxHwCfg) {
		printf("%s(%d) : cannot work without ntx hwconfig !\n",__FUNCTION__,iIsTurnON);
		return ;
	}

	if(6==gptNtxHwCfg->m_val.bLed) {
		// no lights .
	}
	else {
		ntx_gpio_set_value(&gt_ntx_gpio_ACTION_LED,iIsTurnON?0:1);
	}
}

void _led_B(int iIsTurnON)
{
	if(1!=gptNtxHwCfg->m_val.bLed) {
		return ;
	}

#ifdef HAVE_ON_LED //[
	ntx_gpio_set_value(&gt_ntx_gpio_ON_LED,iIsTurnON?0:1);
#endif //]HAVE_ON_LED
}

void _set_ISD_3V3_ON(int value)
{
#ifdef HAVE_ISD_3V3_ON //[
	ntx_gpio_set_value(&gt_ntx_gpio_ISD_3V3_ON, value);
#endif //] HAVE_ISD_3V3_ON
}

void _set_ESD_3V3_ON(int value)
{
#ifdef HAVE_ESD_3V3_ON //[
	ntx_gpio_set_value(&gt_ntx_gpio_ESD_3V3_ON, value);
#endif //]HAVE_ESD_3V3_ON 
}

void _set_TP_3V3_ON(int value)
{

#ifdef HAVE_TP_3V3_ON //[
	ntx_gpio_set_value(&gt_ntx_gpio_TP_3V3_ON, value);
#endif //]HAVE_TP_3V3_ON 
}
void _set_TP_RST(int value)
{
	ntx_gpio_set_value(&gt_ntx_gpio_TP_RST, value);
}


void _set_WIFI_3V3_ON(int value)
{
	if(!gptNtxGpioWifiPwr) {
		printf("%s() : WifiPwr Gpio must be assigned first !\n",__FUNCTION__);
		return ;
	}
	ntx_gpio_set_value((struct NTX_GPIO *)gptNtxGpioWifiPwr, value);
}

void _set_EP_3V3_ON(int value)
{
}


int __get_sd_number(void)
{
	int iBT_PortNum=-1;
	uint soc_sbmr = readl(SRC_BASE_ADDR + 0x4);
	iBT_PortNum=(soc_sbmr>>11)&3;
	return iBT_PortNum;
}

unsigned char ntxhw_get_bootdevice_type(void) 
{
	unsigned char bBootDevType = NTXHW_BOOTDEV_UNKOWN;
	return bBootDevType;
}

int _get_boot_sd_number(void)
{
	static int giBT_PortNum=-1;

	if(-1==giBT_PortNum) {
		//char cCmdA[256];

		giBT_PortNum=__get_sd_number();

		//sprintf(cCmdA,"setenv fastboot_dev mmc%d",giBT_PortNum);
		//run_command(cCmdA, 0);// 

	}
	else {
	}

	return giBT_PortNum;
}

int _get_pcba_id (void)
{
	static int g_pcba_id;

	if(g_pcba_id) {
		return g_pcba_id;
	}

	if(gptNtxHwCfg) {
		switch(gptNtxHwCfg->m_val.bPCB)
		{
			default:
				g_pcba_id = gptNtxHwCfg->m_val.bPCB;
				printf ("[%s-%d] PCBA ID=%d\n",__func__,__LINE__,g_pcba_id);
				break;	
		}
	}
	else {
		printf("%s(): [Warning] No hwconfig !\n",__FUNCTION__);
	}
	return g_pcba_id;
}

int _power_key_status (void)
{
	int iRet;
	iRet=ntx_gpio_key_is_down(gptNtxGpioKey_Power);
	return iRet;
}

int _sd_cd_status (void)
{
	int iRet;
	if(gptNtxGpioESDIN) {
		iRet=ntx_gpio_get_value(gptNtxGpioESDIN);
	}
	else {
		printf("%s():esdin not assinged !\n",__FUNCTION__);
		iRet = 0;
	}
	return iRet?0:1;
}

int _hallsensor_status (void)
{
	if(gptNtxHwCfg&&0!=gptNtxHwCfg->m_val.bHallSensor) {
		// E60Q1X/E60Q0X .
		return ntx_gpio_key_is_down(gptNtxGpioSW_HallSensor);
	}
	else {
		return -1;
	}
	return 0;
}

int ntx_gpio_key_is_fastboot_down(void)
{

	if(	16==gptNtxHwCfg->m_val.bKeyPad || 18==gptNtxHwCfg->m_val.bKeyPad ||
			11==gptNtxHwCfg->m_val.bKeyPad )
	{
		// Keypad type: HOMEPAD/FL_Key .
		return ntx_gpio_key_is_fl_down();
	}
	else {
		if(gptNtxGpioKey_Home) {
			return ntx_gpio_key_is_down(gptNtxGpioKey_Home);
		}
		else if(gptNtxGpioKey_PGDN) {
			return ntx_gpio_key_is_down(gptNtxGpioKey_PGDN);
		}
		else if(gptNtxGpioKey_PGUP) {
			return ntx_gpio_key_is_down(gptNtxGpioKey_PGUP);
		}
		else if(gptNtxGpioKey_R1) {
			return ntx_gpio_key_is_down(gptNtxGpioKey_R1);
		}
		else if(gptNtxGpioKey_L1) {
			return ntx_gpio_key_is_down(gptNtxGpioKey_L1);
		}
		else {
			return 0;
		}
	}
}

int ntx_gpio_key_is_home_down(void)
{
	if(gptNtxGpioKey_Home) {
		return ntx_gpio_key_is_down(gptNtxGpioKey_Home);
	}
	return 0;
}

int ntx_gpio_key_is_pgup_down(void)
{
	if(gptNtxGpioKey_PGUP) {
		return ntx_gpio_key_is_down(gptNtxGpioKey_PGUP);
	}
	return 0;
}
int ntx_gpio_key_is_pgdn_down(void)
{
	if(gptNtxGpioKey_PGDN) {
		return ntx_gpio_key_is_down(gptNtxGpioKey_PGDN);
	}
	return 0;
}


int ntx_gpio_key_is_menu_down(void)
{

	if(gptNtxGpioKey_Menu) {
		return ntx_gpio_key_is_down(gptNtxGpioKey_Menu);
	}
	return 0;
}

int ntx_gpio_key_is_fl_down(void)
{
	if(gptNtxGpioKey_FL) {
		return ntx_gpio_key_is_down(gptNtxGpioKey_FL);
	}
	return 0;
}


static const unsigned char gbMicroPI2C_ChipAddr = 0x43;
static unsigned int guiMicroPI2C_I2C_bus = 2;// I2C3
int msp430_I2C_Chn_set(unsigned int uiI2C_Chn)
{
	switch(uiI2C_Chn) {
	case 0:
	case 1:
	case 2:
		guiMicroPI2C_I2C_bus = uiI2C_Chn;
		break;
	default:
		printf("%s invalid I2C channel #%u!!\n",__FUNCTION__,uiI2C_Chn);
		return -1;
	}
	return 0;
}

int msp430_read_buf(unsigned char bRegAddr,unsigned char *O_pbBuf,int I_iBufSize)
{
	int iRet;
	unsigned int uiCurrI2CBus;

	uiCurrI2CBus = i2c_get_bus_num();
	if(uiCurrI2CBus!=guiMicroPI2C_I2C_bus) {
#ifdef DEBUG_I2C_CHN//[
		printf("%s(): change I2C bus to %d for MSP430\n",
				__FUNCTION__,(int)guiMicroPI2C_I2C_bus);
#endif //]DEBUG_I2C_CHN
		i2c_set_bus_num(guiMicroPI2C_I2C_bus);
	}

	iRet = i2c_read(gbMicroPI2C_ChipAddr, bRegAddr, 1, O_pbBuf, I_iBufSize);

	if(uiCurrI2CBus!=guiMicroPI2C_I2C_bus) {
#ifdef DEBUG_I2C_CHN//[
		printf("%s(): restore I2C bus to %d \n",
				__FUNCTION__,(int)uiCurrI2CBus);
#endif//]DEBUG_I2C_CHN
		i2c_set_bus_num(uiCurrI2CBus);
	}

	return iRet;
}
int msp430_read_reg(unsigned char bRegAddr,unsigned char *O_pbRegVal)
{
	return msp430_read_buf(bRegAddr,O_pbRegVal,1);
}

int msp430_write_buf(unsigned char bRegAddr,unsigned char *I_pbBuf,int I_iBufSize)
{
	int iRet;
	int iChk;
	unsigned int uiCurrI2CBus;

	uiCurrI2CBus = i2c_get_bus_num();
	if(uiCurrI2CBus!=guiMicroPI2C_I2C_bus) {
#ifdef DEBUG_I2C_CHN//[
		printf("%s(): change I2C bus to %d for MSP430\n",
				__FUNCTION__,(int)guiMicroPI2C_I2C_bus);
#endif//]DEBUG_I2C_CHN
		i2c_set_bus_num(guiMicroPI2C_I2C_bus);
	}

	iRet = i2c_write(gbMicroPI2C_ChipAddr,bRegAddr,1,I_pbBuf,I_iBufSize);

	if(uiCurrI2CBus!=guiMicroPI2C_I2C_bus) {
#ifdef DEBUG_I2C_CHN//[
		printf("%s(): restore I2C bus to %d \n",
				__FUNCTION__,(int)uiCurrI2CBus);
#endif //]DEBUG_I2C_CHN
		i2c_set_bus_num(uiCurrI2CBus);
	}

	return iRet;
}
int msp430_write_reg(unsigned char bRegAddr,unsigned char bRegVal)
{
	return msp430_write_buf(bRegAddr,&bRegVal,1);
}


#ifdef PMIC_RC5T619 //[

static const unsigned char gbRicohRC5T619_ChipAddr=0x32;
static const unsigned int guiRicohRC5T619_I2C_bus=1;// I2C2 .

int RC5T619_read_reg(unsigned char bRegAddr,unsigned char *O_pbRegVal)
{
	int iRet = -1;
	int iChk;
	unsigned char bRegVal;
	unsigned int uiCurrI2CBus;

	uiCurrI2CBus = i2c_get_bus_num();
	if(uiCurrI2CBus!=guiRicohRC5T619_I2C_bus) {
#ifdef DEBUG_I2C_CHN//[
		printf("%s(): change I2C bus to %d for RC5T619\n",
				__FUNCTION__,(int)guiRicohRC5T619_I2C_bus);
#endif//] DEBUG_I2C_CHN
		i2c_set_bus_num(guiRicohRC5T619_I2C_bus);
	}

	iChk = i2c_read(gbRicohRC5T619_ChipAddr, bRegAddr, 1, &bRegVal, 1);
	if(0==iChk) {
//		printf("RC5T619 [0x%x]=0x%x\n",bRegAddr,bRegVal);
		if(O_pbRegVal) {
			*O_pbRegVal = bRegVal;
			iRet = 0;
		}
		else {
			iRet = 1;
		}
	}
	else {
		printf("RC5T619 read [0x%x] failed !!\n",bRegAddr);
	}

	if(uiCurrI2CBus!=guiRicohRC5T619_I2C_bus) {
#ifdef DEBUG_I2C_CHN//[
		printf("%s(): restore I2C bus to %d \n",
				__FUNCTION__,(int)uiCurrI2CBus);
#endif //]DEBUG_I2C_CHN
		i2c_set_bus_num(uiCurrI2CBus);
	}

	return iRet;
}
int RC5T619_write_buffer(unsigned char bRegAddr,unsigned char *I_pbRegWrBuf,unsigned short I_wRegWrBufBytes)
{
	int iRet = -1;
	unsigned int uiCurrI2CBus;
	int iChk;

	uiCurrI2CBus = i2c_get_bus_num();
	if(uiCurrI2CBus!=guiRicohRC5T619_I2C_bus) {
#ifdef DEBUG_I2C_CHN//[
		printf("%s(): change I2C bus to %d for RC5T619\n",
				__FUNCTION__,(int)guiRicohRC5T619_I2C_bus);
#endif //]DEBUG_I2C_CHN
		i2c_set_bus_num(guiRicohRC5T619_I2C_bus);
	}

	iChk = i2c_write(gbRicohRC5T619_ChipAddr, bRegAddr, 1, I_pbRegWrBuf, I_wRegWrBufBytes);
	if(0==iChk) {
		iRet=(int)I_wRegWrBufBytes;
	}
	else {
		printf("RC5T619 write to [0x%x] %d bytes failed !!\n",bRegAddr,I_wRegWrBufBytes);
		iRet=iChk ;
	}

	if(uiCurrI2CBus!=guiRicohRC5T619_I2C_bus) {
#ifdef DEBUG_I2C_CHN//[
		printf("%s(): restore I2C bus to %d \n",
				__FUNCTION__,(int)uiCurrI2CBus);
#endif //]DEBUG_I2C_CHN
		i2c_set_bus_num(uiCurrI2CBus);
	}

	return iRet;
}

int RC5T619_write_reg(unsigned char bRegAddr,unsigned char bRegWrVal)
{
	return RC5T619_write_buffer(bRegAddr,&bRegWrVal,1);
}

int RC5T619_set_charger_params(int iChargerType)
{
	unsigned char val;
	if(1!=gptNtxHwCfg->m_val.bPMIC) {
		//printf("%s():skipped ,RC5T619 disabled by hwconfig",__FUNCTION__);
		return -3;
	}

	RC5T619_read_reg (0xBD, &val);
	if (0 == (val & 0xC0))
		return 0;

	switch (iChargerType) {
	default:
	case USB_CHARGER_SDP:
		{
			int retry_cnt = 10;
			unsigned char ilim=8;		// set ILIM 900mA

			if (49==gptNtxHwCfg->m_val.bPCB||69==gptNtxHwCfg->m_val.bPCB)
				ilim = 5;	// set ILIM 600mA for E60QDx, E60QQx
			RC5T619_write_reg (0xB6, ilim);	// REGISET1 , set ILIM_ADP
			RC5T619_write_reg (0xB8, 0x04);	// CHGISET , set ICHG 500mA
			RC5T619_write_reg (0xB7, (0xE0|ilim));	// REGISET2 , set ILIM_USB
			do {
				RC5T619_write_reg (0xB7, (0xE0|ilim));	// REGISET2 , set ILIM_USB
				RC5T619_read_reg (0xB7, &val);
				if (ilim != (val&0x0F))
					printf ("REGISET2 val %02X\n",val);
			} while ((val != (val&0x0F)) && (--retry_cnt));
		}
		break;
	case USB_CHARGER_CDP:
	case USB_CHARGER_DCP:
		//printf ("%s : set 900mA for DCP/CDP \n",__func__);
		RC5T619_write_reg (0xB6, 0x09);	// REGISET1 , set ILIM_ADP 1000mA
		RC5T619_write_reg (0xB7, 0x29);	// REGISET2 , set ILIM_USB 1000mA
		RC5T619_write_reg (0xB8, 0x07);	// CHGISET , set ICHG 800mA
		break;
	}
	return 0;
}

int RC5T619_charger_redetect(void)
{
	int iRet=0;
	int iChk;
	unsigned char bRegAddr,bVal;

	if(1!=gptNtxHwCfg->m_val.bPMIC) {
		//printf("%s():skipped ,RC5T619 disabled by hwconfig",__FUNCTION__);
		return -3;
	}
 
	bRegAddr = 0xDA;
	bVal = 0x01;
	iChk = RC5T619_write_reg(bRegAddr,bVal);
	if(iChk<0) {
		printf("%s():write reg0x%x->0x%x error %d",
				__FUNCTION__,bRegAddr,bVal,iChk);
		return -1;
	}

	return iRet;
}

int RC5T619_disable_NOE(void)
{
	int iRet=0;
	int iChk;

	iChk=RC5T619_write_reg(0x11, 8);	// disable N_OE
	printf("%s():",__FUNCTION__);
	if(iChk<0) {
		printf("failed\n");
		return -1;
	}
	printf(" done\n");
	return iRet;
}

int RC5T619_enable_watchdog(int iIsEnable)
{
	int iRet=0;
	int iChk;
	unsigned char bRegAddr,bVal ;

	if(1!=gptNtxHwCfg->m_val.bPMIC) {
		//printf("%s():skipped ,RC5T619 disabled by hwconfig",__FUNCTION__);
		return -3;
	}

	bRegAddr = 0x0B;
	iChk = RC5T619_read_reg(bRegAddr,&bVal);
	if(iChk<0) {
		printf("%s():read reg0x%x error %d",
				__FUNCTION__,bRegAddr,iChk);
		return -1;
	}

	// RICOH61x_PWR_WD
	if(iIsEnable) {
		bVal &= ~0x0C;
		iChk = RC5T619_write_reg(bRegAddr,bVal);
	}
	else {
		bVal |= 0x0C;
		iChk = RC5T619_write_reg(bRegAddr,bVal);
	}

	if(iChk<0) {
		printf("%s():write reg0x%x error %d",
				__FUNCTION__,bRegAddr,iChk);
		iRet = -2;
	}

	return iRet;
}
#else //][!PMIC_RC5T619
int RC5T619_read_reg(unsigned char bRegAddr,unsigned char *O_pbRegVal){return -1;}
int RC5T619_write_reg(unsigned char bRegAddr,unsigned char bRegWrVal){return -1;}
int RC5T619_write_buffer(unsigned char bRegAddr,unsigned char *I_pbRegWrBuf,unsigned short I_wRegWrBufBytes){return -1;}
int RC5T619_enable_watchdog(int iIsEnable){return -1;}
int RC5T619_charger_redetect(void){return -1;}
#endif //] PMIC_RC5T619


int ntx_detect_usb_plugin(int iIsDetectChargerType) 
{
	int iRet=0;
	if(1==gptNtxHwCfg->m_val.bPMIC) {
#ifdef PMIC_RC5T619 //[
		unsigned char bReg,bRegAddr;
		int iChk;
		int iTryCnt=0;
		const int iTryMax=50;


		// Ricoh RC5T619 .
		bRegAddr=0xbd;
		iChk = RC5T619_read_reg(bRegAddr,&bReg);
		if(iChk>=0) {
			if(bReg&0xc0) 
			{
				iRet = USB_CHARGER_UNKOWN;

				if(iIsDetectChargerType) 
				{
					RC5T619_charger_redetect();
					printf("%s():Charge detecting ...",__FUNCTION__);
					do {
						if(++iTryCnt>=iTryMax) {
							printf("retry(%d) timed out\n",iTryCnt);
							break;
						}

						iChk = RC5T619_read_reg(0xDA,&bReg);	// GCHGDET_REG

						if(0x8!=(bReg&0xc)) {
							// Detecting not completely .
							udelay(10*1000);
							printf(".");
							continue ;
						}

						if (bReg & 0x30) {
							if((bReg & 0x30)==0x01) {
								printf ("set 900mA for CDP (%d)\n",iTryCnt);
								RC5T619_set_charger_params(USB_CHARGER_CDP);
								iRet = USB_CHARGER_CDP;
							}
							else {
								printf ("set 900mA for DCP (%d)\n",iTryCnt);
								RC5T619_set_charger_params(USB_CHARGER_DCP);
								iRet = USB_CHARGER_DCP;
							}
						}
						else {
							printf ("set 500mA for SDP (0x%02x)(%d)\n",bReg,iTryCnt);
							RC5T619_set_charger_params(USB_CHARGER_SDP);
							iRet = USB_CHARGER_SDP;
						}
						break;
					}while(1);

				}// Detecting Charger type ...
			}
		}
#endif //]PMIC_RC5T619
	}
	else {
		iRet = ntx_gpio_get_value(&gt_ntx_gpio_ACIN)?0:1;
	}

#if 0
		// detect the usb id pin .
		if(!ntx_gpio_get_value(&gt_ntx_gpio_USBID)) {
			iRet |= USB_CHARGER_OTG;
		}
#endif

	return iRet;
}


int init_pwr_i2c_function(int iSetAsFunc)
{
	return 0;
}

void EPDPMIC_power_on(int iIsPowerON)
{
#ifdef HAVE_EPDCPMIC_VIN//[
	if(iIsPowerON) {
		ntx_gpio_set_value(&gt_ntx_gpio_EPDPMIC_VIN,1);
	}
	else {
		ntx_gpio_set_value(&gt_ntx_gpio_EPDPMIC_VIN,0);
	}
#endif //] HAVE_EPDPMIC_VIN
}
void EPDPMIC_vcom_onoff(int iIsON)
{
	if(iIsON) {
		ntx_gpio_set_value(&gt_ntx_gpio_EPDPMIC_VCOM,1);
	}
	else {
		ntx_gpio_set_value(&gt_ntx_gpio_EPDPMIC_VCOM,0);
	}
}

int EPDPMIC_isPowerGood(void)
{
	int iRet;
	iRet = ntx_gpio_get_value(&gt_ntx_gpio_EPDPMIC_PWRGOOD)?1:0;
	//printf("%s() = %d\n",__FUNCTION__,iRet);
	return iRet;
}
 
void FP9928_rail_power_onoff(int iIsON)
{
}

void _init_tps65185_power(int iIsWakeup,int iIsActivePwr)
{

	udelay(200);

	ntx_gpio_set_value(&gt_ntx_gpio_65185_WAKEUP,iIsWakeup);
	ntx_gpio_set_value(&gt_ntx_gpio_65185_PWRUP,iIsActivePwr);
	if(iIsActivePwr) {
		udelay(2*1000);
	}

}

void tps65185_rail_power_onoff(int iIsON)
{
	if(iIsON) {
		if(1!=ntx_gpio_get_current_value(&gt_ntx_gpio_65185_WAKEUP)) {
			printf("%s() TPS65185 wakeup\n",__FUNCTION__);
			ntx_gpio_set_value(&gt_ntx_gpio_65185_WAKEUP,1);
			udelay(2000);
		}
		if(1!=ntx_gpio_get_current_value(&gt_ntx_gpio_65185_PWRUP)) {
			printf("%s() TPS65185 Enable RAIL POWER\n",__FUNCTION__);
			ntx_gpio_set_value(&gt_ntx_gpio_65185_PWRUP,1);
			udelay(2000);
		}
	}
	else {
		if(0!=ntx_gpio_get_current_value(&gt_ntx_gpio_65185_PWRUP)) {
			printf("%s() TPS65185 Disable RAIL POWER\n",__FUNCTION__);
			ntx_gpio_set_value(&gt_ntx_gpio_65185_PWRUP,0);
			udelay(2000);
		}
		if(0!=ntx_gpio_get_current_value(&gt_ntx_gpio_65185_WAKEUP)) {
			printf("%s() TPS65185 sleep\n",__FUNCTION__);
			ntx_gpio_set_value(&gt_ntx_gpio_65185_WAKEUP,0);
			udelay(2000);
		}
	}
}

/**********************************************
 *
 * ntx_hw_early_init() 
 *
 * the initial actions in early time .
 * 
 **********************************************/
void ntx_hw_early_init(void)
{
	//EPDPMIC_power_on(0);
	
	udc_init();

	RC5T619_enable_watchdog(0);

	//_led_R(0);
	//_led_G(0);
	//_led_B(0);
}

static void ntx_keys_setup(void)
{
	// assigns power key .
	gptNtxGpioKey_Power = &gt_ntx_gpio_power_key;

	// assigns home key .
	if(11==gptNtxHwCfg->m_val.bKeyPad || 12==gptNtxHwCfg->m_val.bKeyPad || 23==gptNtxHwCfg->m_val.bKeyPad) {
		// KeyPad is FL_Key/NO_Key/TP+PGUP+PGDN .
		gptNtxGpioKey_Home = 0;
	}
	else {
		gptNtxGpioKey_Home = &gt_ntx_gpio_home_key;
	}

	// assigns frontlight key .
	if(12==gptNtxHwCfg->m_val.bKeyPad||14==gptNtxHwCfg->m_val.bKeyPad||
		17==gptNtxHwCfg->m_val.bKeyPad||18==gptNtxHwCfg->m_val.bKeyPad||
		22==gptNtxHwCfg->m_val.bKeyPad||23==gptNtxHwCfg->m_val.bKeyPad) 
	{
		// Keypad is NO_Key || RETURN+HOME+MENU || HOMEPAD || HOME_Key || LEFT+RIGHT+HOME+MENU || TP+PGUP+PGDN
		gptNtxGpioKey_FL = 0;
	}
	else {
		// E60Q1X/E60Q0X .
#ifdef HAVE_FL_KEY  //[
		gptNtxGpioKey_FL = &gt_ntx_gpio_fl_key;
#endif //]HAVE_FL_KEY
	}


	if(0!=gptNtxHwCfg->m_val.bHallSensor) {
		gptNtxGpioSW_HallSensor=&gt_ntx_gpio_hallsensor_key;
	}
	else {
		gptNtxGpioSW_HallSensor=0;
	}

	if(17==gptNtxHwCfg->m_val.bKeyPad) 
	{
		// keypad is 'RETURN+HOME+MENU'
		gptNtxGpioKey_Menu = &gt_ntx_gpio_menu_key;
		gptNtxGpioKey_Return = &gt_ntx_gpio_return_key ;
	}

	if(NTXHWCFG_TST_FLAG(gptNtxHwCfg->m_val.bPCB_Flags,6)) 
	{
		// Headphone detector ON .
		//gptNtxGpioKey_earphone_detector = &gt_ntx_gpio_earphone_detector_sw;
	}



	ntx_gpio_keysA[gi_ntx_gpio_keys] = gptNtxGpioKey_Power;
	gi_ntx_gpio_keys++;
	if(12==gptNtxHwCfg->m_val.bKeyPad) {
		// NO_Key .
	}
	else 
	{


		if(gptNtxGpioKey_Home && 
				(13==gptNtxHwCfg->m_val.bKeyPad||
				 14==gptNtxHwCfg->m_val.bKeyPad||
				 16==gptNtxHwCfg->m_val.bKeyPad||
				 17==gptNtxHwCfg->m_val.bKeyPad||
				 18==gptNtxHwCfg->m_val.bKeyPad||
				 22==gptNtxHwCfg->m_val.bKeyPad||
				 24==gptNtxHwCfg->m_val.bKeyPad))
		{
			// FL+HOME/HOME_Key/FL+HOMEPAD/RETURN+HOME+MENU/HOMEPAD/LEFT+RIGHT+HOME+MENU
			// L1+L2+R1+R2+HOME
			ntx_gpio_keysA[gi_ntx_gpio_keys] = gptNtxGpioKey_Home;
			gi_ntx_gpio_keys++;
		}

		if(gptNtxGpioKey_FL && 
			(11==gptNtxHwCfg->m_val.bKeyPad||
			13==gptNtxHwCfg->m_val.bKeyPad||
			16==gptNtxHwCfg->m_val.bKeyPad)) 
		{
			// FL_Key/FL+HOME/FL+HOMEPAD
			ntx_gpio_keysA[gi_ntx_gpio_keys] = gptNtxGpioKey_FL;
			gi_ntx_gpio_keys++;
		}

		if(gptNtxGpioKey_Menu) 
		{
			// RETURN+HOME+MENU
			ntx_gpio_keysA[gi_ntx_gpio_keys] = gptNtxGpioKey_Menu;
			gi_ntx_gpio_keys++;
		}

		if(gptNtxGpioKey_Return) 
		{
			// RETURN+HOME+MENU
			ntx_gpio_keysA[gi_ntx_gpio_keys] = gptNtxGpioKey_Return;
			gi_ntx_gpio_keys++;
		}

		if(gptNtxGpioKey_Left) 
		{
			// 
			ntx_gpio_keysA[gi_ntx_gpio_keys] = gptNtxGpioKey_Left;
			gi_ntx_gpio_keys++;
		}

		if(gptNtxGpioKey_Right) 
		{
			// 
			ntx_gpio_keysA[gi_ntx_gpio_keys] = gptNtxGpioKey_Right;
			gi_ntx_gpio_keys++;
		}

		if(gptNtxGpioKey_earphone_detector) 
		{
			// 
			ntx_gpio_keysA[gi_ntx_gpio_keys] = gptNtxGpioKey_earphone_detector;
			gi_ntx_gpio_keys++;
		}

		if(gptNtxGpioKey_TPSW) 
		{
			// 
			ntx_gpio_keysA[gi_ntx_gpio_keys] = gptNtxGpioKey_TPSW;
			gi_ntx_gpio_keys++;
		}

		if(gptNtxGpioKey_PGUP) 
		{
			// 
			ntx_gpio_keysA[gi_ntx_gpio_keys] = gptNtxGpioKey_PGUP;
			gi_ntx_gpio_keys++;
		}

		if(gptNtxGpioKey_PGDN)
		{
			// 
			ntx_gpio_keysA[gi_ntx_gpio_keys] = gptNtxGpioKey_PGDN;
			gi_ntx_gpio_keys++;
		}

		if(gptNtxGpioKey_L1)
		{
			ntx_gpio_keysA[gi_ntx_gpio_keys] = gptNtxGpioKey_L1;
			gi_ntx_gpio_keys++;
		}
		if(gptNtxGpioKey_L2)
		{
			ntx_gpio_keysA[gi_ntx_gpio_keys] = gptNtxGpioKey_L2;
			gi_ntx_gpio_keys++;
		}
		if(gptNtxGpioKey_R1)
		{
			ntx_gpio_keysA[gi_ntx_gpio_keys] = gptNtxGpioKey_R1;
			gi_ntx_gpio_keys++;
		}
		if(gptNtxGpioKey_R2)
		{
			ntx_gpio_keysA[gi_ntx_gpio_keys] = gptNtxGpioKey_R2;
			gi_ntx_gpio_keys++;
		}

	}


	if(gi_ntx_gpio_keys>=NTX_GPIO_KEYS) {
		printf("\n\n\n%s(%d) memory overwrite !!!\n\n\n",__FILE__,__LINE__);
		udelay(1000*1000);
	}
}

void ntx_hw_late_init(void) 
{
	int i;

	printf("%s()\n",__FUNCTION__);
	if(!gptNtxHwCfg) {
		_load_isd_hwconfig();
	}

	_load_ntx_sn();

	ntx_keys_setup();



	if(9==gptNtxHwCfg->m_val.bCustomer) {
		_led_R(0);
		_led_G(1);
		_led_B(1);
	}
	else {
		_led_R(0);
		_led_G(1);
		_led_B(0);
	}

  

	if(gptNtxGpioESDIN) {
		ntx_gpio_init(gptNtxGpioESDIN);
	}
	else {
		printf("ESDIN gpio not assigned !!\n");
	}


	ntx_gpio_init(&gt_ntx_gpio_chg);

	if(3==gptNtxHwCfg->m_val.bUIConfig) {
		// MFG mode .
		RC5T619_set_charger_params(USB_CHARGER_CDP);
	}
	else {
		RC5T619_set_charger_params(USB_CHARGER_SDP);
	}
}

