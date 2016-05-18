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
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/sched.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/soc.h>
#include <mach/dpc.h>

#include "dpc_dev.h"

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "dpc: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#define ERROUT(msg...)		{ printk(KERN_ERR "dpc: " msg); }

#define	DUMP_REG	(0)

#if DUMP_REG
static void dbg_reg_dpc(int dev);
static void dbg_reg_mlc(int dev);
#endif

/*------------------------------------------------------------------------------
 *  Local functions
 */

/* 12345'678'[8] -> 12345 [5], 123456'78'[8] -> 123456[6] */
inline static u_short R8G8B8toR5G6B5(u_int RGB)
{
	u_char	R = (u_char)((RGB>>16) & 0xff);
	u_char	G = (u_char)((RGB>>8 ) & 0xff);
	u_char	B = (u_char)((RGB>>0 ) & 0xff);

	u_short R5G6B5 = ((R & 0xF8)<<8) | ((G & 0xFC)<<3) | ((B & 0xF8)>>3);
	DBGOUT(" RGB888:0x%08x -> RGB565:0x%08x\n", RGB, R5G6B5);

	return R5G6B5;
}

/* 12345 [5] -> 12345'123'[8], 123456[6] -> 123456'12'[8] */
inline static u_int R5G6B5toR8G8B8(u_short RGB)
{
	u_char R5  = (RGB >> 11) & 0x1f;
	u_char G6  = (RGB >> 5 ) & 0x3f;
	u_char B5  = (RGB >> 0 ) & 0x1f;

	u_char R8  = ((R5 << 3) & 0xf8) | ((R5 >> 2) & 0x7);
	u_char G8  = ((G6 << 2) & 0xfc) | ((G6 >> 4) & 0x3);
	u_char B8  = ((B5 << 3) & 0xf8) | ((B5 >> 2) & 0x7);

	u_int R8B8G8 = (R8 << 16) | (G8 << 8) | (B8);
	DBGOUT(" RGB565:0x%08x -> RGB888:0x%08x\n", RGB, R8B8G8);

	return R8B8G8;
}

/* 123'45678'[8] -> 123[3], 12'345678'[8] -> 12 [2] */
inline static u_char R8G8B8toR3G3B2(u_int RGB)
{
	u_char	R = (u_char)((RGB>>16) & 0xff);
	u_char	G = (u_char)((RGB>>8 ) & 0xff);
	u_char	B = (u_char)((RGB>>0 ) & 0xff);

	u_char R3G3B2 = ( (R & 0xE0) | ((G & 0xE0)>>3) | ((B & 0xC0)>>6) );
	DBGOUT(" RGB888:0x%08x -> RGB332:0x%08x\n", RGB, R3G3B2);

	return R3G3B2;
}

/* 123[3] -> 123'123'12' [8], 12 [2] -> 12'12'12'12'[8] */
inline static u_int R3G3B2toR8G8B8(u_char RGB)
{
	u_char R3  = (RGB >> 5) & 0x7;
	u_char G3  = (RGB >> 2) & 0x7;
	u_char B2  = (RGB >> 0) & 0x3;

	u_char R8  = ((R3 << 5) | (R3 << 2) | (R3 >> 1) );
	u_char G8  = ((G3 << 5) | (G3 << 2) | (G3 >> 1) );
	u_char B8  = ((B2 << 6) | (B2 << 4) | (B2 << 2) | B2 );

	u_int R8B8G8 = (R8 << 16) | (G8 << 8) | (B8);
	DBGOUT(" RGB332:0x%08x -> RGB888:0x%08x\n", RGB, R8B8G8);

	return R8B8G8;
}

inline static double ksin(double radian)
{
	int    i = 1;
	double d = 0.0;
	double s = radian;

	while (1) {
		d += s;
		s *= -radian * radian/(2 * i * (2 * i + 1));
		i++;
		if (s < 0.000000000000001 && s > -0.000000000000001)
			break;
	}
	return d;
}

inline static double kcos(double radian)
{
	int    i = 1;
	double d = 0.0;
	double s = 1.0;

	while (1) {
		d += s;
		s *= -radian * radian/(2 * i * (2 * i - 1));
		i++;

		if (s < 0.000000000000001 && s > -0.000000000000001)
			break;
	}
	return d;
}

inline static void set_dpc_irq_enable(int module)
{
	struct dpc_info * dpc = DPC_INFO(module);
	int irq = dpc->irq;

	DBGOUT("%s (m:%d, irq:%d)\n", __func__, module, irq);

	NX_DPC_SetInterruptEnableAll(irq, CTRUE);
	enable_irq(irq);
}

inline static void set_dpc_irq_disable(int module)
{
	struct dpc_info * dpc = DPC_INFO(module);
	int irq = dpc->irq;

	DBGOUT("%s (m:%d, irq:%d)\n", __func__, module, irq);

	disable_irq(irq);

	NX_DPC_SetInterruptEnableAll(irq, CFALSE);
}

