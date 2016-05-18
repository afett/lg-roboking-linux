/*
 * Copyright(c) 2011 STCube inc.,
 * All right reserved by Seungwoo Kim <ksw@stcube.com>
 *
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * This does not append a newline
 */

#define CONFIG_SERIAL1
#define CONFIG_BAUDRATE 115200
#define CONFIG_SERIAL_USE_FIFO

#define PHY_BASEADDR_UART0			(0xC0016000)
#define PHY_BASEADDR_UART1			(0xC0016080)

#ifdef CONFIG_SERIAL1
#define PHYS_BASE (PHY_BASEADDR_UART0)
#elif defined(CONFIG_SERIAL2)
#define PHYS_BASE (PHY_BASEADDR_UART1)
#else
#error "Bad: you didn't configure serial ..."
#endif

#define NXP2120_LCON_SYNC_PENDCLR       (1<<7)
#define NXP2120_LCON_SIR_MODE           (1<<6)
#define NXP2120_LCON_PARITY_MASK        (7<<3)
#define NXP2120_LCON_PARITY_ODD         (4<<3)
#define NXP2120_LCON_PARITY_EVEN        (5<<3)
#define NXP2120_LCON_PARITY_NONE        (0<<3)
#define NXP2120_LCON_ONE_STOP_BIT       (0<<2)
#define NXP2120_LCON_TWO_STOP_BIT       (1<<2)
#define NXP2120_LCON_WORDLEN_MASK       (3<<0)
#define NXP2120_LCON_WORDLEN_5BIT       (0<<0)
#define NXP2120_LCON_WORDLEN_6BIT       (1<<0)
#define NXP2120_LCON_WORDLEN_7BIT       (2<<0)
#define NXP2120_LCON_WORDLEN_8BIT       (3<<0)

#define NXP2120_UCON_TX_INT             (1<<9)
#define NXP2120_UCON_RX_INT             (1<<8)
#define NXP2120_UCON_RX_TIMEOUT         (1<<7)
#define NXP2120_UCON_RX_ERRSTATUS       (1<<6)
#define NXP2120_UCON_LOOPBACK_MODE      (1<<5)
#define NXP2120_UCON_SEND_BREAK         (1<<4)
#define NXP2120_UCON_TRANS_MODE_MASK    (3<<2)
#define NXP2120_UCON_TRANS_DISABLE      (0<<2)
#define NXP2120_UCON_TRANS_IRQ_POLLING  (1<<2)
#define NXP2120_UCON_TRANS_DMA_REQ      (2<<2)
#define NXP2120_UCON_RECV_MODE_MASK     (3<<0)
#define NXP2120_UCON_RECV_DISABLE       (0<<0)
#define NXP2120_UCON_RECV_IRQ_POLLING   (1<<0)
#define NXP2120_UCON_RECV_DMA_REQ       (2<<0)

#define NXP2120_FCON_TX_FIFO_TRIG_MASK  (3<<6)
#define NXP2120_FCON_TX_FIFO_TRIG_0BYTE (0<<6)
#define NXP2120_FCON_TX_FIFO_TRIG_4BYTE (1<<6)
#define NXP2120_FCON_TX_FIFO_TRIG_8BYTE (2<<6)
#define NXP2120_FCON_TX_FIFO_TRIG_12BYTE (3<<6)
#define NXP2120_FCON_RX_FIFO_TRIG_MASK  (3<<4)
#define NXP2120_FCON_RX_FIFO_TRIG_0BYTE (0<<4)
#define NXP2120_FCON_RX_FIFO_TRIG_4BYTE (1<<4)
#define NXP2120_FCON_RX_FIFO_TRIG_8BYTE (2<<4)
#define NXP2120_FCON_RX_FIFO_TRIG_12BYTE (3<<4)
#define NXP2120_FCON_TX_FIFO_RESET      (1<<2)
#define NXP2120_FCON_RX_FIFO_RESET      (1<<1)
#define NXP2120_FCON_FIFO_ENABLE        (1<<0)

#define NXP2120_MCON_HALF_CH_ENB        (1<<7)
#define NXP2120_MCON_SCRXENB            (1<<6)
#define NXP2120_MCON_SCTXENB            (1<<5)
#define NXP2120_MCON_AFC                (1<<4)
#define NXP2120_MCON_DTR_ACTIVE         (1<<1)
#define NXP2120_MCON_RTS_ACTIVE         (1<<0)

#define NXP2120_TRSTATUS_TX_EMPTY       (1<<2)
#define NXP2120_TRSTATUS_TX_BUF_EMPTY   (1<<1)
#define NXP2120_TRSTATUS_RX_BUF_READY   (1<<0)

#define NXP2120_ESTATUS_BREAK_DETECT   (1<<3)
#define NXP2120_ESTATUS_FRAME_ERROR    (1<<2)
#define NXP2120_ESTATUS_PARITY_ERROR   (1<<1)
#define NXP2120_ESTATUS_OVERRUN_ERROR  (1<<0)

#define NXP2120_FSTATUS_RX_FIFO_ERROR  (1<<10)
#define NXP2120_FSTATUS_TX_FIFO_FULL   (1<<9)
#define NXP2120_FSTATUS_RX_FIFO_FULL   (1<<8)
#define NXP2120_FSTATUS_TX_FIFO_COUNT  (0xF<<4)
#define NXP2120_FSTATUS_RX_FIFO_COUNT  (0xF<<0)

