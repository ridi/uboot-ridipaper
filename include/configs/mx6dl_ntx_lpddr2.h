#ifndef __mx6dl_ntx_lpddr2_h//[
#define __mx6dl_ntx_lpddr2_h

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

#include "mx6dl_ntx_android.h"

#ifndef CONFIG_MX6DL_NTX_LPDDR2//[
	#define CONFIG_MX6DL_NTX_LPDDR2
#endif //]CONFIG_MX6DL_NTX_LPDDR2

#ifndef CONFIG_MT42L128M32D1 //[
	#define CONFIG_MT42L128M32D1
#endif //]CONFIG_MT42L128M32D1

#endif //]__mx6dl_ntx_lpddr2_h

