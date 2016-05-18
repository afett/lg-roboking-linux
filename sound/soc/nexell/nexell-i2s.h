/*
 *  Copyright(C) 2011 MOSTiTECH co., ltd.
 *  All right reserved by Segunwoo Kim <ksw@mostitech.com>
 *  
 *  This code partialy based on NEXELL's i2s driver
 *
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __NEXELL_I2S_H
#define __NEXELL_I2S_H

#define	NEXELL_SND_SOC_DAI_NAME		"nx-i2s"

#define NX_I2S_FORMAT_I2S            0
#define NX_I2S_FORMAT_LEFT_JUSTIFY   1
#define NX_I2S_FORMAT_RIGHT_JUSTIFY  2

#define NX_I2S_CLKSRC_0              1
#define NX_I2S_CLKSRC_1              2
#define NX_I2S_CLKDIV_0              1
#define NX_I2S_CLKDIV_1              2
#define NX_I2S_CLKDIV_SYNC_PERIOD    3

#define NX_I2S_PERDIV_32             0
#define NX_I2S_PERDIV_48             1
#define NX_I2S_PERDIV_64             2

#define NX_I2S_CLKSRC0_PLL0          0
#define NX_I2S_CLKSRC0_PLL1          1
#define NX_I2S_CLKSRC1_PLL0          0
#define NX_I2S_CLKSRC1_PLL1          1
#define NX_I2S_CLKSRC1_EXT_BIT       2
#define NX_I2S_CLKSRC1_EXT_INVBIT    3
#define NX_I2S_CLKSRC1_MCLK          4

extern struct snd_soc_dai nx_snd_i2s_dai;

#endif /* __NEXELL_I2S_H */

