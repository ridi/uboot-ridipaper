/*
 * Copyright (C) 2012 Freescale Semiconductor, Inc.
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

#include <config.h>
#include <asm/arch/mx6.h>

#ifdef	CONFIG_FLASH_HEADER
#ifndef CONFIG_FLASH_HEADER_OFFSET
# error "Must define the offset of flash header"
#endif

#define CPU_2_BE_32(l) \
       ((((l) & 0x000000FF) << 24) | \
	(((l) & 0x0000FF00) << 8)  | \
	(((l) & 0x00FF0000) >> 8)  | \
	(((l) & 0xFF000000) >> 24))

#define MXC_DCD_ITEM(i, addr, val)   \
dcd_node_##i:                        \
        .word CPU_2_BE_32(addr) ;     \
        .word CPU_2_BE_32(val)  ;     \

.section ".text.flasheader", "x"
	b	_start
	.org	CONFIG_FLASH_HEADER_OFFSET

ivt_header:       .word 0x402000D1 /* Tag=0xD1, Len=0x0020, Ver=0x40 */
app_code_jump_v:  .word _start
reserv1:          .word 0x0
dcd_ptr:          .word dcd_hdr
boot_data_ptr:	  .word boot_data
self_ptr:         .word ivt_header
app_code_csf:     .word 0x0
reserv2:          .word 0x0

boot_data:        .word TEXT_BASE
image_len:        .word _end_of_copy  - TEXT_BASE + CONFIG_FLASH_HEADER_OFFSET
plugin:           .word 0x0

dcd_hdr:          .word 0x403804D2 /* Tag=0xD2, Len=134*8 + 4 + 4, Ver=0x40 */
write_dcd_cmd:    .word 0x043404CC /* Tag=0xCC, Len=134*8 + 4, Param=0x04 */

