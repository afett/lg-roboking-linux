/*
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
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
#ifndef __NAND_ECC_H__
#define __NAND_ECC_H__

#define	HW_ECC_MODE		CFG_NAND_ECC_MODE

#if (HW_ECC_MODE == 16)
#error "16bit hw ecc not support, over oob length"
#endif

#if defined(CONFIG_JFFS2_FS) && (CONFIG_MTD_NAND_NEXELL_HWECC)
#error "hw ecc not support jffs2, jffs2 use autoplacement"
#endif

void 	NX_NAND_CreateLookupTable(void);
void 	NX_NAND_SetResetECC(int EccMode);
void 	NX_NAND_WaitForDecoding(void);
void 	NX_NAND_WaitForEncoding(void);
int		NX_NAND_GetErrorStatus(void);
int		NX_NAND_GetErrorLocation(int *pOddSyn, int *pLocation, int *ErrCnt);
void 	NX_NAND_GetGenECC(unsigned int *pECC, int EccMode);
void 	NX_NAND_SetOriECC(unsigned int *pECC, int EccMode);
void 	NX_NAND_GetOddSyndrome(int *pSyndrome);

#endif /* __NAND_ECC_H__ */