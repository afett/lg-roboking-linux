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
#define DBGOUT(msg...)		{ printk("adc: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

/*-----------------------------------------------------------------------------*/
#define ADC_SPS		500000
static spinlock_t 	lock;
static int 			attach_count = 0;

/*-----------------------------------------------------------------------------*/
void soc_adc_init(void)
{
	DBGOUT("%s\n", __func__);

	NX_ADC_SetBaseAddress((U32)IO_ADDRESS(NX_ADC_GetPhysicalAddress()));
 	NX_ADC_OpenModule();

	NX_ADC_SetInterruptEnableAll(CFALSE);
	NX_ADC_SetClockPClkMode(NX_PCLKMODE_DYNAMIC);

	NX_ADC_SetPrescalerValue((CFG_SYS_PCLK_FREQ + (ADC_SPS>>1))/ADC_SPS);
	NX_ADC_SetPrescalerEnable(CFALSE);
	NX_ADC_SetStandbyMode(CTRUE);	/* FALSE=Power On, TRUE=Power Off */

	spin_lock_init(&lock);
}

/*------------------------------------------------------------------------------
 * 	Description	: enable clock and prescaler of ADC
 *	Return 		: None
 */
void soc_adc_attach(void)
{
	spin_lock(&lock);

	DBGOUT("%s(%d)\n", __func__, attach_count);

	if (0 == attach_count) {
		NX_ADC_SetClockPClkMode(NX_PCLKMODE_ALWAYS);
		NX_ADC_SetPrescalerValue((CFG_SYS_PCLK_FREQ + (ADC_SPS>>1))/ADC_SPS);
		NX_ADC_SetPrescalerEnable(CTRUE);
		NX_ADC_SetStandbyMode(CFALSE);	/* FALSE=Power On, TRUE=Power Off */
	}

	attach_count++;

	spin_unlock(&lock);
}
EXPORT_SYMBOL(soc_adc_attach);

/*------------------------------------------------------------------------------
 * 	Description	: disable clock and prescaler of ADC
 *	Return 		: None
 */
void soc_adc_detach(void)
{
	spin_lock(&lock);

	attach_count--;

	if (0 == attach_count) {
		NX_ADC_SetClockPClkMode(NX_PCLKMODE_DYNAMIC);
		NX_ADC_SetPrescalerEnable(CFALSE);
		NX_ADC_SetStandbyMode(CTRUE);	/* FALSE=Power On, TRUE=Power Off */
	}

	/* clear attach count */
	if (0 > attach_count)
		attach_count = 0;

	DBGOUT("%s(%d)\n", __func__, attach_count);

	spin_unlock(&lock);
}
EXPORT_SYMBOL(soc_adc_detach);

/*------------------------------------------------------------------------------
 * 	Description	: get conversioned data of ADC
 *	In[ch]		: value of adc channel ( 0 ~ 6 )
 *	In[timeout]	: wait timer out (us)
 *	Return 		: -1 failure or 10bit data of ADC
 */
unsigned short soc_adc_read(int ch, u_int timeout)
{
	u_short value = -1;
	u_int   tout  = timeout;

	spin_lock(&lock);

	DBGOUT("%s\n", __func__);

	if (0 == attach_count || ch > 6) {
		printk(KERN_ERR "fail, not attached or not support ADC:%d ...\n", ch);
		return -1;
	}

	while (NX_ADC_IsBusy() && tout--)
		udelay(1);

	if (0 >= tout)
		goto err;

	NX_ADC_SetInputChannel(ch);
	NX_ADC_Start();

	/* wait ADC convert done */
	while (NX_ADC_IsBusy())	{ ;	}

	value = (unsigned short)NX_ADC_GetConvertedData();

err:
	spin_unlock(&lock);

	return value;
}
EXPORT_SYMBOL(soc_adc_read);

/*------------------------------------------------------------------------------
 * 	Description	: set suspend mode,
 *	Return 		: none.
 */
void soc_adc_suspend(void)
{
	PM_DBGOUT("%s\n", __func__);
}
EXPORT_SYMBOL_GPL(soc_adc_suspend);

/*------------------------------------------------------------------------------
 * 	Description	: resume mode
 *	Return 		: none.
 */
void soc_adc_resume(void)
{
	PM_DBGOUT("%s\n", __func__);

	if (! attach_count)
		return;

	if (NX_PCLKMODE_ALWAYS == NX_ADC_GetClockPClkMode())
		return;

	NX_ADC_SetClockPClkMode(NX_PCLKMODE_ALWAYS);
	NX_ADC_SetPrescalerValue((CFG_SYS_PCLK_FREQ + (ADC_SPS>>1))/ADC_SPS);
	NX_ADC_SetPrescalerEnable(CTRUE);
	NX_ADC_SetStandbyMode(CFALSE);	/* FALSE=Power On, TRUE=Power Off */
}
EXPORT_SYMBOL_GPL(soc_adc_resume);