# IOMUXC_BASE_ADDR  = 0x20e0000
/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS0_P - (IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS7_P*/
MXC_DCD_ITEM( 1, 0x020E04bc, 0x00003028)
MXC_DCD_ITEM( 2, 0x020E04c0, 0x00003028)
MXC_DCD_ITEM( 3, 0x020E04c4, 0x00003028)
MXC_DCD_ITEM( 4, 0x020E04c8, 0x00003028)
MXC_DCD_ITEM( 5, 0x020E04cc, 0x00003028)
MXC_DCD_ITEM( 6, 0x020E04d0, 0x00003028)
MXC_DCD_ITEM( 7, 0x020E04d4, 0x00003028)
MXC_DCD_ITEM( 8, 0x020E04d8, 0x00003028)
/* IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM0 - IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM7 */
MXC_DCD_ITEM( 9, 0x020E0470, 0x00000028)
MXC_DCD_ITEM(10, 0x020E0474, 0x00000028)
MXC_DCD_ITEM(11, 0x020E0478, 0x00000028)
MXC_DCD_ITEM(12, 0x020E047c, 0x00000028)
MXC_DCD_ITEM(13, 0x020E0480, 0x00000028)
MXC_DCD_ITEM(14, 0x020E0484, 0x00000028)
MXC_DCD_ITEM(15, 0x020E0488, 0x00000028)
MXC_DCD_ITEM(16, 0x020E048c, 0x00000028)
/* IOMUXC_SW_PAD_CTL_PAD_DRAM_CAS_B */
MXC_DCD_ITEM(17, 0x020E0464, 0x00000030)
/* IOMUXC_SW_PAD_CTL_PAD_DRAM_RAS_B */
MXC_DCD_ITEM(18, 0x020E0490, 0x00000030)
/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK0_P */
MXC_DCD_ITEM(19, 0x020E04ac, 0x00000030)
/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK1_P */
MXC_DCD_ITEM(20, 0x020E04b0, 0x00000030)
/* IOMUXC_SW_PAD_CTL_PAD_DRAM_RESET */
MXC_DCD_ITEM(21, 0x020E0494, 0x00000030)
/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCKE0 */
MXC_DCD_ITEM(22, 0x020E04a4, 0x00000030)
/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCKE1 */
MXC_DCD_ITEM(23, 0x020E04a8, 0x00000030)
/* IOMUXC_SW_PAD_CTL_PAD_DRAM_SDBA2 */
MXC_DCD_ITEM(24, 0x020E04a0, 0x00000000)
/* IOMUXC_SW_PAD_CTL_PAD_DRAM_ODT0 */
MXC_DCD_ITEM(25, 0x020E04b4, 0x00000030)
/* IOMUXC_SW_PAD_CTL_PAD_DRAM_ODT1 */
MXC_DCD_ITEM(26, 0x020E04b8, 0x00000030)
/* IOMUXC_SW_PAD_CTL_GRP_B0DS - IOMUXC_SW_PAD_CTL_GRP_B7DS */
MXC_DCD_ITEM(27, 0x020E0764, 0x00000028)
MXC_DCD_ITEM(28, 0x020E0770, 0x00000028)
MXC_DCD_ITEM(29, 0x020E0778, 0x00000028)
MXC_DCD_ITEM(30, 0x020E077c, 0x00000028)
MXC_DCD_ITEM(31, 0x020E0780, 0x00000028)
MXC_DCD_ITEM(32, 0x020E0784, 0x00000028)
MXC_DCD_ITEM(33, 0x020E078c, 0x00000028)
MXC_DCD_ITEM(34, 0x020E0748, 0x00000028)
/* IOMUXC_SW_PAD_CTL_GRP_ADDDS */
MXC_DCD_ITEM(35, 0x020E074c, 0x00000030)
/* IOMUXC_SW_PAD_CTL_GRP_CTLDS */
MXC_DCD_ITEM(36, 0x020E076c, 0x00000030)
/* IOMUXC_SW_PAD_CTL_GRP_DDRMODE_CTL */
MXC_DCD_ITEM(37, 0x020E0750, 0x00020000)
/* IOMUXC_SW_PAD_CTL_GRP_DDRPKE */
MXC_DCD_ITEM(38, 0x020E0754, 0x00000000)
/* IOMUXC_SW_PAD_CTL_GRP_DDRMODE) */
MXC_DCD_ITEM(39, 0x020E0760, 0x00020000)
/* IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE */
MXC_DCD_ITEM(40, 0x020E0774, 0x00080000)

# MMDC_P0_BASE_ADDR = 0x021b0000
# MMDC_P1_BASE_ADDR = 0x021b4000
MXC_DCD_ITEM(41, 0x021b001c, 0x00008000)
MXC_DCD_ITEM(42, 0x021b401c, 0x00008000)
MXC_DCD_ITEM(43, 0x021b085c, 0x1B4700C7)
MXC_DCD_ITEM(44, 0x021b485c, 0x1B4700C7)
MXC_DCD_ITEM(45, 0x021b0800, 0xa1390000)
MXC_DCD_ITEM(46, 0x021b4800, 0xa1390000)
MXC_DCD_ITEM(47, 0x021b0890, 0x00400000)
MXC_DCD_ITEM(48, 0x021b4890, 0x00400000)
MXC_DCD_ITEM(49, 0x021b08b8, 0x00000800)
MXC_DCD_ITEM(50, 0x021b48b8, 0x00000800)
MXC_DCD_ITEM(51, 0x021b081c, 0x33333333)
MXC_DCD_ITEM(52, 0x021b0820, 0x33333333)
MXC_DCD_ITEM(53, 0x021b0824, 0x33333333)
MXC_DCD_ITEM(54, 0x021b0828, 0x33333333)
MXC_DCD_ITEM(55, 0x021b481c, 0x33333333)
MXC_DCD_ITEM(56, 0x021b4820, 0x33333333)
MXC_DCD_ITEM(57, 0x021b4824, 0x33333333)
MXC_DCD_ITEM(58, 0x021b4828, 0x33333333)
MXC_DCD_ITEM(59, 0x021b082c, 0xf3333333)
MXC_DCD_ITEM(60, 0x021b0830, 0xf3333333)
MXC_DCD_ITEM(61, 0x021b0834, 0xf3333333)
MXC_DCD_ITEM(62, 0x021b0838, 0xf3333333)
MXC_DCD_ITEM(63, 0x021b482c, 0xf3333333)
MXC_DCD_ITEM(64, 0x021b4830, 0xf3333333)
MXC_DCD_ITEM(65, 0x021b4834, 0xf3333333)
MXC_DCD_ITEM(66, 0x021b4838, 0xf3333333)
MXC_DCD_ITEM(67, 0x021b0848, 0x3E424642)
MXC_DCD_ITEM(68, 0x021b4848, 0x4242443C)
MXC_DCD_ITEM(69, 0x021b0850, 0x30302A2A)
MXC_DCD_ITEM(70, 0x021b4850, 0x302E3028)
MXC_DCD_ITEM(71, 0x021b083c, 0x20000000)
MXC_DCD_ITEM(72, 0x021b0840, 0x00000000)
MXC_DCD_ITEM(73, 0x021b483c, 0x20000000)
MXC_DCD_ITEM(74, 0x021b4840, 0x00000000)
MXC_DCD_ITEM(75, 0x021b0858, 0x00000a00)
MXC_DCD_ITEM(76, 0x021b4858, 0x00000a00)
MXC_DCD_ITEM(77, 0x021b000c, 0x53574133)
MXC_DCD_ITEM(78, 0x021b0004, 0x00020024)
MXC_DCD_ITEM(79, 0x021b0010, 0x00110A82)
MXC_DCD_ITEM(80, 0x021b0014, 0x00000093)
MXC_DCD_ITEM(81, 0x021b0018, 0x0000174C)
MXC_DCD_ITEM(82, 0x021b002c, 0x0F9F26D2)
MXC_DCD_ITEM(83, 0x021b0030, 0x00000010)
MXC_DCD_ITEM(84, 0x021b0038, 0x00190778)
MXC_DCD_ITEM(85, 0x021b0008, 0x00000000)
MXC_DCD_ITEM(86, 0x021b0040, 0x0000004F)
MXC_DCD_ITEM(87, 0x021b0400, 0x11420000)
MXC_DCD_ITEM(88, 0x021b0404, 0x00011006)
MXC_DCD_ITEM(89, 0x021b0000, 0xC3110000)
MXC_DCD_ITEM(90, 0x021b400c, 0x53574133)
MXC_DCD_ITEM(91, 0x021b4004, 0x00020024)
MXC_DCD_ITEM(92, 0x021b4010, 0x00110A82)
MXC_DCD_ITEM(93, 0x021b4014, 0x00000093)
MXC_DCD_ITEM(94, 0x021b4018, 0x0000174C)
MXC_DCD_ITEM(95, 0x021b402c, 0x0F9F26D2)
MXC_DCD_ITEM(96, 0x021b4030, 0x00000010)
MXC_DCD_ITEM(97, 0x021b4038, 0x00190778)
MXC_DCD_ITEM(98, 0x021b4008, 0x00000000)
MXC_DCD_ITEM(99, 0x021b4040, 0x00000017)
MXC_DCD_ITEM(100, 0x021b4400, 0x11420000)
MXC_DCD_ITEM(101, 0x021b4404, 0x00011006)
MXC_DCD_ITEM(102, 0x021b4000, 0xC3110000)
MXC_DCD_ITEM(103, 0x021b001c, 0x003f8030)
MXC_DCD_ITEM(104, 0x021b001c, 0xff0a8030)
MXC_DCD_ITEM(105, 0x021b001c, 0x82018030)
MXC_DCD_ITEM(106, 0x021b001c, 0x04028030)
MXC_DCD_ITEM(107, 0x021b001c, 0x01038030)
MXC_DCD_ITEM(108, 0x021b001c, 0x003f8038)
MXC_DCD_ITEM(109, 0x021b001c, 0xff0a8038)
MXC_DCD_ITEM(110, 0x021b001c, 0x82018038)
MXC_DCD_ITEM(111, 0x021b001c, 0x04028038)
MXC_DCD_ITEM(112, 0x021b001c, 0x01038038)
MXC_DCD_ITEM(113, 0x021b401c, 0x003f8030)
MXC_DCD_ITEM(114, 0x021b401c, 0xff0a8030)
MXC_DCD_ITEM(115, 0x021b401c, 0x82018030)
MXC_DCD_ITEM(116, 0x021b401c, 0x04028030)
MXC_DCD_ITEM(117, 0x021b401c, 0x01038030)
MXC_DCD_ITEM(118, 0x021b401c, 0x003f8038)
MXC_DCD_ITEM(119, 0x021b401c, 0xff0a8038)
MXC_DCD_ITEM(120, 0x021b401c, 0x82018038)
MXC_DCD_ITEM(121, 0x021b401c, 0x04028038)
MXC_DCD_ITEM(122, 0x021b401c, 0x01038038)
MXC_DCD_ITEM(123, 0x021b0020, 0x00001800)
MXC_DCD_ITEM(124, 0x021b4020, 0x00001800)
MXC_DCD_ITEM(125, 0x021b0818, 0x00000000)
MXC_DCD_ITEM(126, 0x021b4818, 0x00000000)
MXC_DCD_ITEM(127, 0x021b0800, 0xA1390003)
MXC_DCD_ITEM(128, 0x021b4800, 0xA1390003)
MXC_DCD_ITEM(129, 0x021b08b8, 0x00000800)
MXC_DCD_ITEM(130, 0x021b48b8, 0x00000800)
MXC_DCD_ITEM(131, 0x021b0004, 0x00025576)
MXC_DCD_ITEM(132, 0x021b4004, 0x00025576)
MXC_DCD_ITEM(133, 0x021b001c, 0x00000000)
MXC_DCD_ITEM(134, 0x021b401c, 0x00000000)
				  
#endif


