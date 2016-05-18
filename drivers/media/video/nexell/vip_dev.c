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
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kmod.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <asm/uaccess.h>
#include <asm/system.h>

/* nexell soc headers */
#include <mach/soc.h>

#include "vip_drv.h"
#include "vip_dev.h"
#include "vip_buf.h"

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "vip-dev: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#define ERROUT(msg...)		{ 	\
		printk(KERN_ERR "ERROR: %s, %s line %d: \n",	\
			__FILE__, __FUNCTION__, __LINE__),			\
		printk(KERN_ERR msg); }

#define	WAIT_TIMEOUT	(500)			/* wait 500 msec */

/*------------------------------------------------------------------------------
 * suppor format
 */
static struct vip_fmt out_formats[] = {
	{
		.name     	= "4:2:2, packed, YUYV",
		.fourcc		= V4L2_PIX_FMT_YUYV,
		.linear		=  1,
		.depth    	= 16,
		.type		= V4L2_BUF_TYPE_VIDEO_CAPTURE,
	},
	{
		.name     	= "4:2:0, separated 2D, YUV420",
		.fourcc		= FOURCC_MVS0,
		.linear		=  0,
		.depth    	= 12,
		.type		= V4L2_BUF_TYPE_VIDEO_OVERLAY,
	},
	{
		.name     	= "4:2:2, separated 2D, YUV422",
		.fourcc		= FOURCC_MVS2,
		.linear		=  0,
		.depth    	= 16,
		.type		= V4L2_BUF_TYPE_VIDEO_OVERLAY,
	},
	{
		.name     	= "4:4:4, separated 2D, YUV444",
		.fourcc		= FOURCC_MVS4,
		.linear		=  0,
		.depth    	= 24,
		.type		= V4L2_BUF_TYPE_VIDEO_OVERLAY,
	},
};

/*------------------------------------------------------------------------------
 * globals
 */
static struct vip_dev  *	vipdev = NULL;

static struct vip_sync 		ext_syc  = {0 , };
static struct vip_signal 	ext_sig  = {
	.ex_fd_sig = 0,
	.ex_fd_sel = 0,
	.ex_vd_sig = 0,
	.ex_vd_pol = 0,
};

/*----------------------------------------------------------------------------*/
#define	STAT_PWR_OFF	0x0000
#define	STAT_PWR_ON		0x0001
#define	STAT_DEV_RUN	0x0010
#define	STAT_ISR_RUN	0x0100
#define	STAT_THR_RUN	0x1000

static int dev_status = STAT_PWR_OFF;
static int skip_count = 3;

static void set_power_on(void)
{
	DBGOUT("%s (status 0x%x)\n", __func__, dev_status);

	if (STAT_PWR_ON & dev_status)
		return;

	NX_VIP_SetClockPClkMode(NX_PCLKMODE_ALWAYS); 		// CLKENB: PCLKMODE
	NX_VIP_SetClockBClkMode(NX_BCLKMODE_DYNAMIC);		// CLKENB: BCLKMODE
	NX_VIP_SetClockSource (0, NX_VIP_CLKSRC_ICLKIN);	// CLKGEN: ICLK
	NX_VIP_SetClockDivisor(0, 1);
	NX_VIP_SetClockDivisorEnable(CTRUE);

	dev_status |= STAT_PWR_ON;
}

static void set_power_off(void)
{
	volatile int count = 0;

	DBGOUT("%s (status 0x%x)\n", __func__, dev_status);

	if (STAT_PWR_OFF & dev_status)
		return;

	/* Wait until ISR stopped. */
	while ((STAT_ISR_RUN & dev_status) && 1000 > count++)
		msleep(1);

	DBGOUT("wait for power off (time:%d)\r\n", count);

	NX_VIP_SetClockDivisorEnable(CFALSE);

	dev_status &= ~STAT_PWR_ON;
}

