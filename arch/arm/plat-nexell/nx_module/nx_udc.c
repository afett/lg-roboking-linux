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
//	Module		: USB Device Controller
//	File		: nx_udc.c
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------
#include <plat/nx_udc.h>

static	struct NX_UDC_RegisterSet *__g_pRegister = CNULL;

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b CTRUE	indicates that Initialize is successed.\n
 *			\b CFALSE	indicates that Initialize is failed.\n
 *	@see	NX_UDC_GetNumberOfModule
 */

CBOOL	NX_UDC_Initialize( void )
{
	static CBOOL bInit = CFALSE;

	if( CFALSE == bInit )
	{
		__g_pRegister = CNULL;

		bInit = CTRUE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get number of modules in the chip.
 *	@return		Module's number.
 *	@see		NX_UDC_Initialize
 */
U32		NX_UDC_GetNumberOfModule( void )
{
	return NUMBER_OF_UDC_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@return		Module's physical address
 *	@see		NX_UDC_GetSizeOfRegisterSet,
 *				NX_UDC_SetBaseAddress,		NX_UDC_GetBaseAddress,
 *				NX_UDC_OpenModule,			NX_UDC_CloseModule,
 *				NX_UDC_CheckBusy,			NX_UDC_CanPowerDown
 */
U32		NX_UDC_GetPhysicalAddress( void )
{
	return	(U32)( PHY_BASEADDR_UDC_MODULE );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_UDC_GetPhysicalAddress,
 *				NX_UDC_SetBaseAddress,		NX_UDC_GetBaseAddress,
 *				NX_UDC_OpenModule,			NX_UDC_CloseModule,
 *				NX_UDC_CheckBusy,			NX_UDC_CanPowerDown
 */
U32		NX_UDC_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_UDC_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_UDC_GetPhysicalAddress,	NX_UDC_GetSizeOfRegisterSet,
 *				NX_UDC_GetBaseAddress,
 *				NX_UDC_OpenModule,			NX_UDC_CloseModule,
 *				NX_UDC_CheckBusy,			NX_UDC_CanPowerDown
 */
void	NX_UDC_SetBaseAddress( U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );

	__g_pRegister = (struct NX_UDC_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@return		Module's base address.
 *	@see		NX_UDC_GetPhysicalAddress,	NX_UDC_GetSizeOfRegisterSet,
 *				NX_UDC_SetBaseAddress,
 *				NX_UDC_OpenModule,			NX_UDC_CloseModule,
 *				NX_UDC_CheckBusy,			NX_UDC_CanPowerDown
 */
U32		NX_UDC_GetBaseAddress( void )
{
	return (U32)__g_pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@return		\b CTRUE	indicates that Initialize is successed. \n
 *				\b CFALSE	indicates that Initialize is failed.
 *	@see		NX_UDC_GetPhysicalAddress,	NX_UDC_GetSizeOfRegisterSet,
 *				NX_UDC_SetBaseAddress,		NX_UDC_GetBaseAddress,
 *				NX_UDC_CloseModule,
 *				NX_UDC_CheckBusy,			NX_UDC_CanPowerDown
 */
CBOOL	NX_UDC_OpenModule( void )
{
		return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@return		\b CTRUE	indicates that Deinitialize is successed. \n
 *				\b CFALSE	indicates that Deinitialize is failed.
 *	@see		NX_UDC_GetPhysicalAddress,	NX_UDC_GetSizeOfRegisterSet,
 *				NX_UDC_SetBaseAddress,		NX_UDC_GetBaseAddress,
 *				NX_UDC_OpenModule,
 *				NX_UDC_CheckBusy,			NX_UDC_CanPowerDown
 */
CBOOL	NX_UDC_CloseModule( void )
{
		return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@return		\b CTRUE	indicates that Module is Busy. \n
 *				\b CFALSE	indicates that Module is NOT Busy.
 *	@see		NX_UDC_GetPhysicalAddress,	NX_UDC_GetSizeOfRegisterSet,
 *				NX_UDC_SetBaseAddress,		NX_UDC_GetBaseAddress,
 *				NX_UDC_OpenModule,			NX_UDC_CloseModule,
 *				NX_UDC_CanPowerDown
 */
CBOOL	NX_UDC_CheckBusy( void )
{
		return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@return		\b CTRUE	indicates that Ready to enter power-down stage. \n
 *				\b CFALSE	indicates that This module can't enter to power-down stage.
 *	@see		NX_UDC_GetPhysicalAddress,	NX_UDC_GetSizeOfRegisterSet,
 *				NX_UDC_SetBaseAddress,		NX_UDC_GetBaseAddress,
 *				NX_UDC_OpenModule,			NX_UDC_CloseModule,
 *				NX_UDC_CheckBusy
 */
CBOOL	NX_UDC_CanPowerDown( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number for interrupt controller.
 *	@return		Interrupt number
 *	@see											NX_UDC_SetInterruptEnable,
 *				NX_UDC_GetInterruptEnable,			NX_UDC_SetInterruptEnable32,
 *				NX_UDC_GetInterruptEnable32,		NX_UDC_GetInterruptPending,
 *				NX_UDC_GetInterruptPending32,		NX_UDC_ClearInterruptPending,
 *				NX_UDC_ClearInterruptPending32,		NX_UDC_SetInterruptEnableAll,
 *				NX_UDC_GetInterruptEnableAll,		NX_UDC_GetInterruptPendingAll,
 *				NX_UDC_ClearInterruptPendingAll,	NX_UDC_GetInterruptPendingNumber
 */ 
S32		NX_UDC_GetInterruptNumber( void )
{
	return INTNUM_OF_UDC_MODULE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	IntNum	Interrupt Number ( 0: Reset, 1: Resume, 2: Suspend, 3: VBUS On, 4: VBUS Off,
 *						5: Speed detect, 6: Error, 7: EP0, 8: EP1, 9: EP2, 10: EP3, 11: VBUS ENB ).
 *	@param[in]	Enable	\b CTRUE	indicates that Interrupt Enable. \n
 *						\b CFALSE	indicates that Interrupt Disable.
 *	@return		None.
 *	@remark		USB Device have a many interrupt source. Each Pend have a many pending
 *				source. \n
 *				Interrupt Interface just setting the each interrupt enabe bit and
 *				pending bit. so User should check and clear the pending source bit. \n
 *	@code
 *
 *	//--------------------------------------------------------------------------
 *	//
 *	//	Interrupt Interface just setting each interrupt enable bit and pending bit !
 *	//
 *	//					|<------- setting ---->|
 *	//
 *	//				OR			AND
 *	// <--- Interrupt---+-- Reset	----+-- Enable				[HRESE]
 *	//					|				+-- Host Forced Reset	[HFRES]
 *	//					|
 *	//					+-- Resume [HFRM]
 *	//					|
 *	//					|				AND
 *	//					+-- Suspend ----+-- Enable				[HSUSPE]
 *	//					|				+-- Host Forced Suspend [HFSUSP]
 *	//					|
 *	//					|				AND
 *	//					+-- VBUS ON ----+-- Enable				[VBUSONEN]
 *	//					|				+-- VBUS ON				[VBUSON]
 *	//					|
 *	//					|				AND
 *	//					+-- VBUS OFF----+-- Enable				[VBUSOFFEN]
 *	//					|				+-- VBUS OFF			[VBUSOFF]
 *	//					|
 *	//					|				AND
 *	//					+-- VBUS ENB----+-- Enable				[VBUSINTENB]
 *	//					|				+-- VBUS PEND			[VBUSPEND]
 *	//					|
 *	//					|				AND
 *	//					+-- SPEED DET---+-- Enable				[SPDEN]
 *	//					|				+-- SDE					[SDE]
 *	//					|
 *	//					|			AND
 *	//					+-- Error --+-- Enable[EIE]	OR
 *	//					|			+-- Pend			----+-- EB Underrun Error			[EUERR]
 *	//					|									+-- PID Error					[PERR]
 *	//					|									+-- Byte Align Error			[BAERR]
 *	//					|									+-- Timeout Error				[TMERR]
 *	//					|									+-- Bit Stuff Error				[BSERR]
 *	//					|									+-- Token CRC Error				[TCERR]
 *	//					|									+-- Data CRC Error				[DCERR]
 *	//					|									+-- EB Overrun Error			[EOERR]
 *	//					|									+-- Toggle Bit Missmatch Error	[TBM]
 *	//					|
 *	//					+-- EP0 ----+-- Enable[EP0IE]	OR
 *	//					|			+-- Pend	[EP0I] -----+-- Stall
 *	//					|									+-- Tx
 *	//					|									+-- Rx
 *	//					|			AND
 *	//					+-- EP1 ----+-- Enable[EP1IE]	OR
 *	//					|			+-- Pend	[EP1I] -----+-- FIFO Under Flow			[FUDR]
 *	//					|									+-- FIFO Over Flow			[FOVF]
 *	//					|									+-- Short Packet Received	[SPT]
 *	//					|									+-- FIFO Flushed			[FFS]
 *	//					|									+-- Stall					[FSC]
 *	//					|									+-- Tx						[TPS]
 *	//					|									+-- Rx						[RPS]
 *	//					|									|
 *	//					|									|	AND
 *	//					|									+---+-- First Out Packet Interrupt	[FPID]
 *	//					|									|	+-- Out Start DMA				[OSD]
 *	//					|									|
 *	//					|									|	AND
 *	//					|									+---+-- DMA Total Counter Zero Interrupt Enable [DTZIEN]
 *	//					|										+-- DMA Total Counter Zero					[DTCZ]
 *	//					|
 *	//					|			AND
 *	//					+-- EP2 ----+-- Enable[EP2IE]	OR
 *	//					|			+-- Pend	[EP2I] -----+-- FIFO Under Flow		[FUDR]
 *	//					|									+-- ...
 *	//					|									*-- ...
 *	//					|									*-- ...
 *	//					|									+---+-- DMA Total Counter Zero Interrupt Enable [DTZIEN]
 *	//					|										+-- DMA Total Counter Zero					[DTCZ]
 *	//					|
 *	//					|			AND
 *	//					+-- EP3 ----+-- Enable[EP3IE]	OR
 *	//								+-- Pend	[EP3I] -----+-- FIFO Under Flow		[FUDR]
 *	//														+-- ...
 *	//														*-- ...
 *	//														*-- ...
 *	//														+---+-- DMA Total Counter Zero Interrupt Enable [DTZIEN]
 *	//															+-- DMA Total Counter Zero					[DTCZ]
 *	//
 *	//--------------------------------------------------------------------------
 *	@endcode
 *	@see		NX_UDC_GetInterruptNumber,			
 *				NX_UDC_GetInterruptEnable,			NX_UDC_SetInterruptEnable32,
 *				NX_UDC_GetInterruptEnable32,		NX_UDC_GetInterruptPending,
 *				NX_UDC_GetInterruptPending32,		NX_UDC_ClearInterruptPending,
 *				NX_UDC_ClearInterruptPending32,		NX_UDC_SetInterruptEnableAll,
 *				NX_UDC_GetInterruptEnableAll,		NX_UDC_GetInterruptPendingAll,
 *				NX_UDC_ClearInterruptPendingAll,	NX_UDC_GetInterruptPendingNumber
 */
void	NX_UDC_SetInterruptEnable( S32 IntNum, CBOOL Enable )
{
	const U16 VBUSOFF_BITPOS	= 11;
	const U16 VBUSON_BITPOS		= 10;
	const U16 EIE_BITPOS		= 8;
	const U16 SPDEN_BITPOS		= 6;
	const U16 HSUSPE_BITPOS		= 1;
	const U16 HRESE_BITPOS		= 0;

	register struct NX_UDC_RegisterSet*	pRegister;
	register volatile U16 regvalue;
	register U16 bitPos=0;

	NX_ASSERT( NX_UDC_INT_VBUSENB >= IntNum && 0 <= IntNum );
	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	switch( IntNum )
	{
		case NX_UDC_INT_EP0:
		case NX_UDC_INT_EP1:
		case NX_UDC_INT_EP2:
		case NX_UDC_INT_EP3:

				bitPos = IntNum - NX_UDC_INT_EP0;

				regvalue = pRegister->INTENB;
				regvalue &= ~(1<<bitPos);
				regvalue |= ((U16)Enable <<bitPos);
				pRegister->INTENB = regvalue;
				return;
				//break;

		case NX_UDC_INT_ERROR:

				bitPos = EIE_BITPOS;
				break;

		case NX_UDC_INT_VBUSON:

				bitPos = VBUSON_BITPOS;
				break;

		case NX_UDC_INT_VBUSOFF:

				bitPos = VBUSOFF_BITPOS;
				break;

		case NX_UDC_INT_VBUSENB:

				pRegister->VBUSINTENB = Enable;
				return;

		case NX_UDC_INT_SPEEDDETECT:

				bitPos = SPDEN_BITPOS;
				break;

		case NX_UDC_INT_RESUME:

				return;
				//break;

		case NX_UDC_INT_RESET:

				bitPos = HRESE_BITPOS;
				break;

		case NX_UDC_INT_SUSPEND:

				bitPos = HSUSPE_BITPOS;
				break;

		default:
				NX_ASSERT( CFALSE );
				break;
	}

	regvalue = pRegister->SCR;
	regvalue &= ~(1<<bitPos);
	regvalue |= ((U16)Enable << bitPos);
	pRegister->SCR = regvalue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	IntNum	Interrupt Number ( 0: Reset, 1: Resume, 2: Suspend, 3: VBUS On, 4: VBUS Off,
 *						5: Speed detect, 6: Error, 7: EP0, 8: EP1, 9: EP2, 10: EP3, 11: VBUS ENB ).
 *	@return		\b CTRUE	indicates that Interrupt is enabled. \n
 *				\b CFALSE	indicates that Interrupt is disabled.
 *	@remark		USB Device have a many interrupt source. Each Pend have a many pending
 *				source. \n
 *				Interrupt Interface just setting the each interrupt enabe bit and
 *				pending bit. so User should check and clear the pending source bit. \n
 *	@see		NX_UDC_GetInterruptNumber,			NX_UDC_SetInterruptEnable,
 *													NX_UDC_SetInterruptEnable32,
 *				NX_UDC_GetInterruptEnable32,		NX_UDC_GetInterruptPending,
 *				NX_UDC_GetInterruptPending32,		NX_UDC_ClearInterruptPending,
 *				NX_UDC_ClearInterruptPending32,		NX_UDC_SetInterruptEnableAll,
 *				NX_UDC_GetInterruptEnableAll,		NX_UDC_GetInterruptPendingAll,
 *				NX_UDC_ClearInterruptPendingAll,	NX_UDC_GetInterruptPendingNumber
 */
CBOOL	NX_UDC_GetInterruptEnable( S32 IntNum )
{
	const U16 EP3IE_BITPOS		= 3;
	const U16 EP2IE_BITPOS		= 2;
	const U16 EP1IE_BITPOS		= 1;
	const U16 EP0IE_BITPOS		= 0;
	const U16 VBUSOFF_BITPOS	= 11;
	const U16 VBUSON_BITPOS		= 10;
	const U16 EIE_BITPOS		= 8;
	const U16 SPDEN_BITPOS		= 6;
	const U16 HSUSPE_BITPOS		= 1;
	const U16 HRESE_BITPOS		= 0;

	NX_ASSERT( NX_UDC_INT_VBUSENB >= IntNum && 0 <= IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	switch( IntNum )
	{
		case NX_UDC_INT_EP0:

				return (CBOOL)( (__g_pRegister->INTENB	>> EP0IE_BITPOS ) & 0x01 );
				//break;

		case NX_UDC_INT_EP1:

				return (CBOOL)( (__g_pRegister->INTENB	>> EP1IE_BITPOS ) & 0x01 );
				//break;

		case NX_UDC_INT_EP2:

				return (CBOOL)( (__g_pRegister->INTENB	>> EP2IE_BITPOS ) & 0x01 );
				//break;

		case NX_UDC_INT_EP3:

				return (CBOOL)( (__g_pRegister->INTENB	>> EP3IE_BITPOS ) & 0x01 );
				//break;

		case NX_UDC_INT_ERROR:

				return (CBOOL)( (__g_pRegister->SCR >> EIE_BITPOS) & 0x01 );
				//break;

		case NX_UDC_INT_VBUSON:

				return (CBOOL)( (__g_pRegister->SCR >> VBUSON_BITPOS) & 0x01 );
				//break;

		case NX_UDC_INT_VBUSOFF:

				return (CBOOL)( (__g_pRegister->SCR >> VBUSOFF_BITPOS) & 0x01 );
				//break;

		case NX_UDC_INT_VBUSENB:

				return (CBOOL)(__g_pRegister->VBUSINTENB & 0x01);
				//break;

		case NX_UDC_INT_SPEEDDETECT:

				return (CBOOL)( (__g_pRegister->SCR >> SPDEN_BITPOS) & 0x01 );
				//break;

		case NX_UDC_INT_RESUME:
				return CTRUE;
				//break;

		case NX_UDC_INT_RESET:

				return (CBOOL)( (__g_pRegister->SCR >> HRESE_BITPOS) & 0x01 );
				//break;

		case NX_UDC_INT_SUSPEND:

				return (CBOOL)( (__g_pRegister->SCR >> HSUSPE_BITPOS) & 0x01 );
				//break;

		default:
				NX_ASSERT( CFALSE );
				break;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n
 *							- EnableFlag[0]	: Set RESET			interrupt enable or disable. \n
 *							- EnableFlag[1]	: Set RESUME		interrupt enable or disable. \n
 *							- EnableFlag[2]	: Set SUSPEND		interrupt enable or disable. \n
 *							- EnableFlag[3]	: Set VBUSON		interrupt enable or disable. \n
 *							- EnableFlag[4]	: Set VBUSOFF		interrupt enable or disable. \n
 *							- EnableFlag[5]	: Set SPEEDDETECT	interrupt enable or disable. \n
 *							- EnableFlag[6]	: Set ERROR			interrupt enable or disable. \n
 *							- EnableFlag[7]	: Set EP0			interrupt enable or disable. \n
 *							- EnableFlag[8]	: Set EP1			interrupt enable or disable. \n
 *							- EnableFlag[9]	: Set EP2			interrupt enable or disable. \n
 *							- EnableFlag[10] : Set EP3			interrupt enable or disable. \n
 *							- EnableFlag[11] : Set VBUSENB		interrupt enable or disable. \n
 *	@return		None.
 *	@see		NX_UDC_GetInterruptNumber,			NX_UDC_SetInterruptEnable,
 *				NX_UDC_GetInterruptEnable,			
 *				NX_UDC_GetInterruptEnable32,		NX_UDC_GetInterruptPending,
 *				NX_UDC_GetInterruptPending32,		NX_UDC_ClearInterruptPending,
 *				NX_UDC_ClearInterruptPending32,		NX_UDC_SetInterruptEnableAll,
 *				NX_UDC_GetInterruptEnableAll,		NX_UDC_GetInterruptPendingAll,
 *				NX_UDC_ClearInterruptPendingAll,	NX_UDC_GetInterruptPendingNumber
 */
void	NX_UDC_SetInterruptEnable32( U32 EnableFlag )
{
	register struct NX_UDC_RegisterSet *pRegister;

	NX_ASSERT( CNULL != __g_pRegister );
	pRegister = (struct NX_UDC_RegisterSet *)__g_pRegister;

	pRegister->INTENB		=	((EnableFlag>> 7)&0x0F);			// EP0 ~ EP3
	pRegister->VBUSINTENB	=	((EnableFlag>>11)&0x01);			// VBUSENB
	pRegister->SCR			=	(((EnableFlag>> 0)&0x07) <<	0 |		// RESET, RESUME, SUSPEND
								((EnableFlag>> 3)&0x03) << 10 |		// VBUSON, VBUSOFF
								((EnableFlag>> 5)&0x01) <<	6 |		// SPEEDDETECT
								((EnableFlag>> 6)&0x01) <<	8 );	// ERROR
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt enable bit.
 *	@return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0]	: RESET		interrupt's setting value. \n
 *				- Return Value[1]	: RESUME	interrupt's setting value. \n
 *				- Return Value[2]	: SUSPEND	interrupt's setting value. \n
 *				- Return Value[3]	: VBUSON	interrupt's setting value. \n
 *				- Return Value[4]	: VBUSOFF	interrupt's setting value. \n
 *				- Return Value[5]	: SPEEDDETECT interrupt's setting value. \n
 *				- Return Value[6]	: ERROR		interrupt's setting value. \n
 *				- Return Value[7]	: EP0		interrupt's setting value. \n
 *				- Return Value[8]	: EP1		interrupt's setting value. \n
 *				- Return Value[9]	: EP2		interrupt's setting value. \n
 *				- Return Value[10] : EP3		interrupt's setting value. \n
 *				- Return Value[11] : VBUSENB	interrupt's setting value. \n
 *	@see		NX_UDC_GetInterruptNumber,			NX_UDC_SetInterruptEnable,
 *				NX_UDC_GetInterruptEnable,			NX_UDC_SetInterruptEnable32,
 *													NX_UDC_GetInterruptPending,
 *				NX_UDC_GetInterruptPending32,		NX_UDC_ClearInterruptPending,
 *				NX_UDC_ClearInterruptPending32,		NX_UDC_SetInterruptEnableAll,
 *				NX_UDC_GetInterruptEnableAll,		NX_UDC_GetInterruptPendingAll,
 *				NX_UDC_ClearInterruptPendingAll,	NX_UDC_GetInterruptPendingNumber
 */
U32		NX_UDC_GetInterruptEnable32( void )
{
	register struct NX_UDC_RegisterSet *pRegister;
	register U32	regvalue;

	NX_ASSERT( CNULL != __g_pRegister );
	pRegister = (struct NX_UDC_RegisterSet *)__g_pRegister;

	regvalue = pRegister->SCR;

	return	(U32)(	((pRegister->INTENB & 0x0F)	<< 7) |			// EP0 ~ EP3
					((pRegister->VBUSINTENB & 0x01) << 11)|			// VBUSENB
					((regvalue>> 0) & 0x07)		<< 0	|			// RESET, RESUME, SUSPEND
					((regvalue>>10) & 0x03)		<< 3	|			// VBUSON/OFF
					((regvalue>> 6) & 0x01)		<< 5	|			// SPEEDDETECT
					((regvalue>> 8) & 0x01)		<< 6	);		// ERROR
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *	@param[in]	IntNum	Interrupt Number ( 0: Reset, 1: Resume, 2: Suspend, 3: VBUS On, 4: VBUS Off,
 *						5: Speed detect, 6: Error, 7: EP0, 8: EP1, 9: EP2, 10:EP3, 11: VBUS ENB ).
 *	@return		\b CTRUE	indicates that Pending is seted. \n
 *				\b CFALSE	indicates that Pending is Not Seted.
 *	@remark		USB Device have a many interrupt source. Each Pend have a many pending source. \n
 *				Interrupt Interface just setting the each interrupt enabe bit and
 *				pending bit. so User should check and clear the pending source bit. \n
 *	@see		NX_UDC_GetInterruptNumber,			NX_UDC_SetInterruptEnable,
 *				NX_UDC_GetInterruptEnable,			NX_UDC_SetInterruptEnable32,
 *				NX_UDC_GetInterruptEnable32,		
 *				NX_UDC_GetInterruptPending32,		NX_UDC_ClearInterruptPending,
 *				NX_UDC_ClearInterruptPending32,		NX_UDC_SetInterruptEnableAll,
 *				NX_UDC_GetInterruptEnableAll,		NX_UDC_GetInterruptPendingAll,
 *				NX_UDC_ClearInterruptPendingAll,	NX_UDC_GetInterruptPendingNumber
 */
CBOOL	NX_UDC_GetInterruptPending( S32 IntNum )
{
	const U16 TR_ERROR_BIT_MASK		= 0x3000;
	const U16 SSR_ERROR_BIT_MASK	= 0xFC80;

	const U16 EP3I_BITPOS	= 0x03;
	const U16 EP2I_BITPOS	= 0x02;
	const U16 EP1I_BITPOS	= 0x01;
	const U16 EP0I_BITPOS	= 0x00;
	const U16 VBUSOFF_BITPOS= 0x09;
	const U16 VBUSON_BITPOS = 0x08;
	const U16 SDE_BITPOS	= 0x03;
	const U16 HFRM_BITPOS	= 0x02;
	const U16 HFSUSP_BITPOS = 0x01;
	const U16 HFRES_BITPOS	= 0x00;

	NX_ASSERT( NX_UDC_INT_VBUSENB >= IntNum && 0 <= IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	switch( IntNum )
	{
		case NX_UDC_INT_EP0:

				return (CBOOL)( (__g_pRegister->INTPEND >> EP0I_BITPOS) & 0x01 );
				//break;

		case NX_UDC_INT_EP1:

				return (CBOOL)( (__g_pRegister->INTPEND >> EP1I_BITPOS) & 0x01 );
				//break;

		case NX_UDC_INT_EP2:

				return (CBOOL)( (__g_pRegister->INTPEND >> EP2I_BITPOS) & 0x01 );
				//break;

		case NX_UDC_INT_EP3:

				return (CBOOL)( (__g_pRegister->INTPEND >> EP3I_BITPOS) & 0x01 );
				//break;

		case NX_UDC_INT_ERROR:

				if( (__g_pRegister->TR & TR_ERROR_BIT_MASK) ||
					(__g_pRegister->SSR & SSR_ERROR_BIT_MASK)
					)
				{
					return CTRUE;
				}

				return CFALSE;
				//break;

		case NX_UDC_INT_VBUSON:

				return (CBOOL)( (__g_pRegister->SSR >> VBUSON_BITPOS) & 0x01 );
				//break;

		case NX_UDC_INT_VBUSOFF:

				return (CBOOL)( (__g_pRegister->SSR >> VBUSOFF_BITPOS) & 0x01 );
				//break;

		case NX_UDC_INT_VBUSENB:

				return (CBOOL)( __g_pRegister->VBUSINTPEND & 0x01 );
				//break

		case NX_UDC_INT_SPEEDDETECT:

				return (CBOOL)( (__g_pRegister->SSR >> SDE_BITPOS) & 0x01 );
				//break;

		case NX_UDC_INT_RESUME:

				return (CBOOL)( (__g_pRegister->SSR >> HFRM_BITPOS) & 0x01 );
				//break;

		case NX_UDC_INT_RESET:

				return (CBOOL)( (__g_pRegister->SSR >> HFRES_BITPOS) & 0x01 );
				//break;

		case NX_UDC_INT_SUSPEND:

				return (CBOOL)( (__g_pRegister->SSR >> HFSUSP_BITPOS) & 0x01 );
				//break;

		default:
				NX_ASSERT( CFALSE );
				break;
	}
	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt pending bit.
 *	@return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bit is NOT occured. \n
 *				- Return Value[0]	: RESET		pending state. \n
 *				- Return Value[1]	: RESUME	pending state. \n
 *				- Return Value[2]	: SUSPEND	pending state. \n
 *				- Return Value[3]	: VBUSON	pending state. \n
 *				- Return Value[4]	: VBUSOFF	pending state. \n
 *				- Return Value[5]	: SPEEDDETECT pending state. \n
 *				- Return Value[6]	: ERROR		pending state. \n
 *				- Return Value[7]	: EP0		pending state. \n
 *				- Return Value[8]	: EP1		pending state. \n
 *				- Return Value[9]	: EP2		pending state. \n
 *				- Return Value[10]	: EP3		pending state. \n
 *				- Return Value[11]	: VBUSENB	pending state. \n
 *	@see		NX_UDC_GetInterruptNumber,			NX_UDC_SetInterruptEnable,
 *				NX_UDC_GetInterruptEnable,			NX_UDC_SetInterruptEnable32,
 *				NX_UDC_GetInterruptEnable32,		NX_UDC_GetInterruptPending,
 *													NX_UDC_ClearInterruptPending,
 *				NX_UDC_ClearInterruptPending32,		NX_UDC_SetInterruptEnableAll,
 *				NX_UDC_GetInterruptEnableAll,		NX_UDC_GetInterruptPendingAll,
 *				NX_UDC_ClearInterruptPendingAll,	NX_UDC_GetInterruptPendingNumber
 */
U32		NX_UDC_GetInterruptPending32( void )
{
	register struct NX_UDC_RegisterSet *pRegister;
	register U32	regvalue;

	NX_ASSERT( CNULL != __g_pRegister );
	pRegister = (struct NX_UDC_RegisterSet *)__g_pRegister;

	regvalue = pRegister->SSR;

	return (U32)( ((pRegister->INTPEND & 0x0F)<<7) |			// EP0 ~ EP3
					((pRegister->VBUSINTPEND & 0x01)<<11) |		// VBUSENB
					((regvalue>>0) & 0x07) << 0 |				// RESET, RESUME, SUSPEND
					((regvalue>>8) & 0x03) << 3 |				// VBUSON/OFF
					((regvalue>>3) & 0x01) << 5	);				// SPEEDDETECT
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	IntNum	Interrupt Number ( 0: Reset, 1: Resume, 2: Suspend, 3: VBUS On, 4: VBUS Off,
 *						5: Speed detect, 6: Error, 7: EP0, 8: EP1, 9: EP2, 10: EP3, 11: VBUS ENB ).
 *	@return		None.
 *	@remark		USB Device have a many interrupt source. Each Pend have a many pending source. \n
 *				Interrupt Interface just setting the each interrupt enabe bit and
 *				pending bit. so User should check and clear the pending source bit. \n
 *	@see		NX_UDC_GetInterruptNumber,			NX_UDC_SetInterruptEnable,
 *				NX_UDC_GetInterruptEnable,			NX_UDC_SetInterruptEnable32,
 *				NX_UDC_GetInterruptEnable32,		NX_UDC_GetInterruptPending,
 *				NX_UDC_GetInterruptPending32,		
 *				NX_UDC_ClearInterruptPending32,		NX_UDC_SetInterruptEnableAll,
 *				NX_UDC_GetInterruptEnableAll,		NX_UDC_GetInterruptPendingAll,
 *				NX_UDC_ClearInterruptPendingAll,	NX_UDC_GetInterruptPendingNumber
 */
void	NX_UDC_ClearInterruptPending( S32 IntNum )
{
	const U16 EP3I_BITPOS	= 0x03;
	const U16 EP2I_BITPOS	= 0x02;
	const U16 EP1I_BITPOS	= 0x01;
	const U16 EP0I_BITPOS	= 0x00;
	const U16 VBUSOFF_BITPOS= 0x09;
	const U16 VBUSON_BITPOS = 0x08;
	const U16 SDE_BITPOS	= 0x03;
	const U16 HFRM_BITPOS	= 0x02;
	const U16 HFSUSP_BITPOS = 0x01;
	const U16 HFRES_BITPOS	= 0x00;

	NX_ASSERT( NX_UDC_INT_VBUSENB >= IntNum && 0 <= IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	switch( IntNum )
	{
		case NX_UDC_INT_EP0:

				__g_pRegister->INTPEND = 1 << EP0I_BITPOS;
				break;

		case NX_UDC_INT_EP1:

				__g_pRegister->INTPEND = 1 << EP1I_BITPOS;
				break;

		case NX_UDC_INT_EP2:

				__g_pRegister->INTPEND = 1 << EP2I_BITPOS;
				break;

		case NX_UDC_INT_EP3:

				__g_pRegister->INTPEND = 1 << EP3I_BITPOS;
				break;

		case NX_UDC_INT_ERROR:

				// Nothing
				break;

		case NX_UDC_INT_VBUSON:

				__g_pRegister->SSR = 1 << VBUSON_BITPOS;
				break;

		case NX_UDC_INT_VBUSOFF:

				__g_pRegister->SSR = 1 << VBUSOFF_BITPOS;
				break;

		case NX_UDC_INT_VBUSENB:

				__g_pRegister->VBUSINTPEND = 0x01;
				break;

		case NX_UDC_INT_SPEEDDETECT:

				__g_pRegister->SSR = 1 << SDE_BITPOS;
				break;

		case NX_UDC_INT_RESUME:

				__g_pRegister->SSR = 1 << HFRM_BITPOS;
				break;

		case NX_UDC_INT_RESET:

				__g_pRegister->SSR = 1 << HFRES_BITPOS;
				break;

		case NX_UDC_INT_SUSPEND:

				__g_pRegister->SSR = 1 << HFSUSP_BITPOS;
				break;

		default:
				NX_ASSERT( CFALSE );
				break;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below	\n \n
 *								- PendingFlag[0]	: RESET			pending bit. \n
 *								- PendingFlag[1]	: RESUME		pending bit. \n
 *								- PendingFlag[2]	: SUSPEND		pending bit. \n
 *								- PendingFlag[3]	: VBUSON		pending bit. \n
 *								- PendingFlag[4]	: VBUSOFF		pending bit. \n
 *								- PendingFlag[5]	: SPEEDDETECT	pending bit. \n
 *								- PendingFlag[6]	: ERROR			pending bit. \n
 *								- PendingFlag[7]	: EP0			pending bit. \n
 *								- PendingFlag[8]	: EP1			pending bit. \n
 *								- PendingFlag[9]	: EP2			pending bit. \n
 *								- PendingFlag[10]	: EP3			pending bit. \n
 *								- PendingFlag[11]	: VBUSENB		pending bit. \n
 *	@return		None.
 *	@see		NX_UDC_GetInterruptNumber,			NX_UDC_SetInterruptEnable,
 *				NX_UDC_GetInterruptEnable,			NX_UDC_SetInterruptEnable32,
 *				NX_UDC_GetInterruptEnable32,		NX_UDC_GetInterruptPending,
 *				NX_UDC_GetInterruptPending32,		NX_UDC_ClearInterruptPending,
 *													NX_UDC_SetInterruptEnableAll,
 *				NX_UDC_GetInterruptEnableAll,		NX_UDC_GetInterruptPendingAll,
 *				NX_UDC_ClearInterruptPendingAll,	NX_UDC_GetInterruptPendingNumber
 */
void	NX_UDC_ClearInterruptPending32( U32 PendingFlag )
{
	register struct NX_UDC_RegisterSet *pRegister;

	NX_ASSERT( CNULL != __g_pRegister );
	pRegister = (struct NX_UDC_RegisterSet *)__g_pRegister;

	pRegister->INTPEND		= ((PendingFlag >> 7) & 0xF);			// EP0 ~ EP3
	pRegister->VBUSINTPEND	= ((PendingFlag >> 11) & 0x1);			// VBUSENB
	pRegister->SSR			=(((PendingFlag>>0) & 0x07) << 0 |		// RESET, RESUME, SUSPEND
								((PendingFlag>>3) & 0x03) << 8 |	// VBUSON/OFF
								((PendingFlag>>5) & 0x01) << 3	);	// SPEEDDETECT
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enables or disables.
 *	@param[in]	Enable	\b CTRUE	indicates that Set to all interrupt enable. \n
 *						\b CFALSE	indicates that Set to all interrupt disable.
 *	@return		None.
 *	@remark		USB Device have a many interrupt source. Each Pend have a many pending source. \n
 *				Interrupt Interface just setting the each interrupt enabe bit and
 *				pending bit. so User should check and clear the pending source bit. \n
 *	@see		NX_UDC_GetInterruptNumber,			NX_UDC_SetInterruptEnable,
 *				NX_UDC_GetInterruptEnable,			NX_UDC_SetInterruptEnable32,
 *				NX_UDC_GetInterruptEnable32,		NX_UDC_GetInterruptPending,
 *				NX_UDC_GetInterruptPending32,		NX_UDC_ClearInterruptPending,
 *				NX_UDC_ClearInterruptPending32,	
 *				NX_UDC_GetInterruptEnableAll,		NX_UDC_GetInterruptPendingAll,
 *				NX_UDC_ClearInterruptPendingAll,	NX_UDC_GetInterruptPendingNumber
 */
void	NX_UDC_SetInterruptEnableAll( CBOOL Enable )
{
	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	if( Enable )
	{
		__g_pRegister->INTENB = 0x07;
		__g_pRegister->SCR	|= 0x0D43;
		__g_pRegister->VBUSINTENB = 0x01;
	}
	else
	{
		__g_pRegister->INTENB = 0x00;
		__g_pRegister->SCR	&= ~0x0D43;
		__g_pRegister->VBUSINTENB = 0x00;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enable or not.
 *	@return		\b CTRUE	indicates that At least one( or more ) interrupt is enabled. \n
 *				\b CFALSE	indicates that All interrupt is disabled.
 *	@remark		USB Device have a many interrupt source. Each Pend have a many pending source. \n
 *				Interrupt Interface just setting the each interrupt enabe bit and
 *				pending bit. so User should check and clear the pending source bit. \n
 *	@see		NX_UDC_GetInterruptNumber,			NX_UDC_SetInterruptEnable,
 *				NX_UDC_GetInterruptEnable,			NX_UDC_SetInterruptEnable32,
 *				NX_UDC_GetInterruptEnable32,		NX_UDC_GetInterruptPending,
 *				NX_UDC_GetInterruptPending32,		NX_UDC_ClearInterruptPending,
 *				NX_UDC_ClearInterruptPending32,		NX_UDC_SetInterruptEnableAll,
 *													NX_UDC_GetInterruptPendingAll,
 *				NX_UDC_ClearInterruptPendingAll,	NX_UDC_GetInterruptPendingNumber
 */
CBOOL	NX_UDC_GetInterruptEnableAll( void )
{
	const U16 VBUSINTENB_MASK	= 0x01;
	const U16 INTENB_MASK		= 0x07;
	const U16 SCR_MASK			= 0x0D43;

	NX_ASSERT( CNULL != __g_pRegister );

	if( (__g_pRegister->INTENB & INTENB_MASK) || (__g_pRegister->SCR & SCR_MASK)	||
		(__g_pRegister->VBUSINTENB & VBUSINTENB_MASK)
		)
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are pended or not.
 *	@return		\b CTRUE	indicates that At least one( or more ) pending is seted. \n
 *				\b CFALSE	indicates that All pending is NOT seted.
 *	@remark		USB Device have a many interrupt source. Each Pend have a many pending source. \n
 *				Interrupt Interface just setting the each interrupt enabe bit and
 *				pending bit. so User should check and clear the pending source bit. \n
 *	@see		NX_UDC_GetInterruptNumber,			NX_UDC_SetInterruptEnable,
 *				NX_UDC_GetInterruptEnable,			NX_UDC_SetInterruptEnable32,
 *				NX_UDC_GetInterruptEnable32,		NX_UDC_GetInterruptPending,
 *				NX_UDC_GetInterruptPending32,		NX_UDC_ClearInterruptPending,
 *				NX_UDC_ClearInterruptPending32,		NX_UDC_SetInterruptEnableAll,
 *				NX_UDC_GetInterruptEnableAll,		
 *				NX_UDC_ClearInterruptPendingAll,	NX_UDC_GetInterruptPendingNumber
 */
CBOOL	NX_UDC_GetInterruptPendingAll( void )
{
	const U16 VBUSINTPEND_MASK = 0x01;
	const U16 INTPEND_MASK = 0x07;
	const U16 TR_ERROR_BIT_MASK	= 0x3000;
	const U16 SSR_MASK = 0xFF8F;

	NX_ASSERT( CNULL != __g_pRegister );

	if( (__g_pRegister->INTPEND & INTPEND_MASK) ||
		(__g_pRegister->TR & TR_ERROR_BIT_MASK) ||
		(__g_pRegister->SSR & SSR_MASK)		||
		(__g_pRegister->VBUSINTPEND & VBUSINTPEND_MASK)
		)
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@return		None.
 *	@remark		USB Device have a many interrupt source. Each Pend have a many pending
 *				source. \n
 *				Interrupt Interface just setting the each interrupt enabe bit and
 *				pending bit. so User should check and clear the pending source bit. \n
 *	@see		NX_UDC_GetInterruptNumber,			NX_UDC_SetInterruptEnable,
 *				NX_UDC_GetInterruptEnable,			NX_UDC_SetInterruptEnable32,
 *				NX_UDC_GetInterruptEnable32,		NX_UDC_GetInterruptPending,
 *				NX_UDC_GetInterruptPending32,		NX_UDC_ClearInterruptPending,
 *				NX_UDC_ClearInterruptPending32,		NX_UDC_SetInterruptEnableAll,
 *				NX_UDC_GetInterruptEnableAll,		NX_UDC_GetInterruptPendingAll,
 *													NX_UDC_GetInterruptPendingNumber
 */
void	NX_UDC_ClearInterruptPendingAll( void )
{
	const U16 VBUSINTPEND_MASK	= 0x01;
	const U16 INTPEND_MASK		= 0x07;
	const U16 TR_ERROR_BIT_MASK	= 0x3000;
	const U16 SSR_MASK			= 0xFF8F;

	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->INTPEND =	INTPEND_MASK;
	__g_pRegister->TR		|= TR_ERROR_BIT_MASK;
	__g_pRegister->SSR		=	SSR_MASK;
	__g_pRegister->VBUSINTPEND = VBUSINTPEND_MASK;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number which has the most prority of pended interrupts
 *	@return		Pending Number( If all pending is not set then return -1 ).\n
 *				0: Reset, 1: Resume, 2: Suspend, 3: VBUS On, 4: VBUS Off,
 *				5: Speed detect, 6: Error, 7: EP0, 8: EP1, 9: EP2, 10: EP3, 11: VBUS ENB.
 *	@remark		USB Device have a many interrupt source. Each Pend have a many pending
 *				source. \n
 *				Interrupt Interface just setting the each interrupt enabe bit and
 *				pending bit. so User should check and clear the pending source bit. \n
 *	@see		NX_UDC_GetInterruptNumber,			NX_UDC_SetInterruptEnable,
 *				NX_UDC_GetInterruptEnable,			NX_UDC_SetInterruptEnable32,
 *				NX_UDC_GetInterruptEnable32,		NX_UDC_GetInterruptPending,
 *				NX_UDC_GetInterruptPending32,		NX_UDC_ClearInterruptPending,
 *				NX_UDC_ClearInterruptPending32,		NX_UDC_SetInterruptEnableAll,
 *				NX_UDC_GetInterruptEnableAll,		NX_UDC_GetInterruptPendingAll,
 *				NX_UDC_ClearInterruptPendingAll	
 */
S32		NX_UDC_GetInterruptPendingNumber( void )	// -1 if None
{
	const U16 TR_ERROR_BIT_MASK		= 0x3000;
	const U16 SSR_ERROR_BIT_MASK	= 0xFC80;

	const U16 HFRM_MASK			= 1 << 2;
	const U16 HFSUSP_MASK		= 1 << 1;
	const U16 HFRES_MASK		= 1 << 0;
	const U16 VBUSON_MASK		= 1 << 8;
	const U16 VBUSOFF_MASK		= 1 << 9;
	const U16 SDE_MASK			= 1 << 3;
	const U16 VBUSINTENB_MASK	= 1 << 11;

	const U16 SCR_EIE_MASK			= 1 << 8;
	const U16 SCR_HSUSPE_HRESE_MASK = 0x03;
	const U16 SCR_SPDEN_MASK		= 0x01 << 6;
	const U16 SCR_VBUSOFF_ON_MASK	= 0x03 << 10;

	register struct NX_UDC_RegisterSet	*pRegister;
	register U16 IntPend;
	register U16 IntEnb;
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	IntPend = pRegister->SSR | ((pRegister->VBUSINTPEND)<<11) ;
	IntEnb	= pRegister->SCR;

	IntEnb = ( ((IntEnb & SCR_VBUSOFF_ON_MASK)>>2) | ((IntEnb & SCR_SPDEN_MASK)>>3) |
				(IntEnb & SCR_HSUSPE_HRESE_MASK)	| ((pRegister->VBUSINTENB)<<11) );

	IntPend &=	(IntEnb | HFRM_MASK);	// resume always occur;

	if( 0 != IntPend )
	{
		if( IntPend & VBUSINTENB_MASK )
			return NX_UDC_INT_VBUSENB;

		if( IntPend & HFRES_MASK )
			return NX_UDC_INT_RESET;

		if( IntPend & HFRM_MASK )
			return NX_UDC_INT_RESUME;

		if( IntPend & HFSUSP_MASK )
			return NX_UDC_INT_SUSPEND;

		if( IntPend & VBUSON_MASK )
			return NX_UDC_INT_VBUSON;

		if( IntPend & VBUSOFF_MASK )
			return NX_UDC_INT_VBUSOFF;

		if( IntPend & SDE_MASK )
			return NX_UDC_INT_SPEEDDETECT;

		if( (pRegister->TR & TR_ERROR_BIT_MASK) || (IntPend & SSR_ERROR_BIT_MASK) )
			return (pRegister->SCR & SCR_EIE_MASK) ? NX_UDC_INT_ERROR : -1;
	}
	else	// EP0 ~ EP3
	{
		const S32	EPxPEND[16] = { -1,				NX_UDC_INT_EP0, NX_UDC_INT_EP1, NX_UDC_INT_EP0,
									NX_UDC_INT_EP2, NX_UDC_INT_EP0, NX_UDC_INT_EP1, NX_UDC_INT_EP0,
									NX_UDC_INT_EP3, NX_UDC_INT_EP0, NX_UDC_INT_EP1, NX_UDC_INT_EP0,
									NX_UDC_INT_EP2, NX_UDC_INT_EP0, NX_UDC_INT_EP1, NX_UDC_INT_EP0	};

		IntPend = ((pRegister->INTPEND & pRegister->INTENB) & 0x0F) ;

		return EPxPEND[IntPend];
	}

	return -1;
}

//------------------------------------------------------------------------------
// Clock Control Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Set a PCLK mode
 *	@param[in]	mode	PCLK mode
 *	@return		None.
 *	@see										NX_UDC_GetClockPClkMode,
 *				NX_UDC_SetClockBClkMode,		NX_UDC_GetClockBClkMode,
 *				NX_UDC_SetClockSource,			NX_UDC_GetClockSource,
 *				NX_UDC_SetClockDivisor,			NX_UDC_GetClockDivisor,
 *				NX_UDC_SetClockDivisorEnable,	NX_UDC_GetClockDivisorEnable
 */
void			NX_UDC_SetClockPClkMode( NX_PCLKMODE mode )
{
	const U32 PCLKMODE_POS	=	3;

	register U32 regvalue;
	register struct NX_UDC_RegisterSet* pRegister;

	U32 clkmode=0;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	switch(mode)
	{
		case NX_PCLKMODE_DYNAMIC:	clkmode = 0;		break;
		case NX_PCLKMODE_ALWAYS:	clkmode = 1;		break;
		default: NX_ASSERT( CFALSE );
	}

	regvalue = pRegister->CLKENB;

	regvalue &= ~(1UL<<PCLKMODE_POS);
	regvalue |= ( clkmode & 0x01 ) << PCLKMODE_POS;

	pRegister->CLKENB = regvalue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current PCLK mode
 *	@return		Current PCLK mode
 *	@see		NX_UDC_SetClockPClkMode,
 *				NX_UDC_SetClockBClkMode,		NX_UDC_GetClockBClkMode,
 *				NX_UDC_SetClockSource,			NX_UDC_GetClockSource,
 *				NX_UDC_SetClockDivisor,			NX_UDC_GetClockDivisor,
 *				NX_UDC_SetClockDivisorEnable,	NX_UDC_GetClockDivisorEnable
 */
NX_PCLKMODE	NX_UDC_GetClockPClkMode( void )
{
	const U32 PCLKMODE_POS	= 3;

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->CLKENB & ( 1UL << PCLKMODE_POS ) )
	{
		return NX_PCLKMODE_ALWAYS;
	}

	return	NX_PCLKMODE_DYNAMIC;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a BCLK mode
 *	@param[in]	mode	BCLK mode
 *	@return		None.
 *	@see		NX_UDC_SetClockPClkMode,		NX_UDC_GetClockPClkMode,
 *												NX_UDC_GetClockBClkMode,
 *				NX_UDC_SetClockSource,			NX_UDC_GetClockSource,
 *				NX_UDC_SetClockDivisor,			NX_UDC_GetClockDivisor,
 *				NX_UDC_SetClockDivisorEnable,	NX_UDC_GetClockDivisorEnable
 */
void			NX_UDC_SetClockBClkMode( NX_BCLKMODE mode )
{
	register U32 regvalue;
	register struct NX_UDC_RegisterSet* pRegister;
	U32 clkmode=0;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	switch(mode)
	{
		case NX_BCLKMODE_DISABLE:	clkmode = 0;		break;
		case NX_BCLKMODE_DYNAMIC:	clkmode = 2;		break;
		case NX_BCLKMODE_ALWAYS:	clkmode = 3;		break;
		default: NX_ASSERT( CFALSE );
	}

	regvalue = pRegister->CLKENB;
	regvalue &= ~(0x3);
	regvalue |= clkmode & 0x3;
	pRegister->CLKENB = regvalue;
}


//------------------------------------------------------------------------------
/**
 *	@brief		Get current BCLK mode
 *	@return		Current BCLK mode
 *	@see		NX_UDC_SetClockPClkMode,		NX_UDC_GetClockPClkMode,
 *				NX_UDC_SetClockBClkMode,
 *				NX_UDC_SetClockSource,			NX_UDC_GetClockSource,
 *				NX_UDC_SetClockDivisor,			NX_UDC_GetClockDivisor,
 *				NX_UDC_SetClockDivisorEnable,	NX_UDC_GetClockDivisorEnable
 */
NX_BCLKMODE	NX_UDC_GetClockBClkMode( void )
{
	const U32 BCLKMODE	= 3UL<<0;

	NX_ASSERT( CNULL != __g_pRegister );


	switch( __g_pRegister->CLKENB & BCLKMODE )
	{
		case 0 :	return NX_BCLKMODE_DISABLE;
		case 2 :	return NX_BCLKMODE_DYNAMIC;
		case 3 :	return NX_BCLKMODE_ALWAYS;
	}

	NX_ASSERT( CFALSE );
	return NX_BCLKMODE_DISABLE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock source of clock generator
 *	@param[in]	Index	Select clock generator( 0 : clock generator 0 );
 *	@param[in]	ClkSrc	Select clock source of clock generator( 3: External Clock).\n
 *	@remarks	USB Device controller have one clock generator. so \e Index must set to 0.\n
 *				USB Device only support external clock. so \e ClkSrc must set to 3.
 *	@return		None.
 *	@see		NX_UDC_SetClockPClkMode,		NX_UDC_GetClockPClkMode,
 *				NX_UDC_SetClockBClkMode,		NX_UDC_GetClockBClkMode,
 *												NX_UDC_GetClockSource,
 *				NX_UDC_SetClockDivisor,			NX_UDC_GetClockDivisor,
 *				NX_UDC_SetClockDivisorEnable,	NX_UDC_GetClockDivisorEnable
 */
void			NX_UDC_SetClockSource( U32 Index, U32 ClkSrc )
{
	const U32 CLKSRCSEL_POS		= 2;
	const U32 CLKSRCSEL_MASK	= 0x03 << CLKSRCSEL_POS;

	register U32 ReadValue;

	NX_ASSERT( 1 > Index );
	NX_ASSERT( 3 == ClkSrc );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->CLKGEN;

	ReadValue &= ~CLKSRCSEL_MASK;
	ReadValue |= ClkSrc << CLKSRCSEL_POS;

	__g_pRegister->CLKGEN = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get clock source of specified clock generator.
 *	@param[in]	Index	Select clock generator( 0 : clock generator 0 );
 *	@return		Clock source of clock generator( 3: External Clock ) \n
 *	@remarks	USB Device controller have one clock generator. so \e Index must set to 0.\n
 *				USB Device only support external clock. so \e ClkSrc must set to 3.
 *	@see		NX_UDC_SetClockPClkMode,		NX_UDC_GetClockPClkMode,
 *				NX_UDC_SetClockBClkMode,		NX_UDC_GetClockBClkMode,
 *				NX_UDC_SetClockSource,
 *				NX_UDC_SetClockDivisor,			NX_UDC_GetClockDivisor,
 *				NX_UDC_SetClockDivisorEnable,	NX_UDC_GetClockDivisorEnable
 */
U32				NX_UDC_GetClockSource( U32 Index )
{
	const U32 CLKSRCSEL_POS		= 2;
	const U32 CLKSRCSEL_MASK	= 0x03 << CLKSRCSEL_POS;

	NX_ASSERT( 1 > Index );
	NX_ASSERT( CNULL != __g_pRegister );

	return ( __g_pRegister->CLKGEN & CLKSRCSEL_MASK ) >> CLKSRCSEL_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock divisor of specified clock generator.
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0 );
 *	@param[in]	Divisor		Clock divisor ( 1 ~ 64 ).
 *	@return		None.
 *	@remarks	USB Device controller have one clock generator. so \e Index must set to 0.\n
 *	@see		NX_UDC_SetClockPClkMode,		NX_UDC_GetClockPClkMode,
 *				NX_UDC_SetClockBClkMode,		NX_UDC_GetClockBClkMode,
 *				NX_UDC_SetClockSource,			NX_UDC_GetClockSource,
 *												NX_UDC_GetClockDivisor,
 *				NX_UDC_SetClockDivisorEnable,	NX_UDC_GetClockDivisorEnable
 */
void			NX_UDC_SetClockDivisor( U32 Index, U32 Divisor )
{
	const U32 CLKDIV_POS	=	5;
	const U32 CLKDIV_MASK	=	0x3F << CLKDIV_POS;

	register U32 ReadValue;

	NX_ASSERT( 1 > Index );
	NX_ASSERT( 1 <= Divisor && Divisor <= 64 );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue	=	__g_pRegister->CLKGEN;

	ReadValue	&= ~CLKDIV_MASK;
	ReadValue	|= (Divisor-1) << CLKDIV_POS;

	__g_pRegister->CLKGEN = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get clock divisor of specified clock generator.
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0 );
 *	@return		Clock divisor ( 1 ~ 64 ).
 *	@remarks	USB Device controller have one clock generator. so \e Index must set to 0.\n
 *	@see		NX_UDC_SetClockPClkMode,		NX_UDC_GetClockPClkMode,
 *				NX_UDC_SetClockBClkMode,		NX_UDC_GetClockBClkMode,
 *				NX_UDC_SetClockSource,			NX_UDC_GetClockSource,
 *				NX_UDC_SetClockDivisor,
 *				NX_UDC_SetClockDivisorEnable,	NX_UDC_GetClockDivisorEnable
 */
U32				NX_UDC_GetClockDivisor( U32 Index )
{
	const U32 CLKDIV_POS	=	5;
	const U32 CLKDIV_MASK	=	0x3F << CLKDIV_POS;

	NX_ASSERT( 2 > Index );
	NX_ASSERT( CNULL != __g_pRegister );

	return ((__g_pRegister->CLKGEN & CLKDIV_MASK) >> CLKDIV_POS) + 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock generator's operation
 *	@param[in]	Enable	\b CTRUE	indicates that Enable of clock generator. \n
 *						\b CFALSE	indicates that Disable of clock generator.
 *	@return		None.
 *	@see		NX_UDC_SetClockPClkMode,		NX_UDC_GetClockPClkMode,
 *				NX_UDC_SetClockBClkMode,		NX_UDC_GetClockBClkMode,
 *				NX_UDC_SetClockSource,			NX_UDC_GetClockSource,
 *				NX_UDC_SetClockDivisor,			NX_UDC_GetClockDivisor,
 *												NX_UDC_GetClockDivisorEnable
 */
void			NX_UDC_SetClockDivisorEnable( CBOOL Enable )
{
	const U32	CLKGENENB_POS	=	2;
	const U32	CLKGENENB_MASK	=	1UL << CLKGENENB_POS;

	register U32 ReadValue;

	NX_ASSERT( (0==Enable) ||(1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue	=	__g_pRegister->CLKENB;

	ReadValue	&=	~CLKGENENB_MASK;
	ReadValue	|= (U32)Enable << CLKGENENB_POS;

	__g_pRegister->CLKENB	=	ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get status of clock generator's operation
 *	@return		\b CTRUE	indicates that Clock generator is enabled.\n
 *				\b CFALSE	indicates that Clock generator is disabled.
 *	@see		NX_UDC_SetClockPClkMode,		NX_UDC_GetClockPClkMode,
 *				NX_UDC_SetClockBClkMode,		NX_UDC_GetClockBClkMode,
 *				NX_UDC_SetClockSource,			NX_UDC_GetClockSource,
 *				NX_UDC_SetClockDivisor,			NX_UDC_GetClockDivisor,
 *				NX_UDC_SetClockDivisorEnable
 */
CBOOL			NX_UDC_GetClockDivisorEnable( void )
{
	const U32	CLKGENENB_POS	=	2;
	const U32	CLKGENENB_MASK	=	1UL << CLKGENENB_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return	(CBOOL)( (__g_pRegister->CLKENB & CLKGENENB_MASK) >> CLKGENENB_POS );
}


//------------------------------------------------------------------------------
// DMA Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get DMA peripheral index of USB device controller's Endpoint 1
 *	@return		DMA peripheral index of USB device controller's Endpoint 1
 *	@see								NX_UDC_GetDMAIndex_EP2,
 *				NX_UDC_GetDMAIndex_EP3,	NX_UDC_GetDMABusWidth
 */
U32		NX_UDC_GetDMAIndex_EP1( void )
{
	return DMAINDEX_OF_USBDEVICE_MODULE_EP1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get DMA peripheral index of USB device controller's Endpoint 2
 *	@return		DMA peripheral index of USB device controller's Endpoint 2
 *	@see		NX_UDC_GetDMAIndex_EP1,
 *				NX_UDC_GetDMAIndex_EP3,	NX_UDC_GetDMABusWidth
 */
U32		NX_UDC_GetDMAIndex_EP2( void )
{
	return DMAINDEX_OF_USBDEVICE_MODULE_EP2;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get DMA peripheral index of USB device controller's Endpoint 3
 *	@return		DMA peripheral index of USB device controller's Endpoint 3
 *	@see		NX_UDC_GetDMAIndex_EP1,	NX_UDC_GetDMAIndex_EP2,
 *										NX_UDC_GetDMABusWidth
 */
U32		NX_UDC_GetDMAIndex_EP3( void )
{
	return DMAINDEX_OF_USBDEVICE_MODULE_EP3;
}


//------------------------------------------------------------------------------
/**
 *	@brief		Get bus width of USB device.
 *	@return		DMA bus width of USB device.
 *	@see		NX_UDC_GetDMAIndex_EP1,	NX_UDC_GetDMAIndex_EP2,
 *				NX_UDC_GetDMAIndex_EP3
 */
U32		NX_UDC_GetDMABusWidth( void )
{
	return 16;
}

//------------------------------------------------------------------------------
// Non-Indexed Function
//------------------------------------------------------------------------------
/**
 *	@brief		Indexing a specific endpoint.
 *	@param[in]	dwIndex	Endpoint index ( 0, 1, 2, 3 )
 *	@return		None.
 *	@remark		In most cases, setting the index register value should preced any other
 *				operation.
 *	@see		NX_UDC_GetIndexEP
 */
void	NX_UDC_SetIndexEP( U32 dwIndex )
{
	NX_ASSERT( 4 > dwIndex );
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->INDEX = dwIndex;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current indexing endpoint
 *	@return		Current indexing endpoint
 *	@see		NX_UDC_SetIndexEP
 */
U32		NX_UDC_GetIndexEP( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return __g_pRegister->INDEX;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get address of USB device
 *	@return		Address of USB device
 *	@remarks	The address is transferred from USB Host through "set_address" command.
 */
U32		NX_UDC_GetFunctionAddress( void )
{
	const U16	FADDR_MASK = (1<<7)-1;

	NX_ASSERT( CNULL != __g_pRegister );

	return __g_pRegister->FADDR & FADDR_MASK;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check frame number is valid or Not.
 *	@return		\b CTRUE	indicates that Frame number is valid. \n
 *				\b CFALSE	indicates that Frame number is \b NOT valid. \n
 *	@remarks	Frame Timer Lock is activated when the device frame timer is locked
 *				to the host frame timer. This bit is set by USB after device receives
 *				two valid SOF.
 *	@see		NX_UDC_IsMissingSOF,	NX_UDC_GetFrameNumber
 */
CBOOL	NX_UDC_IsFrameTimerLock( void )
{
	const U16 FTL_BITPOS	= 14;
	const U16 FTL_MASK		= 1 << FTL_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->FNR & FTL_MASK) >> FTL_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check Start of Packet is Missing or Not.
 *	@return		\b CTRUE	indicates that Start of Packet is missing. \n
 *				\b CFALSE	indicates that Start of Packet is \b NOT missing.
 *	@remarks	This function return CTRUE when frame timer locking between device frame timer and
 *				Host frame timer fails.
 *	@see		NX_UDC_IsFrameTimerLock,	NX_UDC_GetFrameNumber
 */
CBOOL	NX_UDC_IsMissingSOF( void )
{
	const U16 SM_BITPOS	= 13;
	const U16 SM_MASK	= 1 << SM_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->FNR & SM_MASK) >> SM_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get frame number
 *	@return		Frame number
 *	@remarks	This frame number is used for the isochronous transaction.
 *	@see		NX_UDC_IsFrameTimerLock,	NX_UDC_IsMissingSOF
 */
U32		NX_UDC_GetFrameNumber( void )
{
	const U16 FN_MASK = (1<<11)-1;
	NX_ASSERT( CNULL != __g_pRegister );

	return ( __g_pRegister->FNR & FN_MASK );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set direction for each endpoint.
 *	@param[in]	dwEndpoint	Endpoint( 1, 2, 3 )
 *	@param[in]	dwDirection	Direction( 0: Rx, 1: Tx )
 *	@return		None.
 *	@remarks	The direction can't be changed dynamically. Only by new enumeration,
 *				the direction can be altered.\n
 *				The endpoint 0 is bi-directional.
 *	@see		NX_UDC_GetDirection
 */
void	NX_UDC_SetDirection( U32 dwEndpoint, U32 dwDirection )
{
	NX_ASSERT( 1 == dwEndpoint || 2 == dwEndpoint || 3 == dwEndpoint );
	NX_ASSERT( 0 == dwDirection || 1 == dwDirection );
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->EDR &= ~(0x01 << dwEndpoint );
	__g_pRegister->EDR |= (dwDirection << dwEndpoint );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get direction for each endpoint.
 *	@param[in]	dwEndpoint	Endpoint( 1, 2, 3 )
 *	@return		\b 0 indicates that Rx endpoint. \n
 *				\b 1 indicates that Tx endpoint.
 *	@see		NX_UDC_SetDirection
 */
U32	NX_UDC_GetDirection( U32 dwEndpoint )
{
	NX_ASSERT( 1 == dwEndpoint || 2 == dwEndpoint || 3 == dwEndpoint );
	NX_ASSERT( CNULL != __g_pRegister );

	return ((__g_pRegister->EDR >> dwEndpoint) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check VBUS is On or Off.
 *	@return		\b CTRUE	VBUS is On.\n
 *				\b CFALSE	VBUS is Off.
 *	@see		NX_UDC_SetExternalVBUSEnable, NX_UDC_GetExternalVBUSEnable
 */
CBOOL	NX_UDC_IsVBUSOn( void )
{
	const U16 VBUS_BITPOS	= 15;
	const U16 VBUS_MASK		= 1 << VBUS_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->TR & VBUS_MASK) >> VBUS_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Error Status
 *	@return		Error Status
 *	@remarks	If error interrupt is occurred, then check and clear of error state. \n
 *				Error interrupt can occur from many error source. so user should check
 *				which error is occurred by NX_UDC_ERROR enumeration.
 *	@code
 *
 *		S32 pend = NX_UDC_GetInterruptPendingNumber()
 *
 *		if( NX_UDC_INT_ERROR == pend )
 *		{
 *			U32 errstatus = NX_UDC_GetErrorStatus();
 *
 *			if( NX_UDC_ERROR_EBUNDERRUN & errstatus )
 *			{
 *				...
 *			}
 *			...
 *			NX_UDC_ClearErrorStatus( errstatus );
 *		}
 *
 *		NX_UDC_ClearInterruptPending( pend );
 *
 *	@endcode
 *	@see	NX_UDC_ClearErrorStatus, NX_UDC_ERROR
 */
U32		NX_UDC_GetErrorStatus( void )
{
	const U16 TR_ERROR_BIT_MASK		= 0x3000;
	const U16 SSR_ERROR_BIT_MASK	= 0xFC80;

	register struct NX_UDC_RegisterSet* pRegister;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	return (U32)( (pRegister->TR & TR_ERROR_BIT_MASK)<<16 | (pRegister->SSR & SSR_ERROR_BIT_MASK) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear error status.
 *	@param[in]	dwErorrStatus	Error status
 *	@return		None.
 *	@remarks	If error interrupt is occurred, then check and clear of error state. \n
 *				Error interrupt can occur from many error source. so user should check
 *				which error is occurred by NX_UDC_ERROR enumeration.
 *	@code
 *
 *		S32 pend = NX_UDC_GetInterruptPendingNumber()
 *
 *		if( NX_UDC_INT_ERROR == pend )
 *		{
 *			U32 errstatus = NX_UDC_GetErrorStatus();
 *
 *			if( NX_UDC_ERROR_EBUNDERRUN & errstatus )
 *			{
 *				...
 *			}
 *			...
 *			NX_UDC_ClearErrorStatus( errstatus );
 *		}
 *
 *		NX_UDC_ClearInterruptPending( pend );
 *
 *	@endcode
 *	@see		NX_UDC_GetErrorStatus, NX_UDC_ERROR
 */
void	NX_UDC_ClearErrorStatus( U32 dwErorrStatus )
{
	#ifdef NX_DEBUG
	const U16 TR_ERROR_BIT_MASK		= 0x3000;
	const U16 SSR_ERROR_BIT_MASK	= 0xFC80;
	#endif

	register struct NX_UDC_RegisterSet* pRegister;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 0 == (dwErorrStatus & ~( (TR_ERROR_BIT_MASK<<16) | SSR_ERROR_BIT_MASK )))

	pRegister = __g_pRegister;

	pRegister->TR	|= (U16)(dwErorrStatus >> 16);
	pRegister->SSR = (U16)(dwErorrStatus & 0xFFFF);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Setting USB Device's test mode.
 *	@param[in]	bEnable		\b CTRUE ( Test Mode ). \n
 *							\b CFALSE ( None ).
 *	@param[in]	bSendPacket \b CTRUE ( Repetitively transmit the test packets to Host ). \n
 *							\b CFALSE ( None ).
 *	@param[in]	bK			\b CTRUE ( The transceiver port enters into the high speed K state ). \n
 *							\b CFALSE ( None ).
 *	@param[in]	bJ			\b CTRUE ( The transceiver port enters into the high speed J state ). \n
 *							\b CFALSE ( None ).
 *	@param[in]	bHighSpeedReceive	\b CTRUE ( The transceiver enters into the high speed
 *									receive mode and must respond to any IN token with NAK handshake ).\n
 *									\b CFALSE( None ).
 *	@return		None.
 *	@remark		bSendPacket, bK, bJ, bHighSpeedReceive setting is meaningul in test mode.
 *	@see		NX_UDC_GetTestMode
 */
void	NX_UDC_SetTestMode( CBOOL bEnable, CBOOL bSendPacket, CBOOL bK, CBOOL bJ, CBOOL bHighSpeedReceive )
{
	const U16 EUERR_BITPOS	= 13;
	const U16 EUERR_MASK	= 1 << EUERR_BITPOS;

	const U16 PERR_BITPOS	= 12;
	const U16 PERR_MASK		= 1 << PERR_BITPOS;

	const U16	TMD_BITPOS	= 4;
	const U16	TPS_BITPOS	= 3;
	const U16	TKS_BITPOS	= 2;
	const U16	TJS_BITPOS	= 1;
	const U16	TSNS_BITPOS	= 0;

	const U16	TESTMODE_MASK = 0x1F;

	register U16 ReadValue;

	NX_ASSERT( 0 == bEnable || 1 == bEnable );
	NX_ASSERT( 0 == bSendPacket || 1 == bSendPacket );
	NX_ASSERT( 0 == bK || 1 == bK );
	NX_ASSERT( 0 == bJ || 1 == bJ );
	NX_ASSERT( 0 == bHighSpeedReceive || 1 == bHighSpeedReceive );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->TR;

	ReadValue &=	~( EUERR_MASK| PERR_MASK | TESTMODE_MASK);
	ReadValue |= ((bEnable << TMD_BITPOS) | (bSendPacket << TPS_BITPOS) | (bK << TKS_BITPOS) |
					(bJ << TJS_BITPOS) | (bHighSpeedReceive << TSNS_BITPOS) );

	__g_pRegister->TR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check USB device's test mode is enable or disable,
 *	@param[out] pbEnable		\b CTURE ( Test mode ), \b CFALSE ( None )
 *	@param[out] pbSendPacket	\b CTURE ( Test Tx mode Set ), \b CFALSE ( None )
 *	@param[out] pbK				\b CTURE ( Test K mode Set ), \b CFALSE ( None )
 *	@param[out] pbJ				\b CTURE ( Test J mode Set ), \b CFALSE ( None )
 *	@param[out] pbHighSpeedReceive \b CTURE ( Test Rx mode Set ), \b CFALSE ( None )
 *	@return		None.
 *	@see		NX_UDC_SetTestMode
 */
void	NX_UDC_GetTestMode( CBOOL *pbEnable, CBOOL *pbSendPacket, CBOOL *pbK, CBOOL *pbJ, CBOOL *pbHighSpeedReceive )
{
	const U16	TMD_BITPOS	= 4;
	const U16	TPS_BITPOS	= 3;
	const U16	TKS_BITPOS	= 2;
	const U16	TJS_BITPOS	= 1;
	const U16	TSNS_BITPOS = 0;

	const U16	TMD_MASK	= 1 << TMD_BITPOS ;
	const U16	TPS_MASK	= 1 << TPS_BITPOS ;
	const U16	TKS_MASK	= 1 << TKS_BITPOS ;
	const U16	TJS_MASK	= 1 << TJS_BITPOS ;
	const U16	TSNS_MASK	= 1 << TSNS_BITPOS;

	register U16 ReadValue;

	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->TR;

	if( CNULL != pbEnable )
	{
		*pbEnable = (CBOOL)( ReadValue & TMD_MASK ) >> TMD_BITPOS ;
	}

	if( CNULL != pbSendPacket )
	{
		*pbSendPacket = (CBOOL)( ReadValue & TPS_MASK ) >> TPS_BITPOS ;
	}

	if( CNULL != pbK )
	{
		*pbK = (CBOOL)( ReadValue & TKS_MASK ) >> TKS_BITPOS ;
	}

	if( CNULL != pbJ )
	{
		*pbJ = (CBOOL)( ReadValue & TJS_MASK ) >> TJS_BITPOS ;
	}

	if( CNULL != pbHighSpeedReceive )
	{
		*pbHighSpeedReceive = (CBOOL)( ReadValue & TSNS_MASK ) >> TSNS_BITPOS ;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get DP data line state.
 *	@return		\b CTRUE	indicates that DP data line is high. \n
 *				\b CFALSE	indicates that DP data line is low.
 *	@remarks	DP informs the status of D+ line.
 *	@see		NX_UDC_IsDMLineHigh
 */
CBOOL		NX_UDC_IsDPLineHigh( void )
{
	const U16 DP_BITPOS	= 6;
	const U16 DP_MASK	= 1 << DP_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->SSR & DP_MASK) >> DP_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get DM data line state.
 *	@return		\b CTRUE	indicates that DM data line is high. \n
 *				\b CFALSE	indicates that DM data line is low.
 *	@remarks	DM informs the status of D+ line.
 *	@see		NX_UDC_IsDPLineHigh
 */
CBOOL	NX_UDC_IsDMLineHigh( void )
{
	const U16 DM_BITPOS	= 5;
	const U16 DM_MASK	= 1 << DM_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->SSR & DM_MASK) >> DM_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get host speed
 *	@return		\b CTRUE	indicates that host is high speed. \n
 *				\b CFALSE	indicates that host is full speed.
 */
CBOOL	NX_UDC_IsHostSpeedHigh( void )
{
	const U16 HSP_BITPOS	= 4;
	const U16 HSP_MASK		= 1 << HSP_BITPOS;

	return (CBOOL)((__g_pRegister->SSR & HSP_MASK) >> HSP_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set DMA Total Counter Zero Interrupt(DTCZI) Enable or Disable.
 *	@param[in]	bEnable \b CTRUE	indicates that DTCZI enable.\n
 *						\b CFALSE	indicates that DTCZI disable.\n
 *	@return		None.
 *	@remarks	check interrupt condition and source. see \b NX_UDC_SetInterruptEnable
 *	@see		NX_UDC_GetDMATotalCounterZeroInterruptEnable, NX_UDC_SetInterruptEnable
 */
void	NX_UDC_SetDMATotalCounterZeroInterruptEnable( CBOOL bEnable )
{
	const U16 DTZIEN_BITPOS	= 14;
	const U16 DTZIEN_MASK	= 1 << DTZIEN_BITPOS;

	register U16	ReadValue;

	NX_ASSERT( (0 == bEnable) || (1 == bEnable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->SCR;

	ReadValue &= ~DTZIEN_MASK;
	ReadValue |= ((U16)bEnable << DTZIEN_BITPOS);

	__g_pRegister->SCR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get DMA Total Counter Zero Interrupt(DTCZI) is enabled or disabled.
 *	@return		\b CTRUE	indicates that DTCZI is enabled.\n
 *				\b CFALSE	indicates that DTCZI is disabled.\n
 *	@remarks	check interrupt condition and source. see \b NX_UDC_SetInterruptEnable
 *	@see		NX_UDC_SetDMATotalCounterZeroInterruptEnable, NX_UDC_SetInterruptEnable
 */
CBOOL	NX_UDC_GetDMATotalCounterZeroInterruptEnable( void )
{
	const U16 DTZIEN_BITPOS	= 14;
	const U16 DTZIEN_MASK	= 1 << DTZIEN_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->SCR & DTZIEN_MASK) >> DTZIEN_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set dual interrupt mode
 *	@param[in]	bEnable	\b CTRUE	indicates that dual interrupt enable.\n
 *						\b CFALSE	indicates that dual interrupt disable.\n
 *	@return		None.
 *	@remarks	\b Dual means that interrrupt is activated until interrupt source is cleared.
 *	@see		NX_UDC_GetDUALInterruptEnable
 */
void	NX_UDC_SetDUALInterruptEnable( CBOOL bEnable )
{
	const U16 DIEN_BITPOS	= 12;
	const U16 DIEN_MASK		= 1 << DIEN_BITPOS;

	register U16	ReadValue;

	NX_ASSERT( (0 == bEnable) || (1 == bEnable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->SCR;

	ReadValue &= ~DIEN_MASK;
	ReadValue |= ((U32)bEnable << DIEN_BITPOS);

	__g_pRegister->SCR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get dual ineterrupt mode
 *	@return		\b CTRUE	indicates that dual interrupt is enabled.\n
 *				\b CFALSE	indicates that dual interrupt is disabled.\n
 *	@remarks	\b Dual means that interrrupt is activated until interrupt source is cleared.
 *	@see		NX_UDC_SetDUALInterruptEnable
 */
CBOOL	NX_UDC_GetDUALInterruptEnable( void )
{
	const U16 DIEN_BITPOS	= 12;
	const U16 DIEN_MASK		= 1 << DIEN_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->SCR & DIEN_MASK) >> DIEN_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set FIFO's byte order
 *	@param[in]	bLittleEndian	\b CTRUE	indicates that byte order is a little endian. \n
 *								\b CFALSE	indicates that byte order is a big endian.
 *	@return		None.
 *	@see		NX_UDC_GetFIFOByteOrder
 */
void	NX_UDC_SetFIFOByteOrder( CBOOL bLittleEndian )
{
	const U16 RWDE_BITPOS	= 9;
	const U16 RRDE_BITPOS	= 5;
	const U16 RWDE_MASK		= 1 << RWDE_BITPOS;
	const U16 RRDE_MASK		= 1 << RRDE_BITPOS;

	register U16 ReadValue;

	NX_ASSERT( 0 == bLittleEndian || 1 == bLittleEndian );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->SCR;

	ReadValue &= ~( RWDE_MASK | RRDE_MASK );

	if( bLittleEndian )
	{
		ReadValue |= 1 << RRDE_BITPOS;
	}
	else
	{
		ReadValue |= 1 << RWDE_BITPOS;
	}

	__g_pRegister->SCR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get FIFO's byte order
 *	@return		\b CTRUE	indicates that byte order is a little endian. \n
 *				\b CFALSE	indicates that byte order is a big endian.
 *	@see		NX_UDC_SetFIFOByteOrder
 */
CBOOL	NX_UDC_GetFIFOByteOrder( void )
{
	const U16 RWDE_BITPOS	= 9;
	const U16 RRDE_BITPOS	= 5;
	const U16 RWDE_MASK		= 1 << RWDE_BITPOS;
	const U16 RRDE_MASK		= 1 << RRDE_BITPOS;

	register U16 ReadValue;

	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->SCR;

	if( 0 != (ReadValue & RRDE_MASK) && 0 == (ReadValue & RWDE_MASK) )
	{
		return CTRUE;
	}
	else if ( 0 == (ReadValue & RRDE_MASK) && 0 != (ReadValue & RWDE_MASK) )
	{
		return CFALSE;
	}

	NX_ASSERT( CFALSE );
	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set speed detection control
 *	@param[in]	bEnable	\b CTRUE	indicates that speed detection control enable. \n
 *						\b CFALSE	indicates that speed detection control disable.
 *	@return		None.
 *	@see		NX_UDC_ResetSpeedDetectionControl
 */
void	NX_UDC_SpeedDetectionControlEnable( CBOOL bEnable )
{
	const U16 SPDCEN_BITPOS	= 7;
	const U16 SPDCEN_MASK	= 1 << SPDCEN_BITPOS;

	register U16	ReadValue;

	NX_ASSERT( (0 == bEnable) || (1 == bEnable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->SCR;

	ReadValue &= ~SPDCEN_MASK;
	ReadValue |= ((U16)bEnable << SPDCEN_BITPOS);

	__g_pRegister->SCR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set speed detection control is reset or not
 *	@param[in]	bEnable	\b CTRUE	indicates that speed detection control reset enable. \n
 *						\b CFALSE	indicates that speed detection control reset disable.
 *	@return		None.
 *	@remarks	Software can reset speed detection logic through this function. \n
 *				This function is used to control speed detection process in case of system
 *				with a long initial time.\n
 *				User should clear reset after setting speed detection logic to reset.
 *	@code
 *
 *		NX_UDC_SpeedDetectionControlEnable(CTRUE) ;
 *
 *		NX_UDC_ResetSpeedDetectionControl(CTRUE);		// Reset
 *		...
 *		NX_UDC_ResetSpeedDetectionControl(CFALSE);		// Normal
 *
 *	@endcode
 *	@see		NX_UDC_SpeedDetectionControlEnable
 */
void	NX_UDC_ResetSpeedDetectionControl( CBOOL bEnable )
{
	const U16 SPDC_BITPOS	= 3;
	const U16 SPDC_MASK		= 1 << SPDC_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	if( bEnable )
	{
		__g_pRegister->SCR &= ~SPDC_MASK;
	}
	else
	{
		__g_pRegister->SCR |= SPDC_MASK;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Resume signal enable or disable.
 *	@param[in]	bEnable	\b CTRUE	indicates that resume signal enable. \n
 *						\b CFALSE	indicates that resume signal disable.
 *	@return		None.
 *	@remarks	If this function is set, the suspened core generate a resume signal.\n
 *				User should disable after resume signal is occurred.
 *	@code
 *
 *		NX_UDC_SetResumeByMCU(CTRUE);	// Set SCR[MFRM] bit
 *		NX_UDC_SetResumeByMCU(CFALSE);	// Clear SCR[MFRM] bit
 *
 *	@endcode
 *	@see		NX_UDC_GetResumeByMCU
 */
void	NX_UDC_SetResumeByMCU( CBOOL bEnable )
{
	const U16 MFRM_BITPOS	= 2;
	const U16 MFRM_MASK		= 1 << MFRM_BITPOS;

	register U16	ReadValue;

	NX_ASSERT( (0 == bEnable) || (1 == bEnable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->SCR;

	ReadValue &= ~MFRM_MASK;
	ReadValue |= ((U32)bEnable << MFRM_BITPOS);

	__g_pRegister->SCR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Resume signal is enabled or disabled.
 *	@return		\b CTRUE	indicates that resume signal is enabled. \n
				\b CFALSE	indicates that resume signal is disabled.
 *	@see		NX_UDC_SetResumeByMCU
 */
CBOOL	NX_UDC_GetResumeByMCU( void )
{
	const U16 MFRM_BITPOS	= 2;
	const U16 MFRM_MASK		= 1 << MFRM_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->SCR & MFRM_MASK) >> MFRM_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Endpoint0's status.
 *	@return		Endpoint0's status.
 *	@remarks	If endpoint0 interrupt is occurred, then check and clear of endpoin0 state.
 *	@code
 *
 *		S32 pend = NX_UDC_GetInterruptPendingNumber()
 *
 *		if( NX_UDC_INT_EP0 == pend )
 *		{
 *			U32 status = NX_UDC_GetEP0Status();
 *
 *			if( NX_UDC_EP0STATUS_SHT & status )
 *			{
 *				...
 *			}
 *			...
 *			NX_UDC_ClearEP0Status( status );
 *		}
 *
 *		NX_UDC_ClearInterruptPending( pend );
 *
 *	@endcode
 *	@see	NX_UDC_ClearEP0Status,		NX_UDC_EP0STATUS
 */
U32		NX_UDC_GetEP0Status( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(__g_pRegister->EP0SR);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear Endpoint0's status
 *	@param[in]	dwEndpointStatus		Endpoint0's status
 *	@return		None.
 *	@remarks	If endpoint0 interrupt is occurred, then check and clear of endpoin0 state.
 *	@see		NX_UDC_GetEP0Status,		NX_UDC_EP0STATUS
 */
void	NX_UDC_ClearEP0Status( U32 dwEndpointStatus )
{
	const U16 EP0SR_MASK = 0x13;

	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->EP0SR = (dwEndpointStatus & EP0SR_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Endpoint0's Tx test mode and PID
 *	@param[in]	bEnable	\b CTRUE	indicates that Tx test mode enable. \n
 *						\b CFALSE	indicates that Tx test mode disable.
 *	@param[in]	dwPID	Data PID ( 0, 1 ).
 *	@return		None.
 *	@remarks	Do not need to be set in normal operation.
 *	@see		NX_UDC_GetEP0PIDToggle
 */
void	NX_UDC_SetEP0PIDToggle( CBOOL bEnable, U32 dwPID )
{
	const U16 TTE_BITPOS	= 3;
	const U16 TTS_BITPOS	= 2;
	const U16 TTE_MASK		= 1 << TTE_BITPOS;
	const U16 TTS_MASK		= 1 << TTS_BITPOS;

	register U16 ReadValue;

	NX_ASSERT( 0 == bEnable || 1 == bEnable );
	NX_ASSERT( 0 == dwPID || 1 == dwPID );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->EP0CR;

	ReadValue &= ~( TTE_MASK | TTS_MASK );
	ReadValue |= ((bEnable << TTE_BITPOS) | (dwPID << TTS_BITPOS) );

	__g_pRegister->EP0CR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Endpoint0's Tx test mode and PID
 *	@param[out] pbEnable	\b CTRUE	indicates that Tx test mode is enabled. \n
 *							\b CFALSE	indicates that Tx test mode is disabled.
 *	@param[out] pdwPID	Data PID ( 0, 1 ).
 *	@return		None.
 *	@remarks	Do not need to be set in normal operation.
 *	@see		NX_UDC_SetEP0PIDToggle
 */
void	NX_UDC_GetEP0PIDToggle( CBOOL *pbEnable, U32 *pdwPID )
{
	const U16 TTE_BITPOS	= 3;
	const U16 TTS_BITPOS	= 2;
	const U16 TTE_MASK		= 1 << TTE_BITPOS;
	const U16 TTS_MASK		= 1 << TTS_BITPOS;

	register U16 ReadValue;

	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->EP0CR;

	if( 0 != pbEnable )
	{
		*pbEnable = (ReadValue & TTE_MASK) ? CTRUE : CFALSE;
	}

	if( 0 != pdwPID )
	{
		*pdwPID = ((ReadValue & TTS_MASK) >> TTS_BITPOS );
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Endpoint0 Stall.
 *	@param[in]	bEnable	\b CTRUE	indicates that Endpoint0 stall enable. \n
 *						\b CFALSE	indicates that Endpoint0 stall disable.
 *	@return		None.
 *	@remarks	Send STALL handshake to Host. \n
 *				User should clear stall state after send stall handshake.
 *	@code
 *
 *		NX_UDC_SetEP0Stall( CTRUE );	// Send Stall handshake
 *		...
 *		NX_UDC_SetEP0Stall( CFALSE );	// Not Send Stall handshake
 *
 *	@endcode
 *	@see		NX_UDC_GetEP0Stall, NX_UDC_SetStall, NX_UDC_GetStall
 */
void	NX_UDC_SetEP0Stall( CBOOL bEnable )
{
	const U16 ESS_BITPOS	= 1;
	const U16 ESS_MASK		= 1 << ESS_BITPOS;

	register U16	ReadValue;

	NX_ASSERT( (0 == bEnable) || (1 == bEnable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->EP0CR;

	ReadValue &= ~ESS_MASK;
	ReadValue |= ((U32)bEnable << ESS_BITPOS);

	__g_pRegister->EP0CR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Endpoint0's stall status
 *	@return		\b CTRUE	indicates that Endpoint0 stall is enabled. \n
				\b CFALSE	indicates that Endpoint0 stall is disabled.
 *	@see		NX_UDC_SetEP0Stall, NX_UDC_SetStall, NX_UDC_GetStall
 */
CBOOL	NX_UDC_GetEP0Stall( void )
{
	const U16 ESS_BITPOS	= 1;
	const U16 ESS_MASK		= 1 << ESS_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->EP0CR & ESS_MASK) >> ESS_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Endpoint0 Tx zero length enable or disable
 *	@param[in]	bEnable	\b CTRUE	indicates that Tx zero length enable. \n
 *						\b CFALSE	indicates that Tx zero length disable.
 *	@return		None.
 *	@remarks	Send Tx zero length data to host.\n
 *				User should disable after enable. \n
 *				Do not need to be set in normal operation. \n
 *	@code
 *
 *		NX_UDC_SetEP0TxZeroLength( CTRUE );	// Send Tx Zero Length Data
 *		...
 *		NX_UDC_SetEP0TxZeroLength( CFALSE );	// Not Send Tx Zero Length Data
 *
 *	@endcode
 *	@see		NX_UDC_SetEP0TxZeroLength
 */
void	NX_UDC_SetEP0TxZeroLength( CBOOL bEnable )
{
	const U16 TZLS_BITPOS	= 0;
	const U16 TZLS_MASK		= 1 << TZLS_BITPOS;

	register U16	ReadValue;

	NX_ASSERT( (0 == bEnable) || (1 == bEnable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->EP0CR;

	ReadValue &= ~TZLS_MASK;
	ReadValue |= ((U32)bEnable << TZLS_BITPOS);

	__g_pRegister->EP0CR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Endpoint0 Tx zero length is enabled or disabled
 *	@return		\b CTRUE	indicates that Tx zero length is enabled.
 *				\b CFALSE	indicates that Tx zero length is disabled.
 *	@see		NX_UDC_SetEP0TxZeroLength
 */
CBOOL	NX_UDC_GetEP0TxZeroLength( void )
{
	const U16 TZLS_BITPOS	= 0;
	const U16 TZLS_MASK		= 1 << TZLS_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->EP0CR & TZLS_MASK) >> TZLS_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a data to be transmitted to the USB FIFO.
 *	@param[in]	dwEndpoint	Endpoint( 0, 1, 2, 3 ).
 *	@param[in]	wData		Specifies a 16-bit data to be transmitted.
 *	@return		None.
 *	@see									NX_UDC_GetData,
 *				NX_UDC_GetDataPointer,		NX_UDC_SetDataEP0,
 *				NX_UDC_GetDataEP0,			NX_UDC_GetDataPointerEP0,
 *				NX_UDC_SetDataEP1,			NX_UDC_GetDataEP1,
 *				NX_UDC_GetDataPointerEP1,	NX_UDC_SetDataEP2,
 *				NX_UDC_GetDataEP2,			NX_UDC_GetDataPointerEP2,
 *				NX_UDC_SetDataEP3,			NX_UDC_GetDataEP3,
 *				NX_UDC_GetDataPointerEP3
 */
void	NX_UDC_SetData( U32 dwEndpoint, U16 wData )
{
	NX_ASSERT( 4 > dwEndpoint );
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->BR[dwEndpoint] = wData;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a data which was received from the USB FIFO.
 *	@param[in]	dwEndpoint	Endpoint( 0, 1, 2, 3 )
 *	@return		A 16-bit data which was received from USB FIFO
 *	@see		NX_UDC_SetData,
 *				NX_UDC_GetDataPointer,		NX_UDC_SetDataEP0,
 *				NX_UDC_GetDataEP0,			NX_UDC_GetDataPointerEP0,
 *				NX_UDC_SetDataEP1,			NX_UDC_GetDataEP1,
 *				NX_UDC_GetDataPointerEP1,	NX_UDC_SetDataEP2,
 *				NX_UDC_GetDataEP2,			NX_UDC_GetDataPointerEP2,
 *				NX_UDC_SetDataEP3,			NX_UDC_GetDataEP3,
 *				NX_UDC_GetDataPointerEP3
 */
U16		NX_UDC_GetData( U32 dwEndpoint	)
{
	NX_ASSERT( 4 > dwEndpoint );
	NX_ASSERT( CNULL != __g_pRegister );

	return __g_pRegister->BR[dwEndpoint];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a pointer to set/get a 16-bit data	on FIFO.
 *	@param[in]	dwEndpoint	Endpoint( 0, 1, 2, 3 ).
 *	@return		A 16-bit data pointer.
 *	@remark		You have only to aceess this pointer by 16-bit mode and do not
 *				increase/decrease this pointer.\n
 *				The example for this is as following.
 *	@code
 *		volatile U16 *pData = NX_UDC_GetDataPointer(0);
 *		*pData = wData;	// Push a 16-bit data to FIFO.
 *		wData = *pData;	// Pop a 16-bit data from FIFO.
 *	@endcode
 *	@see		NX_UDC_SetData,				NX_UDC_GetData,
 *											NX_UDC_SetDataEP0,
 *				NX_UDC_GetDataEP0,			NX_UDC_GetDataPointerEP0,
 *				NX_UDC_SetDataEP1,			NX_UDC_GetDataEP1,
 *				NX_UDC_GetDataPointerEP1,	NX_UDC_SetDataEP2,
 *				NX_UDC_GetDataEP2,			NX_UDC_GetDataPointerEP2,
 *				NX_UDC_SetDataEP3,			NX_UDC_GetDataEP3,
 *				NX_UDC_GetDataPointerEP3
 */
volatile U16*	NX_UDC_GetDataPointer( U32 dwEndpoint )
{
	NX_ASSERT( 4 > dwEndpoint );
	NX_ASSERT( CNULL != __g_pRegister );

	return &__g_pRegister->BR[dwEndpoint];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a data to be transmitted to the USB FIFO.
 *	@param[in]	wData		Specifies a 16-bit data to be transmitted.
 *	@return		None.
 *	@remark		This function is only applied to EP0.
 *	@see		NX_UDC_SetData,				NX_UDC_GetData,
 *				NX_UDC_GetDataPointer,
 *				NX_UDC_GetDataEP0,			NX_UDC_GetDataPointerEP0,
 *				NX_UDC_SetDataEP1,			NX_UDC_GetDataEP1,
 *				NX_UDC_GetDataPointerEP1,	NX_UDC_SetDataEP2,
 *				NX_UDC_GetDataEP2,			NX_UDC_GetDataPointerEP2,
 *				NX_UDC_SetDataEP3,			NX_UDC_GetDataEP3,
 *				NX_UDC_GetDataPointerEP3
 */
void	NX_UDC_SetDataEP0( U16 wData )
{
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->BR[0] = wData;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a data which was received from the USB FIFO.
 *	@return		A 16-bit data which was received from USB FIFO.
 *	@remark		This function is only applied to EP0.
 *	@see		NX_UDC_SetData,				NX_UDC_GetData,
 *				NX_UDC_GetDataPointer,		NX_UDC_SetDataEP0,
 *											NX_UDC_GetDataPointerEP0,
 *				NX_UDC_SetDataEP1,			NX_UDC_GetDataEP1,
 *				NX_UDC_GetDataPointerEP1,	NX_UDC_SetDataEP2,
 *				NX_UDC_GetDataEP2,			NX_UDC_GetDataPointerEP2,
 *				NX_UDC_SetDataEP3,			NX_UDC_GetDataEP3,
 *				NX_UDC_GetDataPointerEP3
 */
U16		NX_UDC_GetDataEP0( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return __g_pRegister->BR[0];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a pointer to set/get a 16-bit data	on FIFO.
 *	@return		A 16-bit data pointer.
 *	@remark		You have only to aceess this pointer by 16-bit mode and do not
 *				increase/decrease this pointer.\n
 *				The example for this is as following.\n
 *				This function is only applied to EP0.
 *	@code
 *		volatile U16 *pData = NX_UDC_GetDataPointerEP0();
 *		*pData = wData;	// Push a 16-bit data to FIFO.
 *		wData = *pData;	// Pop a 16-bit data from FIFO.
 *	@endcode
 *	@see		NX_UDC_SetData,				NX_UDC_GetData,
 *				NX_UDC_GetDataPointer,		NX_UDC_SetDataEP0,
 *				NX_UDC_GetDataEP0,
 *				NX_UDC_SetDataEP1,			NX_UDC_GetDataEP1,
 *				NX_UDC_GetDataPointerEP1,	NX_UDC_SetDataEP2,
 *				NX_UDC_GetDataEP2,			NX_UDC_GetDataPointerEP2,
 *				NX_UDC_SetDataEP3,			NX_UDC_GetDataEP3,
 *				NX_UDC_GetDataPointerEP3
 */
volatile U16*	NX_UDC_GetDataPointerEP0( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return &__g_pRegister->BR[0];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a data to be transmitted to the USB FIFO.
 *	@param[in]	wData		Specifies a 16-bit data to be transmitted.
 *	@return		None.
 *	@remark		This function is only applied to EP1.
 *	@see		NX_UDC_SetData,				NX_UDC_GetData,
 *				NX_UDC_GetDataPointer,		NX_UDC_SetDataEP0,
 *				NX_UDC_GetDataEP0,			NX_UDC_GetDataPointerEP0,
 *											NX_UDC_GetDataEP1,
 *				NX_UDC_GetDataPointerEP1,	NX_UDC_SetDataEP2,
 *				NX_UDC_GetDataEP2,			NX_UDC_GetDataPointerEP2,
 *				NX_UDC_SetDataEP3,			NX_UDC_GetDataEP3,
 *				NX_UDC_GetDataPointerEP3
 */
void	NX_UDC_SetDataEP1( U16 wData )
{
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->BR[1] = wData;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a data which was received from the USB FIFO.
 *	@return		A 16-bit data which was received from USB FIFO.
 *	@remark		This function is only applied to EP1.
 *	@see		NX_UDC_SetData,				NX_UDC_GetData,
 *				NX_UDC_GetDataPointer,		NX_UDC_SetDataEP0,
 *				NX_UDC_GetDataEP0,			NX_UDC_GetDataPointerEP0,
 *				NX_UDC_SetDataEP1,
 *				NX_UDC_GetDataPointerEP1,	NX_UDC_SetDataEP2,
 *				NX_UDC_GetDataEP2,			NX_UDC_GetDataPointerEP2,
 *				NX_UDC_SetDataEP3,			NX_UDC_GetDataEP3,
 *				NX_UDC_GetDataPointerEP3
 */
U16		NX_UDC_GetDataEP1( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return __g_pRegister->BR[1];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a pointer to set/get a 16-bit data	on FIFO.
 *	@return		A 16-bit data pointer.
 *	@remark		You have only to aceess this pointer by 16-bit mode and do not
 *				increase/decrease this pointer.\n
 *				The example for this is as following. \n
 *				This function is only applied to EP1.
 *	@code
 *		volatile U16 *pData = NX_UDC_GetDataPointerEP1();
 *		*pData = wData;	// Push a 16-bit data to FIFO.
 *		wData = *pData;	// Pop a 16-bit data from FIFO.
 *	@endcode
 *	@see		NX_UDC_SetData,				NX_UDC_GetData,
 *				NX_UDC_GetDataPointer,		NX_UDC_SetDataEP0,
 *				NX_UDC_GetDataEP0,			NX_UDC_GetDataPointerEP0,
 *				NX_UDC_SetDataEP1,			NX_UDC_GetDataEP1,
 *											NX_UDC_SetDataEP2,
 *				NX_UDC_GetDataEP2,			NX_UDC_GetDataPointerEP2,
 *				NX_UDC_SetDataEP3,			NX_UDC_GetDataEP3,
 *				NX_UDC_GetDataPointerEP3
 */
volatile U16*	NX_UDC_GetDataPointerEP1( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return &__g_pRegister->BR[1];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a data to be transmitted to the USB FIFO.
 *	@param[in]	wData		Specifies a 16-bit data to be transmitted.
 *	@return		None.
 *	@remark		This function is only applied to EP2.
 *	@see		NX_UDC_SetData,				NX_UDC_GetData,
 *				NX_UDC_GetDataPointer,		NX_UDC_SetDataEP0,
 *				NX_UDC_GetDataEP0,			NX_UDC_GetDataPointerEP0,
 *				NX_UDC_SetDataEP1,			NX_UDC_GetDataEP1,
 *				NX_UDC_GetDataPointerEP1,
 *				NX_UDC_GetDataEP2,			NX_UDC_GetDataPointerEP2,
 *				NX_UDC_SetDataEP3,			NX_UDC_GetDataEP3,
 *				NX_UDC_GetDataPointerEP3
 */
void	NX_UDC_SetDataEP2( U16 wData )
{
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->BR[2] = wData;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a data which was received from the USB FIFO.
 *	@return		A 16-bit data which was received from USB FIFO.
 *	@remark		This function is only applied to EP2.
 *	@see		NX_UDC_SetData,				NX_UDC_GetData,
 *				NX_UDC_GetDataPointer,		NX_UDC_SetDataEP0,
 *				NX_UDC_GetDataEP0,			NX_UDC_GetDataPointerEP0,
 *				NX_UDC_SetDataEP1,			NX_UDC_GetDataEP1,
 *				NX_UDC_GetDataPointerEP1,	NX_UDC_SetDataEP2,
 *											NX_UDC_GetDataPointerEP2,
 *				NX_UDC_SetDataEP3,			NX_UDC_GetDataEP3,
 *				NX_UDC_GetDataPointerEP3
 */
U16		NX_UDC_GetDataEP2( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return __g_pRegister->BR[2];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a pointer to set/get a 16-bit data	on FIFO.
 *	@return		A 16-bit data pointer.
 *	@remark		You have only to aceess this pointer by 16-bit mode and do not
 *				increase/decrease this pointer.\n
 *				The example for this is as following.\n
 *				This function is only applied to EP2.
 *	@code
 *		volatile U16 *pData = NX_UDC_GetDataPointerEP2();
 *		*pData = wData;	// Push a 16-bit data to FIFO.
 *		wData = *pData;	// Pop a 16-bit data from FIFO.
 *	@endcode
 *	@see		NX_UDC_SetData,				NX_UDC_GetData,
 *				NX_UDC_GetDataPointer,		NX_UDC_SetDataEP0,
 *				NX_UDC_GetDataEP0,			NX_UDC_GetDataPointerEP0,
 *				NX_UDC_SetDataEP1,			NX_UDC_GetDataEP1,
 *				NX_UDC_GetDataPointerEP1,	NX_UDC_SetDataEP2,
 *				NX_UDC_GetDataEP2,
 *				NX_UDC_SetDataEP3,			NX_UDC_GetDataEP3,
 *				NX_UDC_GetDataPointerEP3
 */
volatile U16*	NX_UDC_GetDataPointerEP2( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return &__g_pRegister->BR[2];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a data to be transmitted to the USB FIFO.
 *	@param[in]	wData		Specifies a 16-bit data to be transmitted.
 *	@return		None.
 *	@remark		This function is only applied to EP3.
 *	@see		NX_UDC_SetData,				NX_UDC_GetData,
 *				NX_UDC_GetDataPointer,		NX_UDC_SetDataEP0,
 *				NX_UDC_GetDataEP0,			NX_UDC_GetDataPointerEP0,
 *				NX_UDC_SetDataEP1,			NX_UDC_GetDataEP1,
 *				NX_UDC_GetDataPointerEP1,	NX_UDC_SetDataEP2,
 *				NX_UDC_GetDataEP2,			NX_UDC_GetDataPointerEP2,
 *											NX_UDC_GetDataEP3,
 *				NX_UDC_GetDataPointerEP3
 */
void	NX_UDC_SetDataEP3( U16 wData )
{
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->BR[3] = wData;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a data which was received from the USB FIFO.
 *	@return		A 16-bit data which was received from USB FIFO.
 *	@remark		This function is only applied to EP3
 *	@see		NX_UDC_SetData,				NX_UDC_GetData,
 *				NX_UDC_GetDataPointer,		NX_UDC_SetDataEP0,
 *				NX_UDC_GetDataEP0,			NX_UDC_GetDataPointerEP0,
 *				NX_UDC_SetDataEP1,			NX_UDC_GetDataEP1,
 *				NX_UDC_GetDataPointerEP1,	NX_UDC_SetDataEP2,
 *				NX_UDC_GetDataEP2,			NX_UDC_GetDataPointerEP2,
 *				NX_UDC_SetDataEP3,
 *				NX_UDC_GetDataPointerEP3
 */
U16		NX_UDC_GetDataEP3( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return __g_pRegister->BR[3];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a pointer to set/get a 16-bit data	on FIFO.
 *	@return		A 16-bit data pointer.
 *	@remark		You have only to aceess this pointer by 16-bit mode and do not
 *				increase/decrease this pointer.\n
 *				The example for this is as following.\n
 *				This function is only applied to EP3.
 *	@code
 *		volatile U16 *pData = NX_UDC_GetDataPointerEP3();
 *		*pData = wData;	// Push a 16-bit data to FIFO.
 *		wData = *pData;	// Pop a 16-bit data from FIFO.
 *	@endcode
 *	@see		NX_UDC_SetData,				NX_UDC_GetData,
 *				NX_UDC_GetDataPointer,		NX_UDC_SetDataEP0,
 *				NX_UDC_GetDataEP0,			NX_UDC_GetDataPointerEP0,
 *				NX_UDC_SetDataEP1,			NX_UDC_GetDataEP1,
 *				NX_UDC_GetDataPointerEP1,	NX_UDC_SetDataEP2,
 *				NX_UDC_GetDataEP2,			NX_UDC_GetDataPointerEP2,
 *				NX_UDC_SetDataEP3,			NX_UDC_GetDataEP3
 */
volatile U16*	NX_UDC_GetDataPointerEP3( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return &__g_pRegister->BR[3];
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set PHY Control
 *	@param[in]	bEnable			\b CTRUE indicates that PHY control is user set mode. \n
 *								\b CFALSE indicates that PHY control is auto mode.
 *	@param[in]	bPHYReset		\b CTRUE indicates that PHY is reset \n
 *								\b CFALSE indicates that PHY is \b NOT reset.
 *	@param[in]	bPHYPowerdown	\b CTRUE indicates that PHY is power down \n
 *								\b CFALSE indicates that PHY is \b NOT power down.
 *	@return		None.
 *	@remarks	Only user set mode is applied that parameter( bPHYReset and bPHYPowerdown ).
 *	@see		NX_UDC_SetPHYControl
 */
void	NX_UDC_SetPHYControl( CBOOL bEnable, CBOOL bPHYReset, CBOOL bPHYPowerdown )
{
	const U16 URSTC_BITPOS	= 7;
	const U16 SIDC_BITPOS	= 6;
	const U16 PCE_BITPOS	= 0;

	register U16 ReadValue;

	NX_ASSERT( (0 == bPHYReset) || (1 == bPHYReset) );
	NX_ASSERT( (0 == bPHYPowerdown) || (1 == bPHYPowerdown) );
	NX_ASSERT( (0 == bEnable) || (1 == bEnable) );

	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue =	(	(bPHYReset	<< URSTC_BITPOS) |
					(bPHYPowerdown << SIDC_BITPOS)	|
					(bEnable		<< PCE_BITPOS)
					);

	__g_pRegister->PCR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get PHY's setting value.
 *	@param[out] pbEnable		\b CTRUE indicates that PHY control is user set mode. \n
 *								\b CFALSE indicates that PHY control is auto mode.
 *	@param[out] pbPHYReset		\b CTRUE indicates that PHY is reset \n
 *								\b CFALSE indicates that PHY is \b NOT reset.
 *	@param[out] pbPHYPowerdown	\b CTRUE indicates that PHY is power down \n
 *								\b CFALSE indicates that PHY is \b NOT power down.
 *	@return		None.
 *	@see		NX_UDC_SetPHYControl
 */
void	NX_UDC_GetPHYControl( CBOOL *pbEnable, CBOOL *pbPHYReset, CBOOL *pbPHYPowerdown )
{
	const U16 URSTC_BITPOS	= 7;
	const U16 SIDC_BITPOS	= 6;
	const U16 PCE_BITPOS	= 0;

	const U16 URSTC_MASK	= 0x01 << URSTC_BITPOS;
	const U16 SIDC_MASK		= 0x01 << SIDC_BITPOS;
	const U16 PCE_MASK		= 0x01 << PCE_BITPOS;

	register U16 ReadValue;

	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->PCR;

	if( CNULL != pbPHYReset )
	{
		*pbPHYReset = ((ReadValue & URSTC_MASK) >> URSTC_BITPOS);
	}

	if( CNULL != pbPHYPowerdown )
	{
		*pbPHYPowerdown = ((ReadValue & SIDC_MASK) >> SIDC_BITPOS);
	}

	if( CNULL != pbEnable )
	{
		*pbEnable = ((ReadValue & PCE_MASK) >> PCE_BITPOS);
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set external VBUS's operation.
 *	@param[in]	bEnable	\b CTRUE	indicate that external VBUS enable. \n
 *						\b CFALSE	indicate that external VBUS disable.
 *	@return		None.
 *	@see		NX_UDC_GetExternalVBUSEnable
 */
void	NX_UDC_SetExternalVBUSEnable( CBOOL bEnable )
{
	const U16 VBUS_ENB_BITPOS	= 0;
	const U16 VBUS_ENB_MASK		= 1 << VBUS_ENB_BITPOS;

	register U16 ReadValue;

	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->PHYPARAM0 & ~VBUS_ENB_MASK;

	ReadValue |= ((U16)bEnable << VBUS_ENB_BITPOS);

	__g_pRegister->PHYPARAM0 = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get external VBUS's setting value.
 *	@return		\b CTRUE	indicate that external VBUS is enabled. \n
 *				\b CFALSE	indicate that external VBUS is disabled.
 *	@see		NX_UDC_SetExternalVBUSEnable
 */
CBOOL	NX_UDC_GetExternalVBUSEnable( void )
{
	const U16 VBUS_ENB_BITPOS	= 0;
	const U16 VBUS_ENB_MASK		= 1 << VBUS_ENB_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)(__g_pRegister->PHYPARAM0 & VBUS_ENB_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set PHY power up.
 *	@return		None.
 *	@see		NX_UDC_SetPHYPowerDown
 */
void	NX_UDC_SetPHYPowerUp( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->PHYPARAM0 = 0x37;
	__g_pRegister->PHYPARAM2 = 0x38;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set PHY power down.
 *	@return		None.
 *	@see		NX_UDC_SetPHYPowerUp
 */
void	NX_UDC_SetPHYPowerDown( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->PHYPARAM0 = 0x13C;
	__g_pRegister->PHYPARAM2 = 0x30;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set First OUT packet interrupt is enable or disable.
 *	@param[in]	bEnable	\b CTRUE	indicates that First OUT packet interrupt enable. \n
 *						\b CFALSE	indicates that First OUT packet interrupt disable.
 *	@return		None.
 *	@remarks	This Function is only applied in DMA mode. \n
 *	@see													NX_UDC_GetEPStatus,
 *				NX_UDC_ClearEPStatus,						NX_UDC_GetPacketNumberInFIFO
 */
void	NX_UDC_SetFirstOUTPacketInterruptEnable( CBOOL bEnable )
{
	const U16 FPID_BITPOS = 11 ;

	NX_ASSERT( CNULL != __g_pRegister );

	if( bEnable )
		__g_pRegister->ESR = (0 << FPID_BITPOS);
	else
		__g_pRegister->ESR = (1 << FPID_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Endpoint's status
 *	@return		Endpoint's status
 *	@remarks	If endpoint interrupt is occurred, then check and clear of endpoin state.
 *	@code
 *
 *		S32 pend = NX_UDC_GetInterruptPendingNumber()
 *
 *		if( NX_UDC_INT_EP1 == pend )
 *		{
 *			NX_UDC_SetIndexEP(1);
 *			U32 status = NX_UDC_GetEPStatus();
 *
 *			if( NX_UDC_EPSTATUS_FUDR & status )
 *			{
 *				...
 *			}
 *			...
 *			NX_UDC_ClearEPStatus( status );
 *		}
 *
 *		NX_UDC_ClearInterruptPending( pend );
 *
 *	@endcode
 *	@see		NX_UDC_SetFirstOUTPacketInterruptEnable,
 *				NX_UDC_ClearEPStatus,						NX_UDC_GetPacketNumberInFIFO
 */
U32		NX_UDC_GetEPStatus( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(__g_pRegister->ESR);
}


//------------------------------------------------------------------------------
/**
 *	@brief		Clear Endpoint's status
 *	@param[in]	dwEndpointStatus		Endpoint0's status
 *	@return		None.
 *	@remarks	If endpoint interrupt is occurred, then check and clear of endpoin state.
 *	@see		NX_UDC_SetFirstOUTPacketInterruptEnable,	NX_UDC_GetEPStatus,
 *															NX_UDC_GetPacketNumberInFIFO
 */
void	NX_UDC_ClearEPStatus( U32 dwEndpointStatus )
{
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->ESR = (U16)( dwEndpointStatus & 0xFFFF );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get packet number in FIFO
 *	@return		Packet number in FIFO
 *	@see		NX_UDC_SetFirstOUTPacketInterruptEnable,	NX_UDC_GetEPStatus,
 *				NX_UDC_ClearEPStatus
 */
U32		NX_UDC_GetPacketNumberInFIFO( void )
{
	const U16 PSIF_BITPOS	= 2;
	const U16 PSIF_MASK		= 0x03 << PSIF_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return ( __g_pRegister->ESR & PSIF_MASK ) >> PSIF_BITPOS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set IN packet's operation.
 *	@param[in]	bEnable	\b CTRUE	indicates that IN pakcet hold. \n
 *						\b CFALSE	indicates that IN packet \b NOT hold.
 *	@return		None.
 *	@remarks	If IN packet hold is set, USB does not send IN data to Host.
 *	@see									NX_UDC_GetInPacketHold,
 *				NX_UDC_SetOutPacketHold,	NX_UDC_GetOutPacketHold
 */
void	NX_UDC_SetInPacketHold( CBOOL bEnable )
{
	const U16 INPKTHLD_BITPOS	= 12;
	const U16 INPKTHLD_MASK		= 1 << INPKTHLD_BITPOS;

	register U16 ReadValue;

	NX_ASSERT( 0 == bEnable || 1 == bEnable );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->ECR;

	ReadValue &= ~INPKTHLD_MASK;
	ReadValue |= (U16)bEnable << INPKTHLD_BITPOS;

	__g_pRegister->ECR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get setting value of IN packet is hold or Not
 *	@return		\b CTRUE	indicates that IN pakcet hold. \n
				\b CFALSE	indicates that IN packet \b NOT hold.
 *	@remarks	If IN packet hold is set, USB does not send IN data to Host.
 *	@see		NX_UDC_SetInPacketHold,
 *				NX_UDC_SetOutPacketHold,	NX_UDC_GetOutPacketHold
 */
CBOOL	NX_UDC_GetInPacketHold( void )
{
	const U16 INPKTHLD_BITPOS	= 12;
	const U16 INPKTHLD_MASK		= 1 << INPKTHLD_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)(__g_pRegister->ECR & INPKTHLD_MASK) >> INPKTHLD_BITPOS;
}


//------------------------------------------------------------------------------
/**
 *	@brief		Set OUT packet's operation.
 *	@param[in]	bEnable	\b CTRUE	indicates that OUT pakcet hold. \n
 *						\b CFALSE	indicates that OUT packet \b NOT hold.
 *	@return		None.
 *	@remarks	If OUT packet hold is set, USB does not accept OUT data from Host.
 *	@see		NX_UDC_SetInPacketHold,		NX_UDC_GetInPacketHold,
 *											NX_UDC_GetOutPacketHold
 */
void	NX_UDC_SetOutPacketHold( CBOOL bEnable )
{
	const U16 OUTPKTHLD_BITPOS	= 11;
	const U16 OUTPKTHLD_MASK	= 1 << OUTPKTHLD_BITPOS;

	register U16 ReadValue;

	NX_ASSERT( 0 == bEnable || 1 == bEnable );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->ECR;

	ReadValue &= ~OUTPKTHLD_MASK;
	ReadValue |= (U16)bEnable << OUTPKTHLD_BITPOS;

	__g_pRegister->ECR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get setting value of OUT packet is hold or Not
 *	@return		\b CTRUE	indicates that OUT pakcet hold. \n
				\b CFALSE	indicates that OUT packet \b NOT hold.
 *	@remarks	If OUT packet hold is set, USB does not accept OUT data from Host.
 *	@see		NX_UDC_SetInPacketHold,		NX_UDC_GetInPacketHold,
 *				NX_UDC_SetOutPacketHold
 */
CBOOL	NX_UDC_GetOutPacketHold( void )
{
	const U16 OUTPKTHLD_BITPOS	= 11;
	const U16 OUTPKTHLD_MASK	= 1 << OUTPKTHLD_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)(__g_pRegister->ECR & OUTPKTHLD_MASK) >> OUTPKTHLD_BITPOS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set transaction number per micro frame in ISO mode
 *	@param[in]	dwTransactionNumber Transaction number ( 1, 2, 3 )
 *	@return		None.
 *	@see		NX_UDC_GetTransactionNumber
 */
void	NX_UDC_SetTransactionNumber( U32 dwTransactionNumber )
{
	const U16 TNPMF_BITPOS	= 9;
	const U16 TNPMF_MASK	= 0x03 << TNPMF_BITPOS;

	register U16 ReadValue;

	NX_ASSERT( (1==dwTransactionNumber) || (2==dwTransactionNumber) || (3==dwTransactionNumber) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->ECR;

	ReadValue &= ~TNPMF_MASK;
	ReadValue |= dwTransactionNumber << TNPMF_BITPOS;

	__g_pRegister->ECR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get transaction number per micro frame in ISO mode
 *	@return		transaction number
 *	@see		NX_UDC_SetTransactionNumber
 */
U32		NX_UDC_GetTransactionNumber( void )
{
	const U16 TNPMF_BITPOS	= 9;
	const U16 TNPMF_MASK	= 0x03 << TNPMF_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)((__g_pRegister->ECR & TNPMF_MASK) >> TNPMF_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set FIFO mode
 *	@param[in]	bEnable	\b CTRUE	indicates that dual FIFO mode enable. \n
 *						\b CFALSE	indicates that dual FIFO mode disable.
 *	@return		None.
 *	@remarks	If AutoRxDMA mode is disabled, then set to disable of dual FIFO mode.
 *	@see		NX_UDC_GetDualFIFOMode
 */
void	NX_UDC_SetDualFIFOMode( CBOOL bEnable )
{
	const U16 DUEN_BITPOS	= 7;
	const U16 DUEN_MASK		= 1 << DUEN_BITPOS;

	register U16 ReadValue;

	NX_ASSERT( 0 == bEnable || 1 == bEnable );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->ECR;

	ReadValue &= ~DUEN_MASK;
	ReadValue |= (U16)bEnable << DUEN_BITPOS;

	__g_pRegister->ECR = ReadValue;
}


//------------------------------------------------------------------------------
/**
 *	@brief		Get FIFO mode
 *	@return		\b CTRUE	indicates that dual FIFO mode is enabled. \n
				\b CFALSE	indicates that dual FIFO mode is disabled.
 *	@see		NX_UDC_SetDualFIFOMode
 */
CBOOL	NX_UDC_GetDualFIFOMode( void )
{
	const U16 DUEN_BITPOS	= 7;
	const U16 DUEN_MASK		= 1 << DUEN_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->ECR & DUEN_MASK) >> DUEN_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set USB device's transfer mode.
 *	@param[in]	mode Transfer mode
 *	@return		None.
 *	@see		NX_UDC_GetEPTransferMode
 */
void	NX_UDC_SetEPTransferMode( NX_UDC_TRANSFER_MODE mode )
{
	const U16 IME_BITPOS	= 8;
	const U16 IEMS_BITPOS	= 0;
	const U16 IME_MASK		= 1 << IME_BITPOS;
	const U16 IEMS_MASK		= 1 << IEMS_BITPOS;

	register U16 ReadValue;

	NX_ASSERT( NX_UDC_TRANSFER_MODE_BULK >= mode );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->ECR;

	ReadValue &= ~( IME_MASK | IEMS_MASK );

	if( NX_UDC_TRANSFER_MODE_INTERRUPT == mode )
	{
		ReadValue |= 1 << IEMS_BITPOS;
	}
	else if( NX_UDC_TRANSFER_MODE_ISO == mode )
	{
		ReadValue |= 1 << IME_BITPOS;
	}
	else if( NX_UDC_TRANSFER_MODE_BULK == mode )
	{
		// Nothing
	}

	__g_pRegister->ECR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get USB device's transfer mode.
 *	@return		Transfer mode
 *	@see		NX_UDC_SetEPTransferMode
 */
NX_UDC_TRANSFER_MODE	NX_UDC_GetEPTransferMode( void )
{
	const U16 IME_BITPOS	= 8;
	const U16 IEMS_BITPOS	= 0;
	const U16 IME_MASK		= 1 << IME_BITPOS;
	const U16 IEMS_MASK		= 1 << IEMS_BITPOS;

	register U16 ReadValue;

	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->ECR;

	if( ReadValue & IME_MASK )
	{
		return NX_UDC_TRANSFER_MODE_ISO;
	}
	else if ( ReadValue & IEMS_MASK )
	{
		return NX_UDC_TRANSFER_MODE_INTERRUPT;
	}
	else
	{
		return NX_UDC_TRANSFER_MODE_BULK;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set FIFO flush.
 *	@return		None.
 *	@remarks	FIFO flush bit is automatically cleared after FIFO flushed.
 *	@see		NX_UDC_IsFIFOFlushed
 */
void	NX_UDC_FlushFIFO( void )
{
	const U16 FLUSH_BITPOS	= 6;
	const U16 FLUSH_MASK	= 1 << FLUSH_BITPOS;

	register U16	ReadValue;

	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->ECR;

	ReadValue |= FLUSH_MASK;

	__g_pRegister->ECR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check FIFO is flushed or NOT
 *	@return		\b CTRUE	indicates that FIFO is flushed. \n
				\b CFALSE	indicates that FIFO is \b NOT flushed.
 *	@see		NX_UDC_FlushFIFO
 */
CBOOL	NX_UDC_IsFIFOFlushed( void )
{
	const U16 FLUSH_BITPOS	= 6;
	const U16 FLUSH_MASK	= 1 << FLUSH_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->ECR & FLUSH_MASK )
	{
		return CFALSE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Endpoint's Tx toggle enable and PID
 *	@param[in]	bEnable	\b CTRUE	indicates that Tx toggle enable. \n
 *						\b CFALSE	indicates that Tx toggle disable.
 *	@param[in]	dwPID	Data PID ( 0 ~ 3 ) \n
 *							- 0 : Data PID 0. \n
 *							- 1 : Data PID 1. \n
 *							- 2 : Data PID 2 (Only ISO Mode). \n
 *							- 3 : Data PID M (Only ISO Mode). \n
 *	@return		None.
 *	@remarks	Do not need to be set in normal operation. \n
 *				PID 2, PID M is only used in ISO mode.
 *	@see		NX_UDC_GetPIDToggle
 */
void	NX_UDC_SetPIDToggle( CBOOL bEnable, U32 dwPID )
{
	const U16 TTE_BITPOS	= 5;
	const U16 TTS_BITPOS	= 3;
	const U16 TTE_MASK		= 1 << TTE_BITPOS;
	const U16 TTS_MASK		= 0x03 << TTS_BITPOS;

	register U16 ReadValue;

	NX_ASSERT( 0 == bEnable || 1 == bEnable );
	NX_ASSERT( 3 >= dwPID );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->ECR;

	ReadValue &= ~( TTE_MASK | TTS_MASK );
	ReadValue |= ((bEnable << TTE_BITPOS) | (dwPID << TTS_BITPOS) );

	__g_pRegister->ECR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Endpoint's Tx toggle enable and PID
 *	@param[out] pbEnable 	\b CTRUE	indicates that Tx toggle is enabled. \n
 *							\b CFALSE	indicates that Tx toggle is disabled.
 *	@param[out] pdwPID	Data PID ( 0 ~ 3 ) \n
 *							- 0 : Data PID 0. \n
 *							- 1 : Data PID 1. \n
 *							- 2 : Data PID 2 (Only ISO Mode). \n
 *							- 3 : Data PID M (Only ISO Mode). \n
 *	@return		None.
 *	@remarks	Do not need to be set in normal operation.\n
 *				PID 2, PID M is only used in ISO mode.
 *	@see		NX_UDC_SetPIDToggle
 */
void	NX_UDC_GetPIDToggle( CBOOL *pbEnable, U32 *pdwPID )
{
	const U16 TTE_BITPOS	= 5;
	const U16 TTS_BITPOS	= 3;
	const U16 TTE_MASK		= 1 << TTE_BITPOS;
	const U16 TTS_MASK		= 0x03 << TTS_BITPOS;

	register U16 ReadValue;

	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->ECR;

	if( 0 != pbEnable )
	{
		*pbEnable = (CBOOL)((ReadValue & TTE_MASK) >> TTE_BITPOS );
	}

	if( 0 != pdwPID )
	{
		*pdwPID = ((ReadValue & TTS_MASK) >> TTS_BITPOS );
	}

}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clear data PID
 *	@return		None.
 *	@remarks	In Rx Mode : Data toggle bit in core to be compared with the data PID
 *				of received packet is reset to 0.\n
 *				In Tx Mode : Tx data PID to be transmitted to host is reset to 0. \n
 *	@see		NX_UDC_IsClearedDataPID
 */
void	NX_UDC_ClearDataPID( void )
{
	const U16 CDP_BITPOS	= 2;
	const U16 CDP_MASK		= 1 << CDP_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->ECR |= CDP_MASK;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check data PID is cleared or NOT
 *	@return		\b CTRUE	indicates that data PID is cleared. \n
 *				\b CFALSE	indicates that data PID is \b NOT cleared.
	*	@see		NX_UDC_ClearDataPID
 */
CBOOL	NX_UDC_IsClearedDataPID( void )
{
	const U16 CDP_BITPOS	= 2;
	const U16 CDP_MASK		= 1 << CDP_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	if(__g_pRegister->ECR & CDP_MASK)
	{
		return CFALSE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Stall operation
 *	@param[in]	bEnable	\b CTRUE	indicates that Endpoint stall enable. \n
 *						\b CFALSE	indicates that Endpoint stall disable.
 *	@return		None.
 *	@remarks	Send STALL handshake to Host. \n
 *				User should clear stall state after send stall handshake.
 *	@code
 *
 *		NX_UDC_SetStall( CTRUE );	// Send Stall handshake
 *		...
 *		NX_UDC_SetStall( CFALSE );	// Not Send Stall handshake
 *
 *	@endcode
 *	@see	NX_UDC_GetStall, NX_UDC_SetEP0Stall, NX_UDC_GetEP0Stall
 */
void	NX_UDC_SetStall( CBOOL bEnable )
{
	const U16 ESS_BITPOS	= 1;
	const U16 ESS_MASK		= 1 << ESS_BITPOS;

	register U16	ReadValue;

	NX_ASSERT( (0 == bEnable) || (1 == bEnable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->ECR;

	ReadValue &= ~ESS_MASK;
	ReadValue |= ((U32)bEnable << ESS_BITPOS);

	__g_pRegister->ECR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Endpoint0's stall status
 *	@return		\b CTRUE	indicates that Endpoint0 stall is enabled.
				\b CFALSE	indicates that Endpoint0 stall is disabled.
 *	@see		NX_UDC_SetStall,	 NX_UDC_SetEP0Stall,	NX_UDC_GetEP0Stall
 */
CBOOL	NX_UDC_GetStall( void )
{
	const U16 ESS_BITPOS	= 1;
	const U16 ESS_MASK		= 1 << ESS_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->ECR & ESS_MASK) >> ESS_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Rx packet size from USB host
 *	@return		Rx packet size
 *	@see		NX_UDC_SetTxPacketSize,		NX_UDC_GetTxPacketSize
 */
U32		NX_UDC_GetRxPacketSize( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(__g_pRegister->BRCR * 2 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Tx packet size
 *	@param[in]	dwSize	Tx packet size
 *	@return		None.
 *	@remarks	If Zero byte is needed to be transmitted to Host, Set to 0.
 *	@see		NX_UDC_GetTxPacketSize,		NX_UDC_GetRxPacketSize
 */
void		NX_UDC_SetTxPacketSize( U32 dwSize )
{
	NX_ASSERT( 0x400 > dwSize );
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->BWCR = (U16)dwSize;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Tx packet size.
 *	@return		Tx packet size
 *	@see		NX_UDC_SetTxPacketSize,		NX_UDC_GetRxPacketSize
 */
U32		NX_UDC_GetTxPacketSize( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)__g_pRegister->BWCR;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set max packet size
 *	@param[in]	MaxPacket	Max packet size ( 0 ~ 1024 )
 *	@return		None.
 *	@see		NX_UDC_GetMaxPacketSize
 */
void	NX_UDC_SetMaxPacketSize( U32	MaxPacket )
{
	NX_ASSERT( 1024 >= MaxPacket );
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->MPR = (U16)MaxPacket;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get max packet size
 *	@return		Max packet size
 *	@see		NX_UDC_SetMaxPacketSize
 */
U32	NX_UDC_GetMaxPacketSize( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)__g_pRegister->MPR;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set AutoRxDMA operation
 *	@param[in]	bEnable	\b CTRUE	indicates that AutoRxDMA function enable. \n
 *						\b CFALSE	indicates that AutoRxDMA function disable.

 *	@return		None.
 *	@remarks	AutoRxDMA means that Run Rx DMA operation automatically when
 *				first OUT packet is received.\n
 *				This function is disabled when DMA operation is ended.\n
 *				If AutoRxDMA mode is disabled, then set to disable of dual FIFO mode.
 *	@see										NX_UDC_GetAutoRxDMAEnable,
 *				NX_UDC_SetDMAMode,				NX_UDC_GetDMAMode,
 *				NX_UDC_SetTxDMARun,				NX_UDC_GetTxDMARun,
 *				NX_UDC_SetRxDMARun,				NX_UDC_GetRxDMARun,
 *				NX_UDC_SetDMAMaxPacketSize,		NX_UDC_GetDMAMaxPacketSize,
 *				NX_UDC_GetDMARxPacketSize,		NX_UDC_SetDMATransferSize,
 *				NX_UDC_GetDMATransferSize
 */
void	NX_UDC_SetAutoRxDMAEnable( CBOOL bEnable )
{
	const U16 ARDRD_BITPOS	= 5;
	const U16 ARDRD_MASK	= 1 << ARDRD_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	if( bEnable )
	{
		__g_pRegister->DCR &= ~ARDRD_MASK;
	}
	else
	{
		__g_pRegister->DCR |= ARDRD_MASK;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get setting value of AutoRxDMA operation
 *	@return		\b CTRUE	indicates that AutoRxDMA function is enabled. \n
 *				\b CFALSE	indicates that AutoRxDMA function is disabled.
 *	@see		NX_UDC_SetAutoRxDMAEnable,
 *				NX_UDC_SetDMAMode,				NX_UDC_GetDMAMode,
 *				NX_UDC_SetTxDMARun,				NX_UDC_GetTxDMARun,
 *				NX_UDC_SetRxDMARun,				NX_UDC_GetRxDMARun,
 *				NX_UDC_SetDMAMaxPacketSize,		NX_UDC_GetDMAMaxPacketSize,
 *				NX_UDC_GetDMARxPacketSize,		NX_UDC_SetDMATransferSize,
 *				NX_UDC_GetDMATransferSize
 */
CBOOL	NX_UDC_GetAutoRxDMAEnable( void )
{
	const U16 ARDRD_BITPOS	= 5;
	const U16 ARDRD_MASK	= 1 << ARDRD_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	if(__g_pRegister->DCR & ARDRD_MASK)
	{
		return CFALSE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set DMA mode
 *	@param[in]	bEnable	\b CTRUE	indicates that DMA mode enable. \n
 *						\b CFALSE	indicates that DMA mode disable.
 *	@return		None.
 *	@see		NX_UDC_SetAutoRxDMAEnable,		NX_UDC_GetAutoRxDMAEnable,
 *												NX_UDC_GetDMAMode,
 *				NX_UDC_SetTxDMARun,				NX_UDC_GetTxDMARun,
 *				NX_UDC_SetRxDMARun,				NX_UDC_GetRxDMARun,
 *				NX_UDC_SetDMAMaxPacketSize,		NX_UDC_GetDMAMaxPacketSize,
 *				NX_UDC_GetDMARxPacketSize,		NX_UDC_SetDMATransferSize,
 *				NX_UDC_GetDMATransferSize
 */
void	NX_UDC_SetDMAMode( CBOOL bEnable )
{
	const U16 DEN_BITPOS	= 0;
	const U16 FMDE_BITPOS	= 4;

	const U16 DEN_MASK		= 0x01 << DEN_BITPOS;
	const U16 FMDE_MASK		= 0x01 << FMDE_BITPOS;
	const U16 RESERVED_MASK = 0xFFC8;

	register U16 ReadValue;

	NX_ASSERT( (0 == bEnable ) || (1 == bEnable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->DCR;

	ReadValue &= ~( DEN_MASK | RESERVED_MASK );
	ReadValue |= ((bEnable<<DEN_BITPOS) | FMDE_MASK );

	__g_pRegister->DCR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Setting value of DMA mode
 *	@return		\b CTRUE	indicates that DMA mode is enabled. \n
 *				\b CFALSE	indicates that DMA mode is disabled.
 *	@see		NX_UDC_SetAutoRxDMAEnable,		NX_UDC_GetAutoRxDMAEnable,
 *				NX_UDC_SetDMAMode,
 *				NX_UDC_SetTxDMARun,				NX_UDC_GetTxDMARun,
 *				NX_UDC_SetRxDMARun,				NX_UDC_GetRxDMARun,
 *				NX_UDC_SetDMAMaxPacketSize,		NX_UDC_GetDMAMaxPacketSize,
 *				NX_UDC_GetDMARxPacketSize,		NX_UDC_SetDMATransferSize,
 *				NX_UDC_GetDMATransferSize
 */
CBOOL	NX_UDC_GetDMAMode( void )
{
	const U16 DEN_BITPOS	= 0;
	const U16 DEN_MASK		= 0x01 << DEN_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->DCR & DEN_MASK )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set TxDMARun operation
 *	@param[in]	bEnable	\b CTRUE	indicates that TxDMARun enable. \n
 *						\b CFALSE	indicates that TxDMARun disable.
 *	@return		None.
 *	@remarks	This function to set start DMA operation for Tx Endpoint( IN endpoint ).
 *	@see		NX_UDC_SetAutoRxDMAEnable,		NX_UDC_GetAutoRxDMAEnable,
 *				NX_UDC_SetDMAMode,				NX_UDC_GetDMAMode,
 *												NX_UDC_GetTxDMARun,
 *				NX_UDC_SetRxDMARun,				NX_UDC_GetRxDMARun,
 *				NX_UDC_SetDMAMaxPacketSize,		NX_UDC_GetDMAMaxPacketSize,
 *				NX_UDC_GetDMARxPacketSize,		NX_UDC_SetDMATransferSize,
 *				NX_UDC_GetDMATransferSize
 */
void	NX_UDC_SetTxDMARun( CBOOL bEnable )
{
	const U16 TDR_BITPOS	= 2;
	const U16 TDR_MASK		= 1 << TDR_BITPOS;

	register U16	ReadValue;

	NX_ASSERT( (0 == bEnable) || (1 == bEnable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->DCR;

	ReadValue &= ~TDR_MASK;
	ReadValue |= ((U32)bEnable << TDR_BITPOS);

	__g_pRegister->DCR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Setting value of TxDMARun operation
 *	@return		\b CTRUE	indicates that TxDMARun is enabled. \n
 *				\b CFALSE	indicates that TxDMARun is disabled.
 *	@see		NX_UDC_SetAutoRxDMAEnable,		NX_UDC_GetAutoRxDMAEnable,
 *				NX_UDC_SetDMAMode,				NX_UDC_GetDMAMode,
 *				NX_UDC_SetTxDMARun,
 *				NX_UDC_SetRxDMARun,				NX_UDC_GetRxDMARun,
 *				NX_UDC_SetDMAMaxPacketSize,		NX_UDC_GetDMAMaxPacketSize,
 *				NX_UDC_GetDMARxPacketSize,		NX_UDC_SetDMATransferSize,
 *				NX_UDC_GetDMATransferSize
 */
CBOOL	NX_UDC_GetTxDMARun( void )
{
	const U16 TDR_BITPOS	= 2;
	const U16 TDR_MASK		= 1 << TDR_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->DCR & TDR_MASK) >> TDR_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set RxDMARun operation
 *	@param[in]	bEnable	\b CTRUE	indicates that RxDMARun enable. \n
 *						\b CFALSE	indicates that RxDMARun disable.
 *	@return		None.
 *	@remarks	RxDMArun is automatically set when USB receives OUT packet data and
 *				DMAmode is enabled and AutoRxDMARun is enabled.
 *	@see		NX_UDC_SetAutoRxDMAEnable,		NX_UDC_GetAutoRxDMAEnable,
 *				NX_UDC_SetDMAMode,				NX_UDC_GetDMAMode,
 *				NX_UDC_SetTxDMARun,				NX_UDC_GetTxDMARun,
 *												NX_UDC_GetRxDMARun,
 *				NX_UDC_SetDMAMaxPacketSize,		NX_UDC_GetDMAMaxPacketSize,
 *				NX_UDC_GetDMARxPacketSize,		NX_UDC_SetDMATransferSize,
 *				NX_UDC_GetDMATransferSize
 */
void	NX_UDC_SetRxDMARun( CBOOL bEnable )
{
	const U16 RDR_BITPOS	= 1;
	const U16 RDR_MASK		= 1 << RDR_BITPOS;

	register U16	ReadValue;

	NX_ASSERT( (0 == bEnable) || (1 == bEnable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->DCR;

	ReadValue &= ~RDR_MASK;
	ReadValue |= ((U32)bEnable << RDR_BITPOS);

	__g_pRegister->DCR = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get setting value RxDMARun operation
 *	@return		None.
 *	@see		NX_UDC_SetAutoRxDMAEnable,		NX_UDC_GetAutoRxDMAEnable,
 *				NX_UDC_SetDMAMode,				NX_UDC_GetDMAMode,
 *				NX_UDC_SetTxDMARun,				NX_UDC_GetTxDMARun,
 *				NX_UDC_SetRxDMARun,
 *				NX_UDC_SetDMAMaxPacketSize,		NX_UDC_GetDMAMaxPacketSize,
 *				NX_UDC_GetDMARxPacketSize,		NX_UDC_SetDMATransferSize,
 *				NX_UDC_GetDMATransferSize
 */
CBOOL	NX_UDC_GetRxDMARun( void )
{
	const U16 RDR_BITPOS	= 1;
	const U16 RDR_MASK		= 1 << RDR_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->DCR & RDR_MASK) >> RDR_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set max packet size in DMA mode
 *	@param[in]	dwSize	Max packet size.
 *	@return		None.
 *	@see		NX_UDC_SetAutoRxDMAEnable,		NX_UDC_GetAutoRxDMAEnable,
 *				NX_UDC_SetDMAMode,				NX_UDC_GetDMAMode,
 *				NX_UDC_SetTxDMARun,				NX_UDC_GetTxDMARun,
 *				NX_UDC_SetRxDMARun,				NX_UDC_GetRxDMARun,
 *												NX_UDC_GetDMAMaxPacketSize,
 *				NX_UDC_GetDMARxPacketSize,		NX_UDC_SetDMATransferSize,
 *				NX_UDC_GetDMATransferSize
 */
void	NX_UDC_SetDMAMaxPacketSize( U32 dwSize)
{
	NX_ASSERT( CNULL != dwSize );
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->DTCR = dwSize;
	__g_pRegister->DFCR = dwSize;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get setting value of max packet size in DMA mode
 *	@return		Max packet size
 *	@see		NX_UDC_SetAutoRxDMAEnable,		NX_UDC_GetAutoRxDMAEnable,
 *				NX_UDC_SetDMAMode,				NX_UDC_GetDMAMode,
 *				NX_UDC_SetTxDMARun,				NX_UDC_GetTxDMARun,
 *				NX_UDC_SetRxDMARun,				NX_UDC_GetRxDMARun,
 *				NX_UDC_SetDMAMaxPacketSize,
 *				NX_UDC_GetDMARxPacketSize,		NX_UDC_SetDMATransferSize,
 *				NX_UDC_GetDMATransferSize
 */
U32	NX_UDC_GetDMAMaxPacketSize( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(__g_pRegister->DTCR);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get rx packet size in DMA mode
 *	@return		Rx packet size
 *	@see		NX_UDC_SetAutoRxDMAEnable,		NX_UDC_GetAutoRxDMAEnable,
 *				NX_UDC_SetDMAMode,				NX_UDC_GetDMAMode,
 *				NX_UDC_SetTxDMARun,				NX_UDC_GetTxDMARun,
 *				NX_UDC_SetRxDMARun,				NX_UDC_GetRxDMARun,
 *				NX_UDC_SetDMAMaxPacketSize,		NX_UDC_GetDMAMaxPacketSize,
 *												NX_UDC_SetDMATransferSize,
 *				NX_UDC_GetDMATransferSize
 */
U32		NX_UDC_GetDMARxPacketSize( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (U16)(__g_pRegister->DFCR);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set total transfer size in DMA mode.
 *	@param[in]	dwTotalSize	Total transfer size.
 *	@return		None.
 *	@see		NX_UDC_SetAutoRxDMAEnable,		NX_UDC_GetAutoRxDMAEnable,
 *				NX_UDC_SetDMAMode,				NX_UDC_GetDMAMode,
 *				NX_UDC_SetTxDMARun,				NX_UDC_GetTxDMARun,
 *				NX_UDC_SetRxDMARun,				NX_UDC_GetRxDMARun,
 *				NX_UDC_SetDMAMaxPacketSize,		NX_UDC_GetDMAMaxPacketSize,
 *				NX_UDC_GetDMARxPacketSize,
 *				NX_UDC_GetDMATransferSize
 */
void	NX_UDC_SetDMATransferSize( U32	dwTotalSize )
{
	NX_ASSERT( CNULL != dwTotalSize );
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->DTTCR	= dwTotalSize & 0xFFFF;
	__g_pRegister->DTTCR2 = (dwTotalSize >> 16) & 0xFFFF;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get total transfer size in DMA mode.
 *	@return		total transfer size
 *	@see		NX_UDC_SetAutoRxDMAEnable,		NX_UDC_GetAutoRxDMAEnable,
 *				NX_UDC_SetDMAMode,				NX_UDC_GetDMAMode,
 *				NX_UDC_SetTxDMARun,				NX_UDC_GetTxDMARun,
 *				NX_UDC_SetRxDMARun,				NX_UDC_GetRxDMARun,
 *				NX_UDC_SetDMAMaxPacketSize,		NX_UDC_GetDMAMaxPacketSize,
 *				NX_UDC_GetDMARxPacketSize,		NX_UDC_SetDMATransferSize
 */
U32	NX_UDC_GetDMATransferSize( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)( (U32)(__g_pRegister->DTTCR2<<16) | (U32)(__g_pRegister->DTTCR) );
}
