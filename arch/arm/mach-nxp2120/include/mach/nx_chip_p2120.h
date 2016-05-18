//------------------------------------------------------------------------------
//
//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND	WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: nx_base
//	File		: nx_chip_p2120.h
//	Description	: support chip information
//	Author		: Goofy
//	Export		:
//	History		:
//		2010.07.19	Hans modify to nxp-2120
//		2008.04.17	Goofy first draft.
//------------------------------------------------------------------------------
#ifndef __NX_CHIP_P2120_H__
#define __NX_CHIP_P2120_H__

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// To remove following waring on RVDS compiler
//		Warning : #66-D: enumeration value is out of "int" range
//------------------------------------------------------------------------------
#ifdef __arm
#pragma diag_remark 66		// disable #66 warining
#endif

//------------------------------------------------------------------------------
// Module declaration.
//------------------------------------------------------------------------------
#ifndef NX_CHIP_MODULE_DECLARATION
#define	NX_CHIP_MODULE_DECLARATION( _name_, _num_, _addr_, _offset_ )	\
	enum {																\
		NUMBER_OF_ ## _name_ ## _MODULE		= _num_,					\
		PHY_BASEADDR_ ## _name_ ## _MODULE	= _addr_,					\
		OFFSET_OF_ ## _name_ ## _MODULE		= _offset_					\
	};
#endif	// NX_CHIP_MODULE_DECLARATION

