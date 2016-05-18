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
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/delay.h>

/* nexell soc headers */
#include <mach/platform.h>

#if (0)
#define DBGOUT(msg...)		{ printk("pio: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

/*-----------------------------------------------------------------------------*/
#define	ALIVE_INDEX		NUMBER_OF_GPIO_MODULE
static spinlock_t 		lock[ALIVE_INDEX + 1];	/* A, B, C, D, E, alive */

#define	_LOCK(id)		spin_lock(&lock[id])
#define	_UNLOCK(id)		spin_unlock(&lock[id])

/*-----------------------------------------------------------------------------*/
void soc_gpio_init(void)
{
	int i = 0;
	DBGOUT("%s\n", __func__);

	for (; (ALIVE_INDEX + 1) > i; i++)
		spin_lock_init(&lock[i]);
}

/*------------------------------------------------------------------------------
 * 	Description	: set gpio pad function
 *	In[io]		: gpio pad number, 32*n + bit
 * 				: (n= GPIO_A:0, GPIO_B:1, GPIO_C:2, GPIO_D:3, GPIO_E:4, bit= 0 ~ 32)
 *	In[mode]	: gpio pad function
 *				: 0 = GPIO mode
 *				: 1 = Alternate function 1
 *				: 2 = Alternate function 2
 *				: 3 = Alternate function 3
 *	Return 		: none.
 */
void soc_gpio_set_io_func(u_int io, u_int func)
{
	u_int grp = PAD_GET_GRP(io);
	u_int bit = PAD_GET_BIT(io);

	DBGOUT("%s (%d, %d)\n", __func__, grp, bit);

	switch (io & ~(32-1)) {
	case PAD_GPIO_A:
	case PAD_GPIO_B:
	case PAD_GPIO_C:
//	case PAD_GPIO_D:
//	case PAD_GPIO_E:
		_LOCK(grp);
		NX_GPIO_SetPadFunction(grp, bit, func);
		_UNLOCK(grp);
		break;

	default:
		printk("fail, soc gpio io:%d, group:%d (%s)\n", io, grp, __func__);
		break;
	};
}
EXPORT_SYMBOL(soc_gpio_set_io_func);

/*------------------------------------------------------------------------------
 * 	Description	: get gpio pad function
 *	In[io]		: gpio pad number, 32*n + bit
 * 				: (n= GPIO_A:0, GPIO_B:1, GPIO_C:2, GPIO_D:3, GPIO_E:4, bit= 0 ~ 32)
 *	Return		: -1 = invalid gpio
 *				:  0 = GPIO mode
 *				:  1 = Alternate function 1
 *				:  2 = Alternate function 2
 *				:  3 = Alternate function 3
 */
u_int soc_gpio_get_io_func(u_int io)
{
	u_int grp = PAD_GET_GRP(io);
	u_int bit = PAD_GET_BIT(io);
	u_int fun = -1;

	DBGOUT("%s (%d, %d)\n", __func__, grp, bit);

	switch (io & ~(32-1)) {
	case PAD_GPIO_A:
	case PAD_GPIO_B:
	case PAD_GPIO_C:
//	case PAD_GPIO_D:
//	case PAD_GPIO_E:
		_LOCK(grp);
		fun = NX_GPIO_GetPadFunction(grp, bit);
		_UNLOCK(grp);
		break;

	default:
		printk("fail, soc gpio io:%d, group:%d (%s)\n", io, grp, __func__);
		break;
	};

	return fun;
}
EXPORT_SYMBOL(soc_gpio_get_io_func);

/*------------------------------------------------------------------------------
 * 	Description	: set gpio io direction
 *	In[io]		: gpio pad number, 32*n + bit
 * 				: (n= GPIO_A:0, GPIO_B:1, GPIO_C:2, GPIO_D:3, GPIO_E:4, ALIVE:5, bit= 0 ~ 32)
 *	In[out]		: '1' is output mode, '0' is input mode
 *	Return 		: none.
 */
