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
//	File		: nx_ppm.c
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------

#include <plat/nx_ppm.h>

static	struct	NX_PPM_RegisterSet *__g_pRegister = CNULL;

//------------------------------------------------------------------------------
// Module Interface
//------------------------------------------------------------------------------
/**
 *	@brief	Initialize of prototype enviroment & local variables.
 *	@return \b CTRUE	indicate that Initialize is successed.\n
 *			\b CFALSE	indicate that Initialize is failed.\n
 *	@see	NX_PPM_GetNumberOfModule
 */
CBOOL	NX_PPM_Initialize( void )
{
	static CBOOL bInit = CFALSE;

	if( CFALSE == bInit )
	{
		__g_pRegister= CNULL;

		bInit = CTRUE;
	}

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get number of modules in the chip.
 *	@return		Module's number.
 *	@see		NX_PPM_Initialize
 */
U32		NX_PPM_GetNumberOfModule( void )
{
	return NUMBER_OF_PPM_MODULE;
}

//------------------------------------------------------------------------------
// Basic Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get module's physical address.
 *	@return		Module's physical address
 *	@see		NX_PPM_GetSizeOfRegisterSet,
 *				NX_PPM_SetBaseAddress,		NX_PPM_GetBaseAddress,
 *				NX_PPM_OpenModule,			NX_PPM_CloseModule,
 *				NX_PPM_CheckBusy,			NX_PPM_CanPowerDown
 */
U32		NX_PPM_GetPhysicalAddress( void )
{
	return	(U32)(PHY_BASEADDR_PPM_MODULE);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a size, in byte, of register set.
 *	@return		Size of module's register set.
 *	@see		NX_PPM_GetPhysicalAddress,
 *				NX_PPM_SetBaseAddress,		NX_PPM_GetBaseAddress,
 *				NX_PPM_OpenModule,			NX_PPM_CloseModule,
 *				NX_PPM_CheckBusy,			NX_PPM_CanPowerDown
 */
U32		NX_PPM_GetSizeOfRegisterSet( void )
{
	return sizeof( struct NX_PPM_RegisterSet );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a base address of register set.
 *	@param[in]	BaseAddress Module's base address
 *	@return		None.
 *	@see		NX_PPM_GetPhysicalAddress,	NX_PPM_GetSizeOfRegisterSet,
 *				NX_PPM_GetBaseAddress,
 *				NX_PPM_OpenModule,			NX_PPM_CloseModule,
 *				NX_PPM_CheckBusy,			NX_PPM_CanPowerDown
 */
void	NX_PPM_SetBaseAddress( U32 BaseAddress )
{
	NX_ASSERT( CNULL != BaseAddress );

	__g_pRegister = (struct NX_PPM_RegisterSet *)BaseAddress;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a base address of register set
 *	@return		Module's base address.
 *	@see		NX_PPM_GetPhysicalAddress,	NX_PPM_GetSizeOfRegisterSet,
 *				NX_PPM_SetBaseAddress,
 *				NX_PPM_OpenModule,			NX_PPM_CloseModule,
 *				NX_PPM_CheckBusy,			NX_PPM_CanPowerDown
 */
U32		NX_PPM_GetBaseAddress( void )
{
	return (U32)__g_pRegister;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Initialize selected modules with default value.
 *	@return		\b CTRUE	indicate that Initialize is successed. \n
 *				\b CFALSE	indicate that Initialize is failed.
 *	@see		NX_PPM_GetPhysicalAddress,	NX_PPM_GetSizeOfRegisterSet,
 *				NX_PPM_SetBaseAddress,		NX_PPM_GetBaseAddress,
 *				NX_PPM_CloseModule,
 *				NX_PPM_CheckBusy,			NX_PPM_CanPowerDown
 */
CBOOL	NX_PPM_OpenModule( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->PPM_CTRL = 0x0000;
//	__g_pRegister->PPM_STAT = 0x0007;
	WriteIOW(&__g_pRegister->PPM_CTRL, 0x0000);
	WriteIOW(&__g_pRegister->PPM_STAT, 0x0007);

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Deinitialize selected module to the proper stage.
 *	@return		\b CTRUE	indicate that Deinitialize is successed. \n
 *				\b CFALSE	indicate that Deinitialize is failed.
 *	@see		NX_PPM_GetPhysicalAddress,	NX_PPM_GetSizeOfRegisterSet,
 *				NX_PPM_SetBaseAddress,		NX_PPM_GetBaseAddress,
 *				NX_PPM_OpenModule,
 *				NX_PPM_CheckBusy,			NX_PPM_CanPowerDown
 */
CBOOL	NX_PPM_CloseModule( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->PPM_CTRL = 0x0000;
//	__g_pRegister->PPM_STAT = 0x0007;
	WriteIOW(&__g_pRegister->PPM_CTRL, 0x0000);
	WriteIOW(&__g_pRegister->PPM_STAT, 0x0007);

	return CTRUE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether the selected modules is busy or not.
 *	@return		\b CTRUE	indicate that Module is Busy. \n
 *				\b CFALSE	indicate that Module is NOT Busy.
 *	@see		NX_PPM_GetPhysicalAddress,	NX_PPM_GetSizeOfRegisterSet,
 *				NX_PPM_SetBaseAddress,		NX_PPM_GetBaseAddress,
 *				NX_PPM_OpenModule,			NX_PPM_CloseModule,
 *				NX_PPM_CanPowerDown
 */
CBOOL	NX_PPM_CheckBusy( void )
{
	const U16 PPMENB_BITPOS = 15;

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->PPM_CTRL & ( 1 << PPMENB_BITPOS ) )
	{
		return CFALSE;
	}
	else
	{
		return CFALSE;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicaes whether the selected modules is ready to enter power-down stage
 *	@return		\b CTRUE	indicate that Ready to enter power-down stage. \n
 *				\b CFALSE	indicate that This module can't enter to power-down stage.
 *	@see		NX_PPM_GetPhysicalAddress,	NX_PPM_GetSizeOfRegisterSet,
 *				NX_PPM_SetBaseAddress,		NX_PPM_GetBaseAddress,
 *				NX_PPM_OpenModule,			NX_PPM_CloseModule,
 *				NX_PPM_CheckBusy
 */
CBOOL	NX_PPM_CanPowerDown( void )
{
	return CTRUE;
}

//------------------------------------------------------------------------------
// Clock Control Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Set a PCLK mode
 *	@param[in]	mode	PCLK mode
 *	@return		None.
 *	@see										NX_PPM_GetClockPClkMode,
 *				NX_PPM_SetClockSource,			NX_PPM_GetClockSource,
 *				NX_PPM_SetClockDivisor,			NX_PPM_GetClockDivisor,
 *				NX_PPM_SetClockDivisorEnable,	NX_PPM_GetClockDivisorEnable
 */
void			NX_PPM_SetClockPClkMode( NX_PCLKMODE mode )
{
	const U32 PCLKMODE_POS	=	3;

	register U32 regvalue;
	register struct NX_PPM_RegisterSet* pRegister;

	U32 clkmode=0;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	switch(mode)
	{
		case NX_PCLKMODE_DYNAMIC:	clkmode = 0;		break;
		case NX_PCLKMODE_ALWAYS:	clkmode = 1;		break;
		default: NX_ASSERT( CFALSE );
	}

	regvalue = pRegister->PPM_CLKENB;

	regvalue &= ~(1UL<<PCLKMODE_POS);
	regvalue |= ( clkmode & 0x01 ) << PCLKMODE_POS;

//	pRegister->PPM_CLKENB = regvalue;
	WriteIODW(&pRegister->PPM_CLKENB, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get current PCLK mode
 *	@return		Current PCLK mode
 *	@see		NX_PPM_SetClockPClkMode,
 *				NX_PPM_SetClockBClkMode,		NX_PPM_GetClockBClkMode,
 *				NX_PPM_SetClockSource,			NX_PPM_GetClockSource,
 *				NX_PPM_SetClockDivisor,			NX_PPM_GetClockDivisor,
 *				NX_PPM_SetClockDivisorEnable,	NX_PPM_GetClockDivisorEnable
 */
NX_PCLKMODE	NX_PPM_GetClockPClkMode( void )
{
	const U32 PCLKMODE_POS	= 3;

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->PPM_CLKENB & ( 1UL << PCLKMODE_POS ) )
	{
		return NX_PCLKMODE_ALWAYS;
	}

	return	NX_PCLKMODE_DYNAMIC;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock source of clock generator
 *	@param[in]	Index	Select clock generator( 0 : clock generator 0 );
 *	@param[in]	ClkSrc	Select clock source of clock generator.\n
 *						0:PLL0, 1:PLL1, 2:None, 3:XTI \n
 *	@remarks	PPM controller have one clock generator. so \e Index must set to 0.
 *	@return		None.
 *	@see		NX_PPM_SetClockPClkMode,		NX_PPM_GetClockPClkMode,
 *												NX_PPM_GetClockSource,
 *				NX_PPM_SetClockDivisor,			NX_PPM_GetClockDivisor,
 *				NX_PPM_SetClockDivisorEnable,	NX_PPM_GetClockDivisorEnable
 */
void NX_PPM_SetClockSource( U32 Index, U32 ClkSrc )
{
	const U32 CLKSRCSEL_POS		= 2;
	const U32 CLKSRCSEL_MASK	= 0x07 << CLKSRCSEL_POS;

	register U32 ReadValue;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( 4 > ClkSrc );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->PPM_CLKGEN[Index];

	ReadValue &= ~CLKSRCSEL_MASK;
	ReadValue |= ClkSrc << CLKSRCSEL_POS;

//	__g_pRegister->PPM_CLKGEN[Index] = ReadValue;
	WriteIODW(&__g_pRegister->PPM_CLKGEN[Index], ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get clock source of specified clock generator.
 *	@param[in]	Index	Select clock generator( 0 : clock generator 0 );
 *	@return		Clock source of clock generator \n
 *				0:PLL0, 1:PLL1, 2:PLL2, 3:XTI \n
 *	@remarks	PPM controller have one clock generator. so \e Index must set to 0.
 *	@see		NX_PPM_SetClockPClkMode,		NX_PPM_GetClockPClkMode,
 *				NX_PPM_SetClockSource,
 *				NX_PPM_SetClockDivisor,			NX_PPM_GetClockDivisor,
 *				NX_PPM_SetClockDivisorEnable,	NX_PPM_GetClockDivisorEnable
 */
U32				NX_PPM_GetClockSource( U32 Index )
{
	const U32 CLKSRCSEL_POS		= 2;
	const U32 CLKSRCSEL_MASK	= 0x07 << CLKSRCSEL_POS;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( CNULL != __g_pRegister );

	return ( __g_pRegister->PPM_CLKGEN[Index] & CLKSRCSEL_MASK ) >> CLKSRCSEL_POS;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock divisor of specified clock generator.
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0 );
 *	@param[in]	Divisor		Clock divisor ( 1 ~ 256 ).
 *	@return		None.
 *	@remarks	PPM controller have one clock generator. so \e Index must set to 0.
 *	@see		NX_PPM_SetClockPClkMode,		NX_PPM_GetClockPClkMode,
 *				NX_PPM_SetClockSource,			NX_PPM_GetClockSource,
 *												NX_PPM_GetClockDivisor,
 *				NX_PPM_SetClockDivisorEnable,	NX_PPM_GetClockDivisorEnable
 */
void NX_PPM_SetClockDivisor( U32 Index, U32 Divisor )
{
	const U32 CLKDIV_POS	=	5;
	const U32 CLKDIV_MASK	=	0xFF << CLKDIV_POS;

	register U32 ReadValue;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( 1 <= Divisor && Divisor <= 256 );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue	=	__g_pRegister->PPM_CLKGEN[Index];

	ReadValue	&= ~CLKDIV_MASK;
	ReadValue	|= (Divisor-1) << CLKDIV_POS;

//	__g_pRegister->PPM_CLKGEN[Index] = ReadValue;
	WriteIODW(&__g_pRegister->PPM_CLKGEN[Index], ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get clock divisor of specified clock generator.
 *	@param[in]	Index		Select clock generator( 0 : clock generator 0 );
 *	@return		Clock divisor ( 1 ~ 256 ).
 *	@remarks	PPM controller have one clock generator. so \e Index must set to 0.
 *	@see		NX_PPM_SetClockPClkMode,		NX_PPM_GetClockPClkMode,
 *				NX_PPM_SetClockSource,			NX_PPM_GetClockSource,
 *				NX_PPM_SetClockDivisor,
 *				NX_PPM_SetClockDivisorEnable,	NX_PPM_GetClockDivisorEnable
 */
U32 NX_PPM_GetClockDivisor( U32 Index )
{
	const U32 CLKDIV_POS	=	5;
	const U32 CLKDIV_MASK	=	0xFF << CLKDIV_POS;

	NX_ASSERT( 0 == Index );
	NX_ASSERT( CNULL != __g_pRegister );

	return ((__g_pRegister->PPM_CLKGEN[Index] & CLKDIV_MASK) >> CLKDIV_POS) + 1;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set clock generator's operation
 *	@param[in]	Enable	\b CTRUE	indicate that Enable of clock generator. \n
 *						\b CFALSE	indicate that Disable of clock generator.
 *	@return		None.
 *	@see		NX_PPM_SetClockPClkMode,		NX_PPM_GetClockPClkMode,
 *				NX_PPM_SetClockSource,			NX_PPM_GetClockSource,
 *				NX_PPM_SetClockDivisor,			NX_PPM_GetClockDivisor,
 *												NX_PPM_GetClockDivisorEnable
 */
void NX_PPM_SetClockDivisorEnable( CBOOL Enable )
{
	const U32	CLKGENENB_POS	=	2;
	const U32	CLKGENENB_MASK	=	1UL << CLKGENENB_POS;

	register U32 ReadValue;

	NX_ASSERT( (0==Enable) ||(1==Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue	=	__g_pRegister->PPM_CLKENB;

	ReadValue	&=	~CLKGENENB_MASK;
	ReadValue	|= (U32)Enable << CLKGENENB_POS;

//	__g_pRegister->PPM_CLKENB	=	ReadValue;
	WriteIODW(&__g_pRegister->PPM_CLKENB, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get status of clock generator's operation
 *	@return		\b CTRUE	indicate that Clock generator is enabled.\n
 *				\b CFALSE	indicate that Clock generator is disabled.
 *	@see		NX_PPM_SetClockPClkMode,		NX_PPM_GetClockPClkMode,
 *				NX_PPM_SetClockSource,			NX_PPM_GetClockSource,
 *				NX_PPM_SetClockDivisor,			NX_PPM_GetClockDivisor,
 *				NX_PPM_SetClockDivisorEnable
 */
CBOOL NX_PPM_GetClockDivisorEnable( void )
{
	const U32	CLKGENENB_POS	=	2;
	const U32	CLKGENENB_MASK	=	1UL << CLKGENENB_POS;

	NX_ASSERT( CNULL != __g_pRegister );

	return	(CBOOL)( (__g_pRegister->PPM_CLKENB & CLKGENENB_MASK) >> CLKGENENB_POS );
}

//------------------------------------------------------------------------------
// Interrupt Interface
//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number for interrupt controller.
 *	@return		Interrupt number
 *	@see											NX_PPM_SetInterruptEnable,
 *				NX_PPM_GetInterruptEnable,			NX_PPM_SetInterruptEnable32,
 *				NX_PPM_GetInterruptEnable32,		NX_PPM_GetInterruptPending,
 *				NX_PPM_GetInterruptPending32,		NX_PPM_ClearInterruptPending,
 *				NX_PPM_ClearInterruptPending32,		NX_PPM_SetInterruptEnableAll,
 *				NX_PPM_GetInterruptEnableAll,		NX_PPM_GetInterruptPendingAll,
 *				NX_PPM_ClearInterruptPendingAll,	NX_PPM_GetInterruptPendingNumber
 */
S32		NX_PPM_GetInterruptNumber( void )
{
	return	INTNUM_OF_PPM_MODULE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	IntNum	Interrupt Number ( 0:Rising Edge Detect, 1:Falling Edge Detect, 2:Overflow ).
 *	@param[in]	Enable	\b CTRUE	indicate that Interrupt Enable. \n
 *						\b CFALSE	indicate that Interrupt Disable.
 *	@return		None.
 *	@see		NX_PPM_GetInterruptNumber,
 *				NX_PPM_GetInterruptEnable,			NX_PPM_SetInterruptEnable32,
 *				NX_PPM_GetInterruptEnable32,		NX_PPM_GetInterruptPending,
 *				NX_PPM_GetInterruptPending32,		NX_PPM_ClearInterruptPending,
 *				NX_PPM_ClearInterruptPending32,		NX_PPM_SetInterruptEnableAll,
 *				NX_PPM_GetInterruptEnableAll,		NX_PPM_GetInterruptPendingAll,
 *				NX_PPM_ClearInterruptPendingAll,	NX_PPM_GetInterruptPendingNumber
 */
void	NX_PPM_SetInterruptEnable( S32 IntNum, CBOOL Enable )
{
	register U16 ReadValue;

	NX_ASSERT( (3 > IntNum) && (0 <= IntNum) );
	NX_ASSERT( (0 == Enable) || (1 == Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	ReadValue = __g_pRegister->PPM_CTRL;

	ReadValue &= ~(0x01<<IntNum);
	ReadValue |= ((U16)Enable<<IntNum);

//	__g_pRegister->PPM_CTRL = ReadValue;
	WriteIOW(&__g_pRegister->PPM_CTRL, ReadValue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is enabled or disabled.
 *	@param[in]	IntNum	Interrupt Number( 0:Rising Edge Detect, 1:Falling Edge Detect, 2:Overflow ).
 *	@return		\b CTRUE	indicate that Interrupt is enabled. \n
 *				\b CFALSE	indicate that Interrupt is disabled.
 *	@see		NX_PPM_GetInterruptNumber,			NX_PPM_SetInterruptEnable,
 *													NX_PPM_SetInterruptEnable32,
 *				NX_PPM_GetInterruptEnable32,		NX_PPM_GetInterruptPending,
 *				NX_PPM_GetInterruptPending32,		NX_PPM_ClearInterruptPending,
 *				NX_PPM_ClearInterruptPending32,		NX_PPM_SetInterruptEnableAll,
 *				NX_PPM_GetInterruptEnableAll,		NX_PPM_GetInterruptPendingAll,
 *				NX_PPM_ClearInterruptPendingAll,	NX_PPM_GetInterruptPendingNumber
 */
CBOOL	NX_PPM_GetInterruptEnable( S32 IntNum )
{
	NX_ASSERT( (3 > IntNum) && (0 <= IntNum) );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( (__g_pRegister->PPM_CTRL >> IntNum) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set a specified interrupt to be enable or disable.
 *	@param[in]	EnableFlag	Specify interrupt bit for enable of disable. Each bit's meaning is like below	\n
 *							- EnableFlag[0] : Set Rising edge detect interrupt enable or disable. \n
 *							- EnableFlag[1] : Set Falling edge detect interrupt enable or disable. \n
 *							- EnableFlag[2] : Set Overflow interrupt interrupt enable or disable. \n
 *	@return		None.
 *	@see		NX_PPM_GetInterruptNumber,			NX_PPM_SetInterruptEnable,
 *				NX_PPM_GetInterruptEnable,
 *				NX_PPM_GetInterruptEnable32,		NX_PPM_GetInterruptPending,
 *				NX_PPM_GetInterruptPending32,		NX_PPM_ClearInterruptPending,
 *				NX_PPM_ClearInterruptPending32,		NX_PPM_SetInterruptEnableAll,
 *				NX_PPM_GetInterruptEnableAll,		NX_PPM_GetInterruptPendingAll,
 *				NX_PPM_ClearInterruptPendingAll,	NX_PPM_GetInterruptPendingNumber
 */
void	NX_PPM_SetInterruptEnable32( U32 EnableFlag )
{
	const U32	ENB_MASK = 0x07 ;

	register U32 regvalue;

	NX_ASSERT( CNULL != __g_pRegister );

	regvalue = __g_pRegister->PPM_CTRL & ~ENB_MASK;

	regvalue |= EnableFlag & ENB_MASK;

//	__g_pRegister->PPM_CTRL = regvalue;
	WriteIOW(&__g_pRegister->PPM_CTRL, regvalue);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt enable bit.
 *	@return		Current setting value of interrupt. \n
 *				"1" means interrupt is enabled. \n
 *				"0" means interrupt is disabled. \n
 *				- Return Value[0] : Rising edge detect interrupt's setting value. \n
 *				- Return Value[1] : Falling edge detect interrupt's setting value. \n
 *				- Return Value[2] : Overflow interrupt interrupt's setting value. \n
 *	@see		NX_PPM_GetInterruptNumber,			NX_PPM_SetInterruptEnable,
 *				NX_PPM_GetInterruptEnable,			NX_PPM_SetInterruptEnable32,
 *													NX_PPM_GetInterruptPending,
 *				NX_PPM_GetInterruptPending32,		NX_PPM_ClearInterruptPending,
 *				NX_PPM_ClearInterruptPending32,		NX_PPM_SetInterruptEnableAll,
 *				NX_PPM_GetInterruptEnableAll,		NX_PPM_GetInterruptPendingAll,
 *				NX_PPM_ClearInterruptPendingAll,	NX_PPM_GetInterruptPendingNumber
 */
U32		NX_PPM_GetInterruptEnable32( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (U32)(__g_pRegister->PPM_CTRL & 0x07 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether a specified interrupt is pended or not
 *	@param[in]	IntNum	Interrupt Number( 0:Rising Edge Detect, 1:Falling Edge Detect, 2:Overflow ).
 *	@return		\b CTRUE	indicate that Pending is seted. \n
 *				\b CFALSE	indicate that Pending is Not Seted.
 *	@see		NX_PPM_GetInterruptNumber,			NX_PPM_SetInterruptEnable,
 *				NX_PPM_GetInterruptEnable,			NX_PPM_SetInterruptEnable32,
 *				NX_PPM_GetInterruptEnable32,
 *				NX_PPM_GetInterruptPending32,		NX_PPM_ClearInterruptPending,
 *				NX_PPM_ClearInterruptPending32,		NX_PPM_SetInterruptEnableAll,
 *				NX_PPM_GetInterruptEnableAll,		NX_PPM_GetInterruptPendingAll,
 *				NX_PPM_ClearInterruptPendingAll,	NX_PPM_GetInterruptPendingNumber
 */
CBOOL	NX_PPM_GetInterruptPending( S32 IntNum )
{
	NX_ASSERT( (3 > IntNum) && (0 <= IntNum) );
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( (__g_pRegister->PPM_STAT >> IntNum) & 0x01 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates current setting value of interrupt pending bit.
 *	@return		Current setting value of pending bit. \n
 *				"1" means pend bit is occured. \n
 *				"0" means pend bit is NOT occured. \n
 *				- Return Value[0] : Rising edge detect	pending state. \n
 *				- Return Value[1] : Falling edge detect pending state. \n
 *				- Return Value[2] : Overflow interrupt	pending state. \n
 *	@see		NX_PPM_GetInterruptNumber,			NX_PPM_SetInterruptEnable,
 *				NX_PPM_GetInterruptEnable,			NX_PPM_SetInterruptEnable32,
 *				NX_PPM_GetInterruptEnable32,		NX_PPM_GetInterruptPending,
 *													NX_PPM_ClearInterruptPending,
 *				NX_PPM_ClearInterruptPending32,		NX_PPM_SetInterruptEnableAll,
 *				NX_PPM_GetInterruptEnableAll,		NX_PPM_GetInterruptPendingAll,
 *				NX_PPM_ClearInterruptPendingAll,	NX_PPM_GetInterruptPendingNumber
 */
U32		NX_PPM_GetInterruptPending32( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return (CBOOL)( __g_pRegister->PPM_STAT & 0x07 );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	IntNum	Interrupt number( 0:Rising Edge Detect, 1:Falling Edge Detect, 2:Overflow ).
 *	@return		None.
 *	@see		NX_PPM_GetInterruptNumber,			NX_PPM_SetInterruptEnable,
 *				NX_PPM_GetInterruptEnable,			NX_PPM_SetInterruptEnable32,
 *				NX_PPM_GetInterruptEnable32,		NX_PPM_GetInterruptPending,
 *				NX_PPM_GetInterruptPending32,
 *				NX_PPM_ClearInterruptPending32,		NX_PPM_SetInterruptEnableAll,
 *				NX_PPM_GetInterruptEnableAll,		NX_PPM_GetInterruptPendingAll,
 *				NX_PPM_ClearInterruptPendingAll,	NX_PPM_GetInterruptPendingNumber
 */
void	NX_PPM_ClearInterruptPending( S32 IntNum )
{
	NX_ASSERT( (3 > IntNum) && (0 <= IntNum) );
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->PPM_STAT = (0x01 << IntNum);
	WriteIOW(&__g_pRegister->PPM_STAT, (0x01 << IntNum));
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear a pending state of specified interrupt.
 *	@param[in]	PendingFlag		Specify pend bit to clear. Each bit's meaning is like below	\n \n
 *								- PendingFlag[0] : Rising edge detect pending bit. \n
 *								- PendingFlag[1] : Falling edge detect pending bit. \n
 *								- PendingFlag[2] : Overflow interrupt pending bit. \n
 *	@return		None.
 *	@see		NX_PPM_GetInterruptNumber,			NX_PPM_SetInterruptEnable,
 *				NX_PPM_GetInterruptEnable,			NX_PPM_SetInterruptEnable32,
 *				NX_PPM_GetInterruptEnable32,		NX_PPM_GetInterruptPending,
 *				NX_PPM_GetInterruptPending32,		NX_PPM_ClearInterruptPending,
 *													NX_PPM_SetInterruptEnableAll,
 *				NX_PPM_GetInterruptEnableAll,		NX_PPM_GetInterruptPendingAll,
 *				NX_PPM_ClearInterruptPendingAll,	NX_PPM_GetInterruptPendingNumber
 */
void	NX_PPM_ClearInterruptPending32( U32 PendingFlag )
{
	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->PPM_STAT = PendingFlag & 0x07;
	WriteIOW(&__g_pRegister->PPM_STAT, PendingFlag & 0x07);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Set all interrupts to be enables or disables.
 *	@param[in]	Enable	\b CTRUE	indicate that Set to all interrupt enable. \n
 *						\b CFALSE	indicate that Set to all interrupt disable.
 *	@return		None.
 *	@see		NX_PPM_GetInterruptNumber,			NX_PPM_SetInterruptEnable,
 *				NX_PPM_GetInterruptEnable,			NX_PPM_SetInterruptEnable32,
 *				NX_PPM_GetInterruptEnable32,		NX_PPM_GetInterruptPending,
 *				NX_PPM_GetInterruptPending32,		NX_PPM_ClearInterruptPending,
 *				NX_PPM_ClearInterruptPending32,
 *				NX_PPM_GetInterruptEnableAll,		NX_PPM_GetInterruptPendingAll,
 *				NX_PPM_ClearInterruptPendingAll,	NX_PPM_GetInterruptPendingNumber
 */
void	NX_PPM_SetInterruptEnableAll( CBOOL Enable )
{
	const U16	PPMIRQRFO_MASK = 0x07;
	register U32 regval;

	NX_ASSERT( (0 == Enable) || (1 == Enable) );
	NX_ASSERT( CNULL != __g_pRegister );

	regval = __g_pRegister->PPM_CTRL;
	if( Enable )
	{
		regval |= PPMIRQRFO_MASK;
	}
	else
	{
		regval &= ~PPMIRQRFO_MASK;
	}
	WriteIOW(&__g_pRegister->PPM_CTRL, regval);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are enable or not.
 *	@return		\b CTRUE	indicate that At least one( or more ) interrupt is enabled. \n
 *				\b CFALSE	indicate that All interrupt is disabled.
 *	@see		NX_PPM_GetInterruptNumber,			NX_PPM_SetInterruptEnable,
 *				NX_PPM_GetInterruptEnable,			NX_PPM_SetInterruptEnable32,
 *				NX_PPM_GetInterruptEnable32,		NX_PPM_GetInterruptPending,
 *				NX_PPM_GetInterruptPending32,		NX_PPM_ClearInterruptPending,
 *				NX_PPM_ClearInterruptPending32,		NX_PPM_SetInterruptEnableAll,
 *													NX_PPM_GetInterruptPendingAll,
 *				NX_PPM_ClearInterruptPendingAll,	NX_PPM_GetInterruptPendingNumber
 */
CBOOL	NX_PPM_GetInterruptEnableAll( void )
{
	const U16	PPMIRQRFO_MASK = 0x07;

	NX_ASSERT( CNULL != __g_pRegister );

	if( PPMIRQRFO_MASK & __g_pRegister->PPM_CTRL )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Indicates whether some of interrupts are pended or not.
 *	@return		\b CTRUE	indicate that At least one( or more ) pending is seted. \n
 *				\b CFALSE	indicate that All pending is NOT seted.
 *	@see		NX_PPM_GetInterruptNumber,			NX_PPM_SetInterruptEnable,
 *				NX_PPM_GetInterruptEnable,			NX_PPM_SetInterruptEnable32,
 *				NX_PPM_GetInterruptEnable32,		NX_PPM_GetInterruptPending,
 *				NX_PPM_GetInterruptPending32,		NX_PPM_ClearInterruptPending,
 *				NX_PPM_ClearInterruptPending32,		NX_PPM_SetInterruptEnableAll,
 *				NX_PPM_GetInterruptEnableAll,		
 *				NX_PPM_ClearInterruptPendingAll,	NX_PPM_GetInterruptPendingNumber
 */
CBOOL	NX_PPM_GetInterruptPendingAll( void )
{
	const U16	PPMPENDRFO_MASK = 0x07;

	NX_ASSERT( CNULL != __g_pRegister );

	if( PPMPENDRFO_MASK & __g_pRegister->PPM_STAT )
	{
		return CTRUE;
	}

	return CFALSE;
}

//------------------------------------------------------------------------------
/**
 *	@brief		Clear pending state of all interrupts.
 *	@return		None.
 *	@see		NX_PPM_GetInterruptNumber,			NX_PPM_SetInterruptEnable,
 *				NX_PPM_GetInterruptEnable,			NX_PPM_SetInterruptEnable32,
 *				NX_PPM_GetInterruptEnable32,		NX_PPM_GetInterruptPending,
 *				NX_PPM_GetInterruptPending32,		NX_PPM_ClearInterruptPending,
 *				NX_PPM_ClearInterruptPending32,		NX_PPM_SetInterruptEnableAll,
 *				NX_PPM_GetInterruptEnableAll,		NX_PPM_GetInterruptPendingAll,
 *													NX_PPM_GetInterruptPendingNumber
 */
void	NX_PPM_ClearInterruptPendingAll( void )
{
	const U16	PPMPENDRFO_MASK = 0x07;

	NX_ASSERT( CNULL != __g_pRegister );

//	__g_pRegister->PPM_STAT = PPMPENDRFO_MASK;
	WriteIOW(&__g_pRegister->PPM_STAT, PPMPENDRFO_MASK);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get a interrupt number which has the most prority of pended interrupts
 *	@return		Pending Number( If all pending is not set then return -1 ).\n
 *				0 (Rising Edge Detect), 1 (Falling Edge Detect), 2 (Overflow)
 *	@see		NX_PPM_GetInterruptNumber,			NX_PPM_SetInterruptEnable,
 *				NX_PPM_GetInterruptEnable,			NX_PPM_SetInterruptEnable32,
 *				NX_PPM_GetInterruptEnable32,		NX_PPM_GetInterruptPending,
 *				NX_PPM_GetInterruptPending32,		NX_PPM_ClearInterruptPending,
 *				NX_PPM_ClearInterruptPending32,		NX_PPM_SetInterruptEnableAll,
 *				NX_PPM_GetInterruptEnableAll,		NX_PPM_GetInterruptPendingAll,
 *				NX_PPM_ClearInterruptPendingAll	
 */
S32		NX_PPM_GetInterruptPendingNumber( void )	// -1 if None
{
	register struct NX_PPM_RegisterSet	*pRegister;
	U32 pend;
	U32 intnum;

	NX_ASSERT( CNULL != __g_pRegister );

	pRegister = __g_pRegister;

	pend = (pRegister->PPM_CTRL & pRegister->PPM_STAT);

	for( intnum = 0; intnum < 3; intnum++ )
	{
		if( 0 != (pend & ( 1 << intnum )) )
		{
			return intnum;
		}
	}

	return -1;
}

//--------------------------------------------------------------------------
// PPM Operation Function
//--------------------------------------------------------------------------
/**
 *	@brief		Set PPM Enable or Disable
 *	@param[in]	enable		CTRUE( PPM Enable ), CFALSE( PPM Disable )
 *	@return		None.
 *	@see		NX_PPM_GetPPMEnable
 */
void	NX_PPM_SetPPMEnable
(
	CBOOL enable
)
{
	const U32 ENB_MASK	= ( 0x01 << 15 );
	register U32 temp;

	NX_ASSERT( CNULL != __g_pRegister );

	temp = __g_pRegister->PPM_CTRL;

	if( CTRUE == enable )
	{
		temp |= ENB_MASK;
	}
	else
	{
		temp &= ~ENB_MASK;
	}

//	__g_pRegister->PPM_CTRL = (U16)temp;
	WriteIOW(&__g_pRegister->PPM_CTRL, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get State of PPM is Enable or Disable
 *	@return		CTRUE	indicate that PPM is Enabled.\n
 *				CFALSE	indicate that PPM is Disabled.
 *	@see		NX_PPM_SetPPMEnable
 */

CBOOL	NX_PPM_GetPPMEnable ( void )
{
	const U32 ENB_MASK	= ( 0x01 << 15 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->PPM_CTRL & ENB_MASK )
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
 *	@brief		Set Polarity of PPM Input Signal
 *	@param[in]	polarity		Set Polarity ( Invert or Bypass )
 *	@return		None.
 *	@see		NX_PPM_GetInputSignalPolarity
 */
void	NX_PPM_SetInputSignalPolarity
(
	NX_PPM_INPUTPOL polarity
)
{
	const U32 POL_BITPOS	=	14;
	const U32 POL_MASK		=	( 0x01 << POL_BITPOS );

	U32 temp;

	NX_ASSERT( CNULL != __g_pRegister );
	NX_ASSERT( NX_PPM_INPUTPOL_BYPASS >= polarity );

	temp = __g_pRegister->PPM_CTRL;

	if( NX_PPM_INPUTPOL_INVERT == polarity )
	{
		temp &= ~POL_MASK;
	}
	else if( NX_PPM_INPUTPOL_BYPASS == polarity )
	{
		temp |= POL_MASK;
	}

//	__g_pRegister->PPM_CTRL = (U16)temp;
	WriteIOW(&__g_pRegister->PPM_CTRL, (U16)temp);
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Polarity of PPM Input Signal
 *	@return		Value of input signal's polarity ( Invert or Bypass )
 *	@see		NX_PPM_SetInputSignalPolarity
 */

NX_PPM_INPUTPOL	NX_PPM_GetInputSignalPolarity( void )
{
	const U32 POL_MASK	= ( 0x01 << 14 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->PPM_CTRL & POL_MASK )
	{
		return NX_PPM_INPUTPOL_BYPASS;
	}
	else
	{
		return NX_PPM_INPUTPOL_INVERT;
	}
}

//------------------------------------------------------------------------------
/**
 *	@brief		Get Overflow State In High Period
 *	@return		CTRUE indicate that High period value is overflowed.\n
 *				CFALSE indicate that High period value is NOT overflowed.
 *	@remarks	Check overflow state ( high period's value ) when Overflow interrupt is occurred.
 *	@see		NX_PPM_IsLowOverflow
 */
CBOOL	NX_PPM_IsHighOverflow( void )
{
	const U32	HOVER_MASK = ( 0x01 << 3 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->PPM_STAT & HOVER_MASK )
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
 *	@brief		Get Overflow State In Low Period
 *	@return		CTRUE indicate that Low period value is overflowed.\n
 *				CFALSE indicate that Low period value is NOT overflowed.
 *	@remarks	Check overflow state ( low period's value ) when Overflow interrupt is occured.
 *	@see		NX_PPM_CheckHighOverflow
 */

CBOOL	NX_PPM_IsLowOverflow( void )
{
	const U32	LOVER_MASK = ( 0x01 << 4 );

	NX_ASSERT( CNULL != __g_pRegister );

	if( __g_pRegister->PPM_STAT & LOVER_MASK )
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
 *	@brief		Read Count Value of Low Period
 *	@return		Value of Low period ( 0 ~ 0xFFFF )
 *	@see		NX_PPM_GetPPMHighPeriodValue
 */
U32		NX_PPM_GetPPMLowPeriodValue ( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return(U32)( __g_pRegister->PPM_LOWPERIOD );
}

//------------------------------------------------------------------------------
/**
 *	@brief		Read Count Value of High Period
 *	@return		Value of High period ( 0 ~ 0xFFFF )
 *	@see		NX_PPM_GetPPMLowPeriodValue
 */
U32		NX_PPM_GetPPMHighPeriodValue ( void )
{
	NX_ASSERT( CNULL != __g_pRegister );

	return(U32)( __g_pRegister->PPM_HIGHPERIOD );
}
