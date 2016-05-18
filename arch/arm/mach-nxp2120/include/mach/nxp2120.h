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
 *	   2010/09/28 first implementation
 ------------------------------------------------------------------------------*/
#ifndef __NXP2120_H__
#define __NXP2120_H__

/*------------------------------------------------------------------------------
 * Physical Base Address
 */
#define PHY_BASEADDR_ALIVE			(0xC0019000)
#define PHY_BASEADDR_CLKPWR			(0xC000F000)
#define PHY_BASEADDR_INTC			(0xC0000800)
#define PHY_BASEADDR_DMA			(0xC0000000)
#define PHY_BASEADDR_RTC			(0xC0019400)
#define PHY_BASEADDR_TIMER			(0xC0001800)
#define PHY_BASEADDR_ECID			(0xC001F800)
#define PHY_BASEADDR_MCUD			(0xC0014800)
#define PHY_BASEADDR_MCUS			(0xC0015800)
#define PHY_BASEADDR_MLC			(0xC0004000)
#define PHY_BASEADDR_DPC			(0xC000307C)
#define PHY_BASEADDR_ROTATOR		(0xC0004800)
#define PHY_BASEADDR_SCALER			(0xC0003800)
#define PHY_BASEADDR_VIP0			(0xC0002800)
#define PHY_BASEADDR_VIP1			(0xC0010800)
#define PHY_BASEADDR_DEINTERLACE	(0xC0014000)
#define PHY_BASEADDR_CSC			(0xC0009000)
#define PHY_BASEADDR_GRP3D			(0xC001A000)
#define PHY_BASEADDR_MPEG			(0xC0006800)
#define PHY_BASEADDR_H264			(0xC0006000)
#define PHY_BASEADDR_AUDIO			(0xC000D800)
#define PHY_BASEADDR_SDHC			(0xC0009800)
#define PHY_BASEADDR_UART0			(0xC0016000)
#define PHY_BASEADDR_UART1			(0xC0016080)
#define PHY_BASEADDR_SSPSPI			(0xC0007800)
#define PHY_BASEADDR_I2C0			(0xC000E000)
#define PHY_BASEADDR_I2C1			(0xC000E800)
#define PHY_BASEADDR_MPEGTSIF		(0xC0007000)
#define PHY_BASEADDR_OHCI			(0xC0019800)
#define PHY_BASEADDR_EHCI			(0xC000D000)
#define PHY_BASEADDR_UDC			(0xC0018000)
#define PHY_BASEADDR_GPIO			(0xC000A000)
#define PHY_BASEADDR_ADC			(0xC0005000)
#define PHY_BASEADDR_PWM			(0xC000C000)
#define PHY_BASEADDR_PPM			(0xC000A800)
#define PHY_BASEADDR_SPDIF			(0xC0011800)

#define PHY_BASEADDR_NAND_MODULE	(PHY_BASEADDR_MCUS + 0x0000007C)	/* NAND controller */
#define PHY_BASEADDR_NAND_STATIC	(0x2C000000)	/* STATIC(0x00000000) + 0x2C000000 */

/*------------------------------------------------------------------------------
 * Interrupt
 */
#include <mach/nxp2120_irq.h>

/*------------------------------------------------------------------------------
 * prototype header
 */
#ifndef __ASSEMBLY__

#include <plat/nx_adc.h>
#include <plat/nx_alive.h>
#include <plat/nx_audio.h>
#include <plat/nx_clkpwr.h>
#include <plat/nx_csc.h>
#include <plat/nx_deinterlace.h>
#include <plat/nx_dma.h>
#include <plat/nx_dpc.h>
#include <plat/nx_ecid.h>
#include <plat/nx_ehci.h>
#include <plat/nx_gpio.h>
#include <plat/nx_i2c.h>
#include <plat/nx_intc.h>
#include <plat/nx_mcud.h>
#include <plat/nx_mcus.h>
#include <plat/nx_mlc.h>
#include <plat/nx_mpegtsif.h>
#include <plat/nx_ohci.h>
#include <plat/nx_ppm.h>
#include <plat/nx_pwm.h>
#include <plat/nx_rotator.h>
#include <plat/nx_rtc.h>
#include <plat/nx_scaler.h>
#include <plat/nx_sdhc.h>
#include <plat/nx_sspspi.h>
#include <plat/nx_timer.h>
#include <plat/nx_uart.h>
#include <plat/nx_udc.h>
#include <plat/nx_vip.h>
#include <plat/nx_spdif.h>

#endif /* 	__ASSEMBLY__ */
#endif /*	__NXP2120_H__ */