static void set_dpc_enable(int module)
{
	struct dpc_info * dpc = DPC_INFO(module);
	struct vid_data * vid = VID_DATA(module);

	NX_DPC_DITHER RDither, GDither, BDither;
	u_int  DPCFmt = CFG_DISP_PRI_OUT_FORMAT;
    bool   bESync = CFALSE;
    u_int  XResol = CFG_DISP_PRI_RESOL_WIDTH;
    u_int  YResol = CFG_DISP_PRI_RESOL_HEIGHT;
	int  layer, delay;

	u_int  bgcolor  = dpc->bgcolor;
	int    priority = vid->priority;

	DBGOUT("%s (m:%d)\n", __func__, module);

	// Horizontla Up scale
	// Note: Only seconary DPC can scale up of horizontal width.
	if (module == 1)
		NX_DPC_SetHorizontalUpScaler(module, CFALSE, 2, 2);

	/* Set DPC */
   	if ( ((u_int)NX_DPC_FORMAT_RGB555   == DPCFmt) ||
		((u_int)NX_DPC_FORMAT_MRGB555A == DPCFmt) ||
		((u_int)NX_DPC_FORMAT_MRGB555B == DPCFmt) ) {
		RDither = GDither = BDither = NX_DPC_DITHER_5BIT;
	} else if ( ((u_int)NX_DPC_FORMAT_RGB565  == DPCFmt) ||
			 ((u_int)NX_DPC_FORMAT_MRGB565 == DPCFmt) ) {
		RDither = BDither = NX_DPC_DITHER_5BIT;
		GDither           = NX_DPC_DITHER_6BIT;
	} else if ( ((u_int)NX_DPC_FORMAT_RGB666  == DPCFmt) ||
			 ((u_int)NX_DPC_FORMAT_MRGB666 == DPCFmt) ) {
		RDither = GDither = BDither = NX_DPC_DITHER_6BIT;
	} else {
		RDither = GDither = BDither = NX_DPC_DITHER_BYPASS;
	}

	// VCLK2 : CLKGEN0
	NX_DPC_SetClockSource  (module, 0, CFG_DISP_PRI_CLKGEN0_SOURCE);	// CLKSRCSEL
	NX_DPC_SetClockDivisor (module, 0, CFG_DISP_PRI_CLKGEN0_DIV);		// CLKDIV
	NX_DPC_SetClockOutDelay(module, 0, CFG_DISP_PRI_CLKGEN0_DELAY); 	// OUTCLKDELAY

	// VCLK : CLKGEN1
	NX_DPC_SetClockSource  (module, 1, CFG_DISP_PRI_CLKGEN1_SOURCE);	// CLKSRCSEL  : CLKGEN0's out
	NX_DPC_SetClockDivisor (module, 1, CFG_DISP_PRI_CLKGEN1_DIV);		// CLKDIV
	NX_DPC_SetClockOutDelay(module, 1, CFG_DISP_PRI_CLKGEN1_DELAY); 	// OUTCLKDELAY

	NX_DPC_SetMode( module,
			(NX_DPC_FORMAT)DPCFmt,						// FORMAT
			CFALSE,										// SCANMODE
     	    CFG_DISP_PRI_OUT_POL_INVERT,   				// POLFIELD
            CFG_DISP_PRI_OUT_RGB, 						// RGBMODE
            CFALSE,       								// SWAPRB
	        (NX_DPC_YCORDER)CFG_DISP_PRI_OUT_YCORDER,	// YCORDER
	        bESync,										// YCCLIP
    	    bESync,  									// Embedded sync, Note> have to set to CTRUE for ITU-R BT.656
        	(NX_DPC_PADCLK)CFG_DISP_PRI_PADCLKSEL,		// PADCLKSEL
            CFG_DISP_PRI_OUT_CLK_INVERT,				// PADCLKINV
			CFG_DISP_PRI_OUT_DUAL_VIEW);

	NX_DPC_SetHSync(module,
			XResol,
			CFG_DISP_PRI_HSYNC_SYNC_WIDTH,
			CFG_DISP_PRI_HSYNC_FRONT_PORCH,
			CFG_DISP_PRI_HSYNC_BACK_PORCH,
			CFG_DISP_PRI_HSYNC_ACTIVE_HIGH);

	NX_DPC_SetVSync(module,
			YResol,
			CFG_DISP_PRI_VSYNC_SYNC_WIDTH,
			CFG_DISP_PRI_VSYNC_FRONT_PORCH,
			CFG_DISP_PRI_VSYNC_BACK_PORCH,
			CFG_DISP_PRI_VSYNC_ACTIVE_HIGH,
			CFG_DISP_PRI_EVSYNC_ACTIVE_HEIGHT,
			CFG_DISP_PRI_EVSYNC_SYNC_WIDTH,
			CFG_DISP_PRI_EVSYNC_FRONT_PORCH,
			CFG_DISP_PRI_EVSYNC_BACK_PORCH);

	NX_DPC_SetVSyncOffset(module,
			CFG_DISP_PRI_VSYNC_START_OFFSET,
			CFG_DISP_PRI_VSYNC_END_OFFSET,
			CFG_DISP_PRI_EVSYNC_START_OFFSET,
			CFG_DISP_PRI_EVSYNC_END_OFFSET);

	NX_DPC_SetDelay(module,
		  	CFG_DISP_PRI_SYNC_DELAY_RGB_PVD,	// DELAYRGB
       	  	CFG_DISP_PRI_SYNC_DELAY_HS_CP1,		// DELAYHS_CP1
        	CFG_DISP_PRI_SYNC_DELAY_VS_FRAM,	// DELAYVS_FRAM
	        CFG_DISP_PRI_SYNC_DELAY_DE_CP2);	// DELAYDE_CP2

   	NX_DPC_SetDither(module, RDither, GDither, BDither);

	/* Set MLC */
	NX_MLC_SetClockPClkMode(module, NX_PCLKMODE_DYNAMIC);
	NX_MLC_SetClockBClkMode(module, NX_BCLKMODE_DYNAMIC);
	NX_MLC_SetLayerPriority(module, (NX_MLC_PRIORITY)priority);
	NX_MLC_SetBackground(module, bgcolor);
	NX_MLC_SetFieldEnable(module, CFG_DISP_PRI_MLC_INTERLACE);
	NX_MLC_SetScreenSize(module, XResol, YResol);
	NX_MLC_SetRGBLayerGamaTablePowerMode(module, CFALSE, CFALSE, CFALSE);
	NX_MLC_SetRGBLayerGamaTableSleepMode(module, CTRUE, CTRUE, CTRUE);
	NX_MLC_SetRGBLayerGammaEnable(module, CFALSE);
	NX_MLC_SetDitherEnableWhenUsingGamma(module, CFALSE);
	NX_MLC_SetGammaPriority(module, CFALSE);
    NX_MLC_SetTopPowerMode(module, CTRUE);
    NX_MLC_SetTopSleepMode(module, CFALSE);
	NX_MLC_SetMLCEnable(module, CTRUE);

	// Set layer dirty flag
	for (layer = 0; RGB_LAYERS > layer ; layer++)
		NX_MLC_SetDirtyFlag(module, layer);

	NX_MLC_SetDirtyFlag(module, VIDEO_LAYER);

	NX_MLC_SetTopDirtyFlag(module);

	/* DPC ON */
	NX_DPC_SetDPCEnable(module, CTRUE);
	NX_DPC_SetClockDivisorEnable(module, CTRUE);	// CLKENB : Provides internal operating clock.

	/* some delay */
	for (delay=0; delay<100; delay++) { ; }
}

static void set_dpc_disable(int module)
{
	int layer;
	int wait = 60;

	DBGOUT("%s (m:%d)\n", __func__, module);

	/*	MLC layer OFF */
	for (layer = 0; RGB_LAYERS > layer; layer++) {
		NX_MLC_SetLayerEnable(module, layer, CFALSE);
		NX_MLC_SetDirtyFlag(module, layer);
	}

	NX_MLC_SetLayerEnable(module, VIDEO_LAYER, CFALSE);
	NX_MLC_SetDirtyFlag(module, VIDEO_LAYER);

   	NX_MLC_SetTopPowerMode(module, CFALSE);
   	NX_MLC_SetTopSleepMode(module, CTRUE);
	NX_MLC_SetMLCEnable(module, CFALSE);
	NX_MLC_SetTopDirtyFlag(module);

	/* disable interrupt */
	set_dpc_irq_disable(module);

	/* DPC */
	NX_DPC_ClearInterruptPendingAll(module);
	NX_DPC_SetInterruptEnableAll(module, CTRUE);

	/* wait vertical sync */
	while (wait-- > 0 &&
		  CFALSE == NX_DPC_GetInterruptPendingAll(module))
		mdelay(1);

	NX_DPC_SetDPCEnable(module, CFALSE);
	NX_DPC_SetClockPClkMode(module, NX_PCLKMODE_DYNAMIC);
	NX_DPC_SetClockDivisorEnable(module, CFALSE);

	/* enable interrupt */
	set_dpc_irq_enable(module);
}

static void set_vd_alpha(int module, int depth, bool on)
{
	if (depth <= 0 )	depth = 0;
	if (depth >= 15)	depth = 15;

	NX_MLC_SetAlphaBlending(module, VIDEO_LAYER, (on?CTRUE:CFALSE), depth);
	NX_MLC_SetDirtyFlag(module, VIDEO_LAYER);
}

static void set_vd_lu_enhance(int module, int bright, int contra)
{
	DBGOUT("%s (module:%d, contrast:%d, bright:%d)\n", __func__, module, contra, bright);

	if (contra >=    7) contra =    7;
	if (contra <=    0)	contra =    0;
	if (bright >=  127) bright =  127;
	if (bright <= -128) bright = -128;

	NX_MLC_SetVideoLayerLumaEnhance(module, (u_int)contra, (S32)bright);
	NX_MLC_SetDirtyFlag(module, VIDEO_LAYER);
}

static void set_vd_cbr_enhance(int module, int cba, int cbb, int cra, int crb)
{
	int i;

	DBGOUT("%s (module:%d, cba:%d, cbb:%d, cra:%d, crb:%d)\n",
		__func__, module, cba, cbb, cra, crb);

	if (cba > 127) cba=127; if (cba <= -128) cba=-128;
	if (cbb > 127) cbb=127; if (cbb <= -128) cbb=-128;
	if (cra > 127) cra=127; if (cra <= -128) cra=-128;
	if (crb > 127) crb=127; if (crb <= -128) crb=-128;

	for (i=1; 5 > i; i++)
		NX_MLC_SetVideoLayerChromaEnhance(module, i, cba, cbb, cra, crb);

	NX_MLC_SetDirtyFlag(module, VIDEO_LAYER);
}

/*------------------------------------------------------------------------------
 * IRQ handler
 */
static irqreturn_t	dpc_irq_handler(int irq, void *desc)
{
	struct dpc_info * dpc = desc;

	/* clear source irq pend bit */
	NX_DPC_ClearInterruptPendingAll(irq);

	dpc->cond = 1;
	wake_up_interruptible(&dpc->wait);

	return IRQ_HANDLED;
}