static u_int get_format(u_int fourcc)
{
	u_int format = -1;
	switch(fourcc) {
	case FOURCC_MVS0: format = NX_VIP_FORMAT_420;
		break;
	case FOURCC_MVS2: format = NX_VIP_FORMAT_422;
		break;
	case FOURCC_MVS4: format = NX_VIP_FORMAT_444;
		break;
	case FOURCC_YUYV: format = NX_VIP_FORMAT_YUYV;
		break;
	default:
		ERROUT("fail, not support fourcc (%c%c%c%c) \n",
			fourcc&0xFF,(fourcc>>8)&0xFF,(fourcc>>16)&0xFF,(fourcc>>24)&0xFF);
		format = -1;
	}
	return format;
}

static void get_params(struct vip_output *out, struct vip_data *dat)
{
	out->out_dev	= dat->out_dev;
	out->scan_mode	= dat->scan_mode;
	out->fourcc		= dat->fourcc;
	out->c_left		= dat->c_left;
	out->c_top		= dat->c_top;
	out->c_right	= dat->c_right;
	out->c_bottom	= dat->c_bottom;
	out->s_width	= dat->s_width;
	out->s_height	= dat->s_height;
	out->rotate		= dat->rotate;
}

static void set_params(struct vip_data *dat, struct vip_output *out)
{
	dat->out_dev	= out->out_dev;
	dat->scan_mode	= out->scan_mode;
	dat->fourcc		= out->fourcc;
	dat->format		= get_format(out->fourcc);

	/* clip zone: clipper */
	dat->c_left		= out->c_left;
	dat->c_top		= out->c_top;
	dat->c_right	= out->c_right;
	dat->c_bottom	= out->c_bottom;

	/* scale down: decimator */
	dat->s_width	= out->s_width;
	dat->s_height	= out->s_height;

	/* out mode */
	dat->width		= out->width;
	dat->height		= out->height;
	dat->stride 	= out->fourcc == FOURCC_YUYV ? (out->width * 2) : 4096;
	dat->rotate 	= out->rotate & VIP_ROTATE_ENABLE ? 1 : 0;
	dat->v_flip		= out->rotate & VIP_V_FLIP_ENABLE ? 1 : 0;
	dat->h_flip		= out->rotate & VIP_H_FLIP_ENABLE ? 1 : 0;
}

