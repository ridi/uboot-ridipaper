#ifndef __mx6sl_ntx_lpddr2_h//[
#define __mx6sl_ntx_lpddr2_h


#undef PHYS_SDRAM_1_SIZE 
#define PHYS_SDRAM_1_SIZE	(1024 * 1024 * 1024)

#include "mx6sl_ntx_android.h"

#ifndef CONFIG_MX6SL_NTX_LPDDR2//[
	#define CONFIG_MX6SL_NTX_LPDDR2
#endif //]CONFIG_MX6SL_NTX_LPDDR2

#ifndef CONFIG_MT42L128M32D1 //[
	#define CONFIG_MT42L128M32D1
#endif //]CONFIG_MT42L128M32D1

#define CONFIG_MFG

#endif //]__mx6sl_ntx_lpddr2_h

