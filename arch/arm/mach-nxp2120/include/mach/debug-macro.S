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

#include <mach/platform.h>

		.macro	addruart,rx
		mrc	p15, 0, \rx, c1, c0
		tst	\rx, #1							@ MMU enabled?
		ldreq	\rx, =PB_UART_PHYS_BASE		@ physical base
		ldrne	\rx, =PB_UART_VIRT_BASE		@ virtual base
		.endm

		.macro  senduart,rd,rx
		str	\rd, [\rx, #PB_UART_THB]
		.endm

		.macro  busyuart,rd,rx
		.endm

		.macro  waituart,rd,rx
1001:
		ldr	\rd, [\rx, #PB_UART_TRSTATUS]
		tst	\rd, #PB_UART_FIFO_EMPTY
		beq	1001b
		.endm


ENTRY(printtest)
		ldr	r1, =PB_UART_PHYS_BASE
		ldr	r0, =PB_UART_VIRT_BASE
ENDPROC(printtest)
