//------------------------------------------------------------------------------
//
//  Copyright(c) 2011 MOSTiTECH co., ltd.
//  All right reserved by Segunwoo Kim.
//  Slight modification for LINUX
//
//	Copyright (C) 2009 Nexell Co. All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND	WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//	FOR A PARTICULAR PURPOSE.
//
//	Module		: nx_base
//	File		: nx_chip.h
//	Description	: support chip information
//	Author		: Goofy
//	Export		: 
//	History		: 
//		2010.04.27	Hans NXP-2120
//		2007.04.04	Goofy first draft.
//------------------------------------------------------------------------------
#ifndef __NX_CHIP_H__
#define __NX_CHIP_H__

#define NX_CHIP_P2120

#if	defined(NX_CHIP_P2120)
	#include <mach/nx_chip_p2120.h>
#else
	#error "No NX_CHIP_XXX found. You have to define NX_CHIP_XXX properly."
#endif

#endif // __NX_CHIP_H__
