/*------------------------------------------------------------------------------
 *
 *	Copyright (C) 2005 Nexell Co., Ltd All Rights Reserved
 *	Nexell Co. Proprietary & Confidential
 *
 *	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 *  AND	WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 *  BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
 *  FOR A PARTICULAR PURPOSE.
 *
 *	Module     : System memory config
 *	Description:
 *	Author     : Platform Team
 *	Export     :
 *	History    :
 *	   2009/05/13 first implementation
 ------------------------------------------------------------------------------*/
#ifndef __CFG_MEM_H__
#define __CFG_MEM_H__

/*------------------------------------------------------------------------------
 * 	Linux dram memory map
 *
 *	|					|
 *	|	Linux Kernel	|
 *	|	Code			|
 *	|					|
 *	|	Linux Kernel	|
 *	|	PTS				|
 *	-------------------------	0x80208000	: Linux PTS and Kernel Code
 *	|	Linux Kernel	|
 *	|	Params			|
 *	-------------------------	0x80200100	: Linux Params
 *	|   suspend mode	|
 *	|	back up data	|
 *	=========================	0x80200000	: Linux Kernel TEXT_BASE
 *  |                   |
 *  |   U-Boot Code     |
 *  |   (512K)          |
 *  -------------------------   0x80180000  : U-Boot TEXT_BASE
 *  | U-Boot Heap (1M)  |                   : U-Boot Heap size = CONFIG_SYS_MALLOC_LEN
 *  |       or          |
 *  | On-Boot Stack     |
 *  |                   |
 *  -------------------------   0x80080000  : U-Boot Stack
 *  | U-Boot Stack      |
 *  |       or          |
 *  |   On-Boot Code    |
 *  -------------------------   0x80010000  : On-Boot TEXT_BASE
 *  |                   |       (64K)
 *  |   On-Boot         |
 *  |   Page table      |
 *  -------------------------   0x80004000  : On-Boot page table (PTS)
 *  |                   |       (16K ~ 32K)
 *  |   Nand ECC        |
 *  |   or vector table |
 *  =========================   0x80000000  : Memory Base
 *
 ------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * 	 On-Boot/U-Boot Loader zone
 */
#define CFG_BLD_VECT_TABLE_BASE			0x80000000	/* 0K ~ 16K */
#define	CFG_BLD_VECT_TABLE_SIZE			0x00004000	/* 16K */

#define	CFG_BLD_PAGE_TABLE_BASE			0x80004000	/* 16K ~ 32K (must be aligned wiht 16k) */
#define	CFG_BLD_PAGE_TABLE_SIZE			0x00008000	/* 16K */
#define	CFG_BLD_BOOT_STACK_TOP			0x80100000	/* Use under the stacp_top 0x5000~0x100000 (704) */

/*------------------------------------------------------------------------------
 * 	 Kernel / Suspend zone
 */
#define	CFG_KERNEL_TEXT_BASE			0x80000000
#define	CFG_SLEEP_DATA_BASE				0x80000000	/* max 256 byte */

/*------------------------------------------------------------------------------
 * 	 System memory map
 */
#define	CFG_MEM_VIR_SYSTEM_BASE			0x80000000	/* System, must be at an evne 2MB boundary (head.S) */
#define	CFG_MEM_PHY_SYSTEM_BASE			0x80000000	/* System, must be at an evne 2MB boundary (head.S) */
#define	CFG_MEM_PHY_SYSTEM_SIZE			0x07000000

/*------------------------------------------------------------------------------
 * 	 Reserved memory map
 */
#define	CFG_MEM_VIR_LINEAR_BASE			0x87000000	/* System / DMA */
#define	CFG_MEM_PHY_LINEAR_BASE			0x87000000	/* System / DMA */
#define	CFG_MEM_PHY_LINEAR_SIZE			0x01000000	/* 16M */

#define	CFG_MEM_VIR_BLOCK_BASE			0xE0000000	/* For video */
#define	CFG_MEM_PHY_BLOCK_BASE			0x86400000	/* For video */
#define	CFG_MEM_PHY_BLOCK_SIZE			0x01C00000	/* 28MB */

#define	CFG_MEM_VIR_GRP3D_BASE			0xE1C00000	/* For 3D (OpenGL|ES) */
#define	CFG_MEM_PHY_GRP3D_BASE			0x86400000	/* For 3D (OpenGL|ES) */
#define	CFG_MEM_PHY_GRP3D_SIZE			0x01C00000	/* 8MB */

#endif /* __CFG_MEM_H__ */
