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
//	File		: nx_udc.h
//	Description	:
//	Author		: Firmware Team
//	History		:
//------------------------------------------------------------------------------

#ifndef __NX_UDC_H__
#define __NX_UDC_H__

#include <nx_type.h>
#include <mach/nx_chip.h>
#include <mach/nx_clockcontrol.h>

#ifdef	__cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// @defgroup	UDC UDC
//------------------------------------------------------------------------------
//@{

	/// @brief USB Device Module's Register List
	struct	NX_UDC_RegisterSet
	{
		volatile U16	INDEX;						///< 0x00 : Index Register
		volatile U16	INTPEND;					///< 0x02 : Endpoint Interrupt Pending Register
		volatile U16	INTENB;						///< 0x04 : Endpoint Interrupt Enable Register
		volatile U16	FADDR;						///< 0x06 : Frame Address Register
		volatile U16	FNR;						///< 0x08 : Frame Number Register
		volatile U16	EDR;						///< 0x0A : Endpoint Direction Register
		volatile U16	TR;							///< 0x0C : Test Register
		volatile U16	SSR;						///< 0x0E : System Status Register
		volatile U16	SCR;						///< 0x10 : System Control Register
		volatile U16	EP0SR;						///< 0x12 : Eendpoint0 Status Register
		volatile U16	EP0CR;						///< 0x14 : Enddpoint0 Control Register
		volatile U16	ESR;						///< 0x16 : Endpoint Status Register
		volatile U16	ECR;						///< 0x18 : Endpoint Control Register
		volatile U16	BRCR;						///< 0x1A : Byte Read Count Register
		volatile U16	BWCR;						///< 0x1C : Byte Write Count Register
		volatile U16	MPR;						///< 0x1E : Max Packet Register
		volatile U16	DCR;						///< 0x20 : DMA Control Register
		volatile U16	DTCR;						///< 0x22 : DMA Transfer Counter Register
		volatile U16	DFCR;						///< 0x24 : DMA FIFO Counter Register
		volatile U16	DTTCR;						///< 0x26 : DMA Total Transfer Counter Register 1
		volatile U16	DTTCR2;						///< 0x28 : DMA Total Transfer Counter Register 1
		volatile U16	Reserved0[3];				///< 0x2A ~ 0x2E : Reserved Region
		volatile U16	BR[4];						///< 0x30 ~ 0x36 : Buffer Register
		volatile U16	Reserved1[(0x52-0x38)/2];	///< 0x38 ~ 0x50 : Reserved Region
		volatile U16	PCR;						///< 0x52 : PHY Control Register
		volatile U16	Reserved2[(0x840-0x54)/2];	///< 0x54 ~ 0x83E:Reserved Region
		volatile U16	PHYPARAM0;					///< 0x840 : USB PHY Parameter0
		volatile U16	VBUSINTENB;					///< 0x842 : VBUS Interrupt Enable Register
		volatile U16	VBUSINTPEND;				///< 0x844 : VBUS Interrupt Pending Register
		volatile U16	PHYPARAM1;					///< 0x846 : USB PHY Parameter1
		volatile U16	PHYPARAM2;					///< 0x848 : USB PHY Parameter2
		volatile U16	Reserved3[(0x8C0-0x84A)/2];	///< 0x84A ~ 0x8BE : Reserved Region
		volatile U32	CLKENB;						///< 0x8C0: Clock Enable Register
		volatile U32	CLKGEN;						///< 0x8C4: Clock Generate Register
	};

	/// @brief UDC Interrupts for interrupt interfaces
	enum NX_UDC_INT
	{
		NX_UDC_INT_RESET			= 0,	///< Reset Interrupt
		NX_UDC_INT_RESUME			= 1,	///< Resume Interrupt
		NX_UDC_INT_SUSPEND			= 2,	///< Suspend Interrupt
		NX_UDC_INT_VBUSON			= 3,	///< VBUS On Interrupt
		NX_UDC_INT_VBUSOFF			= 4,	///< VBUS Off Interrupt
		NX_UDC_INT_SPEEDDETECT		= 5,	///< Speed Detect End Interrupt
		NX_UDC_INT_ERROR			= 6,	///< Error Interrupt
		NX_UDC_INT_EP0				= 7,	///< Endpoint 0 Interrupt
		NX_UDC_INT_EP1				= 8,	///< Endpoint 1 Interrupt
		NX_UDC_INT_EP2				= 9,	///< Endpoint 2 Interrupt
		NX_UDC_INT_EP3				= 10,	///< Endpoint 3 Interrupt
		NX_UDC_INT_VBUSENB			= 11	///< VBUS Enable Interrupt

	};

	/// @brief	Enpoint Direction
	enum NX_UDC_DIR
	{
		NX_UDC_DIR_RX = 0,			///< Endpoint direction RX.
		NX_UDC_DIR_TX = 1			///< Endpoint direction TX.
	};

	/// @brief UDC Error
	enum NX_UDC_ERROR
	{
		NX_UDC_ERROR_EBUNDERRUN		= (1<<29),	///< Endpoint Buffer Underrun Error
		NX_UDC_ERROR_PID			= (1<<28),	///< PID Error
		NX_UDC_ERROR_BYTEALIGN		= (1<<15),	///< Byte Align Error
		NX_UDC_ERROR_TIMEOUT		= (1<<14),	///< Time Out Error
		NX_UDC_ERROR_BITSTUFF		= (1<<13),	///< Bit Stuff Error
		NX_UDC_ERROR_TOKENCRC		= (1<<12),	///< Token CRC Error
		NX_UDC_ERROR_DATACRC		= (1<<11),	///< Data CRC Error
		NX_UDC_ERROR_EBOVERRUN		= (1<<10),	///< Endpoint Buffer Overrun Error
		NX_UDC_ERROR_TOGGLEMISMATCH	= (1<<7 )	///< Toggle Bit Mismatch Error
	};

	/// @brief Endpoint's interrupt source
	enum	NX_UDC_EPSTATUS
	{
		NX_UDC_EPSTATUS_FUDR	= (1<<15),	///< FIFO Underflow
		NX_UDC_EPSTATUS_FOVF	= (1<<14),	///< FIFO Overflow
		NX_UDC_EPSTATUS_FPID	= (1<<11),	///< First OUT Packet Interrupt Disable in OUT DMA Operation
		NX_UDC_EPSTATUS_OSD		= (1<<10),	///< OUT Start DMA
		NX_UDC_EPSTATUS_DTCZ	= (1<< 9),	///< DMA Total Counter Zero
		NX_UDC_EPSTATUS_SPT		= (1<< 8),	///< Short Packet Received
		NX_UDC_EPSTATUS_FFS		= (1<< 6),	///< FIFO Flushed
		NX_UDC_EPSTATUS_FSC		= (1<< 5),	///< Functional Stall Condition
		NX_UDC_EPSTATUS_LWO		= (1<< 4),	///< Last Word Odd
		NX_UDC_EPSTATUS_PSIF	= (1<< 2),	///< Packet Status In FIFO
		NX_UDC_EPSTATUS_TPS		= (1<< 1),	///< Tx Packet Success
		NX_UDC_EPSTATUS_RPS		= (1<< 0)	///< Rx Packet Success
	};

	/// @brief UDC Transfer Mode
	typedef enum
	{
		NX_UDC_TRANSFER_MODE_INTERRUPT	= 0,	///< Interrupt Mode
		NX_UDC_TRANSFER_MODE_ISO		= 1,	///< Isochronous Mode
		NX_UDC_TRANSFER_MODE_BULK		= 2		///< Bulk Mode

	}	NX_UDC_TRANSFER_MODE;

	/// @brief Endpoint0's interrupt source
	enum NX_UDC_EP0STATUS
	{
		NX_UDC_EP0STATUS_LWO = (1<<6),	///< Last Word Odd
		NX_UDC_EP0STATUS_SHT = (1<<4),	///< Stall Handshake Transmitted
		NX_UDC_EP0STATUS_TST = (1<<1),	///< Tx Successfully Transmitted
		NX_UDC_EP0STATUS_RSR = (1<<0)	///< Rx Successfully Received
	};

