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
//	Module		: USB Host Controller ( EHCI )
//	File		: nx_ehci.c
//	Description:
//	Author		: Firmware Team
//	History	:
//------------------------------------------------------------------------------

#include <plat/nx_ehci.h>

static	struct
{
	struct NX_EHCI_RegisterSet *pRegister;

} __g_ModuleVariables[NUMBER_OF_EHCI_MODULE] = {{ CNULL,}, };

static	U32 __g_CurModuleIndex = 0;
static	struct NX_EHCI_RegisterSet *__g_pRegister = CNULL;

//------------------------------------------------------------------------------
//	Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b CTRUE	indicate that Initialize is successed.\n
 *			\b CFALSE	indicate that Initialize is failed.\n
 *	@see	NX_EHCI_GetNumberOfModule
 */
CBOOL	NX_EHCI_Initialize( void )
{
	static CBOOL bInit = CFALSE;
	U32 i;

	if( CFALSE == bInit )
	{
		__g_CurModuleIndex = 0;

		for( i=0; i < NUMBER_OF_EHCI_MODULE; i++ )
		{
			__g_ModuleVariables[i].pRegister = CNULL;
		}

		bInit = CTRUE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get number of modules in the chip.
 *	@return		Module's number.
 *	@see		NX_EHCI_Initialize
 */
U32		NX_EHCI_GetNumberOfModule( void )
{
	return NUMBER_OF_EHCI_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@return		Module's physical address
 *	@see		NX_EHCI_GetSizeOfRegisterSet,
 *				NX_EHCI_SetBaseAddress,		NX_EHCI_GetBaseAddress,
 *				NX_EHCI_OpenModule,			NX_EHCI_CloseModule,
 *				NX_EHCI_CheckBusy,			NX_EHCI_CanPowerDown
 */
U32		NX_EHCI_GetPhysicalAddress( void )
{
	NX_ASSERT( NUMBER_OF_EHCI_MODULE > __g_CurModuleIndex );

	return	(U32)( PHY_BASEADDR_EHCI_MODULE + (OFFSET_OF_EHCI_MODULE * __g_CurModuleIndex) );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_EHCI_GetPhysicalAddress,
 *				NX_EHCI_SetBaseAddress,		NX_EHCI_GetBaseAddress,
 *				NX_EHCI_OpenModule,			NX_EHCI_CloseModule,
 *				NX_EHCI_CheckBusy,			NX_EHCI_CanPowerDown
 */
U32		NX_EHCI_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_EHCI_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_EHCI_GetPhysicalAddress,	NX_EHCI_GetSizeOfRegisterSet,
 *				NX_EHCI_GetBaseAddress,
 *				NX_EHCI_OpenModule,			NX_EHCI_CloseModule,
 *				NX_EHCI_CheckBusy,			NX_EHCI_CanPowerDown
 */
void	NX_EHCI_SetBaseAddress( U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );
	NX_ASSERT( NUMBER_OF_EHCI_MODULE > __g_CurModuleIndex );

	__g_ModuleVariables[__g_CurModuleIndex].pRegister = (struct NX_EHCI_RegisterSet *)BaseAddress;
	__g_pRegister = (struct NX_EHCI_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@return		Module's base address.
 *	@see		NX_EHCI_GetPhysicalAddress,	NX_EHCI_GetSizeOfRegisterSet,
 *				NX_EHCI_SetBaseAddress,
 *				NX_EHCI_OpenModule,			NX_EHCI_CloseModule,
 *				NX_EHCI_CheckBusy,			NX_EHCI_CanPowerDown
 */
U32		NX_EHCI_GetBaseAddress( void )
{
	NX_ASSERT( NUMBER_OF_EHCI_MODULE > __g_CurModuleIndex );

	return (U32)__g_ModuleVariables[__g_CurModuleIndex].pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@return		\b CTRUE	indicate that Initialize is successed. \n
 *				\b CFALSE	indicate that Initialize is failed.
 *	@see		NX_EHCI_GetPhysicalAddress,	NX_EHCI_GetSizeOfRegisterSet,
 *				NX_EHCI_SetBaseAddress,		NX_EHCI_GetBaseAddress,
 *				NX_EHCI_CloseModule,
 *				NX_EHCI_CheckBusy,			NX_EHCI_CanPowerDown
 */
CBOOL	NX_EHCI_OpenModule( void )
{
		return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@return		\b CTRUE	indicate that Deinitialize is successed. \n
 *				\b CFALSE	indicate that Deinitialize is failed.
 *	@see		NX_EHCI_GetPhysicalAddress,	NX_EHCI_GetSizeOfRegisterSet,
 *				NX_EHCI_SetBaseAddress,		NX_EHCI_GetBaseAddress,
 *				NX_EHCI_OpenModule,
 *				NX_EHCI_CheckBusy,			NX_EHCI_CanPowerDown
 */
CBOOL	NX_EHCI_CloseModule( void )
{
		return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@return		\b CTRUE	indicate that Module is Busy. \n
 *				\b CFALSE	indicate that Module is NOT Busy.
 *	@see		NX_EHCI_GetPhysicalAddress,	NX_EHCI_GetSizeOfRegisterSet,
 *				NX_EHCI_SetBaseAddress,		NX_EHCI_GetBaseAddress,
 *				NX_EHCI_OpenModule,			NX_EHCI_CloseModule,
 *				NX_EHCI_CanPowerDown
 */
CBOOL	NX_EHCI_CheckBusy( void )
{
		return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@return		\b CTRUE	indicate that Ready to enter power-down stage. \n
 *				\b CFALSE	indicate that This module can't enter to power-down stage.
 *	@see		NX_EHCI_GetPhysicalAddress,	NX_EHCI_GetSizeOfRegisterSet,
 *				NX_EHCI_SetBaseAddress,		NX_EHCI_GetBaseAddress,
 *				NX_EHCI_OpenModule,			NX_EHCI_CloseModule,
 *				NX_EHCI_CheckBusy
 */
CBOOL	NX_EHCI_CanPowerDown( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number for interrupt controller.
 *	@return		Interrupt number
 *	@see											NX_EHCI_SetInterruptEnable,
 *				NX_EHCI_GetInterruptEnable,			NX_EHCI_SetInterruptEnable32,
 *				NX_EHCI_GetInterruptEnable32,		NX_EHCI_GetInterruptPending,
 *				NX_EHCI_GetInterruptPending32,		NX_EHCI_ClearInterruptPending,
 *				NX_EHCI_ClearInterruptPending32,	NX_EHCI_SetInterruptEnableAll,
 *				NX_EHCI_GetInterruptEnableAll,		NX_EHCI_GetInterruptPendingAll,
 *				NX_EHCI_ClearInterruptPendingAll,	NX_EHCI_GetInterruptPendingNumber
 */
S32		NX_EHCI_GetInterruptNumber( void )
{
	return	INTNUM_OF_EHCI_MODULE;
}


//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	IntNum	Interrupt Number ( 0 ~ 5 ).
 *	@param[in]	Enable	\b CTRUE	indicate that	Interrupt Enable. \n
 *						\b CFALSE	indicate that	Interrupt Disable.
 *	@return		None.
 *	@see		NX_EHCI_GetInterruptNumber,		
 *				NX_EHCI_GetInterruptEnable,			NX_EHCI_SetInterruptEnable32,
 *				NX_EHCI_GetInterruptEnable32,		NX_EHCI_GetInterruptPending,
 *				NX_EHCI_GetInterruptPending32,		NX_EHCI_ClearInterruptPending,
 *				NX_EHCI_ClearInterruptPending32,	NX_EHCI_SetInterruptEnableAll,
 *				NX_EHCI_GetInterruptEnableAll,		NX_EHCI_GetInterruptPendingAll,
 *				NX_EHCI_ClearInterruptPendingAll,	NX_EHCI_GetInterruptPendingNumber
 */
void	NX_EHCI_SetInterruptEnable( S32 IntNum, CBOOL Enable )
{
	register struct NX_EHCI_RegisterSet*	pRegister;
	register U16 regvalue;

	NX_ASSERT( NX_EHCI_INT_IAAE >= IntNum );
	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	regvalue = pRegister->USBINTR;

	regvalue &= ~( 1 << IntNum );
	regvalue |= ((U16)Enable << IntNum );

	pRegister->USBINTR	= regvalue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	IntNum	Interrupt Number ( 0 ~ 5 ).
 *	@return		\b CTRUE	indicate that	Interrupt is enabled. \n
 *				\b CFALSE	indicate that	Interrupt is disabled.
 *	@see		NX_EHCI_GetInterruptNumber,			NX_EHCI_SetInterruptEnable,
 *													NX_EHCI_SetInterruptEnable32,
 *				NX_EHCI_GetInterruptEnable32,		NX_EHCI_GetInterruptPending,
 *				NX_EHCI_GetInterruptPending32,		NX_EHCI_ClearInterruptPending,
 *				NX_EHCI_ClearInterruptPending32,	NX_EHCI_SetInterruptEnableAll,
 *				NX_EHCI_GetInterruptEnableAll,		NX_EHCI_GetInterruptPendingAll,
 *				NX_EHCI_ClearInterruptPendingAll,	NX_EHCI_GetInterruptPendingNumber
 */
CBOOL	NX_EHCI_GetInterruptEnable( S32 IntNum )
{
	NX_ASSERT( NX_EHCI_INT_IAAE >= IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( __g_pRegister->USBINTR & (1<<IntNum));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n
 *							- EnableFlag[0] : Set USB					interrupt enable or disable. \n
 *							- EnableFlag[1] : Set Error					interrupt enable or disable. \n
 *							- EnableFlag[2] : Set Port Change			interrupt enable or disable. \n
 *							- EnableFlag[3] : Set Frame List Rollover	interrupt enable or disable. \n
 *							- EnableFlag[4] : Set Host System Error		interrupt enable or disable. \n
 *							- EnableFlag[5] : Set Async Advance			interrupt enable or disable. \n
 *	@return		None.
 *	@see		NX_EHCI_GetInterruptNumber,			NX_EHCI_SetInterruptEnable,
 *				NX_EHCI_GetInterruptEnable,
 *				NX_EHCI_GetInterruptEnable32,		NX_EHCI_GetInterruptPending,
 *				NX_EHCI_GetInterruptPending32,		NX_EHCI_ClearInterruptPending,
 *				NX_EHCI_ClearInterruptPending32,	NX_EHCI_SetInterruptEnableAll,
 *				NX_EHCI_GetInterruptEnableAll,		NX_EHCI_GetInterruptPendingAll,
 *				NX_EHCI_ClearInterruptPendingAll,	NX_EHCI_GetInterruptPendingNumber
 */
void	NX_EHCI_SetInterruptEnable32( U32 EnableFlag )
{
	const U32	ENB_MASK = (1<<6)-1;

	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->USBINTR = EnableFlag & ENB_MASK;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt enable bit.
 *	@return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0] : USB					interrupt's setting value. \n
 *				- Return Value[1] : Error				interrupt's setting value. \n
 *				- Return Value[2] : Port Change			interrupt's setting value. \n
 *				- Return Value[3] : Frame List Rollover interrupt's setting value. \n
 *				- Return Value[4] : Host System Error	interrupt's setting value. \n
 *				- Return Value[5] : Async Advance		interrupt's setting value. \n
 *	@see		NX_EHCI_GetInterruptNumber,			NX_EHCI_SetInterruptEnable,
 *				NX_EHCI_GetInterruptEnable,			NX_EHCI_SetInterruptEnable32,
 *													NX_EHCI_GetInterruptPending,
 *				NX_EHCI_GetInterruptPending32,		NX_EHCI_ClearInterruptPending,
 *				NX_EHCI_ClearInterruptPending32,	NX_EHCI_SetInterruptEnableAll,
 *				NX_EHCI_GetInterruptEnableAll,		NX_EHCI_GetInterruptPendingAll,
 *				NX_EHCI_ClearInterruptPendingAll,	NX_EHCI_GetInterruptPendingNumber
 */
U32		NX_EHCI_GetInterruptEnable32( void )
{
	const U32	ENB_MASK = (1<<6)-1;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(__g_pRegister->USBINTR & ENB_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *	@param[in]	IntNum	Interrupt Number ( 0 ~ 5 ).
 *	@return		\b CTRUE	indicate that Pending is seted. \n
 *				\b CFALSE	indicate that Pending is Not Seted.
 *	@see		NX_EHCI_GetInterruptNumber,			NX_EHCI_SetInterruptEnable,
 *				NX_EHCI_GetInterruptEnable,			NX_EHCI_SetInterruptEnable32,
 *				NX_EHCI_GetInterruptEnable32,		
 *				NX_EHCI_GetInterruptPending32,		NX_EHCI_ClearInterruptPending,
 *				NX_EHCI_ClearInterruptPending32,	NX_EHCI_SetInterruptEnableAll,
 *				NX_EHCI_GetInterruptEnableAll,		NX_EHCI_GetInterruptPendingAll,
 *				NX_EHCI_ClearInterruptPendingAll,	NX_EHCI_GetInterruptPendingNumber
 */
CBOOL	NX_EHCI_GetInterruptPending( S32 IntNum )
{
	NX_ASSERT( NX_EHCI_INT_IAAE >= IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( __g_pRegister->USBSTS & (1<<IntNum));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt pending bit.
 *	@return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bit is NOT occured. \n
 *				- Return Value[0] : USB					pending state. \n
 *				- Return Value[1] : Error				pending state. \n
 *				- Return Value[2] : Port Change			pending state. \n
 *				- Return Value[3] : Frame List Rollover pending state. \n
 *				- Return Value[4] : Host System Error	pending state. \n
 *				- Return Value[5] : Async Advance		pending state. \n
 *	@see		NX_EHCI_GetInterruptNumber,			NX_EHCI_SetInterruptEnable,
 *				NX_EHCI_GetInterruptEnable,			NX_EHCI_SetInterruptEnable32,
 *				NX_EHCI_GetInterruptEnable32,		NX_EHCI_GetInterruptPending,
 *													NX_EHCI_ClearInterruptPending,
 *				NX_EHCI_ClearInterruptPending32,	NX_EHCI_SetInterruptEnableAll,
 *				NX_EHCI_GetInterruptEnableAll,		NX_EHCI_GetInterruptPendingAll,
 *				NX_EHCI_ClearInterruptPendingAll,	NX_EHCI_GetInterruptPendingNumber
 */
U32		NX_EHCI_GetInterruptPending32( void )
{
	const U32	PEND_MASK	= (1<<6)-1;

	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(__g_pRegister->USBSTS & PEND_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	IntNum	Interrupt number ( 0 ~ 5 ).
 *	@return		None.
 *	@see		NX_EHCI_GetInterruptNumber,			NX_EHCI_SetInterruptEnable,
 *				NX_EHCI_GetInterruptEnable,			NX_EHCI_SetInterruptEnable32,
 *				NX_EHCI_GetInterruptEnable32,		NX_EHCI_GetInterruptPending,
 *				NX_EHCI_GetInterruptPending32,		
 *				NX_EHCI_ClearInterruptPending32,	NX_EHCI_SetInterruptEnableAll,
 *				NX_EHCI_GetInterruptEnableAll,		NX_EHCI_GetInterruptPendingAll,
 *				NX_EHCI_ClearInterruptPendingAll,	NX_EHCI_GetInterruptPendingNumber
 */
void	NX_EHCI_ClearInterruptPending( S32 IntNum )
{
	NX_ASSERT( NX_EHCI_INT_IAAE >= IntNum );
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->USBSTS = (1<<IntNum);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below	\n \n
 *								- PendingFlag[0] : USB					pending bit. \n
 *								- PendingFlag[1] : Error				pending bit. \n
 *								- PendingFlag[2] : Port Change			pending bit. \n
 *								- PendingFlag[3] : Frame List Rollover	pending bit. \n
 *								- PendingFlag[4] : Host System Error	pending bit. \n
 *								- PendingFlag[5] : Async Advance		pending bit. \n
 *	@return		None.
 *	@see		NX_EHCI_GetInterruptNumber,			NX_EHCI_SetInterruptEnable,
 *				NX_EHCI_GetInterruptEnable,			NX_EHCI_SetInterruptEnable32,
 *				NX_EHCI_GetInterruptEnable32,		NX_EHCI_GetInterruptPending,
 *				NX_EHCI_GetInterruptPending32,		NX_EHCI_ClearInterruptPending,
 *													NX_EHCI_SetInterruptEnableAll,
 *				NX_EHCI_GetInterruptEnableAll,		NX_EHCI_GetInterruptPendingAll,
 *				NX_EHCI_ClearInterruptPendingAll,	NX_EHCI_GetInterruptPendingNumber
 */
void	NX_EHCI_ClearInterruptPending32( U32 PendingFlag )
{
	const U32	PEND_MASK	= (1<<6)-1;

	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->USBSTS = (PendingFlag & PEND_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enables or disables.
 *	@param[in]	Enable	\b CTRUE	indicate that	Set to all interrupt enable. \n
 *						\b CFALSE	indicate that	Set to all interrupt disable.
 *	@return		None.
 *	@see		NX_EHCI_GetInterruptNumber,			NX_EHCI_SetInterruptEnable,
 *				NX_EHCI_GetInterruptEnable,			NX_EHCI_SetInterruptEnable32,
 *				NX_EHCI_GetInterruptEnable32,		NX_EHCI_GetInterruptPending,
 *				NX_EHCI_GetInterruptPending32,		NX_EHCI_ClearInterruptPending,
 *				NX_EHCI_ClearInterruptPending32,	
 *				NX_EHCI_GetInterruptEnableAll,		NX_EHCI_GetInterruptPendingAll,
 *				NX_EHCI_ClearInterruptPendingAll,	NX_EHCI_GetInterruptPendingNumber
 */
void	NX_EHCI_SetInterruptEnableAll( CBOOL Enable )
{
	const U32 INTENBMASK = 0x0000003F;

	NX_ASSERT( (0==Enable) || (1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	if( Enable )
	{
		__g_pRegister->USBINTR = INTENBMASK;
	}
	else
	{
		__g_pRegister->USBINTR = ~INTENBMASK;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enable or not.
 *	@return		\b CTRUE	indicate that	At least one( or more ) interrupt is enabled. \n
 *				\b CFALSE	indicate that	All interrupt is disabled.
 *	@see		NX_EHCI_GetInterruptNumber,			NX_EHCI_SetInterruptEnable,
 *				NX_EHCI_GetInterruptEnable,			NX_EHCI_SetInterruptEnable32,
 *				NX_EHCI_GetInterruptEnable32,		NX_EHCI_GetInterruptPending,
 *				NX_EHCI_GetInterruptPending32,		NX_EHCI_ClearInterruptPending,
 *				NX_EHCI_ClearInterruptPending32,	NX_EHCI_SetInterruptEnableAll,
 *													NX_EHCI_GetInterruptPendingAll,
 *				NX_EHCI_ClearInterruptPendingAll,	NX_EHCI_GetInterruptPendingNumber
 */
CBOOL	NX_EHCI_GetInterruptEnableAll( void )
{
	const U32 INTENBMASK = 0x0000003F;
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( __g_pRegister->USBINTR & INTENBMASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are pended or not.
 *	@return		\b CTRUE	indicate that	At least one( or more ) pending is seted. \n
 *				\b CFALSE	indicate that	All pending is NOT seted.
 *	@see		NX_EHCI_GetInterruptNumber,			NX_EHCI_SetInterruptEnable,
 *				NX_EHCI_GetInterruptEnable,			NX_EHCI_SetInterruptEnable32,
 *				NX_EHCI_GetInterruptEnable32,		NX_EHCI_GetInterruptPending,
 *				NX_EHCI_GetInterruptPending32,		NX_EHCI_ClearInterruptPending,
 *				NX_EHCI_ClearInterruptPending32,	NX_EHCI_SetInterruptEnableAll,
 *				NX_EHCI_GetInterruptEnableAll,		
 *				NX_EHCI_ClearInterruptPendingAll,	NX_EHCI_GetInterruptPendingNumber
 */
CBOOL	NX_EHCI_GetInterruptPendingAll( void )
{
	const U32 INTPENDMASK = 0x0000003F;
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( __g_pRegister->USBSTS & INTPENDMASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@return		None.
 *	@see		NX_EHCI_GetInterruptNumber,			NX_EHCI_SetInterruptEnable,
 *				NX_EHCI_GetInterruptEnable,			NX_EHCI_SetInterruptEnable32,
 *				NX_EHCI_GetInterruptEnable32,		NX_EHCI_GetInterruptPending,
 *				NX_EHCI_GetInterruptPending32,		NX_EHCI_ClearInterruptPending,
 *				NX_EHCI_ClearInterruptPending32,	NX_EHCI_SetInterruptEnableAll,
 *				NX_EHCI_GetInterruptEnableAll,		NX_EHCI_GetInterruptPendingAll,
 *													NX_EHCI_GetInterruptPendingNumber
 */
void	NX_EHCI_ClearInterruptPendingAll( void )
{
	const U32 INTPENDMASK = 0x0000003F;
	NX_ASSERT( CNULL != __g_pRegister );

	__g_pRegister->USBSTS = INTPENDMASK;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number which has the most prority of pended interrupts
 *	@return		Pending Number( If all pending is not set then return -1 ).
 *	@see		NX_EHCI_GetInterruptNumber,			NX_EHCI_SetInterruptEnable,
 *				NX_EHCI_GetInterruptEnable,			NX_EHCI_SetInterruptEnable32,
 *				NX_EHCI_GetInterruptEnable32,		NX_EHCI_GetInterruptPending,
 *				NX_EHCI_GetInterruptPending32,		NX_EHCI_ClearInterruptPending,
 *				NX_EHCI_ClearInterruptPending32,	NX_EHCI_SetInterruptEnableAll,
 *				NX_EHCI_GetInterruptEnableAll,		NX_EHCI_GetInterruptPendingAll,
 *				NX_EHCI_ClearInterruptPendingAll	
 */
S32		NX_EHCI_GetInterruptPendingNumber( void )	// -1 if None
{
	U32 index;
	U32 IntPend;
	U32 IntEnb;
	register struct NX_EHCI_RegisterSet	*pRegister;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	IntEnb	= pRegister->USBINTR;
	IntPend = pRegister->USBSTS;

	IntPend &= IntEnb;

	for( index=0; index <= NX_EHCI_INT_IAAE; index++ )
	{
		if( IntPend & (1<<index) )
			return	index;
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
 *	@see										NX_EHCI_GetClockPClkMode,
 *				NX_EHCI_SetClockBClkMode,		NX_EHCI_GetClockBClkMode,
 *				NX_EHCI_SetClockSource,			NX_EHCI_GetClockSource,
 *				NX_EHCI_SetClockDivisor,		NX_EHCI_GetClockDivisor,
 *				NX_EHCI_SetClockDivisorEnable,	NX_EHCI_GetClockDivisorEnable
 */
void	NX_EHCI_SetClockPClkMode( NX_PCLKMODE mode )
{
	const U32 PCLKMODE_POS	=	3;

	register U32 regvalue;
	register struct NX_EHCI_RegisterSet* pRegister;

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
 *	@see		NX_EHCI_SetClockPClkMode,
 *				NX_EHCI_SetClockBClkMode,		NX_EHCI_GetClockBClkMode,
 *				NX_EHCI_SetClockSource,			NX_EHCI_GetClockSource,
 *				NX_EHCI_SetClockDivisor,		NX_EHCI_GetClockDivisor,
 *				NX_EHCI_SetClockDivisorEnable,	NX_EHCI_GetClockDivisorEnable
 */
NX_PCLKMODE	NX_EHCI_GetClockPClkMode( void )
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
 *	@see		NX_EHCI_SetClockPClkMode,		NX_EHCI_GetClockPClkMode,
 *												NX_EHCI_GetClockBClkMode,
 *				NX_EHCI_SetClockSource,			NX_EHCI_GetClockSource,
 *				NX_EHCI_SetClockDivisor,		NX_EHCI_GetClockDivisor,
 *				NX_EHCI_SetClockDivisorEnable,	NX_EHCI_GetClockDivisorEnable
 */
void			NX_EHCI_SetClockBClkMode( NX_BCLKMODE mode )
{
	register U32 regvalue;
	register struct NX_EHCI_RegisterSet* pRegister;
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
 *	@see		NX_EHCI_SetClockPClkMode,		NX_EHCI_GetClockPClkMode,
 *				NX_EHCI_SetClockBClkMode,
 *				NX_EHCI_SetClockSource,			NX_EHCI_GetClockSource,
 *				NX_EHCI_SetClockDivisor,		NX_EHCI_GetClockDivisor,
 *				NX_EHCI_SetClockDivisorEnable,	NX_EHCI_GetClockDivisorEnable
 */
NX_BCLKMODE	NX_EHCI_GetClockBClkMode( void )
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
 *	@param[in]	Index	Select clock generator( 0 : clock generator 0 ).
 *	@param[in]	ClkSrc	Select clock source of clock generator( 0: PLL0, 1: PLL1 ).
 *	@remarks	USB EHCI controller have one clock generator. so \e Index must set to 0.
 *	@return		None.
 *	@see		NX_EHCI_SetClockPClkMode,		NX_EHCI_GetClockPClkMode,
 *				NX_EHCI_SetClockBClkMode,		NX_EHCI_GetClockBClkMode,
 *												NX_EHCI_GetClockSource,
 *				NX_EHCI_SetClockDivisor,		NX_EHCI_GetClockDivisor,
 *				NX_EHCI_SetClockDivisorEnable,	NX_EHCI_GetClockDivisorEnable
 */
void	NX_EHCI_SetClockSource( U32 Index, U32 ClkSrc )
{
	const U32 CLKSRCSEL_POS		= 2;
	const U32 CLKSRCSEL_MASK	= 0x03 << CLKSRCSEL_POS;

	register U32 ReadValue;

	NX_ASSERT( 1 > Index );
	NX_ASSERT( 2 > ClkSrc );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->CLKGEN;

	ReadValue &= ~CLKSRCSEL_MASK;
	ReadValue |= ClkSrc << CLKSRCSEL_POS;

	__g_pRegister->CLKGEN = ReadValue;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get clock source of specified clock generator.
 *	@param[in]	Index	Select clock generator( 0 : clock generator 0 )
 *	@return		Clock source of clock generator( 0: PLL0, 1: PLL1 ).
 *	@remarks	USB EHCI controller have one clock generator. so \e Index must set to 0.\n
 *	@see		NX_EHCI_SetClockPClkMode,		NX_EHCI_GetClockPClkMode,
 *				NX_EHCI_SetClockBClkMode,		NX_EHCI_GetClockBClkMode,
 *				NX_EHCI_SetClockSource,
 *				NX_EHCI_SetClockDivisor,		NX_EHCI_GetClockDivisor,
 *				NX_EHCI_SetClockDivisorEnable,	NX_EHCI_GetClockDivisorEnable
 */
U32		NX_EHCI_GetClockSource( U32 Index )
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
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0 ).
 *	@param[in]	Divisor		Clock divisor ( 1 ~ 64 ).
 *	@return		None.
 *	@remarks	USB EHCI controller have one clock generator. so \e Index must set to 0.
 *	@see		NX_EHCI_SetClockPClkMode,		NX_EHCI_GetClockPClkMode,
 *				NX_EHCI_SetClockBClkMode,		NX_EHCI_GetClockBClkMode,
 *				NX_EHCI_SetClockSource,			NX_EHCI_GetClockSource,
 *												NX_EHCI_GetClockDivisor,
 *				NX_EHCI_SetClockDivisorEnable,	NX_EHCI_GetClockDivisorEnable
 */
void	NX_EHCI_SetClockDivisor( U32 Index, U32 Divisor )
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
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0 ).
 *	@return		Clock divisor ( 1 ~ 64 ).
 *	@remarks	USB EHCI controller have one clock generator. so \e Index must set to 0.
 *	@see		NX_EHCI_SetClockPClkMode,		NX_EHCI_GetClockPClkMode,
 *				NX_EHCI_SetClockBClkMode,		NX_EHCI_GetClockBClkMode,
 *				NX_EHCI_SetClockSource,			NX_EHCI_GetClockSource,
 *				NX_EHCI_SetClockDivisor,
 *				NX_EHCI_SetClockDivisorEnable,	NX_EHCI_GetClockDivisorEnable
 */
U32		NX_EHCI_GetClockDivisor( U32 Index )
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
 *	@param[in]	Enable	\b CTRUE	indicate that Enable of clock generator. \n
 *						\b CFALSE	indicate that Disable of clock generator.
 *	@return		None.
 *	@see		NX_EHCI_SetClockPClkMode,		NX_EHCI_GetClockPClkMode,
 *				NX_EHCI_SetClockBClkMode,		NX_EHCI_GetClockBClkMode,
 *				NX_EHCI_SetClockSource,			NX_EHCI_GetClockSource,
 *				NX_EHCI_SetClockDivisor,		NX_EHCI_GetClockDivisor,
 *												NX_EHCI_GetClockDivisorEnable
 */
void	NX_EHCI_SetClockDivisorEnable( CBOOL Enable )
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
 *	@return		\b CTRUE	indicate that Clock generator is enabled.\n
 *				\b CFALSE	indicate that Clock generator is disabled.
 *	@see		NX_EHCI_SetClockPClkMode,		NX_EHCI_GetClockPClkMode,
 *				NX_EHCI_SetClockBClkMode,		NX_EHCI_GetClockBClkMode,
 *				NX_EHCI_SetClockSource,			NX_EHCI_GetClockSource,
 *				NX_EHCI_SetClockDivisor,		NX_EHCI_GetClockDivisor,
 *				NX_EHCI_SetClockDivisorEnable
 */
CBOOL	NX_EHCI_GetClockDivisorEnable( void )
{
	const U32	CLKGENENB_POS	=	2;
	const U32	CLKGENENB_MASK	=	1UL << CLKGENENB_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return	(CBOOL)( (__g_pRegister->CLKENB & CLKGENENB_MASK) >> CLKGENENB_POS );
}