void soc_gpio_set_io_dir(u_int io, int out)
{
	u_int grp = PAD_GET_GRP(io);
	u_int bit = PAD_GET_BIT(io);

	DBGOUT("%s (%d, %d)\n", __func__, grp, bit);

	switch (io & ~(32-1)) {
	case PAD_GPIO_A:
	case PAD_GPIO_B:
	case PAD_GPIO_C:
//	case PAD_GPIO_D:
//	case PAD_GPIO_E:
		_LOCK(grp);
		NX_GPIO_SetOutputEnable(grp, bit, out ? CTRUE : CFALSE);
		_UNLOCK(grp);
		break;

	case PAD_GPIO_ALV:
		_LOCK(grp);
		NX_ALIVE_SetOutputEnable(bit, out ? CTRUE : CFALSE);
		_UNLOCK(grp);
		break;

	default:
		printk("fail, soc gpio io:%d, group:%d (%s)\n", io, grp, __func__);
		break;
	};
}
EXPORT_SYMBOL(soc_gpio_set_io_dir);

/*------------------------------------------------------------------------------
 * 	Description	: get gpio io direction
 *	In[io]		: gpio pad number, 32*n + bit
 * 				: (n= GPIO_A:0, GPIO_B:1, GPIO_C:2, GPIO_D:3, GPIO_E:4, ALIVE:5, bit= 0 ~ 32)
 *	Return 		: -1 = invalid gpio.
 *		 		:  0 = gpio's input mode.
 *				:  1 = gpio's output mode.
 */
int soc_gpio_get_io_dir(u_int io)
{
	u_int grp = PAD_GET_GRP(io);
	u_int bit = PAD_GET_BIT(io);
	int	  dir = -1;

	DBGOUT("%s (%d, %d)\n", __func__, grp, bit);

	switch (io & ~(32-1)) {
	case PAD_GPIO_A:
	case PAD_GPIO_B:
	case PAD_GPIO_C:
//	case PAD_GPIO_D:
//	case PAD_GPIO_E:
		_LOCK(grp);
		dir = NX_GPIO_GetOutputEnable(grp, bit) ? 1 : 0;
		_UNLOCK(grp);
		break;

	case PAD_GPIO_ALV:
		_LOCK(grp);
		dir = NX_ALIVE_GetOutputEnable(bit) ? 1 : 0;
		_UNLOCK(grp);
		break;

	default:
		printk("fail, soc gpio io:%d, group:%d (%s)\n", io, grp, __func__);
		break;
	};

	return dir;
}
EXPORT_SYMBOL(soc_gpio_get_io_dir);

/*------------------------------------------------------------------------------
 * 	Description	: set pull up of gpio pin
 *	In[io]		: gpio pad number, 32*n + bit
 * 				: (n= GPIO_A:0, GPIO_B:1, GPIO_C:2, GPIO_D:3, GPIO_E:4, ALIVE:5, bit= 0 ~ 32)
 *	In[up]		: '1' is pull up, '0' is not pull up(floting)
 *	Return 		: none.
 */
void soc_gpio_set_io_pullup(u_int io, int up)
{
	u_int grp = PAD_GET_GRP(io);
	u_int bit = PAD_GET_BIT(io);

	DBGOUT("%s (%d, %d)\n", __func__, grp, bit);

	switch (io & ~(32-1)) {
	case PAD_GPIO_A:
	case PAD_GPIO_B:
	case PAD_GPIO_C:
//	case PAD_GPIO_D:
//	case PAD_GPIO_E:
		_LOCK(grp);
		NX_GPIO_SetPullUpEnable(grp, bit, up ? CTRUE : CFALSE);
		_UNLOCK(grp);
		break;

	case PAD_GPIO_ALV:
		_LOCK(grp);
		NX_ALIVE_SetPullUpEnable(bit, up ? CTRUE : CFALSE);
		_UNLOCK(grp);
		break;

	default:
		printk("fail, soc gpio io:%d, group:%d (%s)\n", io, grp, __func__);
		break;
	};
}
EXPORT_SYMBOL(soc_gpio_set_io_pullup);

/*------------------------------------------------------------------------------
 * 	Description	: get pull up status
 *	In[io]		: gpio pad number, 32*n + bit
 * 				: (n= GPIO_A:0, GPIO_B:1, GPIO_C:2, GPIO_D:3, GPIO_E:4, ALIVE:5, bit= 0 ~ 32)
 *	Return 		: -1 = invalid gpio.
 *		 		:  0 = pullup off.
 *				:  1 = pullup on.
 */
