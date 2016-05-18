/* linux/drivers/media/video/nexell/nx_vip_v4l2.c
 *
 * V4L2 interface support file for Nexell NXP2120 camera(VIP) driver
 *
 * Copyright (c) 2011 MOSTiTECH co., ltd.
 * All right reserved by Seungwoo Kim
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/videodev2.h>
#include <media/v4l2-ioctl.h>
#include <linux/delay.h>

/* NXP SOC's registers */
#include <mach/platform.h>
#include "nx_vip.h"

static struct v4l2_input nx_vip_input_types[] = {
	{
		.index		= 0,
		.name		= "External Camera Input",
		.type		= V4L2_INPUT_TYPE_CAMERA,
		.audioset	= 1,
		.tuner		= 0,
		.std		= V4L2_STD_PAL_BG | V4L2_STD_NTSC_M,
		.status		= 0,
	},
};

static struct v4l2_output nx_vip_output_types[] = {
	{
		.index		= 0,
		.name		= "Memory Output",
		.type		= V4L2_OUTPUT_TYPE_MEMORY,
		.audioset	= 0,
		.modulator	= 0, 
		.std		= 0,
	}, 
};

const static struct v4l2_fmtdesc nx_vip_capture_formats[] = {
	{
		.index		= 0,
		.type		= V4L2_BUF_TYPE_VIDEO_CAPTURE,
		.flags		= FORMAT_FLAGS_PLANAR,
		.description	= "4:2:0, planar, Y-Cb-Cr",
		.pixelformat	= V4L2_PIX_FMT_YUV420,
	},
	{
		.index		= 1,
		.type		= V4L2_BUF_TYPE_VIDEO_CAPTURE,
		.flags		= FORMAT_FLAGS_PLANAR,
		.description	= "4:2:2, planar, Y-Cb-Cr",
		.pixelformat	= V4L2_PIX_FMT_YUV422P,

	},	
	{
		.index		= 2,
		.type		= V4L2_BUF_TYPE_VIDEO_CAPTURE,
		.flags		= FORMAT_FLAGS_PACKED,
		.description	= "4:2:2, packed, YCBYCR",
		.pixelformat	= V4L2_PIX_FMT_YUYV,
	},
	{
		.index		= 3,
		.type		= V4L2_BUF_TYPE_VIDEO_CAPTURE,
		.flags		= FORMAT_FLAGS_PACKED,
		.description	= "4:2:2, packed, CBYCRY",
		.pixelformat	= V4L2_PIX_FMT_UYVY,
	}
};

const static struct v4l2_fmtdesc nx_vip_overlay_formats[] = {
	{
		.index		= 0,
		.type		= V4L2_BUF_TYPE_VIDEO_OVERLAY,
		.flags		= FORMAT_FLAGS_PACKED,
		.description	= "16 bpp RGB, le",
		.pixelformat	= V4L2_PIX_FMT_RGB565,		
	},
	{
		.index		= 1,
		.type		= V4L2_BUF_TYPE_VIDEO_OVERLAY,
		.flags		= FORMAT_FLAGS_PACKED,
		.description	= "24 bpp RGB, le",
		.pixelformat	= V4L2_PIX_FMT_RGB24,		
	},
};

#define NX_VIP_MAX_INPUT_TYPES	ARRAY_SIZE(nx_vip_input_types)
#define NX_VIP_MAX_OUTPUT_TYPES	ARRAY_SIZE(nx_vip_output_types)
#define NX_VIP_MAX_CAPTURE_FORMATS	ARRAY_SIZE(nx_vip_capture_formats)
#define NX_VIP_MAX_OVERLAY_FORMATS	ARRAY_SIZE(nx_vip_overlay_formats)

static int nx_vip_v4l2_querycap(struct file *filp, void *fh,
					struct v4l2_capability *cap)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;

	strcpy(cap->driver, "Nexell VIP Driver");
	strlcpy(cap->card, ctrl->vd->name, sizeof(cap->card));
	sprintf(cap->bus_info, "Nxp2120 AHB-bus");

	cap->version = 0;
	cap->capabilities = (V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING | V4L2_CAP_READWRITE); /* ksw : READWRITE ? */

	return 0;
}