//------------------------------------------------------------------------------
/// @name	Module Interface
//@{
CBOOL	NX_UDC_Initialize( void );
U32		NX_UDC_GetNumberOfModule( void );
//@}

//------------------------------------------------------------------------------
///	@name	Basic Interface
//@{
U32		NX_UDC_GetPhysicalAddress( void );
U32		NX_UDC_GetSizeOfRegisterSet( void );
void	NX_UDC_SetBaseAddress( U32 BaseAddress );
U32		NX_UDC_GetBaseAddress( void );
CBOOL	NX_UDC_OpenModule( void );
CBOOL	NX_UDC_CloseModule( void );
CBOOL	NX_UDC_CheckBusy( void );
CBOOL	NX_UDC_CanPowerDown( void );
//@}

//------------------------------------------------------------------------------
///	@name	Interrupt Interface
//@{
S32		NX_UDC_GetInterruptNumber( void );

void	NX_UDC_SetInterruptEnable( S32 IntNum, CBOOL Enable );
CBOOL	NX_UDC_GetInterruptEnable( S32 IntNum );
CBOOL	NX_UDC_GetInterruptPending( S32 IntNum );
void	NX_UDC_ClearInterruptPending( S32 IntNum );

void	NX_UDC_SetInterruptEnableAll( CBOOL Enable );
CBOOL	NX_UDC_GetInterruptEnableAll( void );
CBOOL	NX_UDC_GetInterruptPendingAll( void );
void	NX_UDC_ClearInterruptPendingAll( void );