inline static int wait_vsync(int module)
{
	struct dpc_info   * dpc  = DPC_INFO(module);
	wait_queue_head_t * wait = &dpc->wait;
	int ret = -1;

	if (NX_DPC_GetDPCEnable(module)) {
		dpc->cond = 0;
		ret = interruptible_sleep_on_timeout(wait, dpc->wait_time);
		if (! ret)
			printk(KERN_ERR "fail, module(%d) wait vsync, ret:%d, cond:%d \n",
				module, ret, dpc->cond);
	}

	return ret;	/* 0: success, -1: fail */
}

inline static int wait_dirty_clear(int module, int layer)
{
	if (CFALSE == NX_MLC_GetLayerEnable(module, layer))
		return 0;

	return wait_vsync(module);
}

/*----------------------------------------------------------------------------*/
void soc_dpc_init(void)
{
	struct dpc_info * dpc = DPC_INFO(0);
	int num, ret = 0;
	int i, l;

	num = NX_MLC_GetNumberOfModule();

	for (i = 0; num > i; i++, dpc++) {

		/* set prototype base */
		NX_MLC_SetBaseAddress(i, (U32)IO_ADDRESS(NX_MLC_GetPhysicalAddress(i)));
		NX_DPC_SetBaseAddress(i, (U32)IO_ADDRESS(NX_DPC_GetPhysicalAddress(i)));
		NX_MLC_OpenModule(i);
		NX_DPC_OpenModule(i);

		/* register irq */
		init_waitqueue_head(&dpc->wait);
		ret = request_irq(dpc->irq, &dpc_irq_handler, IRQF_DISABLED, dpc->name, dpc);
		if (ret) {
			printk(KERN_ERR "fail, dpc request irq %d ...\n", dpc->irq);
			return;
		}
		dpc->wait_time = msecs_to_jiffies(VSYNC_WAIT_TIME);

		/* set module's layer data */
		for (l = 0; ALL_LAYERS > l; l++) {
			if (RGB_LAYERS == l)
				dpc->ldata[l] = &vid_data[i];		/* Video layer */
			else
				dpc->ldata[l] = &rgb_data[i][l];	/* RGB layer */
		}

		/* enable source irq */
		NX_DPC_SetInterruptEnableAll(i, CTRUE);
		DBGOUT("dpc[%d], irq:%d, mlc:0x%08x, dpc:0x%08x\n",
			i, dpc->irq, (u_int)dpc->regs.mlc_base, (u_int)dpc->regs.dpc_base);
	}
}

void soc_dpc_exit(void)
{
	struct dpc_info * dpc = DPC_INFO(0);
	int i, num;

	num = NX_MLC_GetNumberOfModule();

	for (i = 0; num > i; i++, dpc++)
		free_irq(dpc->irq, NULL);
}

/*------------------------------------------------------------------------------
 * Display output control
 */
u_int soc_dpc_get_mode(void)
{
	DBGOUT("%s\n", __func__);
	return CFG_DISP_MAIN_SCREEN;
}

void soc_dpc_get_resolution(int module, int *x_res, int *y_res)
{
	struct dpc_info * dpc = DPC_INFO(module);

	DBGOUT("%s (m:%d)\n", __func__, module);

	*x_res = dpc->x_res;
	*y_res = dpc->y_res;
}

void soc_dpc_set_bgcolor(int module, u_int color)
{
	struct dpc_info * dpc = DPC_INFO(module);

	DBGOUT("%s (m:%d, c:0x%x)\n", __func__, module, color);

	dpc->bgcolor = color;

	NX_MLC_SetBackground(module, color & 0x00FFFFFF);
	NX_MLC_SetTopDirtyFlag(module);
}

u_int soc_dpc_get_bgcolor(int module)
{
	struct dpc_info * dpc = DPC_INFO(module);

	DBGOUT("%s (m:%d)\n", __func__, module);

	return (u_int)dpc->bgcolor;
}

void soc_dpc_wait_vsync(int module)
{
	wait_vsync(module);
}

void soc_dpc_set_out_enable(int module)
{
	struct dpc_info * dpc = DPC_INFO(module);

	DBGOUT("%s (m:%d)\n", __func__, module);

	/* check status */
	if (CTRUE == NX_DPC_GetDPCEnable(module)) {
		printk("dpc:%d is already enabled ...\n", module);
		return;
	}

	/*
	 * step: display -> lcd -> backlight
	 */
	set_dpc_enable(module);

	if (dpc->lcdops && dpc->lcdops->enable)
		dpc->lcdops->enable(module);

	if (dpc->bluops && dpc->bluops->enable)
		dpc->bluops->enable(module);

	/* change status */
	dpc->status	= STAT_ENABLE;
}

void soc_dpc_set_out_disable(int module)
{
	struct dpc_info * dpc = DPC_INFO(module);

	DBGOUT("%s (m:%d)\n", __func__, module);

	/* check status */
	if (CFALSE == NX_DPC_GetDPCEnable(module))
		return;

	/*
	 * step: backlight -> display -> lcd
	 */
	if (dpc->bluops && dpc->bluops->disable)
		dpc->bluops->disable(module);

	set_dpc_disable(module);

	if (dpc->lcdops && dpc->lcdops->disable)
		dpc->lcdops->disable(module);

	/* change status */
	dpc->status	= STAT_DISABLE;
}

void soc_dpc_set_suspend(int module)
{
	struct dpc_info * dpc = DPC_INFO(module);

	volatile u_int *mlc_base = dpc->regs.mlc_base;
	volatile u_int *dpc_base = dpc->regs.dpc_base;
	u_int mlc_size = NX_MLC_GetSizeOfRegisterSet();
	u_int dpc_size = NX_DPC_GetSizeOfRegisterSet();

	PM_DBGOUT("+%s (m:%d)\n", __func__, module);

	if (STAT_SUSPEND & dpc->status) {
		printk("Now dpc:%d is suspend state ...\n", module);
		return;
	}

#if DUMP_REG
	dbg_reg_dpc(module);
	dbg_reg_mlc(module);
#endif

	/* save MLC: save layer enable status */
	memcpy((void*)dpc->regs.mlc_save, (const void*)mlc_base, mlc_size);

	/*
	 * step: backlight -> display -> lcd
	 */
	if (dpc->bluops && dpc->bluops->suspend)
		dpc->bluops->suspend(module);

	set_dpc_disable(module);

	if (dpc->lcdops && dpc->lcdops->suspend)
		dpc->lcdops->suspend(module);

	/* save DPC: save clock disable status */
	memcpy((void*)dpc->regs.dpc_save, (const void*)dpc_base, dpc_size);

	/* change status */
	dpc->status |= STAT_SUSPEND;
	dpc->status &= ~STAT_RESUME;

	PM_DBGOUT("-%s (m:%d)\n", __func__, module);
}