static int nx_vip_v4l2_g_fbuf(struct file *filp, void *fh,
					struct v4l2_framebuffer *fb)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;

	*fb = ctrl->v4l2.frmbuf;

	fb->base = ctrl->v4l2.frmbuf.base;
	fb->capability = V4L2_FBUF_CAP_LIST_CLIPPING;

	fb->fmt.pixelformat  = ctrl->v4l2.frmbuf.fmt.pixelformat;
	fb->fmt.width = ctrl->v4l2.frmbuf.fmt.width;
	fb->fmt.height = ctrl->v4l2.frmbuf.fmt.height;
	fb->fmt.bytesperline = ctrl->v4l2.frmbuf.fmt.bytesperline;

	return 0;
}

static int nx_vip_v4l2_s_fbuf(struct file *filp, void *fh,
					struct v4l2_framebuffer *fb)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;
	struct v4l2_framebuffer *frmbuf = &(ctrl->v4l2.frmbuf);
	int i, bpp;

	bpp = nx_vip_set_output_frame(ctrl, &fb->fmt);

	frmbuf->base  = fb->base;
	frmbuf->flags = fb->flags;
	frmbuf->capability = fb->capability;
	frmbuf->fmt.width = fb->fmt.width;
	frmbuf->fmt.height = fb->fmt.height;
	frmbuf->fmt.field = fb->fmt.field;
	frmbuf->fmt.pixelformat = fb->fmt.pixelformat;
	frmbuf->fmt.bytesperline = fb->fmt.width * bpp / 8;
	frmbuf->fmt.sizeimage = fb->fmt.width * frmbuf->fmt.bytesperline;

	return 0;
}

static int nx_vip_v4l2_enum_fmt_vid_cap(struct file *filp, void *fh,
					struct v4l2_fmtdesc *f)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;
	int index = f->index;

	if (index >= NX_VIP_MAX_CAPTURE_FORMATS)
		return -EINVAL;

	memset(f, 0, sizeof(*f));
	memcpy(f, ctrl->v4l2.fmtdesc + index, sizeof(*f));

	return 0;
}

static int nx_vip_v4l2_g_fmt_vid_cap(struct file *filp, void *fh,
					struct v4l2_format *f)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;
	int size = sizeof(struct v4l2_pix_format);

	memset(&f->fmt.pix, 0, size);
	memcpy(&f->fmt.pix, &(ctrl->v4l2.frmbuf.fmt), size);

	return 0;
}

extern int nx_vip_v4l2_streamon(struct file *filp, void *fh, enum v4l2_buf_type i);
extern int nx_vip_v4l2_streamoff(struct file *filp, void *fh, enum v4l2_buf_type i);

static int nx_vip_v4l2_s_fmt_vid_cap(struct file *filp, void *fh,
					struct v4l2_format *f)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;
	struct v4l2_framebuffer *frmbuf = &(ctrl->v4l2.frmbuf);
	int depth;

	//frmbuf.fmt = f->fmt.pix;

	f->fmt.pix.priv = 1; /* We don't use Input Frame Format... Yet. */ 
	if (f->fmt.pix.priv == V4L2_FMT_IN)
		depth = nx_vip_set_input_frame(ctrl, &f->fmt.pix);
	else {
		depth = nx_vip_set_output_frame(ctrl, &f->fmt.pix);
		if (depth > 0)
		    nx_vip_v4l2_streamon(filp, (void *)ctrl, V4L2_BUF_TYPE_VIDEO_CAPTURE);
	}

	return (depth > 0) ? 0 : -EINVAL;
}

static int nx_vip_v4l2_try_fmt_vid_cap(struct file *filp, void *fh,
					  struct v4l2_format *f)
{
	return 0;
}

static int nx_vip_v4l2_try_fmt_overlay(struct file *filp, void *fh,
					  struct v4l2_format *f)
{
	return 0;
}

static int nx_vip_v4l2_overlay(struct file *filp, void *fh, unsigned int i)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;

	if (i) {
		if (ctrl->in_type != PATH_IN_DMA)
			nx_vip_init_camera(ctrl);

		FSET_PREVIEW(ctrl);
		nx_vip_start_vip(ctrl);
	} else {
		nx_vip_stop_vip(ctrl);

		if (ctrl->out_type != PATH_OUT_LCDFIFO) {
			nx_vip_free_output_memory(&ctrl->out_frame);
			nx_vip_set_output_address(ctrl);
		}
	}
	
	return 0;
}

