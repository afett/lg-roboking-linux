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
//	Module		: SCALER
//	File		: nx_scaler.c
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------
#include <plat/nx_scaler.h>

static	struct	NX_SCALER_RegisterSet *__g_pRegister = CNULL;

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b CTRUE	indicates that Initialize is successed.\n
 *			\b CFALSE	indicates that Initialize is failed.\n
 *	@see	NX_SCALER_GetNumberOfModule
 */
CBOOL	NX_SCALER_Initialize( void )
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
 *	@see		NX_SCALER_Initialize
 */
U32		NX_SCALER_GetNumberOfModule( void )
{
	return NUMBER_OF_SCALER_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@return		Module's physical address
 *	@see		NX_SCALER_GetSizeOfRegisterSet,
 *				NX_SCALER_SetBaseAddress,		NX_SCALER_GetBaseAddress,
 *				NX_SCALER_OpenModule,			NX_SCALER_CloseModule,
 *				NX_SCALER_CheckBusy,			NX_SCALER_CanPowerDown
 */
U32		NX_SCALER_GetPhysicalAddress( void )
{
	return	(U32)( PHY_BASEADDR_SCALER_MODULE );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_SCALER_GetPhysicalAddress,
 *				NX_SCALER_SetBaseAddress,		NX_SCALER_GetBaseAddress,
 *				NX_SCALER_OpenModule,			NX_SCALER_CloseModule,
 *				NX_SCALER_CheckBusy,			NX_SCALER_CanPowerDown
 */
U32		NX_SCALER_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_SCALER_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_SCALER_GetPhysicalAddress,	NX_SCALER_GetSizeOfRegisterSet,
 *				NX_SCALER_GetBaseAddress,
 *				NX_SCALER_OpenModule,			NX_SCALER_CloseModule,
 *				NX_SCALER_CheckBusy,			NX_SCALER_CanPowerDown
 */
void	NX_SCALER_SetBaseAddress( U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );

	__g_pRegister = (struct NX_SCALER_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@return		Module's base address.
 *	@see		NX_SCALER_GetPhysicalAddress,	NX_SCALER_GetSizeOfRegisterSet,
 *				NX_SCALER_SetBaseAddress,
 *				NX_SCALER_OpenModule,			NX_SCALER_CloseModule,
 *				NX_SCALER_CheckBusy,			NX_SCALER_CanPowerDown
 */
U32		NX_SCALER_GetBaseAddress( void )
{
	return (U32)__g_pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@return		\b CTRUE	indicates that Initialize is successed. \n
 *				\b CFALSE	indicates that Initialize is failed.
 *	@see		NX_SCALER_GetPhysicalAddress,	NX_SCALER_GetSizeOfRegisterSet,
 *				NX_SCALER_SetBaseAddress,		NX_SCALER_GetBaseAddress,
 *				NX_SCALER_CloseModule,
 *				NX_SCALER_CheckBusy,			NX_SCALER_CanPowerDown
 */
CBOOL	NX_SCALER_OpenModule( void )
{
//	__g_pRegister->SCRUNREG			= 0x00000000;
//	__g_pRegister->SCCFGREG			= 0x00000000;
//	__g_pRegister->SCINTREG			= 0x00000100;
//	__g_pRegister->SCSRCADDREG		= 0x00000000;
//	__g_pRegister->SCSRCSIZEREG		= 0x00000000;
//	__g_pRegister->SCDESTADDREG		= 0x00000000;
//	__g_pRegister->SCDESTSIZEREG	= 0x00000000;
//	__g_pRegister->DELTAXREG		= 0x00000000;
//	__g_pRegister->DELTAYREG		= 0x00000000;
//	__g_pRegister->HVSOFTREG		= 0x00000000;
//	__g_pRegister->CLKENB			= 0x00000000;
	WriteIODW(&__g_pRegister->SCRUNREG, 0x00000000);
	WriteIODW(&__g_pRegister->SCCFGREG, 0x00000000);
	WriteIODW(&__g_pRegister->SCINTREG, 0x00000100);
	WriteIODW(&__g_pRegister->SCSRCADDREG, 0x00000000);
	WriteIODW(&__g_pRegister->SCSRCSIZEREG, 0x00000000);
	WriteIODW(&__g_pRegister->SCDESTADDREG, 0x00000000);
	WriteIODW(&__g_pRegister->SCDESTSIZEREG, 0x00000000);
	WriteIODW(&__g_pRegister->DELTAXREG, 0x00000000);
	WriteIODW(&__g_pRegister->DELTAYREG, 0x00000000);
	WriteIODW(&__g_pRegister->HVSOFTREG, 0x00000000);
	WriteIODW(&__g_pRegister->CLKENB, 0x00000000);

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@return		\b CTRUE	indicates that Deinitialize is successed. \n
 *				\b CFALSE	indicates that Deinitialize is failed.
 *	@see		NX_SCALER_GetPhysicalAddress,	NX_SCALER_GetSizeOfRegisterSet,
 *				NX_SCALER_SetBaseAddress,		NX_SCALER_GetBaseAddress,
 *				NX_SCALER_OpenModule,
 *				NX_SCALER_CheckBusy,			NX_SCALER_CanPowerDown
 */
CBOOL	NX_SCALER_CloseModule( void )
{
//	__g_pRegister->SCRUNREG			= 0x00000000;
//	__g_pRegister->SCCFGREG			= 0x00000000;
//	__g_pRegister->SCINTREG			= 0x00000100;
//	__g_pRegister->SCSRCADDREG		= 0x00000000;
//	__g_pRegister->SCSRCSIZEREG		= 0x00000000;
//	__g_pRegister->SCDESTADDREG		= 0x00000000;
//	__g_pRegister->SCDESTSIZEREG	= 0x00000000;
//	__g_pRegister->DELTAXREG		= 0x00000000;
//	__g_pRegister->DELTAYREG		= 0x00000000;
//	__g_pRegister->HVSOFTREG		= 0x00000000;
//	__g_pRegister->CLKENB			= 0x00000000;
	WriteIODW(&__g_pRegister->SCRUNREG, 0x00000000);
	WriteIODW(&__g_pRegister->SCCFGREG, 0x00000000);
	WriteIODW(&__g_pRegister->SCINTREG, 0x00000100);
	WriteIODW(&__g_pRegister->SCSRCADDREG, 0x00000000);
	WriteIODW(&__g_pRegister->SCSRCSIZEREG, 0x00000000);
	WriteIODW(&__g_pRegister->SCDESTADDREG, 0x00000000);
	WriteIODW(&__g_pRegister->SCDESTSIZEREG, 0x00000000);
	WriteIODW(&__g_pRegister->DELTAXREG, 0x00000000);
	WriteIODW(&__g_pRegister->DELTAYREG, 0x00000000);
	WriteIODW(&__g_pRegister->HVSOFTREG, 0x00000000);
	WriteIODW(&__g_pRegister->CLKENB, 0x00000000);

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@return		\b CTRUE	indicates that Module is Busy. \n
 *				\b CFALSE	indicates that Module is NOT Busy.
 *	@see		NX_SCALER_GetPhysicalAddress,	NX_SCALER_GetSizeOfRegisterSet,
 *				NX_SCALER_SetBaseAddress,		NX_SCALER_GetBaseAddress,
 *				NX_SCALER_OpenModule,			NX_SCALER_CloseModule,
 *				NX_SCALER_CanPowerDown
 */
CBOOL	NX_SCALER_CheckBusy( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return NX_SCALER_IsBusy();
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@return		\b CTRUE	indicates that Ready to enter power-down stage. \n
 *				\b CFALSE	indicates that This module can't enter to power-down stage.
 *	@see		NX_SCALER_GetPhysicalAddress,	NX_SCALER_GetSizeOfRegisterSet,
 *				NX_SCALER_SetBaseAddress,		NX_SCALER_GetBaseAddress,
 *				NX_SCALER_OpenModule,			NX_SCALER_CloseModule,
 *				NX_SCALER_CheckBusy
 */
CBOOL	NX_SCALER_CanPowerDown( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number for interrupt controller.
 *	@return		Interrupt number
 *	@see												NX_SCALER_SetInterruptEnable,
 *				NX_SCALER_GetInterruptEnable,			NX_SCALER_SetInterruptEnable32,
 *				NX_SCALER_GetInterruptEnable32,			NX_SCALER_GetInterruptPending,
 *				NX_SCALER_GetInterruptPending32,		NX_SCALER_ClearInterruptPending,
 *				NX_SCALER_ClearInterruptPending32,		NX_SCALER_SetInterruptEnableAll,
 *				NX_SCALER_GetInterruptEnableAll,		NX_SCALER_GetInterruptPendingAll,
 *				NX_SCALER_ClearInterruptPendingAll,		NX_SCALER_GetInterruptPendingNumber
 */
S32		NX_SCALER_GetInterruptNumber( void )
{
	return	INTNUM_OF_SCALER_MODULE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	IntNum	Interrupt Number ( 0: Scaler End ).
 *	@param[in]	Enable	\b CTRUE	indicates that Interrupt Enable. \n
 *						\b CFALSE	indicates that Interrupt Disable.
 *	@return		None.
 *	@see		NX_SCALER_GetInterruptNumber,
 *				NX_SCALER_GetInterruptEnable,			NX_SCALER_SetInterruptEnable32,
 *				NX_SCALER_GetInterruptEnable32,			NX_SCALER_GetInterruptPending,
 *				NX_SCALER_GetInterruptPending32,		NX_SCALER_ClearInterruptPending,
 *				NX_SCALER_ClearInterruptPending32,		NX_SCALER_SetInterruptEnableAll,
 *				NX_SCALER_GetInterruptEnableAll,		NX_SCALER_GetInterruptPendingAll,
 *				NX_SCALER_ClearInterruptPendingAll,		NX_SCALER_GetInterruptPendingNumber
 */
void	NX_SCALER_SetInterruptEnable( S32 IntNum, CBOOL Enable )
{
	const U32 SC_INT_ENB_BITPOS = 16;

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( (0 == Enable) || (1 == Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	IntNum = IntNum;

//	__g_pRegister->SCINTREG = ((U32)Enable << SC_INT_ENB_BITPOS);
	WriteIODW(&__g_pRegister->SCINTREG, ((U32)Enable << SC_INT_ENB_BITPOS));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	IntNum	Interrupt Number( 0: Scaler End ).
 *	@return		\b CTRUE	indicates that Interrupt is enabled. \n
 *				\b CFALSE	indicates that Interrupt is disabled.
 *	@see		NX_SCALER_GetInterruptNumber,			NX_SCALER_SetInterruptEnable,
 *														NX_SCALER_SetInterruptEnable32,
 *				NX_SCALER_GetInterruptEnable32,			NX_SCALER_GetInterruptPending,
 *				NX_SCALER_GetInterruptPending32,		NX_SCALER_ClearInterruptPending,
 *				NX_SCALER_ClearInterruptPending32,		NX_SCALER_SetInterruptEnableAll,
 *				NX_SCALER_GetInterruptEnableAll,		NX_SCALER_GetInterruptPendingAll,
 *				NX_SCALER_ClearInterruptPendingAll,		NX_SCALER_GetInterruptPendingNumber
 */
CBOOL	NX_SCALER_GetInterruptEnable( S32 IntNum )
{
	const U32 SC_INT_ENB_BITPOS = 16;

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	IntNum = IntNum;

	return (CBOOL)((__g_pRegister->SCINTREG >> SC_INT_ENB_BITPOS) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n
 *							- EnableFlag[0] : Set Scaler Done interrupt enable or disable. \n
 *	@return		None.
 *	@see		NX_SCALER_GetInterruptNumber,			NX_SCALER_SetInterruptEnable,
 *				NX_SCALER_GetInterruptEnable,
 *				NX_SCALER_GetInterruptEnable32,			NX_SCALER_GetInterruptPending,
 *				NX_SCALER_GetInterruptPending32,		NX_SCALER_ClearInterruptPending,
 *				NX_SCALER_ClearInterruptPending32,		NX_SCALER_SetInterruptEnableAll,
 *				NX_SCALER_GetInterruptEnableAll,		NX_SCALER_GetInterruptPendingAll,
 *				NX_SCALER_ClearInterruptPendingAll,		NX_SCALER_GetInterruptPendingNumber
 */
void	NX_SCALER_SetInterruptEnable32( U32 EnableFlag )
{
	const U32	SC_INT_ENB_BITPOS = 16;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->SCINTREG = ((EnableFlag & 0x01) << SC_INT_ENB_BITPOS);
	WriteIODW(&__g_pRegister->SCINTREG, ((EnableFlag & 0x01) << SC_INT_ENB_BITPOS));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt enable bit.
 *	@return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0] : Scaler Done interrupt's setting value. \n
 *	@see		NX_SCALER_GetInterruptNumber,			NX_SCALER_SetInterruptEnable,
 *				NX_SCALER_GetInterruptEnable,			NX_SCALER_SetInterruptEnable32,
 *														NX_SCALER_GetInterruptPending,
 *				NX_SCALER_GetInterruptPending32,		NX_SCALER_ClearInterruptPending,
 *				NX_SCALER_ClearInterruptPending32,		NX_SCALER_SetInterruptEnableAll,
 *				NX_SCALER_GetInterruptEnableAll,		NX_SCALER_GetInterruptPendingAll,
 *				NX_SCALER_ClearInterruptPendingAll,		NX_SCALER_GetInterruptPendingNumber
 */
U32		NX_SCALER_GetInterruptEnable32( void )
{
	const U32	SC_INT_ENB_BITPOS	= 16;
	const U32	SC_INT_ENB_MASK		= 1<<SC_INT_ENB_BITPOS;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)((__g_pRegister->SCINTREG & SC_INT_ENB_MASK)>>SC_INT_ENB_BITPOS);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *	@param[in]	IntNum	Interrupt Number( 0 : Scaler End ).
 *	@return		\b CTRUE	indicates that Pending is seted. \n
 *				\b CFALSE	indicates that Pending is Not Seted.
 *	@see		NX_SCALER_GetInterruptNumber,			NX_SCALER_SetInterruptEnable,
 *				NX_SCALER_GetInterruptEnable,			NX_SCALER_SetInterruptEnable32,
 *				NX_SCALER_GetInterruptEnable32,
 *				NX_SCALER_GetInterruptPending32,		NX_SCALER_ClearInterruptPending,
 *				NX_SCALER_ClearInterruptPending32,		NX_SCALER_SetInterruptEnableAll,
 *				NX_SCALER_GetInterruptEnableAll,		NX_SCALER_GetInterruptPendingAll,
 *				NX_SCALER_ClearInterruptPendingAll,	NX_SCALER_GetInterruptPendingNumber
 */
CBOOL	NX_SCALER_GetInterruptPending( S32 IntNum )
{
	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	IntNum = IntNum;

	return (CBOOL)( __g_pRegister->SCINTREG & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt pending bit.
 *	@return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bit is NOT occured. \n
 *				- Return Value[0] : Scaler Done pending state. \n
 *	@see		NX_SCALER_GetInterruptNumber,			NX_SCALER_SetInterruptEnable,
 *				NX_SCALER_GetInterruptEnable,			NX_SCALER_SetInterruptEnable32,
 *				NX_SCALER_GetInterruptEnable32,			NX_SCALER_GetInterruptPending,
 *														NX_SCALER_ClearInterruptPending,
 *				NX_SCALER_ClearInterruptPending32,		NX_SCALER_SetInterruptEnableAll,
 *				NX_SCALER_GetInterruptEnableAll,		NX_SCALER_GetInterruptPendingAll,
 *				NX_SCALER_ClearInterruptPendingAll,		NX_SCALER_GetInterruptPendingNumber
 */
U32		NX_SCALER_GetInterruptPending32( void )
{
	const U32 PEND_MASK = 0x01;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(__g_pRegister->SCINTREG & PEND_MASK);
}


//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	IntNum	Interrupt number( 0 : Scaler End ).
 *	@return		None.
 *	@see		NX_SCALER_GetInterruptNumber,			NX_SCALER_SetInterruptEnable,
 *				NX_SCALER_GetInterruptEnable,			NX_SCALER_SetInterruptEnable32,
 *				NX_SCALER_GetInterruptEnable32,			NX_SCALER_GetInterruptPending,
 *				NX_SCALER_GetInterruptPending32,
 *				NX_SCALER_ClearInterruptPending32,		NX_SCALER_SetInterruptEnableAll,
 *				NX_SCALER_GetInterruptEnableAll,		NX_SCALER_GetInterruptPendingAll,
 *				NX_SCALER_ClearInterruptPendingAll,		NX_SCALER_GetInterruptPendingNumber
 */
void	NX_SCALER_ClearInterruptPending( S32 IntNum )
{
	const U32 SC_INT_CLR_BITPOS = 8;
	register U32 regval;

	NX_ASSERT( 0 == IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	IntNum = IntNum;

//	__g_pRegister->SCINTREG |= (0x01 << SC_INT_CLR_BITPOS);
	regval = __g_pRegister->SCINTREG | (0x01 << SC_INT_CLR_BITPOS);
	WriteIODW(&__g_pRegister->SCINTREG, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below	\n \n
 *								- PendingFlag[0] : Scaler Done pending bit. \n
 *	@return		None.
 *	@see		NX_SCALER_GetInterruptNumber,			NX_SCALER_SetInterruptEnable,
 *				NX_SCALER_GetInterruptEnable,			NX_SCALER_SetInterruptEnable32,
 *				NX_SCALER_GetInterruptEnable32,			NX_SCALER_GetInterruptPending,
 *				NX_SCALER_GetInterruptPending32,		NX_SCALER_ClearInterruptPending,
 *														NX_SCALER_SetInterruptEnableAll,
 *				NX_SCALER_GetInterruptEnableAll,		NX_SCALER_GetInterruptPendingAll,
 *				NX_SCALER_ClearInterruptPendingAll,		NX_SCALER_GetInterruptPendingNumber
 */
void	NX_SCALER_ClearInterruptPending32( U32 PendingFlag )
{
	const U32	PEND_BITPOS = 8;
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->SCINTREG |=	(PendingFlag&0x01) << PEND_BITPOS;
	regval = __g_pRegister->SCINTREG | (PendingFlag&0x01) << PEND_BITPOS;
	WriteIODW(&__g_pRegister->SCINTREG, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enables or disables.
 *	@param[in]	Enable	\b CTRUE	indicates that Set to all interrupt enable. \n
 *						\b CFALSE	indicates that Set to all interrupt disable.
 *	@return		None.
 *	@see		NX_SCALER_GetInterruptNumber,			NX_SCALER_SetInterruptEnable,
 *				NX_SCALER_GetInterruptEnable,			NX_SCALER_SetInterruptEnable32,
 *				NX_SCALER_GetInterruptEnable32,			NX_SCALER_GetInterruptPending,
 *				NX_SCALER_GetInterruptPending32,		NX_SCALER_ClearInterruptPending,
 *				NX_SCALER_ClearInterruptPending32,
 *				NX_SCALER_GetInterruptEnableAll,		NX_SCALER_GetInterruptPendingAll,
 *				NX_SCALER_ClearInterruptPendingAll,		NX_SCALER_GetInterruptPendingNumber
 */
void	NX_SCALER_SetInterruptEnableAll( CBOOL Enable )
{
	const U32 SC_INT_ENB_BITPOS = 16;

	NX_ASSERT( (0 == Enable) || (1 == Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->SCINTREG = ((U32)Enable << SC_INT_ENB_BITPOS);
	WriteIODW(&__g_pRegister->SCINTREG, ((U32)Enable << SC_INT_ENB_BITPOS));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enable or not.
 *	@return		\b CTRUE	indicates that At least one( or more ) interrupt is enabled. \n
 *				\b CFALSE	indicates that All interrupt is disabled.
 *	@see		NX_SCALER_GetInterruptNumber,			NX_SCALER_SetInterruptEnable,
 *				NX_SCALER_GetInterruptEnable,			NX_SCALER_SetInterruptEnable32,
 *				NX_SCALER_GetInterruptEnable32,			NX_SCALER_GetInterruptPending,
 *				NX_SCALER_GetInterruptPending32,		NX_SCALER_ClearInterruptPending,
 *				NX_SCALER_ClearInterruptPending32,		NX_SCALER_SetInterruptEnableAll,
 *														NX_SCALER_GetInterruptPendingAll,
 *				NX_SCALER_ClearInterruptPendingAll,		NX_SCALER_GetInterruptPendingNumber
 */
CBOOL	NX_SCALER_GetInterruptEnableAll( void )
{
	const U32 SC_INT_ENB_BITPOS = 16;

	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)((__g_pRegister->SCINTREG >> SC_INT_ENB_BITPOS) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are pended or not.
 *	@return		\b CTRUE	indicates that At least one( or more ) pending is seted. \n
 *				\b CFALSE	indicates that All pending is NOT seted.
 *	@see		NX_SCALER_GetInterruptNumber,			NX_SCALER_SetInterruptEnable,
 *				NX_SCALER_GetInterruptEnable,			NX_SCALER_SetInterruptEnable32,
 *				NX_SCALER_GetInterruptEnable32,			NX_SCALER_GetInterruptPending,
 *				NX_SCALER_GetInterruptPending32,		NX_SCALER_ClearInterruptPending,
 *				NX_SCALER_ClearInterruptPending32,		NX_SCALER_SetInterruptEnableAll,
 *				NX_SCALER_GetInterruptEnableAll,
 *				NX_SCALER_ClearInterruptPendingAll,		NX_SCALER_GetInterruptPendingNumber
 */
CBOOL	NX_SCALER_GetInterruptPendingAll( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)(__g_pRegister->SCINTREG & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@return		None.
 *	@see		NX_SCALER_GetInterruptNumber,			NX_SCALER_SetInterruptEnable,
 *				NX_SCALER_GetInterruptEnable,			NX_SCALER_SetInterruptEnable32,
 *				NX_SCALER_GetInterruptEnable32,			NX_SCALER_GetInterruptPending,
 *				NX_SCALER_GetInterruptPending32,		NX_SCALER_ClearInterruptPending,
 *				NX_SCALER_ClearInterruptPending32,		NX_SCALER_SetInterruptEnableAll,
 *				NX_SCALER_GetInterruptEnableAll,		NX_SCALER_GetInterruptPendingAll,
 *														NX_SCALER_GetInterruptPendingNumber
 */
void	NX_SCALER_ClearInterruptPendingAll( void )
{
	const U32 SC_INT_CLR_BITPOS = 8;
	register U32 regval;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->SCINTREG |= (0x01 << SC_INT_CLR_BITPOS);
	regval = __g_pRegister->SCINTREG | (0x01 << SC_INT_CLR_BITPOS);
	WriteIODW(&__g_pRegister->SCINTREG, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number which has the most prority of pended interrupts
 *	@return		Pending Number( If all pending is not set then return -1 ).\n
 *				0 : Scaler End
 *	@see		NX_SCALER_GetInterruptNumber,			NX_SCALER_SetInterruptEnable,
 *				NX_SCALER_GetInterruptEnable,			NX_SCALER_SetInterruptEnable32,
 *				NX_SCALER_GetInterruptEnable32,			NX_SCALER_GetInterruptPending,
 *				NX_SCALER_GetInterruptPending32,		NX_SCALER_ClearInterruptPending,
 *				NX_SCALER_ClearInterruptPending32,		NX_SCALER_SetInterruptEnableAll,
 *				NX_SCALER_GetInterruptEnableAll,		NX_SCALER_GetInterruptPendingAll,
 *				NX_SCALER_ClearInterruptPendingAll	
 */
S32		NX_SCALER_GetInterruptPendingNumber( void )	// -1 if None
{
	const U32 SC_INT_ENB_BITPOS	= 16;
	const U32 SC_INT_PEND_MASK	= 0x01;

	register struct NX_SCALER_RegisterSet	*pRegister;
	register U32 Pend;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;
	Pend = (pRegister->SCINTREG >> SC_INT_ENB_BITPOS) & pRegister->SCINTREG;

	if( Pend & SC_INT_PEND_MASK )
	{
		return 0;
	}

	return -1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a PCLK mode
 *	@param[in]	mode	PCLK mode
 *	@return		None.
 *	@see										NX_SCALER_GetClockPClkMode,
 *				NX_SCALER_SetClockBClkMode,		NX_SCALER_GetClockBClkMode
 */
void			NX_SCALER_SetClockPClkMode( NX_PCLKMODE mode )
{
	const U32 PCLKMODE_POS	=	3;

	register U32 regvalue;
	register struct NX_SCALER_RegisterSet* pRegister;

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

//	pRegister->CLKENB = regvalue;
	WriteIODW(&pRegister->CLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current PCLK mode
 *	@return		Current PCLK mode
 *	@see		NX_SCALER_SetClockPClkMode,
 *				NX_SCALER_SetClockBClkMode,		NX_SCALER_GetClockBClkMode
 */
NX_PCLKMODE	NX_SCALER_GetClockPClkMode( void )
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
 *	@brief		Set System Bus Clock's operation Mode
 *	@param[in]	mode		BCLK Mode
 *	@return		None.
 *	@see		NX_SCALER_SetClockPClkMode,			NX_SCALER_GetClockPClkMode,
 *													NX_SCALER_GetClockBClkMode
 */
void NX_SCALER_SetClockBClkMode( NX_BCLKMODE mode )
{
	register U32 regvalue;
	register struct NX_SCALER_RegisterSet* pRegister;
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
	regvalue &= ~(0x03);
	regvalue |= clkmode & 0x03;
//	pRegister->CLKENB = regvalue;
	WriteIODW(&pRegister->CLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get System Bus Clock's operation Mode
 *	@return		BCLK Mode
 *	@see		NX_SCALER_SetClockPClkMode,		NX_SCALER_GetClockPClkMode,
 *				NX_SCALER_SetClockBClkMode
 */
NX_BCLKMODE	NX_SCALER_GetClockBClkMode( void )
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
// Scaler Hardware Interface Implementation
//------------------------------------------------------------------------------
/**
 *	@brief		Set Scaler Run
 *	@return		None.
 *	@see		NX_SCALER_Stop,		NX_SCALER_IsBusy
 */
void	NX_SCALER_Run( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->SCRUNREG = 0x01;
	WriteIODW(&__g_pRegister->SCRUNREG, 0x01);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Scaler Stop
 *	@return		None.
 *	@see		NX_SCALER_Run,		NX_SCALER_IsBusy
 */
void	NX_SCALER_Stop( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->SCRUNREG = 0x00;
	WriteIODW(&__g_pRegister->SCRUNREG, 0x00);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check scaler's state ( busy or idle )
 *	@return		CTRUE indicates that scaler is running.\n
 *				CFALSE indicates that scaler is idle.
 *	@see		NX_SCALER_Run,		NX_SCALER_Stop
 */
CBOOL	NX_SCALER_IsBusy( void )
{
	const U32	BUSY_MASK = ( 1 << 24 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->SCINTREG & BUSY_MASK )	{ return CTRUE; }
	else										{ return CFALSE; }
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Scaler's Filter Operation
 *	@param[in]	enable		CTRUE( Filter Enable ), CFALSE( Filter Disable )
 *	@return		None.
 *	@see		NX_SCALER_GetFilterEnable
 */
void	NX_SCALER_SetFilterEnable
(
	CBOOL enable
)
{
	const U32	FENB_MASK	= ( 0x03 << 0 );
	register U32	temp;

	NX_ASSERT( CNULL != __g_pRegister );

	temp = __g_pRegister->SCCFGREG ;

	if( CTRUE == enable )
	{
		temp |= FENB_MASK;
	}
	else
	{
		temp &= ~FENB_MASK;
	}

//	__g_pRegister->SCCFGREG = temp;
	WriteIODW(&__g_pRegister->SCCFGREG, temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Check Scaler's Filter Operation
 *	@return		CTRUE	indicates that Filter is Enabled.\n
 *				CFALSE	indicates that Filter is Disabled.
 *	@see		NX_SCALER_SetFilterEnable
 */
CBOOL	NX_SCALER_GetFilterEnable( void )
{
	const U32	FENB_MASK	= ( 0x03 << 0 );
	register U32	temp;

	NX_ASSERT( CNULL != __g_pRegister );

	temp = __g_pRegister->SCCFGREG ;

	if( temp & FENB_MASK )
	{
		return CTRUE;
	}
	else
	{
		return CFALSE;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Horizontal and Vertical Ration Value
 *	@param[in]	RatioH		Value of Horizontal Ratio( 0x0 ~ 0x3F )
 *	@param[in]	RatioV		Value of Vertical Ratio( 0x0 ~ 0x1F )
 *	@return		None.
 *	@see		NX_SCALER_GetFilterRatio
 */
void	NX_SCALER_SetFilterRatio( U32 RatioH, U32 RatioV )
{
	const U32	H_RATIO_BITPOS =	0;
	const U32	V_RATIO_BITPOS = 16;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( (1<<6) > RatioH );
	NX_ASSERT( (1<<5) > RatioV );

//	__g_pRegister->HVSOFTREG = ( (RatioH << H_RATIO_BITPOS ) | ( RatioV << V_RATIO_BITPOS ) );
	WriteIODW(&__g_pRegister->HVSOFTREG, ( (RatioH << H_RATIO_BITPOS ) | ( RatioV << V_RATIO_BITPOS ) ));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Horizontal and Vertical Ration Value
 *	@param[out] RatioH		Value of Horizontal Ratio( 0x0 ~ 0x3F )
 *	@param[out] RatioV		Value of Vertical Ratio( 0x0 ~ 0x1F )
 *	@return		None.
 *	@remarks	Parameter(RatioH, RatioV) can set to CNULL, when the information is not need.
 *	@see		NX_SCALER_SetFilterRatio
 */
void	NX_SCALER_GetFilterRatio( U32* RatioH, U32* RatioV )
{
	const U32	H_RATIO_BITPOS	=	0;
	const U32	H_RATIO_MASK	= ( 0x3F << H_RATIO_BITPOS );
	const U32	V_RATIO_BITPOS	= 16;
	const U32	V_RATIO_MASK	= ( 0x1F << V_RATIO_BITPOS );
	register U32 temp;

	NX_ASSERT( CNULL != __g_pRegister );

	temp = __g_pRegister->HVSOFTREG ;

	if( CNULL != RatioH )
	{
		*RatioH = ( temp & H_RATIO_MASK ) >> H_RATIO_BITPOS;
	}

	if( CNULL != RatioV )
	{
		*RatioV = ( temp & V_RATIO_MASK ) >> V_RATIO_BITPOS;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Scaler's Rotate Mode
 *	@param[in]	rotate		Value of Rotate Mode
 *	@return		None.
 *	@see		NX_SCALER_GetRotateMode
 */
void	NX_SCALER_SetRotateMode
(
	NX_SCALER_ROTATE rotate
)
{
	const U32	ROT_MASK	= ( 0x07 << 24 );
	const U32	ROT_BITPOS	= 24;
	register U32	temp;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( NX_SCALER_ROTATE_180 >= rotate );

	temp = __g_pRegister->SCCFGREG ;

	temp = ( temp & ~ROT_MASK ) | ( (U32)rotate << ROT_BITPOS );

//	__g_pRegister->SCCFGREG = temp ;
	WriteIODW(&__g_pRegister->SCCFGREG, temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Scaler's Rotate Mode
 *	@return		Value of Rotate Mode
 *	@see		NX_SCALER_SetRotateMode
 */
NX_SCALER_ROTATE	NX_SCALER_GetRotateMode( void )
{
	const U32	ROT_MASK	= ( 0x07 << 24 );
	const U32	ROT_BITPOS	= 24;
	register U32	temp;

	NX_ASSERT( CNULL != __g_pRegister );

	temp = __g_pRegister->SCCFGREG ;

	return(NX_SCALER_ROTATE)( ( temp & ROT_MASK ) >> ROT_BITPOS );
}

//--------------------------------------------------------------------------
// Configuration Function of Image Size and Address
//--------------------------------------------------------------------------
/**
 *	@brief		Set Source Image Address
 *	@param[in]	Addr		Value of Source Image Address
 *	@return		None.
 *	@remarks	Address's Format is 2D Address. so each bit have meaning.\n\n
 *				Addr[27:24] : Segment Address.\n
 *				Addr[23:12] : Base Address Y. Should be arranged in 8-byte. \n
 *				Addr[11:0]	: Base Address X. Should be arranged in 8-byte. \n
 *	@see								NX_SCALER_GetSrcAddr,
 *				NX_SCALER_SetDestAddr,	NX_SCALER_GetDestAddr
 */
void	NX_SCALER_SetSrcAddr( U32 Addr )
{
	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 0 == (0x50007007 & Addr) );
	NX_ASSERT( 0 == (0x50007007 & Addr) );

//	__g_pRegister->SCSRCADDREG	= Addr;
	WriteIODW(&__g_pRegister->SCSRCADDREG, Addr);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Source Image Address
 *	@return		Value of Source Image 2D Address.\n\n
 *				SrcAddr[27:24] : Segment Address.\n
 *				SrcAddr[23:12] : Base Address Y.\n
 *				SrcAddr[11:0]	: Base Address X.\n
 *	@see		NX_SCALER_SetSrcAddr,
 *				NX_SCALER_SetDestAddr,	NX_SCALER_GetDestAddr
 */
U32		NX_SCALER_GetSrcAddr( void )
{
	return __g_pRegister->SCSRCADDREG;
}

//--------------------------------------------------------------------------
/**
 *	@brief		Set Destination Image Address
 *	@param[in]	Addr		Value of Destination Image Address
 *	@return		None.
 *	@remarks	Address's Format is 2D Address. so each bit have meaning.\n\n
 *				Addr[27:24] : Segment Address.\n
 *				Addr[23:12] : Base Address Y. Should be arranged in 8-byte. \n
 *				Addr[11:0]	: Base Address X. Should be arranged in 8-byte. \n
 *	@see		NX_SCALER_SetSrcAddr,	NX_SCALER_GetSrcAddr,
 *										NX_SCALER_GetDestAddr
 */
void	NX_SCALER_SetDestAddr( U32 Addr )
{
	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( 0 == (0x50007007 & Addr) );
	NX_ASSERT( 0 == (0x50007007 & Addr) );

//	__g_pRegister->SCDESTADDREG	= Addr;
	WriteIODW(&__g_pRegister->SCDESTADDREG, Addr);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Destination Image Address
 *	@return		Value of Destination Image 2D Address.\n\n
 *				SrcAddr[27:24]	: Segment Address.\n
 *				SrcAddr[23:12]	: Base Address Y.\n
 *				SrcAddr[11:0]	: Base Address X.\n
 *	@see		NX_SCALER_SetSrcAddr,	NX_SCALER_GetSrcAddr,
 *				NX_SCALER_SetDestAddr
 */
U32		NX_SCALER_GetDestAddr( void )
{
	return __g_pRegister->SCDESTADDREG;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set Source and Destination Image Size
 *	@param[in]	dwSrcWidth		Value of width ( 8 ~ 4096 )
 *	@param[in]	dwSrcHeight		Value of height( 1 ~ 4096 )
 *	@param[in]	dwDestWidth		Value of width ( 8 ~ 4096 )
 *	@param[in]	dwDestHeight	Value of height( 1 ~ 4096 )
 *	@return		None.
 *	@remarks	Width must align to 8.
 *	@see		NX_SCALER_GetSrcImageSize
 */
void	NX_SCALER_SetImageSize
(
	U32	dwSrcWidth, U32 dwSrcHeight, U32 dwDestWidth, U32 dwDestHeight
)
{
	const U32	WIDTH_BITPOS	= 0 ;
	const U32	HEIGHT_BITPOS	= 16 ;
	register U32 temp;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( ( 0x1000 > dwSrcWidth ) && ( 0 == ( dwSrcWidth & 0x07 )) );
	NX_ASSERT( ( 0x1000 > dwSrcHeight) && ( 0 < dwSrcHeight ) );
	NX_ASSERT( ( 0x1000 > dwDestWidth ) && ( 0 == ( dwDestWidth & 0x07 )) );
	NX_ASSERT( ( 0x1000 > dwDestHeight) && ( 0 < dwDestHeight ) );

	temp = ( ( dwSrcHeight - 1 ) << HEIGHT_BITPOS ) | ( ( dwSrcWidth - 1 ) << WIDTH_BITPOS );
//	__g_pRegister->SCSRCSIZEREG = temp;
	WriteIODW(&__g_pRegister->SCSRCSIZEREG, temp);

	temp = ( ( dwDestHeight - 1 ) << HEIGHT_BITPOS ) | ( ( dwDestWidth - 1 ) << WIDTH_BITPOS );
//	__g_pRegister->SCDESTSIZEREG = temp;
	WriteIODW(&__g_pRegister->SCDESTSIZEREG, temp);

	// Setting DeltaX, DeltaY
    //__g_pRegister->DELTAXREG = ( dwSrcWidth * 0x10000 ) / ( dwDestWidth-1 );
    //__g_pRegister->DELTAYREG = ( dwSrcHeight * 0x10000 ) / ( dwDestHeight-1 );
    // 2009.11.10, Goofy : Fix to appear abnormal gray vertical pattern 
    //                     in the region is filled with a single gray color.
//	__g_pRegister->DELTAXREG = ( dwSrcWidth  * 0x10000 ) / ( dwDestWidth  );
//	__g_pRegister->DELTAYREG = ( dwSrcHeight * 0x10000 ) / ( dwDestHeight );
	WriteIODW(&__g_pRegister->DELTAXREG, ( dwSrcWidth  * 0x10000 ) / ( dwDestWidth  ));
	WriteIODW(&__g_pRegister->DELTAYREG, ( dwSrcHeight * 0x10000 ) / ( dwDestHeight ));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Source Image's width and height
 *	@param[out]	pdwSrcWidth		Value of width
 *	@param[out]	pdwSrcHeight	Value of height
 *	@param[out]	pdwDestWidth	Value of width
 *	@param[out]	pdwDestHeight	Value of height
 *	@return		None.
 *	@remarks	Parameter can set to CNULL, when the information is not need.
 *	@see		NX_SCALER_SetImageSize
 */
void		NX_SCALER_GetImageSize
(
	U32	*pdwSrcWidth, U32	*pdwSrcHeight, U32	*pdwDestWidth, U32	*pdwDestHeight
)
{
	const U32	WIDTH_BITPOS	= 0 ;
	const U32	WIDTH_MASK		= ( 0x0FFF << 0 );
	const U32	HEIGHT_BITPOS	= 16 ;
	const U32	HEIGHT_MASK		= ( 0x0FFF <<16 );
	register U32 temp;

	NX_ASSERT( CNULL != __g_pRegister );

	temp = __g_pRegister->SCSRCSIZEREG;

	if( CNULL != pdwSrcWidth ) { *pdwSrcWidth	= ( ( temp & WIDTH_MASK  ) >> WIDTH_BITPOS  ) + 1;	}
	if( CNULL != pdwSrcHeight) { *pdwSrcHeight	= ( ( temp & HEIGHT_MASK ) >> HEIGHT_BITPOS ) + 1;	}

	temp = __g_pRegister->SCDESTSIZEREG;

	if( CNULL != pdwDestWidth ) { *pdwDestWidth		= ( ( temp & WIDTH_MASK  ) >> WIDTH_BITPOS  ) + 1;	}
	if( CNULL != pdwDestHeight) { *pdwDestHeight	= ( ( temp & HEIGHT_MASK ) >> HEIGHT_BITPOS ) + 1;	}
}