void soc_dpc_set_resume(int module)
{
	struct dpc_info * dpc = DPC_INFO(module);

	volatile u_int *mlc_base = dpc->regs.mlc_base;
	volatile u_int *dpc_base = dpc->regs.dpc_base;
	u_int  mlc_size = NX_MLC_GetSizeOfRegisterSet();
	u_int  dpc_size = NX_DPC_GetSizeOfRegisterSet();

	int i = 0, wait = 60;

	PM_DBGOUT("+%s (m:%d)\n", __func__, module);

	if (! (STAT_SUSPEND & dpc->status)) {
		printk("fail, dpc:%d is not suspend state ...\n", module);
		return;
	}

	/* restore register */
	for (i = 0; mlc_size/4 > i; i++)
		WriteIODW((mlc_base+i), dpc->regs.mlc_save[i]);

	for (i = 0; dpc_size/4 > i; i++)
		WriteIODW((dpc_base+i), dpc->regs.dpc_save[i]);

	/*
	 * step: display -> lcd -> backlight
	 */
	NX_MLC_SetTopPowerMode(module, CTRUE);
   	NX_MLC_SetTopSleepMode(module, CFALSE);
	NX_MLC_SetMLCEnable(module, CTRUE);

	for (i = 0; RGB_LAYERS > i; i++)
		NX_MLC_SetDirtyFlag(module, i);

	NX_MLC_SetDirtyFlag(module, VIDEO_LAYER);

	NX_MLC_SetTopDirtyFlag(module);

	/* disable interrupt */
	set_dpc_irq_disable(module);

	NX_DPC_ClearInterruptPendingAll(module);
	NX_DPC_SetInterruptEnableAll(module, CTRUE);
	NX_DPC_SetClockDivisorEnable(module, CTRUE);
	NX_DPC_SetDPCEnable(module, CTRUE);

	while (wait-- > 0 &&
		   CFALSE == NX_DPC_GetInterruptPendingAll(module))
		mdelay(1);

	if (dpc->lcdops && dpc->lcdops->enable)
		dpc->lcdops->resume(module);

	if (dpc->bluops && dpc->bluops->enable)
		dpc->bluops->resume(module);

	/* enable interrupt */
	set_dpc_irq_enable(module);

	/* change status */
	dpc->status &= ~STAT_SUSPEND;
	dpc->status |= STAT_RESUME;

#if DUMP_REG
	dbg_reg_dpc(module);
	dbg_reg_mlc(module);
#endif

	PM_DBGOUT("-%s (m:%d)\n", __func__, module);
}

/*------------------------------------------------------------------------------
 * Display layer "RGB and VIDEO" control
 */
void soc_dpc_set_layer_enable(int module, int layer, int enable)
{
	CBOOL hl, hc, vl, vc;

	DBGOUT("%s (m:%d, l:%d)\n", __func__, module, layer);

	if (VIDEO_LAYER == layer) {
		/* set filter */
		if (! enable) {
			NX_MLC_GetVideoLayerScaleFilter(module, &hl, &hc, &vl, &vc);
			if  (hl | hc | vl | vc) {
				NX_MLC_SetVideoLayerScaleFilter(module, 0, 0, 0, 0);
				wait_dirty_clear(module, layer);
			}
		}
		NX_MLC_SetVideoLayerLineBufferPowerMode(module, (enable ? CTRUE : CFALSE));
		NX_MLC_SetVideoLayerLineBufferSleepMode(module, (enable ? CFALSE: CTRUE ));
	}

	NX_MLC_SetLayerEnable(module, layer, (enable ? CTRUE : CFALSE));
	NX_MLC_SetDirtyFlag  (module, layer);
}

int soc_dpc_get_layer_enable(int module, int layer)
{
	DBGOUT("%s (m:%d, l:%d)\n", __func__, module, layer);
	return NX_MLC_GetLayerEnable(module, layer) ? 1 : 0;
}

void soc_dpc_wait_dirtyclear(int module, int layer)
{
	DBGOUT("%s (m:%d, l:%d)\n", __func__, module, layer);
	wait_dirty_clear(module, layer);
}

/*------------------------------------------------------------------------------
 * Display layer "VIDEO" control
 */
int soc_dpc_set_vd_format(int module, u_int fourcc, int srcw, int srch)
{
	struct vid_data * vid = VID_DATA(module);
	u_int format;

	DBGOUT("%s (m:%d)\n", __func__, module);

	switch (fourcc) {
	case FOURCC_MVS0:
	case FOURCC_YV12: format = NX_MLC_YUVFMT_420;
		break;
	case FOURCC_MVS2:
	case FOURCC_MVN2: format = NX_MLC_YUVFMT_422;
		break;
	case FOURCC_MVS4: format = NX_MLC_YUVFMT_444;
		break;
	case FOURCC_YUY2:
	case FOURCC_YUYV: format = NX_MLC_YUVFMT_YUYV;
		break;
	default:
		printk(KERN_ERR "fail, not support video format (0x%08x)...\n", fourcc);
		return (-1);
	}

	if (0>= srcw || 0 >= srch) {
		printk(KERN_ERR "fail, (%d) invalid source size (%d * %d) \n",
			module, srcw, srch);
		return (-1);
	}

	/* set video data. */
	vid->fourcc = fourcc;
	vid->srcw   = srcw;
	vid->srch   = srch;

	NX_MLC_SetFormatYUV(module, (NX_MLC_YUVFMT)format);
	NX_MLC_SetDirtyFlag(module, VIDEO_LAYER);
	return 0;
}

void soc_dpc_get_vd_format(int module, u_int *fourcc, int *srcw, int *srch)
{
	struct vid_data * vid = VID_DATA(module);

	DBGOUT("%s (m:%d)\n", __func__, module);

	*fourcc = vid->fourcc;
	*srcw   = vid->srcw;
	*srch   = vid->srch;
}

void soc_dpc_set_vd_address(
	int module, u_int lu_a, u_int lu_s, u_int cb_a, u_int cb_s,
	u_int cr_a,	u_int cr_s, int waitvsync
	)
{
	struct vid_data * vid = VID_DATA(module);
	int  layer = VIDEO_LAYER;

	DBGOUT("%s (m:%d, lua:0x%x, cba:0x%x, cra:0x%x - lus:%d, cbs:%d, crs:%d, vs:%d)\n",
		__func__, module, lu_a, cb_a, cr_a, lu_s, cb_s, cr_s, waitvsync);

	/* set prototype */
	if (FOURCC_YUYV == vid->fourcc) {
		NX_MLC_SetVideoLayerAddressYUYV(module, lu_a, lu_s);
	} else {
		NX_MLC_SetVideoLayerStride (module, lu_s, cb_s, cr_s);
		NX_MLC_SetVideoLayerAddress(module, lu_a, cb_a, cr_a);
	}

	NX_MLC_SetDirtyFlag(module, layer);

	/* wait sysnc */
	if (waitvsync)
		wait_dirty_clear(module, layer);
}

void soc_dpc_set_vd_position(
	int module, int left, int top, int right, int bottom, int waitvsync
	)
{
	struct vid_data * vid  = VID_DATA(module);

	int layer = VIDEO_LAYER;
	int srcw  = vid->srcw;
	int srch  = vid->srch;
	int dstw  = right - left;
	int dsth  = bottom - top;
	int hf = 0;
	int vf = 1;

	DBGOUT("%s (m:%d, l:%d, t:%d, r:%d, b:%d, vs:%d)\n",
		__func__, module, left, top, right, bottom, waitvsync);

	/* check position factor */
	if (left >= right ) right  = left+1;
	if (top  >= bottom) bottom = top +1;
	if (0    >= right )	left  -= (right -1),  right  = 1;
	if (0    >= bottom)	top   -= (bottom-1), bottom = 1;

	/* check scale factor */
	if (dstw >= 2048) dstw = 2048;
	if (dsth >= 2048) dsth = 2048;

	/* set filter factor */
	if (srcw == dstw && srch == dsth)
		hf = 0, vf = 0;

	if (dstw > srcw)
		hf = 1, vf = 1;

	vid->h_filter = hf;
	vid->v_filter = vf;

	/* set scale */
	NX_MLC_SetVideoLayerScale(module, srcw, srch, dstw, dsth, (CBOOL)hf, (CBOOL)hf, (CBOOL)vf, (CBOOL)vf);
	NX_MLC_SetPosition(module, layer, left, top, right-1, bottom-1);
	NX_MLC_SetDirtyFlag(module, layer);

	/* wait sysnc */
	if (waitvsync)
		wait_dirty_clear(module, layer);
}

void soc_dpc_set_vd_enable(int module, int enable)
{
	DBGOUT("%s (m:%d, e:%d)\n", __func__, module, enable);
	soc_dpc_set_layer_enable(module, VIDEO_LAYER, enable);
}