int soc_gpio_get_io_pullup(u_int io)
{
	u_int grp = PAD_GET_GRP(io);
	u_int bit = PAD_GET_BIT(io);
	int   up  = -1;

	DBGOUT("%s (%d, %d)\n", __func__, grp, bit);

	switch (io & ~(32-1)) {
	case PAD_GPIO_A:
	case PAD_GPIO_B:
	case PAD_GPIO_C:
//	case PAD_GPIO_D:
//	case PAD_GPIO_E:
		_LOCK(grp);
		up = NX_GPIO_GetPullUpEnable(grp, bit) ? 1 : 0;
		_UNLOCK(grp);
		break;

	case PAD_GPIO_ALV:
		_LOCK(grp);
		up = NX_ALIVE_GetPullUpEnable(bit) ? 1 : 0;
		_UNLOCK(grp);
		break;

	default:
		printk("fail, soc gpio io:%d, group:%d (%s)\n", io, grp, __func__);
		break;
	};

	return up;
}
EXPORT_SYMBOL(soc_gpio_get_io_pullup);

/*------------------------------------------------------------------------------
 * 	Description	: set gpio output level
 *	In[io]		: gpio pad number, 32*n + bit
 * 				: (n= GPIO_A:0, GPIO_B:1, GPIO_C:2, GPIO_D:3, GPIO_E:4, ALIVE:5, bit= 0 ~ 32)
 *	In[high]	: '1' is high level, '0' is low level
 *	Return 		: none.
 */
void soc_gpio_set_out_value(u_int io, int high)
{
	u_int grp = PAD_GET_GRP(io);
	u_int bit = PAD_GET_BIT(io);

	DBGOUT("%s (%d, %d)\n", __func__, grp, bit);

	switch (io & ~(32-1)) {
	case PAD_GPIO_A:
	case PAD_GPIO_B:
	case PAD_GPIO_C:
//	case PAD_GPIO_D:
//	case PAD_GPIO_E:
		_LOCK(grp);
		NX_GPIO_SetOutputValue(grp, bit, high ? CTRUE : CFALSE);
		_UNLOCK(grp);
		break;

	case PAD_GPIO_ALV:
		_LOCK(grp);
		NX_ALIVE_SetOutputValue(bit, high ? CTRUE : CFALSE);
		_UNLOCK(grp);
		break;

	default:
		printk("fail, soc gpio io:%d, group:%d (%s)\n", io, grp, __func__);
		break;
	};
}
EXPORT_SYMBOL(soc_gpio_set_out_value);

/*------------------------------------------------------------------------------
 * 	Description	: get gpio output level
 *	In[io]		: gpio pad number, 32*n + bit
 * 				: (n= GPIO_A:0, GPIO_B:1, GPIO_C:2, GPIO_D:3, GPIO_E:4, ALIVE:5, bit= 0 ~ 32)
 *	Return 		: -1 = invalid gpio.
 *		 		:  0 = gpio's output value is low level.
 *				:  1 = gpio's output value is high level.
 */
int soc_gpio_get_out_value(u_int io)
{
	u_int grp = PAD_GET_GRP(io);
	u_int bit = PAD_GET_BIT(io);
	int   val = -1;

	DBGOUT("%s (%d, %d)\n", __func__, grp, bit);

	switch (io & ~(32-1)) {
	case PAD_GPIO_A:
	case PAD_GPIO_B:
	case PAD_GPIO_C:
//	case PAD_GPIO_D:
//	case PAD_GPIO_E:
		_LOCK(grp);
		val = NX_GPIO_GetOutputValue(grp, bit) ? 1 : 0;
		_UNLOCK(grp);
		break;

	case PAD_GPIO_ALV:
		_LOCK(grp);
		val = NX_ALIVE_GetOutputValue(bit) ? 1 : 0;
		_UNLOCK(grp);
		break;

	default:
		printk("fail, soc gpio io:%d, group:%d (%s)\n", io, grp, __func__);
		break;
	};

	return val;
}
EXPORT_SYMBOL(soc_gpio_get_out_value);

/*------------------------------------------------------------------------------
 * 	Description	: get gpio input value
 *	In[io]		: gpio pad number, 32*n + bit
 * 				: (n= GPIO_A:0, GPIO_B:1, GPIO_C:2, GPIO_D:3, GPIO_E:4, ALIVE:5, bit= 0 ~ 32)
 *	Return 		: -1 = invalid gpio.
 *		 		:  0 = gpio's input value is low level , alive input is low.
 *				:  1 = gpio's input value is high level, alive input is high.
 */
