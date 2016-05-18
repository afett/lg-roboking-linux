/*
 * wm8960.h  --  WM8960 Soc Audio driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _WM8960_H
#define _WM8960_H

/* WM8960 register space */


#define WM8960_CACHEREGNUM 	56

#define WM8960_LINVOL		0x0
#define WM8960_RINVOL		0x1
#define WM8960_LOUT1		0x2
#define WM8960_ROUT1		0x3
#define WM8960_CLOCK1		0x4
#define WM8960_DACCTL1		0x5
#define WM8960_DACCTL2		0x6
#define WM8960_IFACE1		0x7
#define WM8960_CLOCK2		0x8
#define WM8960_IFACE2		0x9
#define WM8960_LDAC		0xa
#define WM8960_RDAC		0xb

#define WM8960_RESET		0xf
#define WM8960_3D		0x10
#define WM8960_ALC1		0x11
#define WM8960_ALC2		0x12
#define WM8960_ALC3		0x13
#define WM8960_NOISEG		0x14
#define WM8960_LADC		0x15
#define WM8960_RADC		0x16
#define WM8960_ADDCTL1		0x17
#define WM8960_ADDCTL2		0x18
#define WM8960_POWER1		0x19
#define WM8960_POWER2		0x1a
#define WM8960_ADDCTL3		0x1b
#define WM8960_APOP1		0x1c
#define WM8960_APOP2		0x1d

#define WM8960_LINPATH		0x20
#define WM8960_RINPATH		0x21
#define WM8960_LOUTMIX		0x22

#define WM8960_ROUTMIX		0x25
#define WM8960_MONOMIX1		0x26
#define WM8960_MONOMIX2		0x27
#define WM8960_LOUT2		0x28
#define WM8960_ROUT2		0x29
#define WM8960_MONO		0x2a
#define WM8960_INBMIX1		0x2b
#define WM8960_INBMIX2		0x2c
#define WM8960_BYPASS1		0x2d
#define WM8960_BYPASS2		0x2e
#define WM8960_POWER3		0x2f
#define WM8960_ADDCTL4		0x30
#define WM8960_CLASSD1		0x31

#define WM8960_CLASSD3		0x33
#define WM8960_PLL1		0x34
#define WM8960_PLL2		0x35
#define WM8960_PLL3		0x36
#define WM8960_PLL4		0x37


#define WM8960_0_LINVOL		0x0100
#define WM8960_0_RINVOL		0x0101
#define WM8960_0_LOUT1		0x0102
#define WM8960_0_ROUT1		0x0103
#define WM8960_0_CLOCK1		0x0104
#define WM8960_0_DACCTL1		0x0105
#define WM8960_0_DACCTL2		0x0106
#define WM8960_0_IFACE1		0x0107
#define WM8960_0_CLOCK2		0x0108
#define WM8960_0_IFACE2		0x0109
#define WM8960_0_LDAC		0x010a
#define WM8960_0_RDAC		0x010b

#define WM8960_0_RESET		0x010f
#define WM8960_0_3D		0x0110
#define WM8960_0_ALC1		0x0111
#define WM8960_0_ALC2		0x0112
#define WM8960_0_ALC3		0x0113
#define WM8960_0_NOISEG		0x0114
#define WM8960_0_LADC		0x0115
#define WM8960_0_RADC		0x0116
#define WM8960_0_ADDCTL1		0x0117
#define WM8960_0_ADDCTL2		0x0118
#define WM8960_0_POWER1		0x0119
#define WM8960_0_POWER2		0x011a
#define WM8960_0_ADDCTL3		0x011b
#define WM8960_0_APOP1		0x011c
#define WM8960_0_APOP2		0x011d

#define WM8960_0_LINPATH		0x0120
#define WM8960_0_RINPATH		0x0121
#define WM8960_0_LOUTMIX		0x0122

#define WM8960_0_ROUTMIX		0x0125
#define WM8960_0_MONOMIX1		0x0126
#define WM8960_0_MONOMIX2		0x0127
#define WM8960_0_LOUT2		0x0128
#define WM8960_0_ROUT2		0x0129
#define WM8960_0_MONO		0x012a
#define WM8960_0_INBMIX1		0x012b
#define WM8960_0_INBMIX2		0x012c
#define WM8960_0_BYPASS1		0x012d
#define WM8960_0_BYPASS2		0x012e
#define WM8960_0_POWER3		0x012f
#define WM8960_0_ADDCTL4		0x0130
#define WM8960_0_CLASSD1		0x0131

#define WM8960_0_CLASSD3		0x0133
#define WM8960_0_PLL1		0x0134
#define WM8960_0_PLL2		0x0135
#define WM8960_0_PLL3		0x0136
#define WM8960_0_PLL4		0x0137

#define WM8960_1_LINVOL		0x0200
#define WM8960_1_RINVOL		0x0201
#define WM8960_1_LOUT1		0x0202
#define WM8960_1_ROUT1		0x0203
#define WM8960_1_CLOCK1		0x0204
#define WM8960_1_DACCTL1		0x0205
#define WM8960_1_DACCTL2		0x0206
#define WM8960_1_IFACE1		0x0207
#define WM8960_1_CLOCK2		0x0208
#define WM8960_1_IFACE2		0x0209
#define WM8960_1_LDAC		0x020a
#define WM8960_1_RDAC		0x020b

