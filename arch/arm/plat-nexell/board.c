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
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <plat/nx_mcud.h>

static	struct NX_MCUD_RegisterSet *__g_pRegister = CNULL;

/* debug macro */
#define DBGOUT(msg...)		{ printk(KERN_INFO msg); }

/* local */
static void init_gpio_pad(void);
static void init_alive_pad(void);
static void init_bus_pad(void);


/*------------------------------------------------------------------------------
 * board interface
 */
void __init board_init(void)
{
	init_gpio_pad();
	init_alive_pad();
	init_bus_pad();

	__g_pRegister = (struct NX_MCUD_RegisterSet *)IO_ADDRESS(NX_MCUD_GetPhysicalAddress());

#if 0
	/*
	    for EMI control set MEMCFG's DS bit for Weak.

		DDR Memory PAD ODT : (Disable, 50, 75, 150ohm)
		NXP2120 DDR PAD ODT : (50, 75, 150ohm)
		DDR Memory PAD Strength : (Normal, Weak)
		NXP2120 DDR PAD Strength : (300, 150, 100, 75, 60, 50, 42.8, 37.5ohm impedence driver)
	*/
	NX_MCUD_SetDIC( NX_MCUD_DIC_WEAK);		// DDR Memory PAD Strength
	/* Set MEMCFG's RTT bit */
	NX_MCUD_SetODT( NX_MCUDRAM_ODT_DISABLE );	// DDR Memory PAD ODT
	/* Apply Mode Register */
	NX_MCUD_ApplyModeSetting();
	while( NX_MCUD_IsBusyModeSetting() );
	/* Set Phy's ODT */
	NX_MCUD_PHYZQ_SetODT( NX_MCUDPHY_ODT_75 );  // NXP2120 DDR PAD ODT (Default 75)
#endif
	printk ("MEMCFG = %08X MEMTIME0 = %08X MEMTIME1 = %08X PHYTERMCTRL = %08X\n", 
                __g_pRegister->MEMCFG, __g_pRegister->MEMTIME0, __g_pRegister->MEMTIME1, __g_pRegister->PHYTERMCTRL) ;

	DBGOUT("%s : done board initialize ...\n", CFG_SYS_BOARD_NAME);
}

/*------------------------------------------------------------------------------
 * set nexell soc pad func.
 */
static void init_gpio_pad(void)
{
	int  io_grp, io_bit;
	U32  io_mod, outval, detect, pullup, strength;

	const U32 io_pad[NUMBER_OF_GPIO_MODULE][32] = {
		/* GPIO group A */
		{
		PAD_GPIOA0,  PAD_GPIOA1,  PAD_GPIOA2,  PAD_GPIOA3,  PAD_GPIOA4,  PAD_GPIOA5,  PAD_GPIOA6,  PAD_GPIOA7,  PAD_GPIOA8,  PAD_GPIOA9,
		PAD_GPIOA10, PAD_GPIOA11, PAD_GPIOA12, PAD_GPIOA13, PAD_GPIOA14, PAD_GPIOA15, PAD_GPIOA16, PAD_GPIOA17, PAD_GPIOA18, PAD_GPIOA19,
		PAD_GPIOA20, PAD_GPIOA21, PAD_GPIOA22, PAD_GPIOA23, PAD_GPIOA24, PAD_GPIOA25, PAD_GPIOA26, PAD_GPIOA27, PAD_GPIOA28, PAD_GPIOA29,
		PAD_GPIOA30, PAD_GPIOA31
		},
		/* GPIO group B */
		{
		PAD_GPIOB0,  PAD_GPIOB1 , PAD_GPIOB2 , PAD_GPIOB3,  PAD_GPIOB4,  PAD_GPIOB5,  PAD_GPIOB6,  PAD_GPIOB7,  PAD_GPIOB8,  PAD_GPIOB9,
		PAD_GPIOB10, PAD_GPIOB11, PAD_GPIOB12, PAD_GPIOB13, PAD_GPIOB14, PAD_GPIOB15, PAD_GPIOB16, PAD_GPIOB17, PAD_GPIOB18, PAD_GPIOB19,
		PAD_GPIOB20, PAD_GPIOB21, PAD_GPIOB22, PAD_GPIOB23, PAD_GPIOB24, PAD_GPIOB25, PAD_GPIOB26, PAD_GPIOB27, PAD_GPIOB28, PAD_GPIOB29,
		PAD_GPIOB30, PAD_GPIOB31
		},
		/* GPIO group C */
		{
		PAD_GPIOC0,  PAD_GPIOC1,  PAD_GPIOC2,  PAD_GPIOC3,  PAD_GPIOC4,  PAD_GPIOC5,  PAD_GPIOC6,  PAD_GPIOC7,  PAD_GPIOC8,  PAD_GPIOC9,
		PAD_GPIOC10, PAD_GPIOC11, PAD_GPIOC12, PAD_GPIOC13, PAD_GPIOC14, PAD_GPIOC15, PAD_GPIOC16, PAD_GPIOC17, PAD_GPIOC18, PAD_GPIOC19,
		PAD_GPIOC20, 		   0, 		    0,  	     0,           0,           0,           0,           0,           0,           0,
		          0,           0
		}
	};

	/* GPIO pad function */
	for (io_grp = 0; NUMBER_OF_GPIO_MODULE > io_grp; io_grp++) {
		for (io_bit = 0; 32 > io_bit; io_bit++) {

			if (! io_pad[io_grp][io_bit])
				continue;

			io_mod = PAD_GET_PADMODE(io_pad[io_grp][io_bit]);

			switch(io_mod) {
			case PAD_MODE_IN:
				NX_GPIO_SetPadFunction (io_grp, io_bit, NX_GPIO_PADFUNC_GPIO);
				NX_GPIO_SetOutputEnable(io_grp, io_bit, CFALSE);
				break;
			case PAD_MODE_OUT:
				NX_GPIO_SetPadFunction (io_grp, io_bit, NX_GPIO_PADFUNC_GPIO);
				NX_GPIO_SetOutputEnable(io_grp, io_bit, CTRUE);
				break;
			case PAD_MODE_ALT1:
				NX_GPIO_SetPadFunction (io_grp, io_bit, NX_GPIO_PADFUNC_1);
				NX_GPIO_SetOutputEnable(io_grp, io_bit, CFALSE);
				break;
			case PAD_MODE_ALT2:
				NX_GPIO_SetPadFunction (io_grp, io_bit, NX_GPIO_PADFUNC_2);
				NX_GPIO_SetOutputEnable(io_grp, io_bit, CFALSE);
				break;
			case PAD_MODE_ALT3:
				NX_GPIO_SetPadFunction (io_grp, io_bit, NX_GPIO_PADFUNC_3);
				NX_GPIO_SetOutputEnable(io_grp, io_bit, CFALSE);
				break;
			case PAD_MODE_INT:
				detect = PAD_GET_DECTMODE(io_pad[io_grp][io_bit]);
				NX_GPIO_SetPadFunction(io_grp, io_bit, NX_GPIO_PADFUNC_GPIO);
				NX_GPIO_SetOutputEnable(io_grp, io_bit, CFALSE);
				NX_GPIO_SetInterruptMode(io_grp, io_bit, detect);
				break;
			case PAD_MODE_SKIP:
			case PAD_NOTEXIST:
			default:
				continue;
			}
			outval   = PAD_GET_OUTLEVEL(io_pad[io_grp][io_bit]);
			pullup   = PAD_GET_PULLUP(io_pad[io_grp][io_bit]);
			strength = PAD_GET_STRENGTH(io_pad[io_grp][io_bit]);

			NX_GPIO_SetOutputValue (io_grp, io_bit, (outval ? CTRUE : CFALSE));
			NX_GPIO_SetPullUpEnable(io_grp, io_bit, (pullup ? CTRUE : CFALSE));
			NX_CLKPWR_SetGPIOPadStrength(io_grp, io_bit, strength);
		}
	}
}

