//	Copyright (C) 2009 Nexell Co., All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: scaler
//	File		: nx_scaler.h
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------
#ifndef __NX_SCALER_H__
#define __NX_SCALER_H__

#include <nx_type.h>
#include <mach/nx_chip.h>
#include <mach/nx_clockcontrol.h>

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	SCALER	SCALER
//------------------------------------------------------------------------------
//@{

	/// @brief	SCALER Module's Register List
	struct NX_SCALER_RegisterSet
	{
		volatile U32	SCRUNREG;				///< 0x00 : Scaler Configuration Register
		volatile U32	SCCFGREG;				///< 0x04 : Scaler Configuration Register1
		volatile U32	SCINTREG;				///< 0x08 : Scaler Interrupt Register
		volatile U32	SCSRCADDREG;			///< 0x0C : Scaler Source Address Register
		volatile U32	SCSRCSIZEREG;			///< 0x10 : Scaler Source Size Register
		volatile U32	SCDESTADDREG;			///< 0x14 : Scaler Destination Register
		volatile U32	SCDESTSIZEREG;			///< 0x18 : Scaler Destination Size Register
		volatile U32	DELTAXREG;				///< 0x1C : Scaler Delta X Register
		volatile U32	DELTAYREG;				///< 0x20 : Scaler Delta Y Register
		volatile U32	HVSOFTREG;				///< 0x24 : Horizontal and Vertical Filter Ratio Register
		volatile U32	__Reserved00[(0x7C0-0x28)/4];	///< 0x28 ~ 0x7BC : Reserved region
		volatile U32	CLKENB;					///< 0x7C0 : Clock Enable Register
	};

	/// @brief	SCALER interrupt for interrupt interface
	enum
	{
		NX_SCALER_INT_DONE = 0		///< Scaler Done interrupt.
	};

	/// @brief	Rotate modes
	typedef enum
	{
		NX_SCALER_ROTATE_0			= 0x00UL,		///< Rotate 0
		NX_SCALER_ROTATE_0_HFLIP	= 0x01UL,		///< Rotate 0 and Horizontal Inversion
		NX_SCALER_ROTATE_180_HFLIP	= 0x02UL,		///< Rotate 180 and Horizontal Inversion
		NX_SCALER_ROTATE_180		= 0x03UL		///< Rotate 180
	}	NX_SCALER_ROTATE;

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_SCALER_Initialize( void );
U32		NX_SCALER_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_SCALER_GetPhysicalAddress( void );
U32		NX_SCALER_GetSizeOfRegisterSet( void );
void	NX_SCALER_SetBaseAddress( U32 BaseAddress );
U32		NX_SCALER_GetBaseAddress( void );
CBOOL	NX_SCALER_OpenModule( void );
CBOOL	NX_SCALER_CloseModule( void );
CBOOL	NX_SCALER_CheckBusy( void );
CBOOL	NX_SCALER_CanPowerDown( void );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
S32		NX_SCALER_GetInterruptNumber( void );

void	NX_SCALER_SetInterruptEnable( S32 IntNum, CBOOL Enable );
CBOOL	NX_SCALER_GetInterruptEnable( S32 IntNum );
CBOOL	NX_SCALER_GetInterruptPending( S32 IntNum );
void	NX_SCALER_ClearInterruptPending( S32 IntNum );

void	NX_SCALER_SetInterruptEnableAll( CBOOL Enable );
CBOOL	NX_SCALER_GetInterruptEnableAll( void );
CBOOL	NX_SCALER_GetInterruptPendingAll( void );
void	NX_SCALER_ClearInterruptPendingAll( void );

void	NX_SCALER_SetInterruptEnable32( U32 EnableFlag );
U32		NX_SCALER_GetInterruptEnable32( void );
U32		NX_SCALER_GetInterruptPending32( void );
void	NX_SCALER_ClearInterruptPending32( U32 PendingFlag );

S32		NX_SCALER_GetInterruptPendingNumber( void );	// -1 if None
//@}

//------------------------------------------------------------------------------
///	@name	Clock Control Interface
//@{
void		NX_SCALER_SetClockPClkMode( NX_PCLKMODE mode );
NX_PCLKMODE	NX_SCALER_GetClockPClkMode( void );
void		NX_SCALER_SetClockBClkMode( NX_BCLKMODE mode );
NX_BCLKMODE	NX_SCALER_GetClockBClkMode( void );
//@}

//--------------------------------------------------------------------------
///	@name Operation Function of Scaler
//@{
void	NX_SCALER_Run( void );
void	NX_SCALER_Stop( void );
CBOOL	NX_SCALER_IsBusy( void );

void	NX_SCALER_SetFilterEnable( CBOOL enable );
CBOOL	NX_SCALER_GetFilterEnable( void );
//@}

//--------------------------------------------------------------------------
/// @name Configuration Function of Scaler
//@{
void	NX_SCALER_SetFilterRatio( U32 RatioH, U32 RatioV );
void	NX_SCALER_GetFilterRatio( U32* RatioH, U32* RatioV );

void				NX_SCALER_SetRotateMode( NX_SCALER_ROTATE rotate );
NX_SCALER_ROTATE	NX_SCALER_GetRotateMode( void );
//@}

//--------------------------------------------------------------------------
///	@name Configuration Function of Image Size and Address
//@{
void	NX_SCALER_SetSrcAddr( U32 Addr );
U32		NX_SCALER_GetSrcAddr( void );

void	NX_SCALER_SetDestAddr( U32 Addr );
U32		NX_SCALER_GetDestAddr( void );

void	NX_SCALER_SetImageSize( U32	dwSrcWidth, U32	dwSrcHeight, U32 dwDestWidth, U32 dwDestHeight );
void	NX_SCALER_GetImageSize( U32	*pdwSrcWidth, U32	*pdwSrcHeight, U32	*pdwDestWidth, U32	*pdwDestHeight );
//@}

//@}

#ifdef	__cplusplus
}
#endif

#endif // __NX_SCALER_H__