void soc_dpc_set_vd_priority(int module, int prior)
{
	struct vid_data * vid = VID_DATA(module);

	DBGOUT("%s (m:%d, p:%d)\n", __func__, module, prior);

	switch (prior) {
	case 0:
		NX_MLC_SetLayerPriority(module, NX_MLC_PRIORITY_VIDEOFIRST);	// PRIORITY-video>0>1>2
		break;
	case 1:
		NX_MLC_SetLayerPriority(module, NX_MLC_PRIORITY_VIDEOSECOND); // PRIORITY-0>video>1>2
		break;
	case 2:
		NX_MLC_SetLayerPriority(module, NX_MLC_PRIORITY_VIDEOTHIRD);	// PRIORITY-0>1>video>2
		break;
	case 3:
		NX_MLC_SetLayerPriority(module, NX_MLC_PRIORITY_VIDEOFOURTH); // PRIORITY-0>1>2>video
		break;
	default: printk(KERN_ERR "Not support video priority num(0~3), (%d) \n", prior);
		return;
	}

	NX_MLC_SetTopDirtyFlag(module);

	vid->priority = prior;
}

int  soc_dpc_get_vd_priority(int module)
{
	struct vid_data * vid = VID_DATA(module);

	DBGOUT("%s (m:%d)\n", __func__, module);

	return (int)vid->priority;
}

void  soc_dpc_set_vd_colorkey(int module, u_int color, int enable)
{
	struct vid_data * vid = VID_DATA(module);

	DBGOUT("%s (m:%d, c:0x%x, e:%d)\n", __func__, module, color, enable);

	soc_dpc_set_rgb_color(module, vid->lscreen, RGB_COL_TRANSP, color, enable);
}

u_int soc_dpc_get_vd_colorkey(int module)
{
	struct vid_data * vid = VID_DATA(module);

	DBGOUT("%s (m:%d)\n", __func__, module);

	return soc_dpc_get_rgb_color(module, vid->lscreen, RGB_COL_TRANSP);
}

void soc_dpc_set_vd_color(int module, u_int type, u_int color, int enable)
{
	struct vid_data * vid = VID_DATA(module);
	int sv,	cv;
	double ang;

	DBGOUT("%s (m:%d, t:0x%x, c:0x%x, e:%d)\n", __func__, module, type, color, enable);

	switch (type) {
	case VID_COL_ALPHA:
		{
			vid->color.alpha = (enable ? color : 15);		// Default 15
			set_vd_alpha(module, vid->color.alpha, enable);
		}
		break;

	case VID_COL_BRIGHT:
		{
			vid->color.bright   = (enable ? color : 0);
			vid->color.contrast = (enable?vid->color.contrast:0);
			set_vd_lu_enhance(module, (int)vid->color.bright, (int)vid->color.contrast);
		}
		break;

	case VID_COL_CONTRAST:
		{
			vid->color.contrast = (enable ? color : 0);
			vid->color.bright   = (enable ? vid->color.bright : 0);
			set_vd_lu_enhance(module, (int)vid->color.bright, (int)vid->color.contrast);
		}
		break;

	case VID_COL_HUE:
		{
 			if ((int)color <=   0) color = 0;
 			if ((int)color >= 360) color = 360;

 			vid->color.hue 		  = (enable ? (int)color : 0);
 			vid->color.saturation = (enable ? vid->color.saturation : 1);

			ang = DEGREE_RADIAN(vid->color.hue);

   			sv = (ksin(ang) * 64 * vid->color.saturation);
   			cv = (kcos(ang) * 64 * vid->color.saturation);

			DBGOUT("hue(h:0x%08lx, s:0x%08lx, cv:0x%08x, sv:0x%08x)\n",
				(ulong)vid->color.hue, (ulong)vid->color.saturation, cv, sv);

   			set_vd_cbr_enhance(module, cv, -sv, sv, cv);
   		}
		break;

	case VID_COL_SATURATION:
		{
	 		if ((int)color <= -100) color = -100;
			if ((int)color >=  100) color =  100;

			vid->color.hue 		  = (enable ? vid->color.hue : 0);
			vid->color.saturation = (enable ? 1 + (0.01 * (int)color) : 1);
			vid->color.satura 	  = (enable ? (int)color : 0);

			ang = (DEGREE_RADIAN(vid->color.hue));

   			sv  = (ksin(ang) * 64 * vid->color.saturation);
   			cv  = (kcos(ang) * 64 * vid->color.saturation);

			DBGOUT("saturation(h:0x%08lx, s:0x%08lx, cv:0x%08x, sv:0x%08x)\n",
				(ulong)vid->color.hue, (ulong)vid->color.saturation, cv, sv);

			set_vd_cbr_enhance(module, cv, -sv, sv, cv);
		}
		break;

	case VID_COL_GAMMA:
		{
			vid->color.gamma = color;
		}
		break;

	default:
		break;
	}
}

u_int  soc_dpc_get_vd_color(int module, u_int type)
{
	struct vid_data * vid = VID_DATA(module);

	DBGOUT("%s (m:%d, t:%d)\n", __func__, module, type);

	switch (type) {
	case VID_COL_ALPHA:			return (u_int)vid->color.alpha;
	case VID_COL_BRIGHT:		return (u_int)vid->color.bright;
	case VID_COL_CONTRAST:		return (u_int)vid->color.contrast;
	case VID_COL_HUE:			return (u_int)vid->color.hue;
	case VID_COL_SATURATION:	return (u_int)vid->color.satura;
	case VID_COL_GAMMA:			return (u_int)vid->color.gamma;
	default: 					break;
	}
	return (-1);
}

void soc_dpc_set_vd_hfilter(int module, int enable)
{
	struct vid_data * vid = VID_DATA(module);
	CBOOL hl, hc, vl, vc;

	hl = hc = vid->h_filter = enable;
	vl = vc = vid->v_filter;

	NX_MLC_SetVideoLayerScaleFilter(module, hl, hc, vl, vc);
}

u_int soc_dpc_get_vd_hfilter(int module)
{
	CBOOL hl, hc, vl, vc;

	NX_MLC_GetVideoLayerScaleFilter(module, &hl, &hc, &vl, &vc);

	if (hl != hc) {
		printk(KERN_INFO "%s: WARN %d horizontal filter Lu=%s, Ch=%s \r\n",
			__func__, module, hl?"On":"Off", hc?"On":"Off" );
	}
	return (u_int)hl;
}

void soc_dpc_set_vd_vfilter(int module, int enable)
{
	struct vid_data * vid = VID_DATA(module);
	CBOOL hl, hc, vl, vc;

	hl = hc = vid->h_filter;
	vl = vc = vid->v_filter = enable;

	NX_MLC_SetVideoLayerScaleFilter(module, hl, hc, vl, vc);
}

u_int soc_dpc_get_vd_vfilter(int module)
{
	CBOOL hl, hc, vl, vc;

	NX_MLC_GetVideoLayerScaleFilter(module, &hl, &hc, &vl, &vc);

	if (hl != hc) {
		printk(KERN_INFO "%s: WARN %d vertical filter Lu=%s, Ch=%s \r\n",
			__func__, module, vl?"On":"Off", vc?"On":"Off");
	}

	return (u_int)vl;
}

/*------------------------------------------------------------------------------
 * Display layer "RGB" control
 */
int  soc_dpc_set_rgb_format(
		int module, int layer, u_int format, int width, int height, int pixelbyte
		)
{
	struct rgb_data * rgb = RGB_DATA(module, layer);

	DBGOUT("%s (m:%d, l:%d, f:0x%x, w:%d, h:%d, b:%d)\n",
		__func__, module, layer, format, width, height, pixelbyte);

	if (NX_MLC_GetLayerEnable(module, layer)) {
		printk(KERN_ERR "dpc:%d layer:%d is running ...\n", module, layer);
		return (-1);
	}

	rgb->format    = format;
	rgb->srcw	   = width;
	rgb->srch      = height;
	rgb->pixelbyte = pixelbyte;

	NX_MLC_SetLockSize(module, layer, CFG_DISP_PRI_MLC_LOCKSIZE);
	NX_MLC_SetAlphaBlending(module, layer, CFALSE, rgb->color.alpha);
	NX_MLC_SetTransparency(module, layer, CFALSE, rgb->color.transcolor);
	NX_MLC_SetColorInversion(module, layer, CFALSE, rgb->color.invertcolor);
	NX_MLC_SetFormatRGB(module, layer, (NX_MLC_RGBFMT)format);
	NX_MLC_SetRGBLayerInvalidPosition(module, layer, 0, 0, 0, 0, 0, CFALSE);
	NX_MLC_SetRGBLayerInvalidPosition(module, layer, 1, 0, 0, 0, 0, CFALSE);

	if (width && height)
		NX_MLC_SetPosition(module, layer, 0, 0, width, height);

	NX_MLC_SetDirtyFlag(module, layer);
	return 0;
}

