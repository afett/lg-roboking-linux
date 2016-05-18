//------------------------------------------------------------------------------
//
//	Copyright (C) 2008 MagicEyes Digital Co., Ltd All Rights Reserved
//	MagicEyes Digital Co. Proprietary & Confidential
//
//	MAGICEYES INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//	AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//	BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR
//	FITNESS FOR A PARTICULAR PURPOSE.
//
//	Module     :
//	File       : nx_ogl_ioctl.h
//	Description:
//	Author     : Billy(billy@mesdigital.com)
//	Export     :
//	History    :
//		2009/02/09 The first implementation.
//------------------------------------------------------------------------------

//+-----------------------------------------------------------------------------
//| Inclusion guard
//+-----------------------------------------------------------------------------
#ifndef __OGL_H__
#define __OGL_H__

//#ifdef linux
//  #ifdef MODULE
//    #include <asm/ioctl.h>
//  #else
//    #include <sys/ioctl.h>
//  #endif //MODULE
//  #define __int64		long long
//#endif

//+-----------------------------------------------------------------------------
//| macro defines
//+-----------------------------------------------------------------------------
#if	defined(linux) || defined(__LINUX__)
  #define VR_MAGIC				'3'
  #define OGL_CTL_CODE( ID )	(((VR_MAGIC)<<16)|(ID))
  #ifndef __int64
  #define __int64		long long
  #endif
#else
  #ifndef CTL_CODE
    #define FILE_ANY_ACCESS		0
    #define METHOD_BUFFERED		0
    #define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
						  ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method))
  #endif //CTL_CODE

  #define FILE_DEVICE_OPENGL	0x0000E100
  #define OGL_CTL_CODE(id)		CTL_CODE(FILE_DEVICE_OPENGL, id, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif

//+-----------------------------------------------------------------------------
//| IOCTLs
//+-----------------------------------------------------------------------------
#define IOCTL_NX_OGL_ALLOC1D					OGL_CTL_CODE(1)
#define IOCTL_NX_OGL_ALLOC2D					OGL_CTL_CODE(2) // width, height, alignX, alignY, bytes_per_pixel
#define IOCTL_NX_OGL_FREE						OGL_CTL_CODE(3)
//#define IOCTL_NX_OGL_SHOW3D					OGL_CTL_CODE(4) // x, y, width, height
//#define IOCTL_NX_OGL_HIDE3D					OGL_CTL_CODE(5) // none
//#define IOCTL_NX_OGL_OVERLAY					OGL_CTL_CODE(6) // bool, color16
#define IOCTL_NX_OGL_HWND						OGL_CTL_CODE(7) // HWND
#define IOCTL_NX_OGL_WAIT_TO_FINISH				OGL_CTL_CODE(8)
//#define IOCTL_NX_OGL_CHECK_OUT_MEMORY  		OGL_CTL_CODE(9) // [hMem?D, offset], ...
#define IOCTL_NX_OGL_SWAP						OGL_CTL_CODE(10) // front memory(32-bit), back memory(32-bit)
//#define IOCTL_NX_OGL_COMMAND					OGL_CTL_CODE(11) // command buffer memory(32-bit), command byte size
#define IOCTL_NX_OGL_WAIT_FOR_SEQUENCE_ID		OGL_CTL_CODE(12)
//  #define IOCTL_NX_OGL_AUTO_INACTIVE_DELAY    OGL_CTL_CODE(13) // millisecond time(32-bit)
//  #define IOCTL_NX_OGL_TRY_TO_ACTIVE			OGL_CTL_CODE(14)

#define IOCTL_NX_OGL_SET_APPNAME				OGL_CTL_CODE(15) // string
#define IOCTL_NX_OGL_QUERY_CONTEXT_LIST_SIZE	OGL_CTL_CODE(16) // 4-byte
#define IOCTL_NX_OGL_QUERY_CONTEXT_LIST			OGL_CTL_CODE(17) // array of open-contexts
#define IOCTL_NX_OGL_ACTIVE_CONTEXT				OGL_CTL_CODE(18) // context's id
#define IOCTL_NX_OGL_DEACTIVE_CONTEXT			OGL_CTL_CODE(19) // context's id
#define IOCTL_NX_OGL_CAPTURE_CONTEXT			OGL_CTL_CODE(20) // context's id, hMem2D

//+-----------------------------------------------------------------------------
//| New Types
//+-----------------------------------------------------------------------------

/* Packet Types */
#define PACKET_COMMANDS			(1)
#define PACKET_MEMORY1D			(2)
#define PACKET_MEMORY2D			(3)

/* Format */
#define FORMAT_STREAM1			(0x0010)
#define FORMAT_STREAM4			(0x0020)
#define FORMAT_STREAM8 			(0x0040)
#define FORMAT_STREAM16			(0x0080)
#define FORMAT_STREAM24			(0x0100)
#define FORMAT_STREAM32			(0x0200)

#define FORMAT_A8				(0x0040)
#define FORMAT_L8				(0x0041)
#define FORMAT_LA8				(0x0080)
#define FORMAT_RGB565			(0x0081)
#define FORMAT_RGBA4444			(0x0082)
#define FORMAT_RGBA5551			(0x0083)
#define FORMAT_RGB888			(0x0100)
#define FORMAT_RGBA8888			(0x0200)

#define FORMAT_MIPMAP			(0x1000)
#define FORMAT_TEXTURE			(0X2000)
#define FORMAT_DISPLAY			(0X4000)
#define FORMAT_DISPLAY_FSAA		(0x8000)

#define FORMAT_TYPE_MASK		(0xf000)
#define FORMAT_STREAM_MASK		(0x03f0)

typedef struct stPacketHeader {
	unsigned int uiPacketType;
	unsigned int uiData[7];
} PacketHeader;

typedef struct stCommandPacketHeader {
	unsigned int uiPacketType;
	unsigned int uiReserved0;
	unsigned __int64 uiCmdSeqID;
	unsigned int uiHasSwapCommand;
	unsigned int uiBackBuffer    ;
	unsigned int uiReserved1[2];
} CommandPacketHeader;

typedef struct stMemory1DPacketHeader {
	unsigned int uiPacketType;
	unsigned int uiHandleMem;
	unsigned int uiOffset;
	unsigned int uiReserved[5];
} Memory1DPacketHeader;

typedef struct stMemory2DPacketHeader {
	unsigned int uiPacketType;
	unsigned int uiHandleMem;
	unsigned int uiOffset;

	unsigned int uiXY; // x << 16 | y
	unsigned int uiWH; // width << 16 | height
	unsigned int uiStrideB;

	unsigned int uiSrcFormat;
	unsigned int uiDstFormat;
} Memory2DPacketHeader;

typedef struct stQueryContextInfo {
	unsigned int uiID;
	unsigned int uiCallerID;
	int nX, nY;
	int nWidth, nHeight;
	char		 szAppName[64];
} QueryContextInfo;

//+-----------------------------------------------------------------------------
//| End of inclusion guard
//+-----------------------------------------------------------------------------
#endif //__OGL_H__