static int	check_params(struct vip_output *out)
{
	struct vip_sync *syc = &ext_syc;

	int   rot_out = out->rotate & VIP_ROTATE_ENABLE ? 1 : 0;
	int   int_out = syc->interlace;
	int   img_w, img_h;
	int   clp_w, clp_h, scl_w, scl_h;

	/* Check scanmode. */
	if (0 == int_out && out->scan_mode == SCAN_INTERLACE)	{
		ERROUT("fail, interlace out, input is progress.\n");
		return -EINVAL;
	}

	if (int_out && out->scan_mode == SCAN_PROGRESS && out->rotate) {
		ERROUT("fail, rotate, interace input and progress out.\n");
		return -EINVAL;
	}

	/* Check clipping zone.
	 * note: Clip width must be multiple of 16. */
	out->c_left   = (out->c_left /16)*16;
	out->c_right  = (out->c_right/16)*16;
	out->c_top    = (out->c_top);
	out->c_bottom = (out->c_bottom);

	if ( (out->c_left >= out->c_right) ||
		(out->c_top  >= out->c_bottom)) {
		ERROUT("fail, the clip(r:%d, b:%d) is less than (l:%d, t:%d).\n",
			out->c_right, out->c_bottom, out->c_left, out->c_top);
		return -EINVAL;
	}

	clp_w = syc->h_active;
	clp_h = syc->v_active;

	if ( out->c_left > clp_w || out->c_right > clp_w ) {
		ERROUT("fail, the clip(l:%d and r:%d) is more than hor active(%d).\n",
			out->c_left, out->c_right, clp_w);
		return -EINVAL;
	}

	if ( out->c_top > clp_h ||
		out->c_bottom > clp_h ) {
		ERROUT("fail, the clip(t:%d and b:%d) is more than ver active(%d).\n",
			out->c_top, out->c_bottom, clp_h);
		return -EINVAL;
	}

	/* Check scaledown length
 	 * note: width must be multiple of 16. */
	out->s_width  = (out->s_width /16)*16;
	out->s_height = (out->s_height);

	scl_w = out->c_right  - out->c_left;
	scl_h = out->c_bottom - out->c_top;

	/* Scale down length must be less than Clip region */
	if (VIP_DECIMATOR == out->out_dev) {
	   	if ((out->s_width > scl_w) || (out->s_height > scl_h)) {
			ERROUT("fail, The scale down (%d*%d) is more than clip (%d*%d).\n",
				out->s_width, out->s_height, scl_w, scl_h);
			return -EINVAL;
		}
	}

	/* Get Image width/height. */
	if (VIP_DECIMATOR == out->out_dev) {
		img_w = out->s_width;
		img_h = out->s_height;
	} else {
		img_w = (out->c_right  - out->c_left);
		img_h = (out->c_bottom - out->c_top) << (int_out ? 1 : 0);
	}

	/* Swap with and height */
	if (rot_out) {
		img_w = img_w + img_h;
		img_h = img_w - img_h;
		img_w = img_w - img_h;
	}

	/* Check Output Format : 420/422/444/YUYV */
	if (0 > get_format(out->fourcc))
		return -1;

	if (FOURCC_YUYV == out->fourcc) {
		if (VIP_ROTATE_ENABLE & out->rotate) {
			ERROUT("fail, YUYV out not support rotate.\n");
			return -EINVAL;
		}

		if (VIP_DECIMATOR == out->out_dev) {
			ERROUT("fail, decimator not support YUYV.\n");
			return -EINVAL;
		}
	}

	/* Set output parameters. */
	out->width  = img_w;
	out->height = img_h;

	DBGOUT(" <output params>\n");
	DBGOUT(" : out port		= %s \n", out->out_dev==VIP_CLIPPER?"clipper":"decimator");
	DBGOUT(" : scan_mode	= %s \n", out->scan_mode==SCAN_INTERLACE?"interlace":"progress");
	DBGOUT(" : fourCC		= %c%c%c%c \n",
		out->fourcc&0xFF,(out->fourcc>>8)&0xFF,(out->fourcc>>16)&0xFF,(out->fourcc>>24)&0xFF);
	DBGOUT(" : clip left	= %d \n", out->c_left);
	DBGOUT(" : clip top		= %d \n", out->c_top);
	DBGOUT(" : clip right	= %d \n", out->c_right);
	DBGOUT(" : clip bottom	= %d \n", out->c_bottom);
	DBGOUT(" : scale width	= %d \n", out->s_width);
	DBGOUT(" : scale height	= %d \n", out->s_height);
	DBGOUT(" : rotate		= %s \n", out->rotate & VIP_ROTATE_ENABLE?"on":"off");
	DBGOUT(" : hor flip		= %s \n", out->rotate & VIP_H_FLIP_ENABLE?"on":"off");
	DBGOUT(" : ver flip		= %s \n", out->rotate & VIP_V_FLIP_ENABLE?"on":"off");
	DBGOUT(" : out width	= %d \n", out->width);
	DBGOUT(" : out height	= %d \n", out->height);
	DBGOUT(" ------------------------------\n");

	return 0;
}

static void init_params(struct vip_data *dat, struct vip_sync *syc)
{
	/* image max size */
	dat->max_w 		= syc->h_active;
	dat->max_h 		= syc->v_active * (syc->interlace ? 2 : 1);

	/* clip region */
	dat->c_left		= 0;
	dat->c_top		= 0;
	dat->c_right	= syc->h_active & ~(16-1);	/* align 16 */
	dat->c_bottom	= syc->v_active;

	/* scale down */
	dat->s_width	= syc->h_active & ~(16-1);	/* align 16 */
	dat->s_height	= syc->v_active;

	/* out type. */
	dat->out_dev    = VIP_CLIPPER;
	dat->scan_mode  = syc->interlace ? SCAN_INTERLACE : SCAN_PROGRESS;
	dat->fourcc     = VIP_OUT_FOURCC;
	dat->format     = get_format(dat->fourcc);

	if (VIP_CLIPPER == dat->out_dev) {
		dat->width	= (dat->c_right  - dat->c_left);
		dat->height = (dat->c_bottom - dat->c_top) * (syc->interlace ? 2 : 1);
	} else {
		dat->width	= dat->s_width;
		dat->height	= dat->s_height;
	}

	/* rotate */
	dat->rotate	= 0;
	dat->v_flip	= 0;
	dat->h_flip	= 0;
}

