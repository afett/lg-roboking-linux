//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: USB Host Controller ( EHCI )
//	File		: nx_ehci.h
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_EHCI_H__
#define __NX_EHCI_H__

#include <nx_type.h>
#include <mach/nx_chip.h>
#include <mach/nx_clockcontrol.h>

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	EHCI EHCI
//------------------------------------------------------------------------------
//@{

/// @brief USB Host EHCI Module's Register List
struct	NX_EHCI_RegisterSet
{
	volatile U8		CAPLENGTH;					///< 0x00 : Capability Register Length Register
	volatile U8		Reserved0;					///< 0x01 : Reserved Region
	volatile U16	HCIvERSION;					///< 0x02 : Interface Version Number Register
	volatile U32	HCSPARAMS;					///< 0x04 : Structural Parameters Register
	volatile U32	HCCPARAMS;					///< 0x08 : Capability Parameters Register
	volatile U32	HCSPPORTROUTE;				///< 0x0C : Companion Port Route Description Register
	volatile U32	USBCMD;						///< 0x10 : USB Command Register
	volatile U32	USBSTS;						///< 0x14 : USB Status Register
	volatile U32	USBINTR;					///< 0x18 : USB Interrupt Register
	volatile U32	FRINDEX;					///< 0x1C : USB Frame Index Register
	volatile U32	CTRLDSSEGMENT;				///< 0x20 : Control Data Structure Segment Register
	volatile U32	PERIODICLISTBASE;			///< 0x24 : Periodic Frame List Base Address Register
	volatile U32	ASYNCLISTADDR;				///< 0x28 : Current Asynchronous List Address Register
	volatile U32	Reserved1[(0x50-0x2C)/4];	///< 0x2C ~ 0x4C : Reserved Region
	volatile U32	CONFIGFLAG ;				///< 0x50 : Configure Flag Register
	volatile U32	PORTSC;						///< 0x54 : Port Status and Control Register
	volatile U32	Reserved2[(0x3C0-0x58)/4];	///< 0x58 ~ 0x3BC : Reserved Region
	volatile U32	CLKENB;						///< 0x3C0 : Clock Enable Register
	volatile U32	CLKGEN;						///< 0x3C4 : Clock Generator Register
};

/// @brief	EHCI interrupt for interrupt interface
enum NX_EHCI_INT
{
	NX_EHCI_INT_UIE		=	0,		///< USB Interrupt.
	NX_EHCI_INT_UEIE	=	1,		///< USB Error Interrupt.
	NX_EHCI_INT_PCIE	=	2,		///< USB Port Change Interrupt.
	NX_EHCI_INT_FLRE	=	3,		///< USB Frame List Rollover Interrupt.
	NX_EHCI_INT_HSEE	=	4,		///< USB Host System Error Interrupt.
	NX_EHCI_INT_IAAE	=	5		///< USB Async Advance Interrupt.
};

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_EHCI_Initialize( void );
U32		NX_EHCI_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_EHCI_GetPhysicalAddress( void );
U32		NX_EHCI_GetSizeOfRegisterSet( void );
void	NX_EHCI_SetBaseAddress( U32 BaseAddress );
U32		NX_EHCI_GetBaseAddress( void );
CBOOL	NX_EHCI_OpenModule( void );
CBOOL	NX_EHCI_CloseModule( void );
CBOOL	NX_EHCI_CheckBusy( void );
CBOOL	NX_EHCI_CanPowerDown( void );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
S32		NX_EHCI_GetInterruptNumber( void );

void	NX_EHCI_SetInterruptEnable( S32 IntNum, CBOOL Enable );
CBOOL	NX_EHCI_GetInterruptEnable( S32 IntNum );
CBOOL	NX_EHCI_GetInterruptPending( S32 IntNum );
void	NX_EHCI_ClearInterruptPending( S32 IntNum );

void	NX_EHCI_SetInterruptEnableAll( CBOOL Enable );
CBOOL	NX_EHCI_GetInterruptEnableAll( void );
CBOOL	NX_EHCI_GetInterruptPendingAll( void );
void	NX_EHCI_ClearInterruptPendingAll( void );

void	NX_EHCI_SetInterruptEnable32( U32 EnableFlag );
U32		NX_EHCI_GetInterruptEnable32( void );
U32		NX_EHCI_GetInterruptPending32( void );
void	NX_EHCI_ClearInterruptPending32( U32 PendingFlag );

S32		NX_EHCI_GetInterruptPendingNumber( void );	// -1 if None
//@}

//------------------------------------------------------------------------------
///	@name	Clock Control Interface
//@{
void		NX_EHCI_SetClockPClkMode( NX_PCLKMODE mode );
NX_PCLKMODE	NX_EHCI_GetClockPClkMode( void );
void		NX_EHCI_SetClockBClkMode( NX_BCLKMODE mode );
NX_BCLKMODE	NX_EHCI_GetClockBClkMode( void );
void		NX_EHCI_SetClockSource( U32 Index, U32 ClkSrc );
U32			NX_EHCI_GetClockSource( U32 Index );
void		NX_EHCI_SetClockDivisor( U32 Index, U32 Divisor );
U32			NX_EHCI_GetClockDivisor( U32 Index );
void		NX_EHCI_SetClockDivisorEnable( CBOOL Enable );
CBOOL		NX_EHCI_GetClockDivisorEnable( void );
//@}

//@}

#ifdef	__cplusplus
}
#endif


#endif // __NX_UHC_H__
