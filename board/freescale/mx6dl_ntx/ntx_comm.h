#ifndef _ntx_comm_h//[
#define _ntx_comm_h


#include <asm/arch/mx6.h>
#include <asm/arch/mx6dl_pins.h>
#include <asm/arch/iomux-v3.h>


#ifdef __cplusplus //[
extern C {
#endif //] __cplusplus

extern int giNTX_VFAT_partNO;
extern int giNTX_RootFS_partNO;
extern int giNTX_RecoveryFS_partNO;
extern int giNTX_BootImg_partNO;
extern int giNTX_RecoveryImg_partNO;
extern int giNTX_System_partNO;
extern int giNTX_Data_partNO;
extern int giNTX_Cache_partNO;
extern int giNTX_Vendor_partNO;
extern int giNTX_Misc_partNO;
extern int giNTX_Emergency_partNO;




#ifndef _NTX_GPIO//[
#define _NTX_GPIO

typedef struct tagNTX_GPIO {
	iomux_v3_cfg_t tIOMUXv3_PAD_CFG;
	u32 GPIO_Grp; //  gpio group .
	u32	GPIO_Num; // gpio number .
	int iDefaultValue; // default out value/compare value .
	int iIsInited; // 
	const char *pszName; // 
	int iDirection; // 1:input ; 0:output ; 2:Btn .
	int iCurrentVal; // 1:input ; 0:output ; 2:Btn .
} NTX_GPIO;


#endif //]_NTX_GPIO



int ntx_gpio_init(NTX_GPIO *I_pt_gpio);
int ntx_gpio_set_value(NTX_GPIO *I_pt_gpio,int iOutVal);
int ntx_gpio_get_value(NTX_GPIO *I_pt_gpio);

int ntx_gpio_key_is_down(NTX_GPIO *I_pt_gpio);

unsigned long long ntx_get_timeout_ustick(unsigned long dwUS);
int ntx_is_tick_timeout(unsigned long long u64TimeoutTick);

int ntx_wait_powerkey(int iWaitSecs,int iWaitPwrKeyStateChgTimes,int iChkEnterPwrKeyState);

#ifdef __cplusplus //[
}
#endif //] __cplusplus

#endif//]_ntx_comm_h