static void init_device(struct vip_data *dat)
{
	struct vip_sync   *syc = &ext_syc;
	struct vip_signal *sig = &ext_sig;

	int		h_active = syc->h_active * 2;
	int		v_active = syc->v_active;
	u_int 	format 	 = dat->format;
	u_int   d_order  = dat->d_order;

	int 	clipper   = dat->out_dev == VIP_CLIPPER   ? 1 : 0;
	int 	decimator = dat->out_dev == VIP_DECIMATOR ? 1 : 0;
	int		scl_sw    = dat->c_right - dat->c_left;
	int		scl_sh    = dat->c_bottom- dat->c_top;
	int		scl_dw    = dat->s_width;
	int		scl_dh    = dat->s_height;

	CBOOL	ext_sync = syc->external_sync ? CTRUE : CFALSE;
	CBOOL 	int_out = dat->scan_mode == SCAN_INTERLACE ? CTRUE: CFALSE;
	CBOOL	rot_out	 = dat->rotate ? CTRUE : CFALSE;
	CBOOL	v_flip	 = dat->v_flip ? CTRUE : CFALSE;
	CBOOL	h_flip	 = dat->h_flip ? CTRUE : CFALSE;
	CBOOL	invert   = dat->invert ? CTRUE : CFALSE;

	DBGOUT(" in port   = %d, scan:%s, format:%d, hf:%d, vf:%d, ro:%d\r\n",
		dat->in_port, int_out?"interlace":"progress", format, h_flip, v_flip, rot_out);
	DBGOUT(" decimator = %s, %d * %d -> %d * %d \n",
		decimator?"on ":"off", scl_sw, scl_sh, scl_dw, scl_dh);
	DBGOUT(" clipper   = %s, L:%d, T:%d, R:%d, B:%d \n",
		clipper?"on":"off", dat->c_left, dat->c_top, dat->c_right, dat->c_bottom);

	DBGOUT(" <sync info>\n");
	DBGOUT(" : ext = %d \r\n", syc->external_sync);
	DBGOUT(" : w   = %d \r\n", syc->h_active);
	DBGOUT(" : hsw = %d \r\n", syc->h_sync_width);
	DBGOUT(" : hfp = %d \r\n", syc->h_front_porch);
	DBGOUT(" : hbp = %d \r\n", syc->h_back_porch);
	DBGOUT(" : h   = %d \r\n", syc->v_active);
	DBGOUT(" : vsw = %d \r\n", syc->v_sync_width);
	DBGOUT(" : vfp = %d \r\n", syc->v_front_porch);
	DBGOUT(" : vbp = %d \r\n", syc->v_back_porch);
	DBGOUT(" ------------------------------\n");

	/* syc generation: depend on video decoder or camera decoder's parameter */
	NX_VIP_SetHVSync(
				ext_sync, 					// VIP_CONFIG: EXTSYNCENB-656, DRANGE
				h_active,					// VIP_IMGWIDTH
				v_active,					// VIP_IMGHEIGHT
			   	syc->h_sync_width  * 2,	// HSW: VIP_HEND= hfp+hsw-7
			   	syc->h_front_porch * 2,	// HFP: VIP_HBEGIN
			   	syc->h_back_porch  * 2,	// HBP:
			   	syc->v_sync_width,			// VSW: VIP_VEND=vfp+vsw+1
			   	syc->v_front_porch,		// VFP: VIP_VBEGIN
			   	syc->v_back_porch 			// VBP:
			   	);

	/* Data order, width(fix: 8 bit) */
	NX_VIP_SetDataMode((NX_VIP_DATAORDER)d_order, 8);
	NX_VIP_SetFieldMode(
				sig->ex_fd_sig?CTRUE:CFALSE,		// VIP_SYNCCTRL: EXTFIELDENB
				(NX_VIP_FIELDSEL)sig->ex_fd_sel,	// VIP_SYNCCTRL: FIELDSEL
				int_out,							// VIP_SCANMODE: progressive/interlaced
				invert								// VIP_SCANMODE: FIELDINV(invert/bypass)
				);

	NX_VIP_SetDValidMode(sig->ex_vd_sig?CTRUE:CFALSE, sig->ex_vd_pol?CTRUE:CFALSE);
	NX_VIP_SetFIFOResetMode(NX_VIP_FIFORESET_FRAMEEND);

	/* clipper */
	if (clipper) {
		NX_VIP_SetClipRegion(dat->c_left, dat->c_top, dat->c_right, dat->c_bottom);
		NX_VIP_SetClipperFormat((NX_VIP_FORMAT)format, rot_out,	h_flip,	v_flip);
	}

	/* Decimator */
	if (decimator) {
		NX_VIP_SetDecimation(scl_sw, scl_sh, scl_dw, scl_dh);
		NX_VIP_SetDecimatorFormat((NX_VIP_FORMAT)format, rot_out, h_flip, v_flip);
	}

	NX_VIP_SetInputPort((NX_VIP_INPUTPORT)dat->in_port);
}