#define WM8960_1_RESET		0x020f
#define WM8960_1_3D		0x0210
#define WM8960_1_ALC1		0x0211
#define WM8960_1_ALC2		0x0212
#define WM8960_1_ALC3		0x0213
#define WM8960_1_NOISEG		0x0214
#define WM8960_1_LADC		0x0215
#define WM8960_1_RADC		0x0216
#define WM8960_1_ADDCTL1		0x0217
#define WM8960_1_ADDCTL2		0x0218
#define WM8960_1_POWER1		0x0219
#define WM8960_1_POWER2		0x021a
#define WM8960_1_ADDCTL3		0x021b
#define WM8960_1_APOP1		0x021c
#define WM8960_1_APOP2		0x021d

#define WM8960_1_LINPATH		0x0220
#define WM8960_1_RINPATH		0x0221
#define WM8960_1_LOUTMIX		0x0222

#define WM8960_1_ROUTMIX		0x0225
#define WM8960_1_MONOMIX1		0x0226
#define WM8960_1_MONOMIX2		0x0227
#define WM8960_1_LOUT2		0x0228
#define WM8960_1_ROUT2		0x0229
#define WM8960_1_MONO		0x022a
#define WM8960_1_INBMIX1		0x022b
#define WM8960_1_INBMIX2		0x022c
#define WM8960_1_BYPASS1		0x022d
#define WM8960_1_BYPASS2		0x022e
#define WM8960_1_POWER3		0x022f
#define WM8960_1_ADDCTL4		0x0230
#define WM8960_1_CLASSD1		0x0231

#define WM8960_1_CLASSD3		0x0233
#define WM8960_1_PLL1		0x0234
#define WM8960_1_PLL2		0x0235
#define WM8960_1_PLL3		0x0236
#define WM8960_1_PLL4		0x0237

/*
 * WM8960 Clock dividers
 */
#define WM8960_SYSCLKDIV 		0
#define WM8960_DACDIV			1
#define WM8960_ADCDIV			2
#define WM8960_OPCLKDIV			3
#define WM8960_DCLKDIV			4
#define WM8960_TOCLKSEL			5
#define WM8960_SYSCLKSEL		6
#define WM8960_BCLKDIV  		7

#define WM8960_SYSCLK_DIV_1		(0 << 1)
#define WM8960_SYSCLK_DIV_2		(2 << 1)

#define WM8960_SYSCLK_MCLK		(0 << 0)
#define WM8960_SYSCLK_PLL		(1 << 0)

#define WM8960_DAC_DIV_1		(0 << 3)
#define WM8960_DAC_DIV_1_5		(1 << 3)
#define WM8960_DAC_DIV_2		(2 << 3)
#define WM8960_DAC_DIV_3		(3 << 3)
#define WM8960_DAC_DIV_4		(4 << 3)
#define WM8960_DAC_DIV_5_5		(5 << 3)
#define WM8960_DAC_DIV_6		(6 << 3)

#define WM8960_ADC_DIV_1		(0 << 6)
#define WM8960_ADC_DIV_1_5		(1 << 6)
#define WM8960_ADC_DIV_2		(2 << 6)
#define WM8960_ADC_DIV_3		(3 << 6)
#define WM8960_ADC_DIV_4		(4 << 6)
#define WM8960_ADC_DIV_5_5		(5 << 6)
#define WM8960_ADC_DIV_6		(6 << 6)

#define WM8960_DCLK_DIV_1_5		(0 << 6)
#define WM8960_DCLK_DIV_2		(1 << 6)
#define WM8960_DCLK_DIV_3		(2 << 6)
#define WM8960_DCLK_DIV_4		(3 << 6)
#define WM8960_DCLK_DIV_6		(4 << 6)
#define WM8960_DCLK_DIV_8		(5 << 6)
#define WM8960_DCLK_DIV_12		(6 << 6)
#define WM8960_DCLK_DIV_16		(7 << 6)

#define WM8960_TOCLK_F19		(0 << 1)
#define WM8960_TOCLK_F21		(1 << 1)

#define WM8960_OPCLK_DIV_1		(0 << 0)
#define WM8960_OPCLK_DIV_2		(1 << 0)
#define WM8960_OPCLK_DIV_3		(2 << 0)
#define WM8960_OPCLK_DIV_4		(3 << 0)
#define WM8960_OPCLK_DIV_5_5		(4 << 0)
#define WM8960_OPCLK_DIV_6		(5 << 0)

#define WM8960_BCLK_DIV_1		(0 << 0)
#define WM8960_BCLK_DIV_1_5		(1 << 0)
#define WM8960_BCLK_DIV_2		(2 << 0)
#define WM8960_BCLK_DIV_3		(3 << 0)
#define WM8960_BCLK_DIV_4	    (4 << 0)
#define WM8960_BCLK_DIV_5_5		(5 << 0)
#define WM8960_BCLK_DIV_6		(6 << 0)
#define WM8960_BCLK_DIV_8		(7 << 0)
#define WM8960_BCLK_DIV_11		(8 << 0)
#define WM8960_BCLK_DIV_12		(9 << 0)
#define WM8960_BCLK_DIV_16	    (10 << 0)
#define WM8960_BCLK_DIV_22		(11 << 0)
#define WM8960_BCLK_DIV_24		(12 << 0)
#define WM8960_BCLK_DIV_32		(13 << 0)

extern struct snd_soc_dai wm8960_dai;
extern struct snd_soc_dai wm8960_2_dai;
extern struct snd_soc_dai wm8960_3_dai;
extern struct snd_soc_codec_device soc_codec_dev_two_wm8960;

#define WM8960_DRES_400R 0
#define WM8960_DRES_200R 1
#define WM8960_DRES_600R 2
#define WM8960_DRES_150R 3
#define WM8960_DRES_MAX  3

struct wm8960_data {
	int dres;
};

#endif