int soc_gpio_get_in_value(u_int io)
{
	u_int grp = PAD_GET_GRP(io);
	u_int bit = PAD_GET_BIT(io);
	int   val = -1;

	DBGOUT("%s (%d, %d)\n", __func__, grp, bit);

	switch (io & ~(32-1)) {
	case PAD_GPIO_A:
	case PAD_GPIO_B:
	case PAD_GPIO_C:
//	case PAD_GPIO_D:
//	case PAD_GPIO_E:
		_LOCK(grp);
		val = NX_GPIO_GetInputValue(grp, bit) ? 1 : 0;
		_UNLOCK(grp);
		break;

	case PAD_GPIO_ALV:
		_LOCK(grp);
		val = NX_ALIVE_GetInterruptPending(bit) ? 1 : 0;
		_UNLOCK(grp);
		break;

	default:
		printk("fail, soc gpio io:%d, group:%d (%s)\n", io, grp, __func__);
		break;
	};

	return val;
}
EXPORT_SYMBOL(soc_gpio_get_in_value);

/*------------------------------------------------------------------------------
 * 	Description	: enable gpio interrupt
 *	In[io]		: gpio pad number, 32*n + bit
 * 				: (n= GPIO_A:0, GPIO_B:1, GPIO_C:2, GPIO_D:3, GPIO_E:4, ALIVE:5, bit= 0 ~ 32)
 *	In[on]		: gpio interrupt enable or disable
 *	Return 		: none.
 */
void soc_gpio_set_int_enable(u_int io, int on)
{
	u_int grp = PAD_GET_GRP(io);
	u_int bit = PAD_GET_BIT(io);

	DBGOUT("%s (%d, %d)\n", __func__, grp, bit);

	switch (io & ~(32-1)) {
	case PAD_GPIO_A:
	case PAD_GPIO_B:
	case PAD_GPIO_C:
//	case PAD_GPIO_D:
//	case PAD_GPIO_E:
		_LOCK(grp);
		NX_GPIO_SetInterruptEnable(grp, bit, on ? CTRUE : CFALSE);
		_UNLOCK(grp);
		break;

	case PAD_GPIO_ALV:
		_LOCK(grp);
		NX_ALIVE_SetInterruptEnable(bit, on ? CTRUE : CFALSE);
		_UNLOCK(grp);
		break;

	default:
		printk("fail, soc gpio io:%d, group:%d (%s)\n", io, grp, __func__);
		break;
	};
}
EXPORT_SYMBOL(soc_gpio_set_int_enable);

/*------------------------------------------------------------------------------
 * 	Description	: get gpio interrupt mode
 *	In[io]		: gpio pad number, 32*n + bit
 * 				: (n= GPIO_A:0, GPIO_B:1, GPIO_C:2, GPIO_D:3, GPIO_E:4, ALIVE:5, bit= 0 ~ 32)
 *	Return 		: -1 = invalid gpio.
 *		 		:  0 = gpio's interrupt disable.
 *				:  1 = gpio's interrupt enable.
 */
int soc_gpio_get_int_enable(u_int io)
{
	u_int grp = PAD_GET_GRP(io);
	u_int bit = PAD_GET_BIT(io);
	int   enb = -1;

	DBGOUT("%s (%d, %d)\n", __func__, grp, bit);

	switch (io & ~(32-1)) {
	case PAD_GPIO_A:
	case PAD_GPIO_B:
	case PAD_GPIO_C:
//	case PAD_GPIO_D:
//	case PAD_GPIO_E:
		_LOCK(grp);
		enb = NX_GPIO_GetInterruptEnable(grp, bit) ? 1 : 0;
		_UNLOCK(grp);
		break;

	case PAD_GPIO_ALV:
		_LOCK(grp);
		enb = NX_ALIVE_GetInterruptEnable(bit) ? 1 : 0;
		_UNLOCK(grp);
		break;

	default:
		printk("fail, soc gpio io:%d, group:%d (%s)\n", io, grp, __func__);
		break;
	};

	return enb;
}
EXPORT_SYMBOL(soc_gpio_get_int_enable);

/*------------------------------------------------------------------------------
 * 	Description	: set gpio interrupt mode
 *	In[io]		: gpio pad number, 32*n + bit
 * 				: (n= GPIO_A:0, GPIO_B:1, GPIO_C:2, GPIO_D:3, GPIO_E:4, ALIVE:5, bit= 0 ~ 32)
 *	In[mode]	: gpio interrupt detect mode
 *				: 0 = Low level detect
 *				: 1 = High level detect
 *				: 2 = Falling edge detect
 *				: 3 = Rising edge detect
 *				: alive interrupt detect mode
 *				: 0 = async low level detect mode
 *				: 1 = async high level detect mode
 *				: 2 = sync falling edge detect mode
 *				: 3 = sync rising edge detect mode
 *				: 4 = sync low level detect mode
 *				: 5 = sync high level detect mode
 *	Return 		: none.
 */