static int nx_vip_v4l2_g_ctrl(struct file *filp, void *fh,
					struct v4l2_control *c)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;
	struct nx_vip_out_frame *frame = &ctrl->out_frame;

	switch (c->id) {
	case V4L2_CID_OUTPUT_ADDR:
		c->value = frame->addr[c->value].phys_y;
		break;

	default:
		err("invalid control id: %d\n", c->id);
		return -EINVAL;
	}
	
	return 0;
}

static int nx_vip_v4l2_s_ctrl(struct file *filp, void *fh,
					struct v4l2_control *c)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;
	struct nx_vip_out_frame *frame = &ctrl->out_frame;
	struct nx_vip_window_offset *offset = &ctrl->in_cam->offset;

	switch (c->id) {
#if 0
	case V4L2_CID_EFFECT_ORIGINAL:
		frame->effect.type = EFFECT_ORIGINAL;
		nx_vip_change_effect(ctrl);
		break;

	case V4L2_CID_EFFECT_NEGATIVE:
		frame->effect.type = EFFECT_NEGATIVE;
		nx_vip_change_effect(ctrl);
		break;

	case V4L2_CID_EFFECT_EMBOSSING:
		frame->effect.type = EFFECT_EMBOSSING;
		nx_vip_change_effect(ctrl);
		break;

	case V4L2_CID_EFFECT_ARTFREEZE:
		frame->effect.type = EFFECT_ARTFREEZE;
		nx_vip_change_effect(ctrl);
		break;

	case V4L2_CID_EFFECT_SILHOUETTE:
		frame->effect.type = EFFECT_SILHOUETTE;
		nx_vip_change_effect(ctrl);
		break;

	case V4L2_CID_EFFECT_ARBITRARY:
		frame->effect.type = EFFECT_ARBITRARY;
		frame->effect.pat_cb = PAT_CB(c->value);
		frame->effect.pat_cr = PAT_CR(c->value);
		nx_vip_change_effect(ctrl);
		break;

	case V4L2_CID_ROTATE_ORIGINAL:
		frame->flip = FLIP_ORIGINAL;
		ctrl->rot90 = 0;
		nx_vip_change_rotate(ctrl);
		break;

	case V4L2_CID_HFLIP:
		frame->flip = FLIP_X_AXIS;
		ctrl->rot90 = 0;
		nx_vip_change_rotate(ctrl);
		break;

	case V4L2_CID_VFLIP:
		frame->flip = FLIP_Y_AXIS;
		ctrl->rot90 = 0;
		nx_vip_change_rotate(ctrl);
		break;

	case V4L2_CID_ROTATE_180:
		frame->flip = FLIP_XY_AXIS;
		ctrl->rot90 = 0;
		nx_vip_change_rotate(ctrl);
		break;

	case V4L2_CID_ROTATE_90:
		frame->flip = FLIP_ORIGINAL;
		ctrl->rot90 = 1;
		nx_vip_change_rotate(ctrl);
		break;

	case V4L2_CID_ROTATE_270:
		frame->flip = FLIP_XY_AXIS;
		ctrl->rot90 = 1;
		nx_vip_change_rotate(ctrl);
		break;

	case V4L2_CID_ROTATE_90_HFLIP:
		frame->flip = FLIP_X_AXIS;
		ctrl->rot90 = 1;
		nx_vip_change_rotate(ctrl);
		break;

	case V4L2_CID_ROTATE_90_VFLIP:
		frame->flip = FLIP_Y_AXIS;
		ctrl->rot90 = 1;
		nx_vip_change_rotate(ctrl);
		break;

	case V4L2_CID_ZOOM_IN:
		if (nx_vip_check_zoom(ctrl, c->id) == 0) {
			offset->h1 += NX_VIP_ZOOM_PIXELS;
			offset->h2 += NX_VIP_ZOOM_PIXELS;
			offset->v1 += NX_VIP_ZOOM_PIXELS;
			offset->v2 += NX_VIP_ZOOM_PIXELS;
			nx_vip_restart_dma(ctrl);
		}

		break;

	case V4L2_CID_ZOOM_OUT:
		if (nx_vip_check_zoom(ctrl, c->id) == 0) {
			offset->h1 -= NX_VIP_ZOOM_PIXELS;
			offset->h2 -= NX_VIP_ZOOM_PIXELS;
			offset->v1 -= NX_VIP_ZOOM_PIXELS;
			offset->v2 -= NX_VIP_ZOOM_PIXELS;
			nx_vip_restart_dma(ctrl);
		}

		break;

	case V4L2_CID_AUTO_WHITE_BALANCE:
		nx_vip_i2c_command(ctrl, I2C_CAM_WB, c->value);
		break;

	case V4L2_CID_ACTIVE_CAMERA:
		nx_vip_set_active_camera(ctrl, c->value);
		nx_vip_i2c_command(ctrl, I2C_CAM_WB, WB_AUTO);
		break;

	case V4L2_CID_TEST_PATTERN:
		nx_vip_set_active_camera(ctrl, NX_VIP_TPID);
		nx_vip_set_test_pattern(ctrl, c->value);
		break;

	case V4L2_CID_NR_FRAMES:
		nx_vip_set_nr_frames(ctrl, c->value);
		break;

	case V4L2_CID_INPUT_ADDR:
		nx_vip_alloc_input_memory(&ctrl->in_frame, \
						(dma_addr_t) c->value);
		nx_vip_set_input_address(ctrl);
		break;

	case V4L2_CID_INPUT_ADDR_Y:
	case V4L2_CID_INPUT_ADDR_RGB:
		nx_vip_alloc_y_memory(&ctrl->in_frame, \
						(dma_addr_t) c->value);
		nx_vip_set_input_address(ctrl);
		break;

	case V4L2_CID_INPUT_ADDR_CB:	/* fall through */
	case V4L2_CID_INPUT_ADDR_CBCR:
		nx_vip_alloc_cb_memory(&ctrl->in_frame, \
						(dma_addr_t) c->value);
		nx_vip_set_input_address(ctrl);
		break;

	case V4L2_CID_INPUT_ADDR_CR:
		nx_vip_alloc_cr_memory(&ctrl->in_frame, \
						(dma_addr_t) c->value);
		nx_vip_set_input_address(ctrl);
		break;

	case V4L2_CID_RESET:
		ctrl->rot90 = 0;
		ctrl->in_frame.flip = FLIP_ORIGINAL;
		ctrl->out_frame.flip = FLIP_ORIGINAL;
		ctrl->out_frame.effect.type = EFFECT_ORIGINAL;
		ctrl->scaler.bypass = 0;
		nx_vip_reset(ctrl);
		break;

	case V4L2_CID_JPEG_INPUT:	/* fall through */
	case V4L2_CID_SCALER_BYPASS:
		ctrl->scaler.bypass = 1;
		break;
#endif
	case V4L2_CID_BLACK_LEVEL:
	    // Do nothing but implemented..
	   
	    break;
	case V4L2_CID_BRIGHTNESS:
	    // Do nothing but implemeted..
	    nx_vip_i2c_command(ctrl, I2C_CAM_BRIGHTNESS, c->value);
	    break;

	case V4L2_CID_POWER_SAVE:
	    // Do nothing but implemeted..
	    nx_vip_i2c_command(ctrl, I2C_CAM_POWER_SAVE, c->value);
	    break;

	default:
		err("invalid control id: %d\n", c->id);
		return -EINVAL;
	}


	return 0;
}

