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
#ifndef __SPI_H__
#define __SPI_H__

/*------------------------------------------------------------------------------
 * 	data
 */
struct spi_info{
	unsigned int 	channel;	/* 0 ~ 2  spica has 3 spi channels */
	unsigned char *	inbuff;
	unsigned char *	outbuff;
	unsigned int 	size;
};

struct spi_clock {
	unsigned int 	channel;
	unsigned int 	min;		/* read min range */
	unsigned int 	max;		/* read max range */
	unsigned int 	clock;		/* r/w current clock */
};

/*------------------------------------------------------------------------------
 * 	ioctl
 */
#include "ioc_magic.h"

enum {
	IOCTL_SPI_READ		= _IO(IOC_NX_MAGIC, 1),
	IOCTL_SPI_WRITE		= _IO(IOC_NX_MAGIC, 2),
	IOCTL_SPI_GET_CLK	= _IO(IOC_NX_MAGIC, 3),
	IOCTL_SPI_SET_CLK	= _IO(IOC_NX_MAGIC, 4),
};

#endif // __SPI_H__