void  soc_dpc_get_rgb_format(
		int module, int layer, u_int *format, int *width, int *height, int *pixelbyte
		)
{
	struct rgb_data * rgb = RGB_DATA(module, layer);

	DBGOUT("%s (m:%d, l:%d)\n", __func__, module, layer);

	*format    = rgb->format;
	*width	   = rgb->srcw;
	*height    = rgb->srch;
	*pixelbyte = rgb->pixelbyte;
}

void soc_dpc_set_rgb_position(int module, int layer, int startx, int starty, int waitvsync)
{
	struct rgb_data * rgb = RGB_DATA(module, layer);

	int left   = startx;
	int top    = starty;
	int right  = startx + rgb->srcw;
	int bottom = starty + rgb->srch;

	DBGOUT("%s (m:%d, l:%d, - l:%d, t:%d, r:%d, b:%d, w:%d)\n",
		__func__, module, layer, left, top, right, bottom, waitvsync);

	NX_MLC_SetPosition(module, layer, left, top, right-1, bottom-1);
	NX_MLC_SetDirtyFlag(module, layer);

	/* wait sysnc */
	if (waitvsync)
		wait_dirty_clear(module, layer);
}

void soc_dpc_set_rgb_invalidzone(
	int module, int layer, int num, int left, int top,
	int right, int bottom, int enable, int waitvsync
	)
{
	DBGOUT("%s (m:%d, l:%d, n:%d - l:%d, t:%d, r:%d, b:%d, e:%d, w:%d)\n",
		__func__, module, layer, num, left, top, right, bottom, enable, waitvsync);

	NX_MLC_SetRGBLayerInvalidPosition(module, layer, num, left, top, right, bottom, enable);
	NX_MLC_SetDirtyFlag(module, layer);

	/* wait sysnc */
	if (waitvsync)
		wait_dirty_clear(module, layer);
}

void soc_dpc_set_rgb_address(
	int module, int layer, u_int addr, u_int hstride, u_int vstride, int waitvsync
	)
{
	DBGOUT("%s (m:%d, l:%d, a:0x%x, h:%d, v:%d, w:%d)\n",
		__func__, module, layer, addr, hstride, vstride, waitvsync);

	NX_MLC_SetRGBLayerStride (module, layer, hstride, vstride);
	NX_MLC_SetRGBLayerAddress(module, layer, addr);
	NX_MLC_SetDirtyFlag(module, layer);

	/* wait sysnc */
	if (waitvsync)
		wait_dirty_clear(module, layer);
}

void soc_dpc_set_rgb_enable(int module, int layer, int enable)
{
	DBGOUT("%s (m:%d, l:%d, e:%d)\n", __func__, module, layer, enable);

	NX_MLC_SetLayerEnable(module, layer, (enable ? CTRUE : CFALSE));
	NX_MLC_SetDirtyFlag(module, layer);
}

void  soc_dpc_set_rgb_color(int module, int layer, u_int type, u_int color, int enable)
{
	struct rgb_data * rgb = RGB_DATA(module, layer);

	DBGOUT("%s (m:%d, l:%d, t:%d, c:0x%x, e:%d)\n",
		 __func__, module, layer, type, color, enable);

	switch (type) {
	case RGB_COL_ALPHA:
		{
			if (color <= 0 )	color = 0;
			if (color >= 15)	color = 15;

			rgb->color.alpha = (enable?color:15);

			NX_MLC_SetAlphaBlending(module, layer, (enable ? CTRUE : CFALSE), color);
			NX_MLC_SetDirtyFlag(module, layer);
		}
		break;

	case RGB_COL_TRANSP:
		{
			if (1 == rgb->pixelbyte)
				color = R8G8B8toR3G3B2((u_int)color), color = R3G3B2toR8G8B8((u_char)color);
			if (2 == rgb->pixelbyte)
				color = R8G8B8toR5G6B5((u_int)color), color = R5G6B5toR8G8B8((u_short)color);

			rgb->color.transcolor = (enable?color:0);

			NX_MLC_SetTransparency(module, layer, (enable ? CTRUE : CFALSE), (color & 0x00FFFFFF));
			NX_MLC_SetDirtyFlag(module, layer);
		}
		break;

	case RGB_COL_INVERT:
		{
			if (1 == rgb->pixelbyte)
				color = R8G8B8toR3G3B2((u_int)color), color = R3G3B2toR8G8B8((u_char)color);
			if (2 == rgb->pixelbyte)
				color = R8G8B8toR5G6B5((u_int)color), color = R5G6B5toR8G8B8((u_short)color);

			rgb->color.invertcolor = (enable?color:0);

			NX_MLC_SetColorInversion(module, layer, (enable ? CTRUE : CFALSE), (color & 0x00FFFFFF));
			NX_MLC_SetDirtyFlag(module, layer);
		}
		break;

	default:
		break;
	}
}

u_int  soc_dpc_get_rgb_color(int module, int layer, u_int type)
{
	struct rgb_data * rgb = RGB_DATA(module, layer);

	DBGOUT("%s (m:%d, l:%d, t:%d)\n", __func__, module, layer, type);

	switch (type) {
	case RGB_COL_ALPHA:		return (u_int)rgb->color.alpha;
	case RGB_COL_TRANSP:	return (u_int)rgb->color.transcolor;
	case RGB_COL_INVERT:	return (u_int)rgb->color.invertcolor;
	default: 				break;
	}
	return 0;
}


/*------------------------------------------------------------------------------
 * External LCD device
 */
int	soc_dpc_set_lcd_out(int module, u_int format, u_int type)
{
	struct dpc_info * dpc = DPC_INFO(module);
	int ret = -1;

	DBGOUT("%s (m:%d, f:%x, t:%x)\n", __func__, module, format, type);

	if (dpc->lcdops && dpc->lcdops->setformat)
		ret = dpc->lcdops->setformat(module, format, type, 0);

	return ret;
}

int	soc_dpc_get_lcd_out(int module, u_int *format, u_int *type)
{
	struct dpc_info * dpc = DPC_INFO(module);
	int ret = -1;

	DBGOUT("%s (m:%d)\n", __func__, module);

	if (dpc->lcdops && dpc->lcdops->getformat)
		ret = dpc->lcdops->getformat(module, format, type);

	return ret;
}

/*------------------------------------------------------------------------------
 *	External Backlight Unit
 */
int soc_dpc_register_blu(u_int dev, struct dpc_blu_ops *ops)
{
	struct dpc_info * dpc = DPC_INFO(dev);

	DBGOUT("%s (m:%d, ops:0x%x)\n", __func__, dev, (u_int)ops);

	if (dpc->bluops) {
		printk(KERN_ERR "fail, module(%d) backlight unit ops exist ...\n", dev);
		return (-1);
	}

	dpc->bluops = ops;
	return 0;
}

/*------------------------------------------------------------------------------
 *	External LCD Unit
 */
int soc_dpc_register_lcd(u_int dev, struct dpc_lcd_ops *ops)
{
	struct dpc_info * dpc = DPC_INFO(dev);

	DBGOUT("%s (m:%d, ops:0x%x)\n", __func__, dev, (u_int)ops);

	if (dpc->lcdops) {
		printk(KERN_ERR "fail, module(%d) LCD unit ops exist ...\n", dev);
		return (-1);
	}

	dpc->lcdops = ops;
	return 0;
}

