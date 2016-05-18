/*
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
#ifndef __ASM_ARCH_SYSTEM_H
#define __ASM_ARCH_SYSTEM_H

#include <mach/platform.h>
#include <mach/devices.h>

/* Register bit definition */
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

#define NXP2120_UCON_TX_INT_LEVEL       (1<<9)
#define NXP2120_UCON_RX_INT_LEVEL       (1<<8)
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

#define NXP2120_INTCON_MENB            (1<<7)
#define NXP2120_INTCON_ERRENB          (1<<6)
#define NXP2120_INTCON_RXENB           (1<<5)
#define NXP2120_INTCON_TXENB           (1<<4)
#define NXP2120_INTCON_MPEND           (1<<3)
#define NXP2120_INTCON_ERRPEND         (1<<2)
#define NXP2120_INTCON_RXPEND          (1<<1)
#define NXP2120_INTCON_TXPEND          (1<<0)

#define NXP2120_UART_CLKENB_PCLKMODE   (1<<3)
#define NXP2120_UART_CLKENB_CLKGENB    (1<<2)
#define NXP2120_UART_CLKENB_RESERVED   (3<<0)

#define NXP2120_UART_CLKGEN_SRC_SEL_MASK (0x07 << 2)
#define NXP2120_UART_CLKGEN_SRC_PLL0   (0x0 << 2)
#define NXP2120_UART_CLKGEN_SRC_PLL1   (0x1 << 2)

#define	UART_WAIT_READY		{ volatile int cnt; for (cnt=0; cnt <0xffff; cnt++); }

static inline void arch_idle(void)
{
	/*
	 * This should do all the clock switching
	 * and wait for interrupt tricks
	 */
	cpu_do_idle();
}

static inline void setup_uart_port1(void)
{
    struct NX_UART_RegisterSet *uart;
	unsigned short reg;

    uart = (struct NX_UART_RegisterSet *)IO_ADDRESS(PHY_BASEADDR_UART1);

	/* Clock & Interrupt Disable */
	uart->CLKENB = uart->CLKENB & ~NXP2120_UART_CLKENB_CLKGENB;
	uart->INTCON = /*uart->INTCON |*/ 0x0F; /* All Four Pending Bits Clear, and disable IRQ all */

	/* UARTCLKGEN, UARTCLKENB */
	uart->CLKGEN = NXP2120_UART_CLKGEN_SRC_PLL1 | ((CFG_UART_DEBUG_CLKDIV-1) << 5);

	// Tx Rx Operation : Default Polling
	uart->UCON = NXP2120_UCON_RX_INT_LEVEL | NXP2120_UCON_TRANS_IRQ_POLLING |
	             NXP2120_UCON_RECV_IRQ_POLLING | NXP2120_UCON_RX_TIMEOUT;
	uart->MCON = NXP2120_MCON_HALF_CH_ENB | NXP2120_MCON_SCRXENB;

	reg = uart->LCON & ~0x3F;
	// Frame Configuration : Data 8 - Parity 0 - Stop 1 for default.
	uart->LCON = reg | NXP2120_LCON_PARITY_NONE | NXP2120_LCON_ONE_STOP_BIT | NXP2120_LCON_WORDLEN_8BIT;

	/* BRDn: baud rate divisor */
	uart->BRD = 1 - 1;

	/* FCON: fifo control */
	uart->FCON = uart->FCON | NXP2120_FCON_TX_FIFO_RESET | NXP2120_FCON_RX_FIFO_RESET;
    uart->FCON = uart->FCON | NXP2120_FCON_FIFO_ENABLE;

	/* TIMEOUTREG: receive timeout */
	uart->TIMEOUT = 0x01F; /* 0x1F is default */

	/* INTSTATUSREG: Interrupt status */
	uart->INTCON = 0;
    uart->INTCON = uart->INTCON |  0x0F;
	uart->LCON = uart->LCON | NXP2120_LCON_SYNC_PENDCLR;

	uart->CLKENB = NXP2120_UART_CLKENB_PCLKMODE | NXP2120_UART_CLKENB_CLKGENB;

	UART_WAIT_READY;
}

static inline void uart1_set_termios(void)
{
    struct NX_UART_RegisterSet *uart;
	unsigned short reg;

    uart = (struct NX_UART_RegisterSet *)IO_ADDRESS(PHY_BASEADDR_UART1);

	// set nexell uart register
	uart->CLKENB =uart->CLKENB & ~NXP2120_UART_CLKENB_CLKGENB;
	uart->INTCON = 0x0F;

	reg = uart->LCON & ~0x3F;
	uart->LCON  = reg | NXP2120_LCON_PARITY_NONE | NXP2120_LCON_ONE_STOP_BIT | NXP2120_LCON_WORDLEN_8BIT;

	uart->BRD = 1 - 1;
    uart->FCON = uart->FCON | NXP2120_FCON_TX_FIFO_RESET | NXP2120_FCON_RX_FIFO_RESET;
    uart->FCON = uart->FCON | NXP2120_FCON_FIFO_ENABLE;

	// Enable RX Interrupt
	uart->INTCON = 0;
	uart->INTCON = uart->INTCON | 0x0F; /* Clear Interrupt Pending all */
	uart->LCON = uart->LCON | NXP2120_LCON_SYNC_PENDCLR;

	uart->CLKENB = NXP2120_UART_CLKENB_PCLKMODE | NXP2120_UART_CLKENB_CLKGENB;

	UART_WAIT_READY;
}

static inline void arch_halt()
{
    struct NX_UART_RegisterSet *uart;
	char	notify[] = "GOODBYE";
	int		i = 0;
	int		cnt = 0;
	unsigned int reg;

	setup_uart_port1();
	uart1_set_termios();

	uart = (struct NX_UART_RegisterSet *)IO_ADDRESS(PHY_BASEADDR_UART1);

    for (cnt = 0 ; cnt < 10 ; cnt++)
    {
    	i = 0;
	    while (0 == (uart->FSTATUS & NXP2120_FSTATUS_TX_FIFO_FULL))
	    {
			uart->THB = notify[i++];
			if (notify[i] == 0)
				break;
	    }
	    msleep(100);
	}
}

static inline void arch_reset(char mode, const char *cmd)
{
    arch_halt();

	printk(KERN_INFO "system reset: %s ...\n", cmd);

	NX_CLKPWR_SetSoftwareResetEnable(CTRUE);
	NX_CLKPWR_DoSoftwareReset();
}

#endif
