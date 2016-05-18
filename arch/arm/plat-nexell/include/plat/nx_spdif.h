//	
//  Copyright (C) 2011 STcube co., ltd.
//  All right reserved by Seungwoo Kim <ksw@stcube.com> 
//
//
//	Module		: SPDIF
//	File		: nx_spdif.h
//	Description	:
//	Author		: Seungwoo Kim
//	History		: 2011. 10. 4. ver 0.5 initial draft 
//------------------------------------------------------------------------------
#ifndef __NX_SPDIF_H__
#define __NX_SPDIF_H__

#include <nx_type.h>
#include <mach/nx_chip.h>
#include <mach/nx_clockcontrol.h>

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	SPDIF SPDIF
//------------------------------------------------------------------------------
//@{

	/// @brief	SPDIF Module's Register List
	struct	NX_SPDIF_RegisterSet
	{
		volatile U32 SPDIF_CONTROL;		///< 0x00 : SPDIF Control Register
		volatile U32 SPDIF_PEND;			///< 0x04 : SPDIF Pending Register
		volatile U32 SPDIF_USER_A0;		///< 0x08 : SPDIF User A[0] Register [31:0]
		volatile U32 SPDIF_USER_A1;		///< 0x0C : SPDIF User A[1] Register [63:32]
		volatile U32 SPDIF_USER_A2;		///< 0x10 : SPDIF User A[2] Register [95:64]
		volatile U32 SPDIF_USER_A3;		///< 0x14 : SPDIF User A[3] Register [127:96]
		volatile U32 SPDIF_USER_A4;		///< 0x18 : SPDIF User A[4] Register [159:128]
		volatile U32 SPDIF_USER_A5;		///< 0x1C : SPDIF User A[5] Register [191:160]
		volatile U32 SPDIF_USER_B0;		///< 0x20 : SPDIF User B[0] Register [31:0]
		volatile U32 SPDIF_USER_B1;		///< 0x24 : SPDIF User B[1] Register [63:32]
		volatile U32 SPDIF_USER_B2;		///< 0x28 : SPDIF User B[2] Register [95:64]
		volatile U32 SPDIF_USER_B3;		///< 0x2C : SPDIF User B[3] Register [127:96]
		volatile U32 SPDIF_USER_B4;		///< 0x30 : SPDIF User B[4] Register [159:128]
		volatile U32 SPDIF_USER_B5;		///< 0x34 : SPDIF User B[5] Register [191:160]
		volatile U32 SPDIF_STATUS_A0;		///< 0x38 : SPDIF Status A[0] Register [31:0]
		volatile U32 SPDIF_STATUS_A1;		///< 0x3C : SPDIF Status A[1] Register [63:32]
		volatile U32 SPDIF_STATUS_A2;		///< 0x40 : SPDIF Status A[2] Register [95:64]
		volatile U32 SPDIF_STATUS_A3;		///< 0x44 : SPDIF Status A[3] Register [127:96]
		volatile U32 SPDIF_STATUS_A4;		///< 0x48 : SPDIF Status A[4] Register [159:128]
		volatile U32 SPDIF_STATUS_A5;		///< 0x4C : SPDIF Status A[5] Register [191:160]
		volatile U32 SPDIF_STATUS_B0;		///< 0x50 : SPDIF Status B[0] Register [31:0]
		volatile U32 SPDIF_STATUS_B1;		///< 0x54 : SPDIF Status B[1] Register [63:32]
		volatile U32 SPDIF_STATUS_B2;		///< 0x58 : SPDIF Status B[2] Register [95:64]
		volatile U32 SPDIF_STATUS_B3;		///< 0x5C : SPDIF Status B[3] Register [127:96]
		volatile U32 SPDIF_STATUS_B4;		///< 0x60 : SPDIF Status B[4] Register [159:128]
		volatile U32 SPDIF_STATUS_B5;		///< 0x64 : SPDIF Status B[5] Register [191:160]
	};

#ifdef	__cplusplus
}
#endif

#endif // __NX_SPDIF_H__