#if DUMP_REG
static void dbg_reg_dpc(int dev)
{
/*
	U32 DPCBASE = IO_ADDRESS(0xC0003000 + (0x400*dev));
	printk("==========================================================\n");
	printk(" DPCHTOTAL				= 0x%04x\n", *(U16*)(DPCBASE + 0x07C));
	printk(" DPCHSWIDTH				= 0x%04x\n", *(U16*)(DPCBASE + 0x07E));
	printk(" DPCHASTART				= 0x%04x\n", *(U16*)(DPCBASE + 0x080));
	printk(" DPCHAEND				= 0x%04x\n", *(U16*)(DPCBASE + 0x082));
	printk(" DPCVTOTAL				= 0x%04x\n", *(U16*)(DPCBASE + 0x084));
	printk(" DPCVSWIDTH				= 0x%04x\n", *(U16*)(DPCBASE + 0x086));
	printk(" DPCVASTART				= 0x%04x\n", *(U16*)(DPCBASE + 0x088));
	printk(" DPCVAEND				= 0x%04x\n", *(U16*)(DPCBASE + 0x08A));
	printk(" DPCCTRL1				= 0x%04x\n", *(U16*)(DPCBASE + 0x08E));
	printk(" DPCEVTOTAL				= 0x%04x\n", *(U16*)(DPCBASE + 0x090));
	printk(" DPCEVSWIDTH			= 0x%04x\n", *(U16*)(DPCBASE + 0x092));
	printk(" DPCEVASTART			= 0x%04x\n", *(U16*)(DPCBASE + 0x094));
	printk(" DPCEVAEND				= 0x%04x\n", *(U16*)(DPCBASE + 0x096));
	printk(" DPCCTRL2				= 0x%04x\n", *(U16*)(DPCBASE + 0x098));
	printk(" DPCVSEOFFSET			= 0x%04x\n", *(U16*)(DPCBASE + 0x09A));
	printk(" DPCVSSOFFSET			= 0x%04x\n", *(U16*)(DPCBASE + 0x09C));
	printk(" DPCEVSEOFFSET			= 0x%04x\n", *(U16*)(DPCBASE + 0x09E));
	printk(" DPCEVSSOFFSET			= 0x%04x\n", *(U16*)(DPCBASE + 0x0A0));
	printk(" DPCDELAY0				= 0x%04x\n", *(U16*)(DPCBASE + 0x0A2));
	printk(" DPCUPSCALECON0 		= 0x%04x\n", *(U16*)(DPCBASE + 0x0A4));
	printk(" DPCUPSCALECON1 		= 0x%04x\n", *(U16*)(DPCBASE + 0x0A6));
	printk(" DPCUPSCALECON2 		= 0x%04x\n", *(U16*)(DPCBASE + 0x0A8));
	printk(" DPCCLKENB       		= 0x%08x\n", *(U32*)(DPCBASE + 0x1C0));
	printk(" DPCCLKGEN[0][0] 		= 0x%08x\n", *(U32*)(DPCBASE + 0x1C4));
	printk(" DPCCLKGEN[0][1] 		= 0x%08x\n", *(U32*)(DPCBASE + 0x1C8));
	printk(" DPCCLKGEN[1][0] 		= 0x%08x\n", *(U32*)(DPCBASE + 0x1CC));
	printk(" DPCCLKGEN[1][1] 		= 0x%08x\n", *(U32*)(DPCBASE + 0x1D0));
	printk(" DPCCTRL0				= 0x%04x\n", *(U16*)(DPCBASE + 0x08C));
	printk("==========================================================\n");
*/
}