static void init_alive_pad(void)
{
	int  io_bit;
	U32  io_mod, outval, detect, pullup, io_num;

	const U32 alv_pad[] = {
		PAD_GPIOALV0, PAD_GPIOALV1,  PAD_GPIOALV2, PAD_GPIOALV3
			};

	io_num = sizeof(alv_pad)/sizeof(alv_pad[0]);

	/* Alive pad function */
	for (io_bit = 0; io_num > io_bit; io_bit++) {

		io_mod = PAD_GET_PADMODE(alv_pad[io_bit]);

		switch(io_mod) {
		case PAD_MODE_IN   :
		case PAD_MODE_DECT :
			NX_ALIVE_SetOutputEnable(io_bit, CFALSE);
			break;
		case PAD_MODE_OUT  :
			NX_ALIVE_SetOutputEnable(io_bit, CTRUE);
			break;
		default :
			DBGOUT("\n Unknown GPIO ALIVE Mode(0x%x)\n", io_mod);
			continue;
		}
		outval = PAD_GET_PADMODE(alv_pad[io_bit]);
		pullup = PAD_GET_PULLUP(alv_pad[io_bit]);
		detect = PAD_GET_DECTMODE(alv_pad[io_bit]);

		NX_ALIVE_SetOutputValue (io_bit, (outval ? CTRUE : CFALSE));
		NX_ALIVE_SetPullUpEnable(io_bit, (pullup ? CTRUE : CFALSE));

		// For wakeup source
		NX_ALIVE_SetDetectMode(detect, io_bit, (io_mod == PAD_MODE_DECT ? CTRUE : CFALSE));
		NX_ALIVE_SetDetectEnable(io_bit, (io_mod == PAD_MODE_DECT ? CTRUE : CFALSE));
	}
}

static void init_bus_pad(void)
{
	int  io_bit;
	U32  strength, buspad, busnum;

	const U32 bus_pad[] = {
		PAD_BUS_STATIC_CNTL, PAD_BUS_STATIC_ADDR,  PAD_BUS_STATIC_DATA, PAD_BUS_VSYNC, PAD_BUS_HSYNC, PAD_BUS_DE
			};

	busnum = sizeof(bus_pad)/sizeof(bus_pad[0]);

	/* BUS pad function */
	for (io_bit = 0; busnum > io_bit; io_bit++) {
		buspad   = PAD_GET_BUS(bus_pad[io_bit]);
		strength = PAD_GET_STRENGTH(bus_pad[io_bit]);
		NX_CLKPWR_SetBusPadStrength(buspad, strength);
	}
}