void s3c_stream_on(struct nx_vip_control *ctrl)
{
    ctrl->out_frame.skip_frames = 0;
	FSET_CAPTURE(ctrl);
	FSET_IRQ_NORMAL(ctrl);
	nx_vip_start_vip(ctrl);
	ctrl->streamon = 1;
}

void s3c_stream_off(struct nx_vip_control *ctrl)
{
    FSET_STOP(ctrl);
	UNMASK_USAGE(ctrl);
	UNMASK_IRQ(ctrl);

	ctrl->streamon = 0;

	nx_vip_stop_vip(ctrl);
}

int nx_vip_v4l2_streamon(struct file *filp, void *fh,
					enum v4l2_buf_type i)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;
	
	if (i != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	if (ctrl->in_type != PATH_IN_DMA)
		nx_vip_init_camera(ctrl);

	s3c_stream_on(ctrl);

	return 0;
}

int nx_vip_v4l2_streamoff(struct file *filp, void *fh,
					enum v4l2_buf_type i)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;
	
	if (i != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	s3c_stream_off(ctrl);

	/* Now deallocate memory */
	nx_vip_free_output_memory(&ctrl->out_frame);
	nx_vip_set_output_address(ctrl);
	
	return 0;
}

static int nx_vip_v4l2_g_input(struct file *filp, void *fh,
					unsigned int *i)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;

	*i = ctrl->v4l2.input->index;

	return 0;
}