void soc_gpio_set_int_mode(u_int io, u_int mode)
{
	u_int grp = PAD_GET_GRP(io);
	u_int bit = PAD_GET_BIT(io);

	DBGOUT("%s (%d, %d, %d)\n", __func__, grp, bit, mode);

	switch (io & ~(32-1)) {
	case PAD_GPIO_A:
	case PAD_GPIO_B:
	case PAD_GPIO_C:
//	case PAD_GPIO_D:
//	case PAD_GPIO_E:
		_LOCK(grp);
		NX_GPIO_SetInterruptMode(grp, bit, mode);
		_UNLOCK(grp);
		break;

	case PAD_GPIO_ALV:
		_LOCK(grp);
		NX_ALIVE_SetOutputEnable(bit, CFALSE);
		NX_ALIVE_SetDetectEnable(bit, CTRUE);
		NX_ALIVE_SetDetectMode((NX_ALIVE_DETECTMODE)mode, bit, CTRUE);
		_UNLOCK(grp);
		break;

	default:
		printk("fail, soc gpio io:%d, group:%d (%s)\n", io, grp, __func__);
		break;
	};
}
EXPORT_SYMBOL(soc_gpio_set_int_mode);

/*------------------------------------------------------------------------------
 * 	Description	: get gpio interrupt mode
 *	In[io]		: gpio pad number, 32*n + bit
 * 				: (n= GPIO_A:0, GPIO_B:1, GPIO_C:2, GPIO_D:3, GPIO_E:4, bit= 0 ~ 32)
 *	Return		: -1 = invalid gpio
 *				:  0 = Low level detect
 *				:  1 = High level detect
 *				:  2 = Falling edge detect
 *				:  3 = Rising edge detect
 */
int soc_gpio_get_int_mode(u_int io)
{
	u_int grp = PAD_GET_GRP(io);
	u_int bit = PAD_GET_BIT(io);
	int	  mod = -1;

	DBGOUT("%s (%d, %d)\n", __func__, grp, bit);

	switch (io & ~(32-1)) {
	case PAD_GPIO_A:
	case PAD_GPIO_B:
	case PAD_GPIO_C:
//	case PAD_GPIO_D:
//	case PAD_GPIO_E:
		_LOCK(grp);
		mod = NX_GPIO_GetInterruptMode(grp, bit);
		_UNLOCK(grp);
		break;

	default:
		printk("fail, soc gpio io:%d, group:%d (%s)\n", io, grp, __func__);
		break;
	};

	return mod;
}
EXPORT_SYMBOL(soc_gpio_get_int_mode);

/*------------------------------------------------------------------------------
 * 	Description	: indicates whether a specified interrupt is pended or not
 *	In[io]		: gpio pad number, 32*n + bit
 * 				: (n= GPIO_A:0, GPIO_B:1, GPIO_C:2, GPIO_D:3, GPIO_E:4, ALIVE:5, bit= 0 ~ 32)
 *	Return 		: -1 = invalid gpio.
 *		 		:  0 = interrupt not pend.
 *				:  1 = interrupt pended.
 */
int soc_gpio_get_int_pend(u_int io)
{
	u_int grp = PAD_GET_GRP(io);
	u_int bit = PAD_GET_BIT(io);
	int  pend = -1;

	DBGOUT("%s (%d, %d)\n", __func__, grp, bit);

	switch (io & ~(32-1)) {
	case PAD_GPIO_A:
	case PAD_GPIO_B:
	case PAD_GPIO_C:
//	case PAD_GPIO_D:
//	case PAD_GPIO_E:
		_LOCK(grp);
		pend = NX_GPIO_GetInterruptPending(grp, bit) ? 1 : 0;
		_UNLOCK(grp);
		break;

	case PAD_GPIO_ALV:
		_LOCK(grp);
		pend = NX_ALIVE_GetInterruptPending(bit) ? 1 : 0;
		_UNLOCK(grp);
		break;

	default:
		printk("fail, soc gpio io:%d, group:%d (%s)\n", io, grp, __func__);
		break;
	};

	return pend;
}
EXPORT_SYMBOL(soc_gpio_get_int_pend);