#define NXP2120_MSTATUS_DELTA_DCD      (1<<7)
#define NXP2120_MSTATUS_DELTA_RI       (1<<6)
#define NXP2120_MSTATUS_DELTA_DSR      (1<<5)
#define NXP2120_MSTATUS_DELTA_CTS      (1<<4)
#define NXP2120_MSTATUS_DCD            (1<<3)
#define NXP2120_MSTATUS_RI             (1<<2)
#define NXP2120_MSTATUS_DSR            (1<<1)
#define NXP2120_MSTATUS_CTS            (1<<0)

#define NXP2120_UART_CLKENB_PCLKMODE   (1<<3)
#define NXP2120_UART_CLKENB_CLKGENB    (1<<2)
#define NXP2120_UART_CLKENB_RESERVED   (3<<0)

#define NXP2120_UART_CLKGEN_SRC_SEL_MASK (0x07 << 2)
#define NXP2120_UART_CLKGEN_SRC_PLL0   (0x0 << 2)
#define NXP2120_UART_CLKGEN_SRC_PLL1   (0x1 << 2)

typedef unsigned short int U16;
typedef unsigned int U32;

/* for UART */
struct	NX_UART_RegisterSet	{
		volatile U16	LCON;			///< 0x00 : Line Control Register
		volatile U16	UCON;			///< 0x02 : Control Register
		volatile U16	FCON;			///< 0x04 : FIFO Control Register
		volatile U16	MCON;			///< 0x06 : Modem Control Register
		volatile U16	TRSTATUS;		///< 0x08 : Tx/Rx Status Register
		volatile U16	ESTATUS;		///< 0x0a : Error Status Register
		volatile U16	FSTATUS;		///< 0x0c : FIFO Status Register
		volatile U16	MSTATUS;		///< 0x0e : Modem Status Register
		volatile U16	THB;			///< 0x10 : Transmit Buffer Register
		volatile U16	RHB;			///< 0x12 : Receive Buffer Register
		volatile U16	BRD;			///< 0x14 : Baud Rate Divisor Register
		volatile U16	TIMEOUT;		///< 0x16 : Receive TimeOut Register
		volatile U16	INTCON;			///< 0x18 : Interrupt Control Register
		volatile U16	__Reserved[0x13];///< 0x1A ~ 0x3E : Reserved Region
		volatile U32	CLKENB;			///< 0x40 : Clock Enable Register
		volatile U32	CLKGEN;			///< 0x44 : Clock Generate Register
};

static inline void putc(int c)
{
    struct NX_UART_RegisterSet *uart = (struct NX_UART_RegisterSet*)PHYS_BASE;

#ifdef CONFIG_SERIAL_USE_FIFO
    while ((uart->FSTATUS & NXP2120_FSTATUS_TX_FIFO_FULL)) ;
#else
	/* wait for room in the tx FIFO */
	while (!(uart->TRSTATUS & NXP2120_TRSTATUS_TX_BUF_EMPTY)) ;
#endif

#ifdef CONFIG_HWFLOW
	/* Wait for CTS up */
	while (hwflow && !(uart->MSTATUS & 0x1));
#endif

	uart->THB = c;
}

static inline void flush(void)
{
    /* Do nothing that we don't have Software Flush. */
}

static void arch_decomp_setup(void) {
    int baudrate = CONFIG_BAUDRATE;
	int src_clk,div,mod;
    struct NX_UART_RegisterSet *uart = (struct NX_UART_RegisterSet*)PHYS_BASE;

    /* set LCON to 8bit, 1stop bit No parity configuration */
    uart->LCON = NXP2120_LCON_PARITY_NONE | NXP2120_LCON_ONE_STOP_BIT | NXP2120_LCON_WORDLEN_8BIT;
    /* Set UCON to int/polling mode RX/TX */
    uart->UCON = NXP2120_UCON_TRANS_IRQ_POLLING | NXP2120_UCON_RECV_IRQ_POLLING;
	/* determin using FIFO or not. */
#ifdef CONFIG_SERIAL_USE_FIFO
    uart->FCON |= NXP2120_FCON_FIFO_ENABLE | NXP2120_FCON_TX_FIFO_RESET | NXP2120_FCON_RX_FIFO_RESET;
#else
    uart->FCON &= ~NXP2120_FCON_FIFO_ENABLE;
#endif
    /* set Clock Enable Register */
    uart->CLKENB |= NXP2120_UART_CLKENB_PCLKMODE | NXP2120_UART_CLKENB_CLKGENB;
    /* set baudrate, by base register set close to 7.3728MHz */
    src_clk = 192000000; /* PLL1 192MHz... Must be setup with **not** constant. */
    div = src_clk / 7372800;
    mod = src_clk % 7372800;
    if (mod < 368640) { /* Then it can be usable as below +5% value... */
        div -= 1; /* As set value, it would be 1 less integer */
    } else
    if (mod > 7004160) { /* Then it can be usable as above -5% value...*/
        div = div + 1 - 1;
    } else {
        /* It would be some problem that Serial Port would not work properly. */
    }
    uart->CLKGEN = (div << 5) | NXP2120_UART_CLKGEN_SRC_PLL1;
    /* Now baudrate setting */
    uart->BRD = (7372800 / (baudrate * 16)) - 1;
}

#define arch_decomp_wdog()