static irqreturn_t	do_irq_handler(int irq, void *desc)
{
	/* clear pend */
	NX_VIP_ClearInterruptPendingAll();

	/* Current field is Odd -> the last field is Even, It is end frame, so change buffer.
 	   Note> GetFieldStatus return value 'CTRUE' indicate the last field is even field. */
	if (NX_VIP_GetFieldStatus()) {

		struct vip_dev   *dev = (struct vip_dev *)desc;
		struct vip_data  *dat = dev->data;
		struct vm_buffer *vmb = NULL;
		u_int lu_a = 0x0, cb_a = 0x0, cr_a = 0x0;

		/* set status */
		dev_status |= STAT_ISR_RUN;

		/* stop */
		if (! (dev_status & STAT_DEV_RUN)) {
			NX_VIP_SetInterruptEnableAll(CFALSE);
			NX_VIP_SetVIPEnable(CFALSE, CFALSE, CFALSE, CFALSE);
			/* change state */
			dev_status &= ~STAT_ISR_RUN;
			return IRQ_HANDLED;
		}

		if (skip_count) {
			skip_count--;
			return IRQ_HANDLED;
		}

		vmb = container_of(get_intr_buf_next(), struct vm_buffer, list);
//		DBGOUT("ISR -> vmb=0x%08x\n", vmb);
		if (! vmb) {
			printk(KERN_ERR "Not empty buffer.\n");
			return IRQ_HANDLED;
		}

		lu_a = vmb->vm.LuMemory.Address;
		cb_a = vmb->vm.CbMemory.Address;
		cr_a = vmb->vm.CrMemory.Address;
#if 0
		/* set time stamp */
		do_gettimeofday(&buf->vmb.ts);
#endif
		/* Decimator : change buffer */
		if (VIP_DECIMATOR == dat->out_dev) {
			NX_VIP_SetDecimatorAddr((NX_VIP_FORMAT)dat->format,
					dat->width, dat->height, lu_a, cb_a, cr_a);
		/* Clipper : change buffer */
		} else {
			if (NX_VIP_FORMAT_YUYV == dat->format) {
				NX_VIP_SetClipperAddrYUYV(lu_a, dat->stride);
			} else {
				NX_VIP_SetClipperAddr((NX_VIP_FORMAT)dat->format,
					dat->width, dat->height, lu_a, cb_a, cr_a);
			}
		}

		/* set event */
		dev->p_cond = 1;
		dev->c_cond = 1;
		wake_up(&dev->wait_q);
	}
	return IRQ_HANDLED;
}