/*------------------------------------------------------------------------------
 * 	Description	: clear pending state of gpio interrupts
 *	In[io]		: gpio pad number, 32*n + bit
 * 				: (n= GPIO_A:0, GPIO_B:1, GPIO_C:2, GPIO_D:3, GPIO_E:4, ALIVE:5, bit= 0 ~ 32)
 *	Return 		: none.
 */
void soc_gpio_clr_int_pend(u_int io)
{
	u_int grp = PAD_GET_GRP(io);
	u_int bit = PAD_GET_BIT(io);

	DBGOUT("%s (%d, %d)\n", __func__, grp, bit);

	switch (io & ~(32-1)) {
	case PAD_GPIO_A:
	case PAD_GPIO_B:
	case PAD_GPIO_C:
//	case PAD_GPIO_D:
//	case PAD_GPIO_E:
		_LOCK(grp);
		NX_GPIO_ClearInterruptPending(grp, bit);
		NX_GPIO_GetInterruptPending(grp, bit);
		_UNLOCK(grp);
		break;

	case PAD_GPIO_ALV:
		_LOCK(grp);
		NX_ALIVE_ClearInterruptPending(bit);
		NX_ALIVE_GetInterruptPending(bit);
		_UNLOCK(grp);
		break;

	default:
		printk("fail, soc gpio io:%d, group:%d (%s)\n", io, grp, __func__);
		break;
	};
}
EXPORT_SYMBOL(soc_gpio_clr_int_pend);

/*------------------------------------------------------------------------------
 * 	Description	: enable alive detect mode
 *	In[io]		: alive pad number, 0~7
 *	In[on]		:  alive detect mode interrupt enable or disable
 *	Return 		: none.
 */
void soc_alv_set_det_enable(u_int io, int on)
{
	u_int bit = PAD_GET_BIT(io);

	DBGOUT("%s (%d)\n", __func__, bit);

	_LOCK(ALIVE_INDEX);

	NX_ALIVE_SetDetectEnable(bit, on ? CTRUE : CFALSE);

	_UNLOCK(ALIVE_INDEX);
}
EXPORT_SYMBOL_GPL(soc_alv_set_det_enable);

/*------------------------------------------------------------------------------
 * 	Description	: get alive detect mode type
 *	In[io]		: alive pad number, 0~7
 *	Return 		: 0 = no detect mode.
 *				: 1 = detect mode.
 */
int soc_alv_get_det_enable(u_int io)
{
	u_int bit = PAD_GET_BIT(io);
	int   mod = 0;

	DBGOUT("%s (%d)\n", __func__, bit);

	_LOCK(ALIVE_INDEX);

	mod = NX_ALIVE_GetDetectEnable(bit) ? 1 : 0;

	_UNLOCK(ALIVE_INDEX);

	return mod;
}
EXPORT_SYMBOL_GPL(soc_alv_get_det_enable);

/*------------------------------------------------------------------------------
 * 	Description	: set alive pad detection mode
 *	In[io]		: alive pad number, 0~7
 *	In[mode]	: alive detection mode
 *				: 0 = async low level detect mode
 *				: 1 = async high level detect mode
 *				: 2 = sync falling edge detect mode
 *				: 3 = sync rising edge detect mode
 *				: 4 = sync low level detect mode
 *				: 5 = sync high level detect mode
 *	In[on]		: alive detection mode enable or disable
 *	Return 		: none.
 */
void soc_alv_set_det_mode(u_int io, u_int mode, int on)
{
	u_int bit = PAD_GET_BIT(io);

	DBGOUT("%s (%d)\n", __func__, bit);

	_LOCK(ALIVE_INDEX);

	NX_ALIVE_SetDetectMode(mode, bit, on ? CTRUE : CFALSE);

	_UNLOCK(ALIVE_INDEX);
}
EXPORT_SYMBOL_GPL(soc_alv_set_det_mode);

/*------------------------------------------------------------------------------
 * 	Description	: get alive pad detection mode
 *	In[io]		: alive pad number, 0~7
 *	Return 		: 0 = detect mode is disabled.
 *				: 1 = detect mode is enabled.
 */
int soc_alv_get_det_mode(u_int io, u_int mode)
{
	u_int bit = PAD_GET_BIT(io);
	int   mod = 0;

	DBGOUT("%s (%d)\n", __func__, bit);

	_LOCK(ALIVE_INDEX);

	mod = NX_ALIVE_GetDetectMode(mode, bit) ? 1 : 0;

	_UNLOCK(ALIVE_INDEX);

	return mod;
}
EXPORT_SYMBOL_GPL(soc_alv_get_det_mode);