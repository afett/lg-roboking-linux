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
//	File		: nx_type.h
//	Description	: support base type
//	Author		: Goofy
//	Export		:
//	History		:
//		2010.04.27	Hans
//		2007.04.04	Goofy	First draft
//------------------------------------------------------------------------------
#ifndef __NX_TYPE_H__
#define	__NX_TYPE_H__

//------------------------------------------------------------------------------
/// @name Basic data types
///
/// @brief Basic data type define and Data type constants are implemen \n
///	tation-dependent ranges of values allowed for integral data types. \n
/// The constants listed below give the ranges for the integral data types
//------------------------------------------------------------------------------
/// @{
///
typedef char				S8;				///< 8bit signed integer(s.7) value
typedef short				S16;			///< 16bit signed integer(s.15) value
typedef int					S32;			///< 32bit signed integer(s.31) value
typedef long long			S64;			///< 64bit signed integer(s.63) value
typedef unsigned char		U8;				///< 8bit unsigned integer value
typedef unsigned short		U16;			///< 16bit unsigned integer value
typedef unsigned int		U32;			///< 32bit unsigned integer value
typedef unsigned long long	U64;			///< 64bit unsigned integer value


#define S8_MIN			-128				///< signed char min value
#define S8_MAX			127					///< signed char max value
#define S16_MIN			-32768				///< signed short min value
#define S16_MAX			32767				///< signed short max value
#define S32_MIN			-2147483648			///< signed integer min value
#define S32_MAX			2147483647			///< signed integer max value

#define U8_MIN			0					///< unsigned char min value
#define U8_MAX			255					///< unsigned char max value
#define U16_MIN			0					///< unsigned short min value
#define U16_MAX			65535				///< unsigned short max value
#define U32_MIN			0					///< unsigned integer min value
#define U32_MAX			4294967295			///< unsigned integer max value
/// @}

//==============================================================================
/// @name Boolean data type
///
/// C and C++ has difference boolean type. so we use signed integer type \n
/// instead of bool and you must use CTRUE or CFALSE macro for CBOOL type
//
//==============================================================================
/// @{
typedef S32	CBOOL;							///< boolean type is 32bits signed integer
#define CTRUE	1							///< true value is	integer one
#define CFALSE	0							///< false value is	integer zero
/// @}

//==============================================================================
/// @name NULL data type
///
///	CNULL is the null-pointer value used with many pointer operations \n
/// and functions. and this value is signed integer zero
//==============================================================================
/// @{
#define CNULL	0							///< null pointer is integer zero
/// @}

//------------------------------------------------------------------------------
/// @name Basic type's size check
///
/// @brief this is compile time assert for check type's size. if exist don't intended size\n
/// of Basic type then it's occur compile type error
//------------------------------------------------------------------------------
/// @{
#ifndef __GNUC__
#define NX_CASSERT(expr) typedef char __NX_C_ASSERT__[(expr)?1:-1]
#else
#define NX_CASSERT(expr)
#endif

NX_CASSERT	(sizeof(S8)		== 1);
NX_CASSERT	(sizeof(S16)	== 2);
NX_CASSERT	(sizeof(S32)	== 4);
NX_CASSERT	(sizeof(S64)   == 8);
NX_CASSERT	(sizeof(U8)		== 1);
NX_CASSERT	(sizeof(U16)	== 2);
NX_CASSERT	(sizeof(U32)	== 4);
NX_CASSERT	(sizeof(U64)   == 8);
NX_CASSERT	(sizeof(CBOOL)	== 4);
/// @}

/* Now From Nx_Debug.h, Define NX_ASSERT for debug */
/* NX_ASSERT, NX_DEBUG_CODE                        */
#ifdef NX_DEBUG
	#ifdef UNDER_CE
		#define	NX_TRACE(expr)		RETAILMSG( 1, expr )

	#elif __LINUX__
		#define TEXT(str)			str
		#define NX_TRACE(expr)		NX_DBG_TRACE	expr

		#ifdef __PRINTK__
			#include <linux/kernel.h>
			#define NX_DBG_TRACE(expr...)	printk(KERN_INFO expr)
		#else
			#ifdef USE_HOSTCC
			#include <stdio.h>
			#else
			extern void printf(const char *fmt, ...);
			#endif

			#define NX_DBG_TRACE(expr...)	printf(expr)
		#endif
	#else

		//----------------------------------------------------------------------
		///	@brief Extern function for print formated string
		/// You have to implement NX_DEBUG_PRINTF function for NX_TRACE.
		//----------------------------------------------------------------------
		#ifdef	__cplusplus
			extern "C" void NX_DEBUG_PRINTF(const char * const, ... );
		#else
			extern void NX_DEBUG_PRINTF(const char * const, ... );
		#endif

		#define NX_TRACE(expr)		NX_DEBUG_PRINTF expr
	#endif
#else
	#define NX_TRACE(expr)
#endif // NX_DEBUG
//------------------------------------------------------------------------------
/// NX_ASSERT
/// @brief	runtime assert in normal case
/// @code
///	int UnKnownFunction(int a, int b) {
///
///		int Result;
///	...
///
///		for(int i = 0; i < 10; i++) {
///			NX_ASSERT(i < MAXNUM_OF_NKNOWNARRY);
///			UnknownArray[i] = RandomNumberGen();
///		}
///
///		...
/// }
/// @endcode
//
//------------------------------------------------------------------------------
#ifdef NX_DEBUG
	#define NX_ASSERT(expr)																\
	{																					\
		if (!(expr))																	\
		{																				\
			NX_TRACE((TEXT("%s(%d) : %s %s (%s)\n"),									\
				TEXT(__FILE__), __LINE__, __FUNCTION__, TEXT("ASSERT"), TEXT(#expr)));	\
			NX_DEBUG_BREAK();															\
		}																				\
	}
#else
	#define NX_ASSERT(expr)
#endif // NX_DEBUG

//------------------------------------------------------------------------------
///	NX_DEBUG_CODE
/// @brief	for debug code or debug varible declare
///
/// @code
/// {
///	NX_DEBUG_CODE(int iDebugCounter = 0);
///		...
///		NX_DEBUG_CODE(iDebugCount++);
///		...
///		NX_DEBUG_CODE(
///			if(iDebugCount == x)
///				NX_TRACE("... found!!!\n");
///		)
/// }
/// @endcode
//------------------------------------------------------------------------------
#ifdef NX_DEBUG
	#define NX_DEBUG_CODE(code) code
#else
	#define NX_DEBUG_CODE(code)
#endif // NX_DEBUG


/* WORKAROUND CODE for DEBUG/Turnarround BUS implementation BUG */
//#define NXP_2120_BUS_BUG_TURNAROUND
#ifdef __cplusplus
extern "C" {
#endif
#ifdef NXP_2120_BUS_BUG_TURNAROUND
void WriteIODW(volatile unsigned int *Addr, unsigned int Data);
void WriteIOW(volatile unsigned short *Addr, unsigned short Data);
void WriteIOB(volatile unsigned char *Addr, unsigned char Data);
#else
#define WriteIODW(Addr,Data) (*(volatile unsigned int*)Addr)=((unsigned int)Data)
#define WriteIOW(Addr,Data) (*(volatile unsigned short*)Addr)=((unsigned short)Data)
#define WriteIOB(Addr,Data) (*(volatile unsigned char*)Addr)=((unsigned char)Data)
#endif
#ifdef __cplusplus
}
#endif


#endif	// __NX_TYPE_H__