#if (VIP_OVERLAY_OUT)
static int stream_thread(void *data)
{
	struct vip_dev *dev = data;
	struct vm_buffer  *vbn = NULL;
	struct vm_buffer  *vbo = NULL;

	u_int  lu_a, cb_a, cr_a;
	u_int  lu_s, cb_s, cr_s;
	u_int  fourcc;
	int    width;
	int    module, first = 1;
	u_int  t_out;

	wait_event(dev->wait_q, dev->p_cond);
	dev->p_cond = 0;
	DBGOUT("Start Thread: %s\n", __func__);

	while (STAT_THR_RUN & dev_status) {

		vbn = container_of(get_prev_buf_fill(), struct vm_buffer, list);
		if (! vbn) {
			t_out = wait_event_interruptible_timeout(dev->wait_q,
							dev->p_cond, dev->w_time);
			if (0 == dev->p_cond) {
				if (STAT_DEV_RUN & dev_status)
					ERROUT("timeout, ret=%d, wait=%d\n", t_out, (dev->w_time*1000)/HZ);
				continue;
			}

			dev->p_cond = 0;
			vbn = container_of(get_prev_buf_fill(), struct vm_buffer, list);
			if (! vbn)
				continue;
		}
		DBGOUT("FIL -> lu_a=0x%08x, 0x%x\n", (u_int)vbn->vm.LuMemory.Address, (u_int)vbn);
		fourcc = dev->data->fourcc;
		width  = dev->data->width;
		module = dev->data->dpc_dev;
		lu_a  = vbn->vm.LuMemory.Address;
		cb_a  = vbn->vm.CbMemory.Address;
		cr_a  = vbn->vm.CrMemory.Address;

		if (FOURCC_YUYV == fourcc) {
			lu_s = (width * 2), cb_s = 0, cr_s = 0;
		} else {
			lu_s = 4096, cb_s = 4096, cr_s = 4096;
		}

		soc_dpc_set_vd_address(module, lu_a, lu_s, cb_a, cb_s, cr_a, cr_s, 0);
		if (first) {
			soc_dpc_set_vd_enable(module, 1);
			first = 0;
		}

		/* free buffer */
		if (vbo) {
			lu_a = vbo->vm.LuMemory.Address;
			cb_a = vbo->vm.CbMemory.Address;
			cr_a = vbo->vm.CrMemory.Address;
			set_prev_buf_free(&vbo->list);
		}
		vbo = vbn;
	}

	DBGOUT("Stop Thread: %s\n", __func__);
	return 0;
}
#endif

/*----------------------------------------------------------------------------*/
int vip_dev_init(struct vip_dev *dev, struct vip_plat_data *plat)
{
	struct vip_data *dat = NULL;
	int ret = -1;
	DBGOUT("%s\n", __func__);

	dat = kzalloc(sizeof(struct vip_data), GFP_KERNEL);
	if (! dat) {
		ERROUT("fail, allcate device data ...\n");
		return -ENOMEM;
	}

	/* init platform data  */
	dat->in_port = plat->in_port;
	dat->dpc_dev = plat->dpc_device;
	dat->halign  = plat->hor_align;
	dat->valign  = plat->ver_align;
	dat->buffers = plat->buff_count;
	skip_count   = plat->skip_count;

	ret = vip_buf_init();
	if (0 > ret) {
		ERROUT("fail, init video buffer ...\n");
		goto err_init;
	}

	ret = request_irq(IRQ_PHY_VIP, &do_irq_handler, IRQF_DISABLED,
				V4L2_DEV_NAME, dev);
	if (ret) {
		ERROUT("fail, request irq %d ...\n", IRQ_PHY_VIP);
		goto err_irq;
	}

	vipdev  = dev;
	dev->data = dat;

	init_waitqueue_head(&dev->wait_q);

	/* set prototype */
	NX_VIP_SetBaseAddress((U32)IO_ADDRESS(NX_VIP_GetPhysicalAddress()));
	NX_VIP_OpenModule();

	return ret;

err_irq:
	free_irq(IRQ_PHY_VIP, NULL);
	vip_buf_exit();

err_init:
	if (dat)
		kfree(dat);

	dev->data = NULL;
	return ret;
}

void vip_dev_exit(struct vip_dev *dev)
{
	DBGOUT("%s\n", __func__);
	if (! dev)
		return;

	if (dev->data) {
		kfree(dev->data);
		dev->data = NULL;
	}

	free_irq(IRQ_PHY_VIP, NULL);
	vipdev = NULL;
}