static int nx_vip_v4l2_s_input(struct file *filp, void *fh,
					unsigned int i)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;

	if (i >= NX_VIP_MAX_INPUT_TYPES)
		return -EINVAL;

	ctrl->v4l2.input = &nx_vip_input_types[i];

	if (nx_vip_input_types[i].type == V4L2_INPUT_TYPE_CAMERA)
		ctrl->in_type = PATH_IN_ITU_CAMERA;
	else
		ctrl->in_type = PATH_IN_DMA;

	return 0;
}

static int nx_vip_v4l2_g_output(struct file *filp, void *fh,
					unsigned int *i)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;

	*i = ctrl->v4l2.output->index;

	return 0;
}

static int nx_vip_v4l2_s_output(struct file *filp, void *fh,
					unsigned int i)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;

	if (i >= NX_VIP_MAX_OUTPUT_TYPES)
		return -EINVAL;

	ctrl->v4l2.output = &nx_vip_output_types[i];

	if (nx_vip_output_types[i].type == V4L2_OUTPUT_TYPE_MEMORY)
		ctrl->out_type = PATH_OUT_DMA;
	else
		ctrl->out_type = PATH_OUT_LCDFIFO;

	return 0;
}

static int nx_vip_v4l2_enum_input(struct file *filp, void *fh,
					struct v4l2_input *i)
{
	if (i->index >= NX_VIP_MAX_INPUT_TYPES)
		return -EINVAL;

	memcpy(i, &nx_vip_input_types[i->index], sizeof(struct v4l2_input));

	return 0;
}

static int nx_vip_v4l2_enum_output(struct file *filp, void *fh,
					struct v4l2_output *o)
{
	if ((o->index) >= NX_VIP_MAX_OUTPUT_TYPES)
		return -EINVAL;

	memcpy(o, &nx_vip_output_types[o->index], sizeof(struct v4l2_output));

	return 0;
}

static int nx_vip_v4l2_reqbufs(struct file *filp, void *fh,
					struct v4l2_requestbuffers *b)
{
    struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;
    
	if (b->memory != V4L2_MEMORY_MMAP) {
		err("V4L2_MEMORY_MMAP is only supported\n");
		return -EINVAL;
	}

	/* control user input */
	if (b->count > 4)
		b->count = 4;
	else if (b->count < 1)
		b->count = 1;
/* We should allocate framebuffer for read.? */
    if (NULL == ctrl->out_frame.addr[0].virt_y) {
        printk("alloc frame buffer for output\n");
        ctrl->out_frame.format = FORMAT_YCBCR422;
        ctrl->out_frame.width = ctrl->in_cam->width;
        ctrl->out_frame.height = ctrl->in_cam->height;
        ctrl->out_frame.nr_frames = b->count;
        nx_vip_alloc_output_memory(ctrl);
    }
	return 0;
}

static int nx_vip_v4l2_querybuf(struct file *filp, void *fh,
					struct v4l2_buffer *b)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;
	
	if (b->type != V4L2_BUF_TYPE_VIDEO_OVERLAY && \
		b->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	if (b->memory != V4L2_MEMORY_MMAP)
		return -EINVAL;

	printk("v4l2 querybuf : buf size = %d\n", ctrl->out_frame.buf_size); 
	b->length = ctrl->out_frame.buf_size;

	/*
	 * NOTE: we use the m.offset as an index for multiple frames out.
	 * Because all frames are not contiguous, we cannot use it as
	 * original purpose.
	 * The index value used to find out which frame user wants to mmap.
	 */
	b->m.offset = b->index * PAGE_SIZE;
	printk("offset = %x, index = %d, PAGE_SIZE = %x\n", b->m.offset, b->index, PAGE_SIZE);

	return 0;
}