static void dbg_reg_mlc(int dev)
{
	struct NX_MLC_RegisterSet *MLC =
		(struct NX_MLC_RegisterSet*)IO_ADDRESS(NX_MLC_GetPhysicalAddress(dev));

	printk("==========================================================\n");
	printk("MLCBASE			    	    =0x%08x -> 0x%08x\n", (u_int)NX_MLC_GetPhysicalAddress(dev), (u_int)MLC);
	printk("MLCCONTROLT			    	=0x%08x\n", MLC->MLCCONTROLT);
	printk("MLCSCREENSIZE		    	=0x%08x\n", MLC->MLCSCREENSIZE);
	printk("MLCBGCOLOR		      		=0x%08x\n", MLC->MLCBGCOLOR);

	// MLCRGBLAYER[0]
	printk("RGB_0 MLCLEFTRIGHT	        =0x%08x\n", MLC->MLCRGBLAYER[0].MLCLEFTRIGHT);
	printk("RGB_0 MLCTOPBOTTOM 	        =0x%08x\n", MLC->MLCRGBLAYER[0].MLCTOPBOTTOM);
	printk("RGB_0 MLCINVALIDLEFTRIGHT0  =0x%08x\n", MLC->MLCRGBLAYER[0].MLCINVALIDLEFTRIGHT0);
	printk("RGB_0 MLCINVALIDTOPBOTTOM0  =0x%08x\n", MLC->MLCRGBLAYER[0].MLCINVALIDTOPBOTTOM0);
	printk("RGB_0 MLCINVALIDLEFTRIGHT1  =0x%08x\n", MLC->MLCRGBLAYER[0].MLCINVALIDLEFTRIGHT1);
	printk("RGB_0 MLCINVALIDTOPBOTTOM1  =0x%08x\n", MLC->MLCRGBLAYER[0].MLCINVALIDTOPBOTTOM1);
	printk("RGB_0 MLCCONTROL	        =0x%08x\n", MLC->MLCRGBLAYER[0].MLCCONTROL);
	printk("RGB_0 MLCHSTRIDE	        =0x%08x\n", MLC->MLCRGBLAYER[0].MLCHSTRIDE);
	printk("RGB_0 MLCVSTRIDE	        =0x%08x\n", MLC->MLCRGBLAYER[0].MLCVSTRIDE);
	printk("RGB_0 MLCTPCOLOR	        =0x%08x\n", MLC->MLCRGBLAYER[0].MLCTPCOLOR);
	printk("RGB_0 MLCINVCOLOR		    =0x%08x\n", MLC->MLCRGBLAYER[0].MLCINVCOLOR);
	printk("RGB_0 MLCADDRESS	        =0x%08x\n", MLC->MLCRGBLAYER[0].MLCADDRESS);

	// MLCRGBLAYER[1]
	printk("RGB_1 MLCLEFTRIGHT	        =0x%08x\n", MLC->MLCRGBLAYER[1].MLCLEFTRIGHT);
	printk("RGB_1 MLCTOPBOTTOM 	        =0x%08x\n", MLC->MLCRGBLAYER[1].MLCTOPBOTTOM);
	printk("RGB_1 MLCINVALIDLEFTRIGHT0  =0x%08x\n", MLC->MLCRGBLAYER[1].MLCINVALIDLEFTRIGHT0);
	printk("RGB_1 MLCINVALIDTOPBOTTOM0  =0x%08x\n", MLC->MLCRGBLAYER[1].MLCINVALIDTOPBOTTOM0);
	printk("RGB_1 MLCINVALIDLEFTRIGHT1  =0x%08x\n", MLC->MLCRGBLAYER[1].MLCINVALIDLEFTRIGHT1);
	printk("RGB_1 MLCINVALIDTOPBOTTOM1  =0x%08x\n", MLC->MLCRGBLAYER[1].MLCINVALIDTOPBOTTOM1);
	printk("RGB_1 MLCCONTROL	        =0x%08x\n", MLC->MLCRGBLAYER[1].MLCCONTROL);
	printk("RGB_1 MLCHSTRIDE	        =0x%08x\n", MLC->MLCRGBLAYER[1].MLCHSTRIDE);
	printk("RGB_1 MLCVSTRIDE	        =0x%08x\n", MLC->MLCRGBLAYER[1].MLCVSTRIDE);
	printk("RGB_1 MLCTPCOLOR	        =0x%08x\n", MLC->MLCRGBLAYER[1].MLCTPCOLOR);
	printk("RGB_1 MLCINVCOLOR		    =0x%08x\n", MLC->MLCRGBLAYER[1].MLCINVCOLOR);
	printk("RGB_1 MLCADDRESS	        =0x%08x\n", MLC->MLCRGBLAYER[1].MLCADDRESS);

	// MLCVIDEOLAYER
	printk("VIDEO MLCLEFTRIGHT          =0x%08x\n", MLC->MLCVIDEOLAYER.MLCLEFTRIGHT);
	printk("VIDEO MLCTOPBOTTOM			=0x%08x\n", MLC->MLCVIDEOLAYER.MLCTOPBOTTOM);
	printk("VIDEO MLCCONTROL	        =0x%08x\n", MLC->MLCVIDEOLAYER.MLCCONTROL);
	printk("VIDEO MLCVSTRIDE	        =0x%08x\n", MLC->MLCVIDEOLAYER.MLCVSTRIDE);
	printk("VIDEO MLCTPCOLOR	        =0x%08x\n", MLC->MLCVIDEOLAYER.MLCTPCOLOR);
	printk("VIDEO MLCADDRESS	        =0x%08x\n", MLC->MLCVIDEOLAYER.MLCADDRESS);
	printk("VIDEO MLCADDRESSCB	        =0x%08x\n", MLC->MLCVIDEOLAYER.MLCADDRESSCB);
	printk("VIDEO MLCADDRESSCR	        =0x%08x\n", MLC->MLCVIDEOLAYER.MLCADDRESSCR);
	printk("VIDEO MLCVSTRIDECB	        =0x%08x\n", MLC->MLCVIDEOLAYER.MLCVSTRIDECB);
	printk("VIDEO MLCVSTRIDECR	        =0x%08x\n", MLC->MLCVIDEOLAYER.MLCVSTRIDECR);
	printk("VIDEO MLCHSCALE	            =0x%08x\n", MLC->MLCVIDEOLAYER.MLCHSCALE);
	printk("VIDEO MLCVSCALE	            =0x%08x\n", MLC->MLCVIDEOLAYER.MLCVSCALE);
	printk("VIDEO MLCLUENH	            =0x%08x\n", MLC->MLCVIDEOLAYER.MLCLUENH);
	printk("VIDEO MLCCHENH[0]			=0x%08x\n", MLC->MLCVIDEOLAYER.MLCCHENH[0]);
	printk("VIDEO MLCCHENH[1]			=0x%08x\n", MLC->MLCVIDEOLAYER.MLCCHENH[1]);
	printk("VIDEO MLCCHENH[2]			=0x%08x\n", MLC->MLCVIDEOLAYER.MLCCHENH[2]);
	printk("VIDEO MLCCHENH[3]			=0x%08x\n", MLC->MLCVIDEOLAYER.MLCCHENH[3]);

	// MLCRGBLAYER2
	printk("RGB_2 MLCLEFTRIGHT	        =0x%08x\n", MLC->MLCRGBLAYER2.MLCLEFTRIGHT);
	printk("RGB_2 MLCTOPBOTTOM 	        =0x%08x\n", MLC->MLCRGBLAYER2.MLCTOPBOTTOM);
	printk("RGB_2 MLCINVALIDLEFTRIGHT0  =0x%08x\n", MLC->MLCRGBLAYER2.MLCINVALIDLEFTRIGHT0);
	printk("RGB_2 MLCINVALIDTOPBOTTOM0  =0x%08x\n", MLC->MLCRGBLAYER2.MLCINVALIDTOPBOTTOM0);
	printk("RGB_2 MLCINVALIDLEFTRIGHT1  =0x%08x\n", MLC->MLCRGBLAYER2.MLCINVALIDLEFTRIGHT1);
	printk("RGB_2 MLCINVALIDTOPBOTTOM1	=0x%08x\n", MLC->MLCRGBLAYER2.MLCINVALIDTOPBOTTOM1);
	printk("RGB_2 MLCCONTROL	        =0x%08x\n", MLC->MLCRGBLAYER2.MLCCONTROL);
	printk("RGB_2 MLCHSTRIDE	        =0x%08x\n", MLC->MLCRGBLAYER2.MLCHSTRIDE);
	printk("RGB_2 MLCVSTRIDE	        =0x%08x\n", MLC->MLCRGBLAYER2.MLCVSTRIDE);
	printk("RGB_2 MLCTPCOLOR	        =0x%08x\n", MLC->MLCRGBLAYER2.MLCTPCOLOR);
	printk("RGB_2 MLCINVCOLOR 	        =0x%08x\n", MLC->MLCRGBLAYER2.MLCINVCOLOR);
	printk("RGB_2 MLCADDRESS	        =0x%08x\n", MLC->MLCRGBLAYER2.MLCADDRESS);

	printk("MLCGAMMACONT	        	=0x%08x\n", MLC->MLCGAMMACONT);
	printk("MLCRGAMMATABLEWRITE			=0x%08x\n", MLC->MLCRGAMMATABLEWRITE);
	printk("MLCGGAMMATABLEWRITE			=0x%08x\n", MLC->MLCGGAMMATABLEWRITE);
	printk("MLCBGAMMATABLEWRITE			=0x%08x\n", MLC->MLCBGAMMATABLEWRITE);
	printk("MLCCLKENB;              	=0x%08x\n", MLC->MLCCLKENB);
	printk("==========================================================\n");
}
#endif

/*----------------------------------------------------------------------------*/
/* DISPLAY */
EXPORT_SYMBOL(soc_dpc_get_mode);
EXPORT_SYMBOL(soc_dpc_get_resolution);
EXPORT_SYMBOL(soc_dpc_set_bgcolor);
EXPORT_SYMBOL(soc_dpc_get_bgcolor);
EXPORT_SYMBOL(soc_dpc_wait_vsync);
EXPORT_SYMBOL(soc_dpc_set_out_enable);
EXPORT_SYMBOL(soc_dpc_set_out_disable);
EXPORT_SYMBOL(soc_dpc_set_suspend);
EXPORT_SYMBOL(soc_dpc_set_resume);
/* LAYER */
EXPORT_SYMBOL(soc_dpc_set_layer_enable);
EXPORT_SYMBOL(soc_dpc_get_layer_enable);
EXPORT_SYMBOL(soc_dpc_wait_dirtyclear);
/* VIDEO */
EXPORT_SYMBOL(soc_dpc_set_vd_format);
EXPORT_SYMBOL(soc_dpc_get_vd_format);
EXPORT_SYMBOL(soc_dpc_set_vd_address);
EXPORT_SYMBOL(soc_dpc_set_vd_position);
EXPORT_SYMBOL(soc_dpc_set_vd_enable);
EXPORT_SYMBOL(soc_dpc_set_vd_priority);
EXPORT_SYMBOL(soc_dpc_get_vd_priority);
EXPORT_SYMBOL(soc_dpc_set_vd_colorkey);
EXPORT_SYMBOL(soc_dpc_get_vd_colorkey);
EXPORT_SYMBOL(soc_dpc_set_vd_color);
EXPORT_SYMBOL(soc_dpc_get_vd_color);
EXPORT_SYMBOL(soc_dpc_set_vd_hfilter);
EXPORT_SYMBOL(soc_dpc_get_vd_hfilter);
EXPORT_SYMBOL(soc_dpc_set_vd_vfilter);
EXPORT_SYMBOL(soc_dpc_get_vd_vfilter);
/* RGB */
EXPORT_SYMBOL(soc_dpc_set_rgb_format);
EXPORT_SYMBOL(soc_dpc_get_rgb_format);
EXPORT_SYMBOL(soc_dpc_set_rgb_position);
EXPORT_SYMBOL(soc_dpc_set_rgb_invalidzone);
EXPORT_SYMBOL(soc_dpc_set_rgb_address);
EXPORT_SYMBOL(soc_dpc_set_rgb_enable);
EXPORT_SYMBOL(soc_dpc_set_rgb_color);
EXPORT_SYMBOL(soc_dpc_get_rgb_color);
/* EXT LCD */
EXPORT_SYMBOL(soc_dpc_set_lcd_out);
EXPORT_SYMBOL(soc_dpc_get_lcd_out);
/* External device */
EXPORT_SYMBOL(soc_dpc_register_blu);
EXPORT_SYMBOL(soc_dpc_register_lcd);