int vip_dev_open(void)
{
	struct vip_dev     *dev = vipdev;
	struct vip_data    *dat = dev->data;
	struct vip_sub_ops *ops = dat->sub_ops;
	struct vip_sync    *syc = &ext_syc;
	int ret = 0;

	DBGOUT("%s\n", __func__);

	if (! ops || ! ops->get_sync) {
		ERROUT("fail, not exist sub device ops ...\n");
		return -EINVAL;
	}

	if (ops->open)
		ops->open();

	ops->get_sync(syc);

	init_params(dat, syc);
	init_device(dat);

	/* thread */
#if (VIP_OVERLAY_OUT)
	dev_status  = STAT_THR_RUN;
	dev->p_cond = 0;
	dev->c_cond = 0;
	dev->w_time = MAX_SCHEDULE_TIMEOUT;
	dev->kthread = kthread_run(stream_thread, dev, "vip-thread");
	if (IS_ERR(dev->kthread)) {
		ERROUT("fail, kernel_thread \n");
		ret = PTR_ERR(dev->kthread);
		return ret;
	}
#endif

	return ret;
}

void vip_dev_close(void)
{
	DBGOUT("%s\n", __func__);

	vip_dev_stop();
	vip_dev_release_buf();

#if (VIP_OVERLAY_OUT)
	/* stop status */
	dev_status &= ~STAT_THR_RUN;

	/* shutdown thread */
	if (dev->kthread) {
		struct vip_dev *dev = vipdev;
		dev->w_time = 1;
		dev->p_cond = 1;
		dev->c_cond = 1;
		wake_up(&dev->wait_q);	// 	kthread_stop(dev->kthread);
		dev->kthread = NULL;
	}
#endif
}

void vip_dev_start(void)
{
	struct vip_dev   *dev = vipdev;
	struct vip_data  *dat = dev->data;
	struct vm_buffer *vmb = NULL;

	int clipper   = dat->out_dev == VIP_CLIPPER   ? 1 : 0;
	int decimator = dat->out_dev == VIP_DECIMATOR ? 1 : 0;

	u_int format = dat->format;
	int	  width  = dat->width;
	int	  height = dat->height;
	int	  stride = dat->stride;

	u_int lu_a = 0x0, cb_a = 0x0, cr_a = 0x0;

	/* get output address */
	vmb = container_of(get_intr_buf_empty(), struct vm_buffer, list);

	lu_a = vmb->vm.LuMemory.Address;
	cb_a = vmb->vm.CbMemory.Address;
	cr_a = vmb->vm.CrMemory.Address;

	DBGOUT("%s\n", __func__);
	DBGOUT(" device = %s, out = (%d*%d), ro = %s, format = %d\r\n",
		clipper?"clipper":"decimator", width, height, dat->rotate?"on":"off", format);
	DBGOUT("lu_a=0x%08x, cb_a=0x%08x, cr_a=0x%08x, stride=%d\n",
		lu_a, cb_a, cr_a, stride);

#if (VIP_OVERLAY_OUT)
	/* set display */
	soc_dpc_set_vd_format(dat->module, dat->fourcc, width, height);
//	soc_dpc_set_vd_priority(dat->module, 1);
	soc_dpc_set_vd_position(dat->module, 0, 0, width, height, 0);
#endif

	/* Set decimator address. */
	if (decimator) {
		NX_VIP_SetDecimatorAddr((NX_VIP_FORMAT)format, width, height, lu_a, cb_a, cr_a);
	}

	/* Set clipper address. */
	if (clipper) {
		if (NX_VIP_FORMAT_YUYV == (NX_VIP_FORMAT)format)
			NX_VIP_SetClipperAddrYUYV(lu_a, stride);
		else
			NX_VIP_SetClipperAddr((NX_VIP_FORMAT)format, width, height, lu_a, cb_a, cr_a);
	}

	/* Clock enable. */
	set_power_on();

#if 0
	DebugRegister();
#endif

	/* Set device status. */
	dev_status |= STAT_DEV_RUN;

	/* Enable. */
	NX_VIP_ClearInterruptPendingAll();
	NX_VIP_SetVIPEnable(CTRUE, CTRUE, (clipper?CTRUE:CFALSE), (decimator?CTRUE:CFALSE));
	NX_VIP_SetInterruptEnableAll(CFALSE);
	NX_VIP_SetInterruptEnable(NX_VIP_INT_DONE, CTRUE);

	/* event */
	dev_status |= STAT_DEV_RUN;
	dev->p_cond = 0;
	dev->c_cond = 0;
#if (VIP_OVERLAY_OUT)
	dev->w_time = WAIT_TIMEOUT/(1000/msecs_to_jiffies(1000));
	wake_up(&dev->wait_q);
#endif
}