static int nx_vip_v4l2_qbuf(struct file *filp, void *fh,
				struct v4l2_buffer *b)
{
	return 0;
}

static int nx_vip_v4l2_dqbuf(struct file *filp, void *fh,
				struct v4l2_buffer *b)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;
	struct nx_vip_out_frame *frame = &ctrl->out_frame;

	//ctrl->out_frame.cfn = nx_vip_get_frame_count(ctrl);
	b->index = (frame->cfn + 2) % frame->nr_frames;

	return 0;
}

static int nx_vip_v4l2_cropcap(struct file *filp, void *fh,
					struct v4l2_cropcap *a)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;

	if (a->type != V4L2_BUF_TYPE_VIDEO_CAPTURE &&
		a->type != V4L2_BUF_TYPE_VIDEO_OVERLAY)
		return -EINVAL;

	/* crop limitations */
	ctrl->v4l2.crop_bounds.left = 0;
	ctrl->v4l2.crop_bounds.top = 0;
	ctrl->v4l2.crop_bounds.width = ctrl->in_cam->width;
	ctrl->v4l2.crop_bounds.height = ctrl->in_cam->height;

	/* crop default values */
	ctrl->v4l2.crop_defrect.left = \
			(ctrl->in_cam->width - NX_VIP_CROP_DEF_WIDTH) / 2;

	ctrl->v4l2.crop_defrect.top = \
			(ctrl->in_cam->height - NX_VIP_CROP_DEF_HEIGHT) / 2;

	ctrl->v4l2.crop_defrect.width = NX_VIP_CROP_DEF_WIDTH;
	ctrl->v4l2.crop_defrect.height = NX_VIP_CROP_DEF_HEIGHT;

	a->bounds = ctrl->v4l2.crop_bounds;
	a->defrect = ctrl->v4l2.crop_defrect;

	return 0;
}

static int nx_vip_v4l2_g_crop(struct file *filp, void *fh,
				struct v4l2_crop *a)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;

	if (a->type != V4L2_BUF_TYPE_VIDEO_CAPTURE &&
		a->type != V4L2_BUF_TYPE_VIDEO_OVERLAY)
		return -EINVAL;

	a->c = ctrl->v4l2.crop_current;

	return 0;
}

static int nx_vip_v4l2_s_crop(struct file *filp, void *fh,
				struct v4l2_crop *a)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;
	struct nx_vip_camera *cam = ctrl->in_cam;

	if (a->type != V4L2_BUF_TYPE_VIDEO_CAPTURE &&
		a->type != V4L2_BUF_TYPE_VIDEO_OVERLAY)
		return -EINVAL;

	if (a->c.height < 0)
		return -EINVAL;

	if (a->c.width < 0)
		return -EINVAL;

	if ((a->c.left + a->c.width > cam->width) || \
		(a->c.top + a->c.height > cam->height))
		return -EINVAL;

	ctrl->v4l2.crop_current = a->c;

	cam->offset.h1 = (cam->width - a->c.width) / 2;
	cam->offset.v1 = (cam->height - a->c.height) / 2;

	cam->offset.h2 = cam->offset.h1;
	cam->offset.v2 = cam->offset.v1;

	nx_vip_restart_vip(ctrl);

	return 0;
}

static int nx_vip_v4l2_s_parm(struct file *filp, void *fh,
				struct v4l2_streamparm *a)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;

	if (a->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

#if 0 /* We don't have anything to do woth capturemode. */
	if (a->parm.capture.capturemode == V4L2_MODE_HIGHQUALITY) {
		info("changing to max resolution\n");
		nx_vip_change_resolution(ctrl, CAM_RES_MAX);
	} else {
		info("changing to default resolution\n");
		nx_vip_change_resolution(ctrl, CAM_RES_DEFAULT);
	}
#endif

	nx_vip_restart_vip(ctrl);

	return 0;
}

static int nx_vip_v4l2_g_parm(struct file *filp, void *fh,
				struct v4l2_streamparm *a)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;

	if (a->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;

	a->parm.capture.capturemode =  !V4L2_MODE_HIGHQUALITY;

	return 0;
}


/* Custom I2C control IOCTL */
struct i2c_struc {
	short int subaddr;
	short int val;
};

