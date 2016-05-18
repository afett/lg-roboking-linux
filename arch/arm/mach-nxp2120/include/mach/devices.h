/*
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef __DEVICES_H__
#define __DEVICES_H__

#include <linux/platform_device.h>

/* cpu */
#define UART_DEV_NAME 			"nx-uart"
#define	FB_DEV_NAME				"nx-fb"
#define	EHCI_DEV_NAME			"nx-ehci"
#define	OHCI_DEV_NAME			"nx-ohci"
#define	GADGET_DEV_NAME			"nx-gadget"
#define	I2C_DEV_NAME			"nx-i2c"
#define	EHCI_DEV_NAME			"nx-ehci"
#define	OHCI_DEV_NAME			"nx-ohci"
#define	NAND_DEV_NAME			"nx-nand"
#define	KEYPAD_DEV_NAME			"nx-keypad"
#define	TS_DEV_NAME				"nx-ts"			/* Touch */
#define	RTC_DEV_NAME			"nx-rtc"
#define	SDHC_DEV_NAME			"nx-sdhc"
#define	V4L2_DEV_NAME			"nx-v4l2"		/* with vip */

/* graphic */
#define VMEM_DEV_MAJOR			240
#define VMEM_DEV_NAME			"vmem"

#define OGL_DEV_MAJOR			249
#define OGL_DEV_NAME			"ogl"

#define DPC_DEV_MAJOR			241
#define DPC_DEV_NAME			"dpc"

#define VIP_DEV_MAJOR			261
#define	VIP_DEV_NAME			"vip"

#define VIP0_DEV_MINOR			0
#define	VIP0_DEV_NAME			"vip0"

#define VIP1_DEV_MINOR			1
#define	VIP1_DEV_NAME			"vip1"


/* board */
#define DM9000_DEV_NAME 		"dm9000"

/* misc */
#define PWM_DEV_MAJOR			243
#define	PWM_DEV_NAME			"pwm"

#define GPIO_DEV_MAJOR			244
#define GPIO_DEV_NAME			"gpio"

#define ADC_DEV_MAJOR			250
#define	ADC_DEV_NAME			"adc"

#define SPI_DEV_MAJOR			252
#define SPI_DEV_NAME			"spi"

#define PMI_DEV_MAJOR			253
#define PMI_DEV_NAME			"pmi"

/*--------------------------------------------------------------------
 *	Frame buffer platform data
 */
struct fb_plat_data {
	int 	module;		/* 0: primary, 1: secondary */
	int 	layer;		/* RGB 0, 1, 2 */
	int		x_res;		/* x resolution */
	int		y_res;		/* y resolution */
	int		pixelbit;	/* bit per pixel */
	u_int	format;		/* RGB format */
	int		buffers;	/* set 2 when support pan, default 1 */
};

/*--------------------------------------------------------------------
 *	I2C platform data
 */
struct i2c_plat_data {
	int 	port;
	int 	irq;
	u_int	io_scl;		/* scl pad */
	u_int	io_sda;		/* scl pad */
	u_int	clksrc;		/* GPIO: 0, I2C hw: 16 or 256 */
	u_int	clkscale;
	u_int	cntmax;		/* Qyab\rter period count max register */
	int		polling;	/* wait ack method */
};

/*--------------------------------------------------------------------
 *	SDHC platform data
 */
struct sdhc_plat_data {
	int io_detect;
	int io_wprotect;
};

/*--------------------------------------------------------------------
 *	Touch platform data
 */
struct adc_ts_plat_data {
	struct {
		int		p_x;		/* touch x+ */
		int		m_x;		/* touch x- */
		int		p_y;		/* touch y+ */
		int		m_y;		/* touch y- */
		int		detect;		/* detect pen down */
		int		control;	/* detect pen control */
	} io;
	struct {
		int		x_ch;		/* adc x channel */
		int		y_ch;		/* adc y channel */
		int 	x_min;
		int 	x_max;
		int 	y_min;
		int 	y_max;
		int		pointercal[7]; /* calibration value (tslib) */
	} adc;
};

/*--------------------------------------------------------------------
 *	Keypad platform data
 */
struct key_plat_data {
	int				bt_count;
	unsigned int  *	bt_io;
	unsigned int  *	bt_code;
};

/*--------------------------------------------------------------------
 *	v4l2 platfrom data
 */
struct nx_platform_vip {
	int		in_port;		/* vip input port 0, 1 */
	int		dpc_device;		/* display out module */
	int		hor_align;
	int		ver_align;
	int		buff_count;		/* alloc buffer count */
	int		skip_count;		/* first frame skip count */
	
	int    (*gpio_reset)(int reset);
	int    (*cfg_gpio)(void);
};

struct poa030_i2c_platform_data {
    int  id;  
    char *i2c_name;
    char *device_name;
};

extern void *boot_alloc_mem;

#endif	/* __DEVICES_H__ */