void vip_dev_stop(void)
{
	struct vip_dev *dev = vipdev;
	volatile int count = 0;
	DBGOUT("%s\n", __func__);

	if (! (STAT_ISR_RUN & dev_status))
		return;

	/* Stopped device. */
	dev_status &= ~STAT_DEV_RUN;

	/* Wait until ISR stopped. */
	while ((STAT_ISR_RUN & dev_status) && 1000 > count++)
		msleep(1);

	dev->w_time = MAX_SCHEDULE_TIMEOUT;

	DBGOUT("wait for power stop (time:%d)\r\n", count);

	/* Clock disable. */
	set_power_off();
}

void * vip_dev_get_format(u_int format, int index)
{
	struct vip_fmt *fmt;
	unsigned int i = 0;
	DBGOUT("%s, format=0x%x, index=%d \n", __func__, format, index);

	if (! format && -1 == index) {
		ERROUT("fail, invalid format=0x%x, index=%d \n", format, index);
		return NULL;
	}

	if (index >= 0)
		return (void*)&out_formats[index];

	for (i = 0; ARRAY_SIZE(out_formats) > i; i++) {
		fmt = &out_formats[i];
		if (fmt->fourcc == format)
			break;
	}

	if (ARRAY_SIZE(out_formats) == i) {
		ERROUT("fail, not support fourcc (%c%c%c%c) \n",
			format&0xFF,(format>>8)&0xFF, (format>>16)&0xFF,(format>>24)&0xFF);
		return NULL;
	}
	return (void*)&out_formats[i];
}


int vip_dev_set_format(u_int fourcc)
{
	struct vip_data  *dat = vipdev->data;
	struct vip_output out;

	DBGOUT(" %s\n", __func__);

	if (STAT_DEV_RUN & dev_status) {
		ERROUT("%s: fail, run status..\n", __func__);
		return -1;
	}

	/* get current output */
	get_params(&out, dat);

	out.fourcc = fourcc;

	if (0 > check_params(&out))
		return -EINVAL;

	/* set new output */
	set_params (dat, &out);
	init_device(dat);

	return 0;
}

int vip_dev_set_clip(int left, int top, int right, int bottom)
{
	struct vip_data  *dat = vipdev->data;
	struct vip_sync  *syc = &ext_syc;
	struct vip_output out;

	DBGOUT("%s (l=%d, t=%d, r=%d, b=%d) \n", __func__,
		left, top, right, bottom);

	if (STAT_ISR_RUN & dev_status) {
		ERROUT("%s: fail, run status..\n", __func__);
		return -EINVAL;
	}

	/* get current output */
	get_params(&out, dat);

	out.c_left   = left;
	out.c_top	 = top;
	out.c_right  = right;
	out.c_bottom = syc->interlace ? bottom>>1 : bottom;

	if (0 > check_params(&out))
		return -EINVAL;

	/* set new output */
	set_params(dat, &out);
	init_device(dat);

	return 0;
}

int vip_dev_alloc_buf(int count)
{
	struct vip_data *dat = vipdev->data;
	DBGOUT("%s\n", __func__);

	return vip_buf_alloc(dat->fourcc, dat->width, dat->height,
			dat->halign, dat->valign, count);
}

void vip_dev_release_buf(void)
{
	DBGOUT("%s\n", __func__);
	vip_buf_free();
}