//------------------------------------------------------------------------------
//							(	Name , Number,	Addr	,	Offset	)
//------------------------------------------------------------------------------
NX_CHIP_MODULE_DECLARATION( ALIVE		, 1, 0xC0019000, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( CLKPWR		, 1, 0xC000F000, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( INTC		, 1, 0xC0000800, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( DMA			, 8, 0xC0000000, 0x00000080 )
NX_CHIP_MODULE_DECLARATION( RTC			, 1, 0xC0019400, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( TIMER		, 5, 0xC0001800, 0x00000080 )
NX_CHIP_MODULE_DECLARATION( ECID		, 1, 0xC001F800, 0x00000000 )

NX_CHIP_MODULE_DECLARATION( MCUD		, 1, 0xC0014800, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( MCUS		, 1, 0xC0015800, 0x00000000 )

NX_CHIP_MODULE_DECLARATION( MLC			, 1, 0xC0004000, 0x00000400 )
NX_CHIP_MODULE_DECLARATION( DPC			, 1, 0xC000307C, 0x00000400 )
NX_CHIP_MODULE_DECLARATION( ROTATOR		, 1, 0xC0004800, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( SCALER		, 1, 0xC0003800, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( VIP			, 2, 0xC0002800, 0x0000E000 )
NX_CHIP_MODULE_DECLARATION( DEINTERLACE	, 1, 0xC0014000, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( CSC			, 1, 0xC0009000, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( GRP3D		, 1, 0xC001A000, 0x00000000 )

NX_CHIP_MODULE_DECLARATION( MPEG		, 1, 0xC0006800, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( H264		, 1, 0xC0006000, 0x00000000 )

NX_CHIP_MODULE_DECLARATION( AUDIO		, 1, 0xC000D800, 0x00000000 )

NX_CHIP_MODULE_DECLARATION( SDHC		, 1, 0xC0009800, 0x00003000 )
NX_CHIP_MODULE_DECLARATION( UART0		, 1, 0xC0016000, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( UART1		, 1, 0xC0016080, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( SSPSPI		, 1, 0xC0007800, 0x00000800 )
NX_CHIP_MODULE_DECLARATION( I2C0		, 1, 0xC000E000, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( I2C1		, 1, 0xC000E800, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( MPEGTSIF	, 1, 0xC0007000, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( OHCI		, 1, 0xC0019800, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( EHCI		, 1, 0xC000D000, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( UDC			, 1, 0xC0018000, 0x00000000 )

NX_CHIP_MODULE_DECLARATION( GPIO		, 3, 0xC000A000, 0x00000040 )
NX_CHIP_MODULE_DECLARATION( ADC			, 1, 0xC0005000, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( PWM			, 1, 0xC000C000, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( PPM			, 1, 0xC000A800, 0x00000000 )
NX_CHIP_MODULE_DECLARATION( SPDIF		, 1, 0xC0011800, 0x00000000 )

#ifdef __arm	// for RVDS
#pragma diag_default 66		// return to default setting for #66 warning
#endif


//------------------------------------------------------------------------------
// Interrupt Number of mudules for the interrupt controller.
//------------------------------------------------------------------------------
enum {
	INTNUM_OF_DPC0_MODULE		=	0,

	INTNUM_OF_VIP_MODULE		=	2,
	INTNUM_OF_DMA_MODULE		=	3,
	INTNUM_OF_TIMER0_MODULE		=	4,
	INTNUM_OF_CLKPWR_MODULE		=	5,

	INTNUM_OF_MPEGTSIF_MODULE	=	7,

	INTNUM_OF_UART0_MODULE		= 10,
	INTNUM_OF_TIMER1_MODULE		= 11,
	INTNUM_OF_SSPSPI0_MODULE	= 12,
	INTNUM_OF_GPIO_MODULE		= 13,
	INTNUM_OF_SDHC0_MODULE		= 14,
	INTNUM_OF_TIMER2_MODULE		= 15,
	INTNUM_OF_H264_MODULE		= 16,
	INTNUM_OF_MPEG_MODULE		= 17,

	INTNUM_OF_VLC_MODULE		= 19,
	INTNUM_OF_UDC_MODULE		= 20,
	INTNUM_OF_TIMER3_MODULE		= 21,
	INTNUM_OF_DEINTERLACE_MODULE= 22,
	INTNUM_OF_PPM_MODULE		= 23,
	INTNUM_OF_AUDIO_MODULE		= 24,
	INTNUM_OF_ADC_MODULE		= 25,
	INTNUM_OF_MCUS_MODULE		= 26,
	INTNUM_OF_GRP3D_MODULE		= 27,

	INTNUM_OF_ROTATOR_MODULE	= 29,
	INTNUM_OF_SCALER_MODULE		= 30,
	INTNUM_OF_RTC_MODULE		= 31,
	INTNUM_OF_I2C0_MODULE		= 32,
	INTNUM_OF_I2C1_MODULE		= 33,
	INTNUM_OF_UART1_MODULE		= 34,

	INTNUM_OF_CSC_MODULE		= 41,

	INTNUM_OF_TIMER4_MODULE		= 43,

	INTNUM_OF_ALIVE_MODULE		= 50,
	INTNUM_OF_EHCI_MODULE		= 51,
	INTNUM_OF_OHCI_MODULE		= 52,

	INTNUM_OF_VIP1_MODULE		= 54,
	INTNUM_OF_SPDIF_MODULE		= 55
};

//------------------------------------------------------------------------------
// DMA peripheral index of modules for the DMA controller.
//------------------------------------------------------------------------------
enum {
	DMAINDEX_OF_UART0_MODULE_TX			=	0,
	DMAINDEX_OF_UART0_MODULE_RX			=	1,
	DMAINDEX_OF_UART1_MODULE_TX			=	2,
	DMAINDEX_OF_UART1_MODULE_RX			=	3,


	DMAINDEX_OF_USBDEVICE_MODULE_EP1	= 12,
	DMAINDEX_OF_USBDEVICE_MODULE_EP2	= 13,
	DMAINDEX_OF_USBDEVICE_MODULE_EP3	= 14,

	DMAINDEX_OF_SDHC0_MODULE			= 16,
	DMAINDEX_OF_MPEGTSIF_MODULE			= 17,

	DMAINDEX_OF_SSPSPI0_MODULE_TX		= 18,
	DMAINDEX_OF_SSPSPI0_MODULE_RX		= 19,

	DMAINDEX_OF_AUDIO_MODULE_PCMOUT		= 24,
	DMAINDEX_OF_AUDIO_MODULE_SPDIFOUT	= 25,
	DMAINDEX_OF_AUDIO_MODULE_PCMIN		= 26,
	DMAINDEX_OF_AUDIO_MODULE_MICIN		= 27,
	DMAINDEX_OF_AUDIO_MODULE_ADC1IN		= 28,
	DMAINDEX_OF_AUDIO_MODULE_ADC2IN		= 29,

	DMAINDEX_OF_MCUS_MODULE				= 31,

	DMAINDEX_OF_AES_MODULE_RX			= 43,
	DMAINDEX_OF_AES_MODULE_TX			= 44,
	DMAINDEX_OF_SPDIF_MODULE_RX			= 45
};


#ifdef __cplusplus
}
#endif

#endif // __NX_CHIP_P2120_H__