void	NX_UDC_SetInterruptEnable32( U32 EnableFlag );
U32		NX_UDC_GetInterruptEnable32( void );
U32		NX_UDC_GetInterruptPending32( void );
void	NX_UDC_ClearInterruptPending32( U32 PendingFlag );

S32		NX_UDC_GetInterruptPendingNumber( void );	// -1 if None
//@}

//------------------------------------------------------------------------------
///	@name	Clock Control Interface
//@{
void		NX_UDC_SetClockPClkMode( NX_PCLKMODE mode );
NX_PCLKMODE	NX_UDC_GetClockPClkMode( void );
void		NX_UDC_SetClockBClkMode( NX_BCLKMODE mode );
NX_BCLKMODE	NX_UDC_GetClockBClkMode( void );
void		NX_UDC_SetClockSource( U32 Index, U32 ClkSrc );
U32			NX_UDC_GetClockSource( U32 Index );
void		NX_UDC_SetClockDivisor( U32 Index, U32 Divisor );
U32			NX_UDC_GetClockDivisor( U32 Index );
void		NX_UDC_SetClockDivisorEnable( CBOOL Enable );
CBOOL		NX_UDC_GetClockDivisorEnable( void );
//@}

//------------------------------------------------------------------------------
///	@name	DMA Interface
//@{
U32		NX_UDC_GetDMAIndex_EP1( void );
U32		NX_UDC_GetDMAIndex_EP2( void );
U32		NX_UDC_GetDMAIndex_EP3( void );
U32		NX_UDC_GetDMABusWidth( void );
//@}

//------------------------------------------------------------------------------
///	@name	Non-Indexed Function
//@{
void	NX_UDC_SetIndexEP( U32 dwIndex );
U32		NX_UDC_GetIndexEP( void );

U32		NX_UDC_GetFunctionAddress( void );

CBOOL	NX_UDC_IsFrameTimerLock( void );
CBOOL	NX_UDC_IsMissingSOF( void );
U32		NX_UDC_GetFrameNumber( void );

void	NX_UDC_SetDirection( U32 dwEndpoint, U32 dwDirection ); // 0: Rx, 1:Tx
U32		NX_UDC_GetDirection( U32 dwEndpoint );

CBOOL	NX_UDC_IsVBUSOn( void );

U32		NX_UDC_GetErrorStatus( void );
void	NX_UDC_ClearErrorStatus( U32 dwErorrStatus);

void	NX_UDC_SetTestMode( CBOOL bEnable, CBOOL bSendPacket, CBOOL bK, CBOOL bJ, CBOOL bHighSpeedReceive );
void	NX_UDC_GetTestMode( CBOOL *pbEnable, CBOOL *pbSendPacket, CBOOL *pbK, CBOOL *pbJ, CBOOL *pbHighSpeedReceive );

CBOOL	NX_UDC_IsDPLineHigh( void );
CBOOL	NX_UDC_IsDMLineHigh( void );
CBOOL	NX_UDC_IsHostSpeedHigh( void );

void	NX_UDC_SetDMATotalCounterZeroInterruptEnable( CBOOL bEnable );
CBOOL	NX_UDC_GetDMATotalCounterZeroInterruptEnable( void );

void	NX_UDC_SetDUALInterruptEnable( CBOOL bEnable );
CBOOL	NX_UDC_GetDUALInterruptEnable( void );

void	NX_UDC_SetFIFOByteOrder( CBOOL bLittleEndian );
CBOOL	NX_UDC_GetFIFOByteOrder( void );

void	NX_UDC_SpeedDetectionControlEnable( CBOOL bEnable );
void	NX_UDC_ResetSpeedDetectionControl( CBOOL bEnable );

void	NX_UDC_SetResumeByMCU( CBOOL bEnable );
CBOOL	NX_UDC_GetResumeByMCU( void );

U32		NX_UDC_GetEP0Status( void );
void	NX_UDC_ClearEP0Status( U32 dwEndpointStatus );

void	NX_UDC_SetEP0PIDToggle( CBOOL bEnable, U32 dwPID );
void	NX_UDC_GetEP0PIDToggle( CBOOL *pbEnable, U32 *pdwPID );

void	NX_UDC_SetEP0Stall( CBOOL bEnable );
CBOOL	NX_UDC_GetEP0Stall( void );