#define VIDIOC_I2C_CTRL_R	_IOWR('V',100,struct i2c_struc)
#define VIDIOC_I2C_CTRL_W	_IOW('V',101,struct i2c_struc)
#define VIDIOC_READ_ARM		_IOWR('V',102, int)
#define VIDIOC_CAM_PWR_STATUS _IOR('V',103, int)
#define VIDIOC_SHUTDOWN_CAM _IOW('V',104, int)
//#define VIDIOC_CHANGE_CAM   _IOW('V',105, int)
//#define VIDIOC_REQUEST_OTHER _IOW('V',106, int)

static int nx_vip_v4l2_default(struct file *file, void *fh,
					int cmd, void *arg)
{
    struct nx_vip_control *ctrl = (struct nx_vip_control *) fh;

    //printk("V4L2 default ioctl cmd = %x %x\n", cmd, VIDIOC_I2C_CTRL_W);
    if (cmd == VIDIOC_I2C_CTRL_R) {
        struct i2c_struc *i2cs= arg;
        //printk("v4l2 ctrlr\n");
        nx_vip_i2c_read(ctrl, (int)i2cs->subaddr, &i2cs->val);

    } else
    if (cmd == VIDIOC_I2C_CTRL_W) {
        struct i2c_struc *i2cs= arg;
        //printk("v4l2 ctrlw\n");
        nx_vip_i2c_write(ctrl, (int)i2cs->subaddr, (int)i2cs->val);
    } else
    if (cmd == VIDIOC_READ_ARM) {
        // just arm the read file
        int *ra = arg;
        //s3c_arm_read(cam, ra);
    } else
    if (cmd == VIDIOC_CAM_PWR_STATUS) {
        int *ra = arg;
        *ra = !ctrl->pwrdn;
    } else
    if (cmd == VIDIOC_SHUTDOWN_CAM) {
        int *ra = arg;
      	if (ctrl->gpio_reset) {
      	    ctrl->gpio_reset(*ra);
      	    ctrl->pwrdn = *ra;
      	}
    } else   
        return -EINVAL;
    return 0;
}

const struct v4l2_ioctl_ops nx_vip_v4l2_ops = {
	.vidioc_querycap		= nx_vip_v4l2_querycap,
	.vidioc_g_fbuf			= nx_vip_v4l2_g_fbuf,
	.vidioc_s_fbuf			= nx_vip_v4l2_s_fbuf,
	.vidioc_enum_fmt_vid_cap	= nx_vip_v4l2_enum_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap		= nx_vip_v4l2_g_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap		= nx_vip_v4l2_s_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap		= nx_vip_v4l2_try_fmt_vid_cap,
	.vidioc_try_fmt_vid_overlay	= nx_vip_v4l2_try_fmt_overlay,
	.vidioc_overlay			= nx_vip_v4l2_overlay,
	.vidioc_g_ctrl			= nx_vip_v4l2_g_ctrl,
	.vidioc_s_ctrl			= nx_vip_v4l2_s_ctrl,
	.vidioc_streamon		= nx_vip_v4l2_streamon,
	.vidioc_streamoff		= nx_vip_v4l2_streamoff,
	.vidioc_g_input			= nx_vip_v4l2_g_input,
	.vidioc_s_input			= nx_vip_v4l2_s_input,
	.vidioc_g_output		= nx_vip_v4l2_g_output,
	.vidioc_s_output		= nx_vip_v4l2_s_output,
	.vidioc_enum_input		= nx_vip_v4l2_enum_input,
	.vidioc_enum_output		= nx_vip_v4l2_enum_output,
	.vidioc_reqbufs			= nx_vip_v4l2_reqbufs,
	.vidioc_querybuf		= nx_vip_v4l2_querybuf,
	.vidioc_qbuf			= nx_vip_v4l2_qbuf,
	.vidioc_dqbuf			= nx_vip_v4l2_dqbuf,
	.vidioc_cropcap			= nx_vip_v4l2_cropcap,
	.vidioc_g_crop			= nx_vip_v4l2_g_crop,
	.vidioc_s_crop			= nx_vip_v4l2_s_crop,
	.vidioc_g_parm          = nx_vip_v4l2_g_parm,
	.vidioc_s_parm			= nx_vip_v4l2_s_parm,
	.vidioc_default         = nx_vip_v4l2_default,
};
