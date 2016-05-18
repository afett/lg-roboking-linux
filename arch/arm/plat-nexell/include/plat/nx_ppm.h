//------------------------------------------------------------------------------
//
//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: Pulse Period Measurement
//	File		: nx_ppm.h
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_PPM_H__
#define __NX_PPM_H__

#include <nx_type.h>
#include <mach/nx_chip.h>
#include <mach/nx_clockcontrol.h>

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
///	@defgroup	PPM PPM
//------------------------------------------------------------------------------
//@{

	/// @brief	PPM Module's Register List
	struct NX_PPM_RegisterSet
	{
		volatile U16	PPM_CTRL;					///< 0x00 : Control Register
		volatile U16	__Reserved00;				///< 0x02 : Reserved Regopm
		volatile U16	PPM_STAT;					///< 0x04 : Status Register
		volatile U16	PPM_LOWPERIOD;				///< 0x06 : Low Period Register
		volatile U16	PPM_HIGHPERIOD;				///< 0x08 : High Period Register
		volatile U8		__Reserved01[0x40-0x0A];	///< 0x0A ~ 0x3C : Reserved Region
		volatile U32	PPM_CLKENB;					///< 0x40 : Clock Enable Register
		volatile U32	PPM_CLKGEN[1];				///< 0x44 : Clock Generator Register
	};

	/// @brief PPM interrupt for interrupt interface.
	enum
	{
		NX_PPM_INT_RISEEDGE = 0,	///< Rising edge detect interrupt.
		NX_PPM_INT_FALLEDGE = 1,	///< Falling edge detect interrupt.
		NX_PPM_INT_OVERFLOW = 2		///< Overflow interrupt.
	};

	/// @brief	Set Polarity of Input Signal
	typedef enum
	{
		NX_PPM_INPUTPOL_INVERT		= 0UL,	///< Input Signal Invert
		NX_PPM_INPUTPOL_BYPASS		= 1UL	///< Input Signal Bypass

	}	NX_PPM_INPUTPOL;

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_PPM_Initialize( void );
U32		NX_PPM_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_PPM_GetPhysicalAddress( void );
U32		NX_PPM_GetSizeOfRegisterSet( void );
void	NX_PPM_SetBaseAddress( U32 BaseAddress );
U32		NX_PPM_GetBaseAddress( void );
CBOOL	NX_PPM_OpenModule( void );
CBOOL	NX_PPM_CloseModule( void );
CBOOL	NX_PPM_CheckBusy( void );
CBOOL	NX_PPM_CanPowerDown( void );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
S32		NX_PPM_GetInterruptNumber( void );

void	NX_PPM_SetInterruptEnable( S32 IntNum, CBOOL Enable );
CBOOL	NX_PPM_GetInterruptEnable( S32 IntNum );
CBOOL	NX_PPM_GetInterruptPending( S32 IntNum );
void	NX_PPM_ClearInterruptPending( S32 IntNum );

void	NX_PPM_SetInterruptEnableAll( CBOOL Enable );
CBOOL	NX_PPM_GetInterruptEnableAll( void );
CBOOL	NX_PPM_GetInterruptPendingAll( void );
void	NX_PPM_ClearInterruptPendingAll( void );

void	NX_PPM_SetInterruptEnable32( U32 EnableFlag );
U32		NX_PPM_GetInterruptEnable32( void );
U32		NX_PPM_GetInterruptPending32( void );
void	NX_PPM_ClearInterruptPending32( U32 PendingFlag );

S32		NX_PPM_GetInterruptPendingNumber( void );	// -1 if None
//@}

//------------------------------------------------------------------------------
///	@name	Clock Control Interface
//@{
void		NX_PPM_SetClockPClkMode( NX_PCLKMODE mode );
NX_PCLKMODE	NX_PPM_GetClockPClkMode( void );
void		NX_PPM_SetClockSource( U32 Index, U32 ClkSrc );
U32			NX_PPM_GetClockSource( U32 Index );
void		NX_PPM_SetClockDivisor( U32 Index, U32 Divisor );
U32			NX_PPM_GetClockDivisor( U32 Index );
void		NX_PPM_SetClockDivisorEnable( CBOOL Enable );
CBOOL		NX_PPM_GetClockDivisorEnable( void );
//@}

//--------------------------------------------------------------------------
/// @name PPM Operation Function
//@{
void		NX_PPM_SetPPMEnable ( CBOOL enable );
CBOOL		NX_PPM_GetPPMEnable ( void );

void			NX_PPM_SetInputSignalPolarity( NX_PPM_INPUTPOL polarity );
NX_PPM_INPUTPOL	NX_PPM_GetInputSignalPolarity( void );

CBOOL		NX_PPM_IsHighOverflow( void );
CBOOL		NX_PPM_IsLowOverflow( void );

U32			NX_PPM_GetPPMLowPeriodValue ( void );
U32			NX_PPM_GetPPMHighPeriodValue ( void );
//@}

//@}

#ifdef	__cplusplus
}
#endif

#endif // __NX_PPM_H__