void	NX_UDC_SetEP0TxZeroLength( CBOOL bEnable );
CBOOL	NX_UDC_GetEP0TxZeroLength( void );

void			NX_UDC_SetData( U32 dwEndpoint, U16 wData );
U16				NX_UDC_GetData( U32 dwEndpoint	);
volatile U16*	NX_UDC_GetDataPointer( U32 EPnum );

void			NX_UDC_SetDataEP0( U16 wData );
U16				NX_UDC_GetDataEP0( void );
volatile U16*	NX_UDC_GetDataPointerEP0( void );

void			NX_UDC_SetDataEP1( U16 wData );
U16				NX_UDC_GetDataEP1( void );
volatile U16*	NX_UDC_GetDataPointerEP1( void );

void			NX_UDC_SetDataEP2( U16 wData );
U16				NX_UDC_GetDataEP2( void );
volatile U16*	NX_UDC_GetDataPointerEP2( void );

void			NX_UDC_SetDataEP3( U16 wData );
U16				NX_UDC_GetDataEP3( void );
volatile U16*	NX_UDC_GetDataPointerEP3( void );

void	NX_UDC_SetPHYControl( CBOOL bEnable, CBOOL bPHYReset, CBOOL bPHYPowerdown );
void	NX_UDC_GetPHYControl( CBOOL *pbEnable, CBOOL *pbPHYReset, CBOOL *pbPHYPowerdown );

void	NX_UDC_SetExternalVBUSEnable( CBOOL bEnable );
CBOOL	NX_UDC_GetExternalVBUSEnable( void );

void	NX_UDC_SetPHYPowerUp( void );
void	NX_UDC_SetPHYPowerDown( void );

//@}

//------------------------------------------------------------------------------
///	@name	Indexed Function
//@{
void	NX_UDC_SetFirstOUTPacketInterruptEnable( CBOOL bEnable );
U32		NX_UDC_GetEPStatus( void );
void	NX_UDC_ClearEPStatus( U32 dwEndpointStatus );
U32		NX_UDC_GetPacketNumberInFIFO( void );		// PKT Number

void	NX_UDC_SetInPacketHold( CBOOL bEnable );
CBOOL	NX_UDC_GetInPacketHold( void );

void	NX_UDC_SetOutPacketHold( CBOOL bEnable );
CBOOL	NX_UDC_GetOutPacketHold( void );

void	NX_UDC_SetTransactionNumber( U32 dwTransactionNumber );
U32		NX_UDC_GetTransactionNumber( void );

void	NX_UDC_SetDualFIFOMode( CBOOL bEnable );
CBOOL	NX_UDC_GetDualFIFOMode( void );

void	NX_UDC_SetEPTransferMode( NX_UDC_TRANSFER_MODE mode );
NX_UDC_TRANSFER_MODE	NX_UDC_GetEPTransferMode( void );

void	NX_UDC_FlushFIFO( void );
CBOOL	NX_UDC_IsFIFOFlushed( void );

void	NX_UDC_SetPIDToggle( CBOOL bEnable, U32 dwPID );	// dwToggle : 0, 1, 2(ISO), M(ISO)
void	NX_UDC_GetPIDToggle( CBOOL *pbEnable, U32 *pdwPID );

void	NX_UDC_ClearDataPID( void );
CBOOL	NX_UDC_IsClearedDataPID( void );

void	NX_UDC_SetStall( CBOOL bEnable );
CBOOL	NX_UDC_GetStall( void );

U32		NX_UDC_GetRxPacketSize( void );

void	NX_UDC_SetTxPacketSize( U32 dwSize );
U32		NX_UDC_GetTxPacketSize( void );

void	NX_UDC_SetMaxPacketSize( U32	MaxPacket );
U32		NX_UDC_GetMaxPacketSize( void );

void	NX_UDC_SetAutoRxDMAEnable( CBOOL bEnable );
CBOOL	NX_UDC_GetAutoRxDMAEnable( void );

void	NX_UDC_SetDMAMode( CBOOL bEnable );
CBOOL	NX_UDC_GetDMAMode( void );

void	NX_UDC_SetTxDMARun( CBOOL bEnable );
CBOOL	NX_UDC_GetTxDMARun( void );

void	NX_UDC_SetRxDMARun( CBOOL bEnable );
CBOOL	NX_UDC_GetRxDMARun( void );

void	NX_UDC_SetDMAMaxPacketSize( U32 dwSize);
U32		NX_UDC_GetDMAMaxPacketSize( void );

U32		NX_UDC_GetDMARxPacketSize( void );

void	NX_UDC_SetDMATransferSize( U32	dwTotalSize );
U32		NX_UDC_GetDMATransferSize( void );
//@}


//@}

#ifdef	__cplusplus
}
#endif

#endif
