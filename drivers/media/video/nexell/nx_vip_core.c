/* linux/drivers/media/video/nexell/nx_vip_core.c
 *
 * Core file for Nexell NXP2120 camera(VIP) driver
 *
 * Copyright (c) 2011 MOSTiTECH co., ltd.
 * All right reserved by Seungwoo Kim
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/clk.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <linux/irq.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/videodev2.h>

#include <asm/io.h>
#include <asm/memory.h>

#include <mach/platform.h>
#include <mach/devices.h>
#include "nx_vip.h"

static struct nx_vip_camera test_pattern = {
	.id 		= NX_VIP_TPID,
	.order422	= CAM_ORDER422_8BIT_YCBYCR,
	.clockrate	= 0,
	.width		= 640,
	.height		= 480,
	.offset		= {
		.h1 = 0,
		.h2 = 0,
		.v1 = 0,
		.v2 = 0,
	},

	.polarity	= {
		.pclk	= 0,
		.vsync	= 0,
		.href	= 0,
		.hsync	= 0,
	},

	.initialized	= 0,	
};

struct nx_vip_config nx_vip;

struct nx_platform_vip *to_vip_plat(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);

	return (struct nx_platform_vip *) pdev->dev.platform_data;
}

int nx_vip_i2c_read(struct nx_vip_control *ctrl, int subaddr, int *val)
{
    return ctrl->in_cam->i2c_read(subaddr, val);
}

int nx_vip_i2c_write(struct nx_vip_control *ctrl,int subaddr, int val)
{
    return ctrl->in_cam->i2c_write(subaddr, val);
}

void nx_vip_i2c_command(struct nx_vip_control *ctrl, u32 cmd, int arg)
{
	struct i2c_client *client = ctrl->in_cam->client;

	if (client)
		client->driver->command(client, cmd, (void *) arg);
	else
		err("i2c client is not registered\n");
}

void nx_vip_reset_camera(void)
{
    /* Do something for GPIO ? */
    
}

void nx_vip_register_camera(struct nx_vip_camera *cam)
{
	nx_vip.camera[cam->id] = cam;
    //printk("register cam = %d\n", cam->id);
	nx_vip_reset_camera();
}

void nx_vip_unregister_camera(struct nx_vip_camera *cam)
{
	int i = 0;

	for (i = 0; i < NX_VIP_MAX_CTRLS; i++) {
		if (nx_vip.ctrl[i].in_cam == cam)
			nx_vip.ctrl[i].in_cam = NULL;
	}
	
	nx_vip.camera[cam->id] = NULL;
}

void nx_vip_set_active_camera(struct nx_vip_control *ctrl, int id)
{
	ctrl->in_cam = nx_vip.camera[id];
	//printk("cam id = %d, clk= %d\n", ctrl->in_cam->id,ctrl->in_cam->clockrate); 

	// Anything we should do?
}

void nx_vip_init_camera(struct nx_vip_control *ctrl)
{
	struct nx_vip_camera *cam = ctrl->in_cam;

	if (cam && cam->id != NX_VIP_TPID && !cam->initialized) {
		nx_vip_i2c_command(ctrl, I2C_CAM_INIT, 0);
		nx_vip_change_resolution(ctrl, CAM_RES_DEFAULT);
		cam->initialized = 1;
	}
}

static int nx_vip_set_output_format(struct nx_vip_control *ctrl,
					struct v4l2_pix_format *fmt)
{
	struct nx_vip_out_frame *frame = &ctrl->out_frame;
	int depth = -1;

	frame->width = fmt->width;
	frame->height = fmt->height;

	switch (fmt->pixelformat) {
	case V4L2_PIX_FMT_YUV420:
		frame->format = FORMAT_YCBCR420;
		frame->planes = 3;
		depth = 12;
		break;

	case V4L2_PIX_FMT_YUYV:
		frame->format = FORMAT_YCBCR422;
		frame->planes = 1;
		frame->order_1p = OUT_ORDER422_YCBYCR;
		depth = 16;
		break;

	case V4L2_PIX_FMT_YVYU:
		frame->format = FORMAT_YCBCR422;
		frame->planes = 1;
		frame->order_1p = OUT_ORDER422_YCRYCB;
		depth = 16;
		break;

	case V4L2_PIX_FMT_UYVY:
		frame->format = FORMAT_YCBCR422;
		frame->planes = 1;
		frame->order_1p = OUT_ORDER422_CBYCRY;
		depth = 16;
		break;

	case V4L2_PIX_FMT_VYUY:
		frame->format = FORMAT_YCBCR422;
		frame->planes = 1;
		frame->order_1p = OUT_ORDER422_CRYCBY;
		depth = 16;
		break;
	case V4L2_PIX_FMT_YUV422P:
		frame->format = FORMAT_YCBCR422;
		frame->planes = 3;
		depth = 16;
		break;
	default :
	    depth = -1;
	}

	switch (fmt->field) {
	case V4L2_FIELD_INTERLACED:
	case V4L2_FIELD_INTERLACED_TB:
	case V4L2_FIELD_INTERLACED_BT:
		frame->scan = SCAN_TYPE_INTERLACE;
		break;

	default:
		frame->scan = SCAN_TYPE_PROGRESSIVE;
		break;
	}

	return depth;
}

int nx_vip_set_output_frame(struct nx_vip_control *ctrl,
				struct v4l2_pix_format *fmt)
{
	struct nx_vip_out_frame *frame = &ctrl->out_frame;
	int depth = 0;

	depth = nx_vip_set_output_format(ctrl, fmt);

	if ((frame->addr[0].virt_y == NULL) && (depth > 0)) {
		if (nx_vip_alloc_output_memory(ctrl)) {
			err("cannot allocate memory\n");
		}
		//printk("cfn = %d, addr[0].virt_y = %x, addr[1].virt_y = %x, addr[2].virt_y = %x, addr[3].virt_y = %x", 
	    //frame->cfn,frame->addr[0].virt_y, frame->addr[1].virt_y, 
	    //frame->addr[2].virt_y, frame->addr[3].virt_y); 
	}

	return depth;
}

int nx_vip_alloc_output_memory(struct nx_vip_control *ctrl)
{
    struct nx_vip_out_frame *info = &ctrl->out_frame;
    /* We should know that Output memory would be DMA coherent area */
    /* But with reserved mem as bootmem area, we could avoid complexity
       of DMA coherent and memory leakage. */
    info->buf_size = info->width * info->height * 2;
    info->addr[0].phys_y = (dma_addr_t)boot_alloc_mem + ctrl->id * 4 * 1024 * 1024;
    info->addr[0].virt_y = ioremap(info->addr[0].phys_y, 4 * 1024 * 1024);
    info("alloc boot mem vir=%x, phys=%x\n", info->addr[0].virt_y, info->addr[0].phys_y);
    info->addr[1].virt_y = info->addr[0].virt_y + 1024;
    info->addr[1].phys_y = info->addr[0].phys_y + 1024;
    info->addr[2].virt_y = info->addr[0].virt_y + 2048;
    info->addr[2].phys_y = info->addr[0].phys_y + 2048;
    info->addr[3].virt_y = info->addr[0].virt_y + 1024 + 2048;
    info->addr[3].phys_y = info->addr[0].phys_y + 1024 + 2048;
    
    if (info->planes > 1) {
        info->addr[0].lu_start = info->addr[0].phys_y;
        info->addr[0].lu_end   = info->addr[0].phys_y + info->width + 4096 * info->height;
        info->addr[0].cb_start = info->addr[0].phys_y + 4096 * info->height;
        info->addr[0].cb_end   = info->addr[0].phys_y + info->width/2 + 4096 * info->height * 2;
        info->addr[0].cr_start = info->addr[0].phys_y + 4096 * info->height * 2;
        info->addr[0].cr_end   = info->addr[0].phys_y + info->width/2 + 4096 * info->height * 3;
        
        info->addr[1].lu_start = info->addr[1].phys_y;
        info->addr[1].lu_end   = info->addr[1].phys_y + info->width + 4096 * info->height;
        info->addr[1].cb_start = info->addr[1].phys_y + 4096 * info->height;
        info->addr[1].cb_end   = info->addr[1].phys_y + info->width/2 + 4096 * info->height * 2;
        info->addr[1].cr_start = info->addr[1].phys_y + 4096 * info->height * 2;
        info->addr[1].cr_end   = info->addr[1].phys_y + info->width/2 + 4096 * info->height * 3;
        
        info->addr[2].lu_start = info->addr[2].phys_y;
        info->addr[2].lu_end   = info->addr[2].phys_y + info->width + 4096 * info->height;
        info->addr[2].cb_start = info->addr[2].phys_y + 4096 * info->height;
        info->addr[2].cb_end   = info->addr[2].phys_y + info->width/2 + 4096 * info->height * 2;
        info->addr[2].cr_start = info->addr[2].phys_y + 4096 * info->height * 2;
        info->addr[2].cr_end   = info->addr[2].phys_y + info->width/2 + 4096 * info->height * 3;
        
        info->addr[3].lu_start = info->addr[3].phys_y;
        info->addr[3].lu_end   = info->addr[3].phys_y + info->width + 4096 * info->height;
        info->addr[3].cb_start = info->addr[3].phys_y + 4096 * info->height;
        info->addr[3].cb_end   = info->addr[3].phys_y + info->width/2 + 4096 * info->height * 2;
        info->addr[3].cr_start = info->addr[3].phys_y + 4096 * info->height * 2;
        info->addr[3].cr_end   = info->addr[3].phys_y + info->width/2 + 4096 * info->height * 3;
    }
    
    memset(info->addr[0].virt_y, 0, 4 * 1024 * 1024);

    return 0;
}
void nx_vip_free_output_memory(struct nx_vip_out_frame *info)
{
    info->addr[0].virt_y = NULL;
    info->addr[0].phys_y = NULL;
    info->addr[1].virt_y = NULL;
    info->addr[1].phys_y = NULL;
    info->addr[2].virt_y = NULL;
    info->addr[2].phys_y = NULL;
    info->addr[3].virt_y = NULL;
    info->addr[3].phys_y = NULL;
}
void nx_vip_set_output_address(struct nx_vip_control *ctrl)
{
}

void nx_vip_start_vip(struct nx_vip_control *ctrl)
{
    int cam_w, cam_h;
    /* Stream On */
    info("start vip %d\n", ctrl->id);
    cam_w = ctrl->in_cam->width;
    cam_h = ctrl->in_cam->height;
    ctrl->regs->VIP_IMGWIDTH = cam_w + 2;
    ctrl->regs->VIP_IMGHEIGHT = cam_h;
    ctrl->regs->VIP_VBEGIN = 0;
    ctrl->regs->VIP_VEND   = 1;
    ctrl->regs->VIP_HBEGIN = 12;
    ctrl->regs->VIP_HEND   = 18;
    ctrl->regs->CLIP_LEFT = 0;
    ctrl->regs->CLIP_RIGHT = 320;
    ctrl->regs->CLIP_TOP = 0;
    ctrl->regs->CLIP_BOTTOM = 240;
    ctrl->regs->CLIP_BASEADDRH = ctrl->out_frame.addr[0].phys_y >> 16;
    ctrl->regs->CLIP_BASEADDRL = ctrl->out_frame.addr[0].phys_y & 0xFFFF;
    ctrl->regs->CLIP_STRIDEH = 0;
    ctrl->regs->CLIP_STRIDEL = 4096;
    
    ctrl->regs->CLIP_LUSEG =  ctrl->out_frame.addr[0].lu_start >> 24;
    ctrl->regs->CLIP_LULEFT = ctrl->out_frame.addr[0].lu_start;
    ctrl->regs->CLIP_LUTOP = ctrl->out_frame.addr[0].lu_start >> 12;
    ctrl->regs->CLIP_LURIGHT = ctrl->out_frame.addr[0].lu_end;
    ctrl->regs->CLIP_LUBOTTOM = ctrl->out_frame.addr[0].lu_end >> 12;
    
    ctrl->regs->CLIP_CRSEG = ctrl->out_frame.addr[0].cr_start >> 24;
    ctrl->regs->CLIP_CRLEFT = ctrl->out_frame.addr[0].cr_start;
    ctrl->regs->CLIP_CRTOP = ctrl->out_frame.addr[0].cr_start >> 12;
    ctrl->regs->CLIP_CRRIGHT = ctrl->out_frame.addr[0].cr_end;
    ctrl->regs->CLIP_CRBOTTOM =ctrl->out_frame.addr[0].cr_end >> 12;
    
    ctrl->regs->CLIP_CBSEG = ctrl->out_frame.addr[0].cb_start >> 24;
    ctrl->regs->CLIP_CBLEFT = ctrl->out_frame.addr[0].cb_start;
    ctrl->regs->CLIP_CBTOP = ctrl->out_frame.addr[0].cb_start >> 12;
    ctrl->regs->CLIP_CBRIGHT = ctrl->out_frame.addr[0].cb_end;
    ctrl->regs->CLIP_CBBOTTOM =ctrl->out_frame.addr[0].cb_end >> 12;
    
    if (ctrl->out_frame.planes > 1) {
        ctrl->regs->CLIP_YUYVENB = 0; /* Disnable YUV422 mode */
        if (ctrl->out_frame.format == FORMAT_YCBCR420) {
            ctrl->regs->CLIP_FORMAT = 0; /* YUV 4:2:0 */
        } else {
            ctrl->regs->CLIP_FORMAT = 1; /* YUV 4:2:2  */ 
        }
    } else {
        ctrl->regs->CLIP_YUYVENB = 1; /* Enable YUV422 linear mode */
    }

    ctrl->regs->VIP_ODINT = 0x000;
    ctrl->regs->VIP_CONFIG |= 1; /* Enable VIP */
    ctrl->regs->VIP_CDENB  |= 0x100 | 0x02; /* Enable Seperator and Clipper */
    ctrl->buf_index = 0;
    FSET_CAPTURE(ctrl);
}

void nx_vip_stop_vip(struct nx_vip_control *ctrl)
{
    /* Stream Off */
    info("stop vip\n");
    ctrl->regs->VIP_CONFIG &= ~1; /* Disable VIP */
    ctrl->regs->VIP_CDENB  &= ~(0x100 | 0x02); /* Disable Seperator and Clipper */
    FSET_STOP(ctrl);
}

void nx_vip_restart_vip(struct nx_vip_control *ctrl)
{
	nx_vip_stop_vip(ctrl);
	nx_vip_start_vip(ctrl);
}

void nx_vip_change_resolution(struct nx_vip_control *ctrl,
					enum nx_vip_cam_res_t res)
{
	struct nx_vip_camera *cam = ctrl->in_cam;

//	nx_vip_stop_decimator(ctrl);
	nx_vip_i2c_command(ctrl, I2C_CAM_RESOLUTION, res);

	switch (res) {
	case CAM_RES_QVGA:
		info("resolution changed to QVGA (320x240) mode\n");
		cam->width = 320;
		cam->height = 240;
		break;

	case CAM_RES_QSVGA:
		info("resolution changed to QSVGA (400x300) mode\n");
		cam->width = 400;
		cam->height = 300;
		break;

	case CAM_RES_VGA:
		info("resolution changed to VGA (640x480) mode\n");
		cam->width = 640;
		cam->height = 480;
		break;

	case CAM_RES_SVGA:
		info("resolution changed to SVGA (800x600) mode\n");
		cam->width = 800;
		cam->height = 600;
		break;

	case CAM_RES_XGA:
		info("resolution changed to XGA (1024x768) mode\n");
		cam->width = 1024;
		cam->height = 768;
		break;

	case CAM_RES_SXGA:
		info("resolution changed to SXGA (1280x1024) mode\n");
		cam->width = 1280;
		cam->height = 1024;
		break;

	case CAM_RES_UXGA:
		info("resolution changed to UXGA (1600x1200) mode\n");
		cam->width = 1600;
		cam->height = 1200;
		break;

	case CAM_RES_DEFAULT:	/* fall through */
	case CAM_RES_MAX:
		/* nothing to do */
		break;
	}
}

static irqreturn_t nx_vip_irq0(int irq, void *dev_id)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) dev_id;
	//struct NX_GPIO_RegisterSet *gpioa;
	int status,st2;

    //gpioa = IO_ADDRESS(PHY_BASEADDR_GPIO);
    //gpioa->GPIOxOUTENB = 0x01;
    //gpioa->GPIOxALTFN[0] &= ~3;
	
	status = ctrl->regs->VIP_HVINT; 
	st2 = ctrl->regs->VIP_ODINT;
	ctrl->regs->VIP_HVINT |= 3; /* All pending IRQ clear */
	ctrl->regs->VIP_ODINT |= 1;

	if (status & 1) { /* VSYNC */
	    //printk("vsync0\n");
	    FSET_CAPTURE(ctrl);
	    if (IS_CAPTURE(ctrl)) {
	        dev_dbg(ctrl->dev, "irq is in capture state\n");
	        FSET_HANDLE_IRQ(ctrl);
	        ctrl->buf_index++;
	        ctrl->buf_index &= 0x03;
	        if (ctrl->out_frame.planes == 1) {
	            ctrl->regs->CLIP_BASEADDRH = ctrl->out_frame.addr[ctrl->buf_index].phys_y >> 16;
	            ctrl->regs->CLIP_BASEADDRL = ctrl->out_frame.addr[ctrl->buf_index].phys_y & 0xFFFF;
	        } else {
	            ctrl->regs->CLIP_LUSEG =  ctrl->out_frame.addr[ctrl->buf_index].lu_start >> 24;
	            ctrl->regs->CLIP_LULEFT = ctrl->out_frame.addr[ctrl->buf_index].lu_start;
	            ctrl->regs->CLIP_LUTOP = ctrl->out_frame.addr[ctrl->buf_index].lu_start >> 12;
	            ctrl->regs->CLIP_LURIGHT = ctrl->out_frame.addr[ctrl->buf_index].lu_end;
	            ctrl->regs->CLIP_LUBOTTOM = ctrl->out_frame.addr[ctrl->buf_index].lu_end >> 12;
	            
	            ctrl->regs->CLIP_CRSEG = ctrl->out_frame.addr[ctrl->buf_index].cr_start >> 24;
	            ctrl->regs->CLIP_CRLEFT = ctrl->out_frame.addr[ctrl->buf_index].cr_start;
	            ctrl->regs->CLIP_CRTOP = ctrl->out_frame.addr[ctrl->buf_index].cr_start >> 12;
	            ctrl->regs->CLIP_CRRIGHT = ctrl->out_frame.addr[ctrl->buf_index].cr_end;
	            ctrl->regs->CLIP_CRBOTTOM =ctrl->out_frame.addr[ctrl->buf_index].cr_end >> 12;
	            
	            ctrl->regs->CLIP_CBSEG = ctrl->out_frame.addr[ctrl->buf_index].cb_start >> 24;
	            ctrl->regs->CLIP_CBLEFT = ctrl->out_frame.addr[ctrl->buf_index].cb_start;
	            ctrl->regs->CLIP_CBTOP = ctrl->out_frame.addr[ctrl->buf_index].cb_start >> 12;
	            ctrl->regs->CLIP_CBRIGHT = ctrl->out_frame.addr[ctrl->buf_index].cb_end;
	            ctrl->regs->CLIP_CBBOTTOM =ctrl->out_frame.addr[ctrl->buf_index].cb_end >> 12;
	        }

		    wake_up_interruptible(&ctrl->waitq);
		}
	    
	}
	if (status & 2) { /* HSYNC */
	    //printk("hsync0\n");
	    //gpioa->GPIOxOUT = 0x01;
	}
	if (st2 & 1) {
	    //printk("clipper done\n");
	}
    //gpioa->GPIOxOUT = 0x0000;

	return IRQ_HANDLED;
}

#if 0
static irqreturn_t nx_vip_irq1(int irq, void *dev_id)
{
	struct nx_vip_control *ctrl = (struct nx_vip_control *) dev_id;
	//struct NX_GPIO_RegisterSet *gpioa;
	int status,st2;

    //gpioa = IO_ADDRESS(PHY_BASEADDR_GPIO);
    //gpioa->GPIOxOUTENB = 0x01;
    //gpioa->GPIOxALTFN[0] &= ~3;
	
	status = ctrl->regs->VIP_HVINT; 
	st2 = ctrl->regs->VIP_ODINT;
	ctrl->regs->VIP_HVINT |= 3; /* All pending IRQ clear */
	ctrl->regs->VIP_ODINT |= 1;

	if (status & 1) { /* VSYNC */
	    //printk("vsync1\n");
	    FSET_CAPTURE(ctrl);
	    if (IS_CAPTURE(ctrl)) {
	        dev_dbg(ctrl->dev, "irq is in capture state\n");
	        FSET_HANDLE_IRQ(ctrl);
	        ctrl->buf_index++;
	        ctrl->buf_index &= 0x03;
#if 0
	        ctrl->regs->CLIP_BASEADDRH = ctrl->out_frame.addr[ctrl->buf_index].phys_y >> 16;
	        ctrl->regs->CLIP_BASEADDRL = ctrl->out_frame.addr[ctrl->buf_index].phys_y & 0xFFFF;
#endif
	        
	        ctrl->regs->CLIP_LUSEG =  ctrl->out_frame.addr[ctrl->buf_index].lu_start >> 24;
	        ctrl->regs->CLIP_LULEFT = ctrl->out_frame.addr[ctrl->buf_index].lu_start;
	        ctrl->regs->CLIP_LUTOP = ctrl->out_frame.addr[ctrl->buf_index].lu_start >> 12;
	        ctrl->regs->CLIP_LURIGHT = ctrl->out_frame.addr[ctrl->buf_index].lu_end;
	        ctrl->regs->CLIP_LUBOTTOM = ctrl->out_frame.addr[ctrl->buf_index].lu_end >> 12;
    
	        ctrl->regs->CLIP_CRSEG = ctrl->out_frame.addr[ctrl->buf_index].cr_start >> 24;
	        ctrl->regs->CLIP_CRLEFT = ctrl->out_frame.addr[ctrl->buf_index].cr_start;
	        ctrl->regs->CLIP_CRTOP = ctrl->out_frame.addr[ctrl->buf_index].cr_start >> 12;
	        ctrl->regs->CLIP_CRRIGHT = ctrl->out_frame.addr[ctrl->buf_index].cr_end;
	        ctrl->regs->CLIP_CRBOTTOM =ctrl->out_frame.addr[ctrl->buf_index].cr_end >> 12;
    
	        ctrl->regs->CLIP_CBSEG = ctrl->out_frame.addr[ctrl->buf_index].cb_start >> 24;
	        ctrl->regs->CLIP_CBLEFT = ctrl->out_frame.addr[ctrl->buf_index].cb_start;
	        ctrl->regs->CLIP_CBTOP = ctrl->out_frame.addr[ctrl->buf_index].cb_start >> 12;
	        ctrl->regs->CLIP_CBRIGHT = ctrl->out_frame.addr[ctrl->buf_index].cb_end;
	        ctrl->regs->CLIP_CBBOTTOM =ctrl->out_frame.addr[ctrl->buf_index].cb_end >> 12;

		    wake_up_interruptible(&ctrl->waitq);
		}
	    
	}
	if (status & 2) { /* HSYNC */
	    //printk("hsync1\n");
	    //gpioa->GPIOxOUT = 0x01;
	}
	if (st2 & 1) {
	    //printk("clipper done\n");
	}
    //gpioa->GPIOxOUT = 0x0000;

	return IRQ_HANDLED;
}
#endif

static
struct nx_vip_control *nx_vip_register_controller(struct platform_device *pdev)
{
	struct nx_platform_vip *pdata;
	struct nx_vip_control *ctrl;
	struct resource *res;
	int i = NX_VIP_MAX_CTRLS - 1;
	int id = pdev->id;


	pdata = to_vip_plat(&pdev->dev);

	ctrl = &nx_vip.ctrl[id];
	ctrl->id = id;
	ctrl->dev = &pdev->dev;
	ctrl->vd = &nx_vip_video_device[id];
	ctrl->rot90 = 0;
	ctrl->vd->minor = id;
	ctrl->out_frame.nr_frames = pdata->buff_count;
	ctrl->out_frame.skip_frames = pdata->skip_count;
	ctrl->scaler.line_length = 0; //pdata->line_length;
	ctrl->streamon = 0;

	sprintf(ctrl->name, "%s%d", NX_VIP_NAME, id);
	strcpy(ctrl->vd->name, ctrl->name);

	atomic_set(&ctrl->in_use, 0);
	mutex_init(&ctrl->lock);
	init_waitqueue_head(&ctrl->waitq);

	/* get resource for io memory */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		err("failed to get io memory region\n");
		return NULL;
	}

	/* request mem region */
	res = request_mem_region(res->start, res->end - res->start + 1, pdev->name);
	if (!res) {
		err("failed to request io memory region\n");
	    return NULL;
	}

	/* ioremap for register block */
	ctrl->regs = (struct NX_VIP_RegisterSet*)ioremap(res->start, res->end - res->start + 1);

	if (!ctrl->regs) {
		err("failed to remap io region\n");
		return NULL;
	}

	/* irq */
	ctrl->irq = platform_get_irq(pdev, 0);

	//if (id == 0) {
	    if (request_irq(ctrl->irq, nx_vip_irq0, IRQF_DISABLED, ctrl->name, ctrl))
		    err("request_irq failed\n");
	//} else {
	//    if (request_irq(ctrl->irq, nx_vip_irq1, IRQF_DISABLED, ctrl->name, ctrl))
	//	    err("request_irq failed\n");
	//}

	/* Enable Clock */
	ctrl->regs->VIPCLKENB = 0x0E; /* PCLK_ALWAYS and BCLK_DYNAMIC */
	ctrl->regs->VIPCLKGEN[0][0] = 0x8000 | (0x03 << 2); /* OUTDISABLE, ICLK */
	/* Configuration */
	ctrl->regs->VIP_CONFIG = 0x02 | (0x00 << 2); /* 8bit interface, CrYCbY order */
	/* Enable IRQ ? */
	ctrl->regs->VIP_HVINT |= 0x100; /* Enable V sync IRQ */
	//nx_vip_reset(ctrl);
	ctrl->regs->VIP_VIP1 = id;

	return ctrl;
}

static int nx_vip_unregister_controller(struct platform_device *pdev)
{
	struct nx_vip_control *ctrl;
	struct nx_platform_vip *pdata;
	int id = pdev->id;

	ctrl = &nx_vip.ctrl[id];

	nx_vip_free_output_memory(&ctrl->out_frame);

	pdata = to_vip_plat(ctrl->dev);

	if (1)
		iounmap(ctrl->regs);

	memset(ctrl, 0, sizeof(*ctrl));
	
	return 0;
}

static int nx_vip_mmap(struct file* filp, struct vm_area_struct *vma)
{
	struct nx_vip_control *ctrl = filp->private_data;
	struct nx_vip_out_frame *frame = &ctrl->out_frame;

	u32 size = vma->vm_end - vma->vm_start;
	u32 pfn, total_size = frame->buf_size;

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	vma->vm_flags |= VM_RESERVED;

	/* page frame number of the address for a source frame to be stored at. */
	pfn = __phys_to_pfn(frame->addr[vma->vm_pgoff].phys_y);

	if (size > total_size) {
		err("the size of mapping is too big\n");
		return -EINVAL;
	}

	if ((vma->vm_flags & VM_WRITE) && !(vma->vm_flags & VM_SHARED)) {
		err("writable mapping must be shared\n");
		return -EINVAL;
	}

	if (remap_pfn_range(vma, vma->vm_start, pfn, size, vma->vm_page_prot)) {
		err("mmap fail\n");
		return -EINVAL;
	}

	return 0;
}

static u32 nx_vip_poll(struct file *filp, poll_table *wait)
{
	struct nx_vip_control *ctrl = filp->private_data;
	u32 mask = 0;

	poll_wait(filp, &ctrl->waitq, wait);

	if (IS_IRQ_HANDLING(ctrl))
		mask = POLLIN | POLLRDNORM;

	FSET_STOP(ctrl);

	return mask;
}

static
ssize_t nx_vip_read(struct file *filp, char *buf, size_t count, loff_t *pos)
{
	struct nx_vip_control *ctrl = filp->private_data;
	size_t end;
	char *ptr, *pp, *bf;
	int ww,hh, i;
	int ret;

	if (IS_CAPTURE(ctrl)) {
		if (wait_event_interruptible(ctrl->waitq, IS_IRQ_HANDLING(ctrl)))
				return -ERESTARTSYS;

		FSET_STOP(ctrl);
	}

	end = min_t(size_t, ctrl->out_frame.buf_size, count);

	//ptr = nx_vip_get_current_frame(ctrl);
	ptr = ctrl->out_frame.addr[(ctrl->buf_index-1) & 0x03].virt_y;
	/* This is bit complex that stride is 4096 */
	ww = ctrl->out_frame.width;
	hh = ctrl->out_frame.height;
	pp = ptr;
	bf = buf;
	if (1 == ctrl->out_frame.planes) {
	    ww *= 2;
	    for (i=0; i<hh; i++) {
	        ret = copy_to_user(bf, pp, ww);
	        bf += ww;
	        pp += 4096;
	    }
	} else {
	    /* Copy Y component */
	    for (i=0; i<hh; i++) {
	        ret = copy_to_user(bf, pp, ww);
	        bf += ww;
	        pp += 4096;
	    }
	    /* Copy Cr/Cb component */
	    if (FORMAT_YCBCR420 == ctrl->out_frame.format) {
	        hh >>= 1;
	    }
	    ww >>= 1;
	    for (i=0; i<hh; i++) {
	        ret = copy_to_user(bf, pp, ww);
	        bf += ww;
	        pp += 4096;
	    }
	    if (FORMAT_YCBCR420 == ctrl->out_frame.format) {
	        pp += 4096 * hh;
	    }
	    for (i=0; i<hh; i++) {
	        ret = copy_to_user(bf, pp, ww);
	        bf += ww;
	        pp += 4096;
	    }
	}

	return end;
}

static
ssize_t nx_vip_write(struct file *filp, const char *b, size_t c, loff_t *offset)
{
	return 0;
}

static int nx_vip_open(struct file *filp) /* Theres no inode information since Ver 2.X */
{
	struct nx_vip_control *ctrl;
	int id, ret;

	//id = MINOR(inode->i_rdev); /* now inode info is not from Open... */
	id = iminor(filp->f_path.dentry->d_inode);
	ctrl = &nx_vip.ctrl[id];

	mutex_lock(&ctrl->lock);

	if (atomic_read(&ctrl->in_use)) {
		ret = -EBUSY;
		goto resource_busy;
	} else {
		atomic_inc(&ctrl->in_use);
		//nx_vip_reset(ctrl);
		filp->private_data = ctrl;
	}

	mutex_unlock(&ctrl->lock);
	// Now set active camera here... for 0? No. id
	nx_vip_set_active_camera(ctrl, id);
	nx_vip_init_camera(ctrl);

	return 0;

resource_busy:
	mutex_unlock(&ctrl->lock);
	return ret;
}

static int nx_vip_release(struct file *filp)
{
	struct nx_vip_control *ctrl;
	int id;

	//id = MINOR(inode->i_rdev);
	id = iminor(filp->f_path.dentry->d_inode);
	ctrl = &nx_vip.ctrl[id];

	mutex_lock(&ctrl->lock);

	atomic_dec(&ctrl->in_use);
	filp->private_data = NULL;

	mutex_unlock(&ctrl->lock);

	return 0;
}

static const struct v4l2_file_operations nx_vip_fops = {
	.owner = THIS_MODULE,
	.open = nx_vip_open,
	.release = nx_vip_release,
	.ioctl = video_ioctl2,
	.read = nx_vip_read,
	.write = nx_vip_write,
	.mmap = nx_vip_mmap,
	.poll = nx_vip_poll,
};

static void nx_vip_vdev_release(struct video_device *vdev)
{
	kfree(vdev);
}

struct video_device nx_vip_video_device[NX_VIP_MAX_CTRLS] = {
	[0] = {
		.vfl_type = 0, /* VID_TYPE_OVERLAY | VID_TYPE_CAPTURE | VID_TYPE_CLIPPING | VID_TYPE_SCALES */
		.fops = &nx_vip_fops,
		.ioctl_ops = &nx_vip_v4l2_ops,
		.release  = nx_vip_vdev_release,
	},
	[1] = {
		.vfl_type = 0, /* VID_TYPE_OVERLAY | VID_TYPE_CAPTURE | VID_TYPE_CLIPPING | VID_TYPE_SCALES */
		.fops = &nx_vip_fops,
		.ioctl_ops = &nx_vip_v4l2_ops,
		.release  = nx_vip_vdev_release,
	},
	[2] = {
		.vfl_type = 0, /* VID_TYPE_OVERLAY | VID_TYPE_CAPTURE | VID_TYPE_CLIPPING | VID_TYPE_SCALES */
		.fops = &nx_vip_fops,
		.ioctl_ops = &nx_vip_v4l2_ops,
		.release  = nx_vip_vdev_release,
	},
};

static int nx_vip_init_global(struct platform_device *pdev)
{
	/* camera clock */
	/* Enable Pclk and Bclk */

	nx_vip.dma_start = 0; /* Some DMA memory... */
	nx_vip.dma_total = 16 * 1024 * 1024;
	nx_vip.dma_current = nx_vip.dma_start;

	/* test pattern */
	nx_vip.camera[test_pattern.id] = &test_pattern;

	return 0;
}

static int nx_vip_probe(struct platform_device *pdev)
{
	struct nx_platform_vip *pdata;
	struct nx_vip_control *ctrl;
	struct clk *srclk;
	int ret;
	unsigned gcon;

	ctrl = nx_vip_register_controller(pdev);
	if (!ctrl) {
		err("cannot register nx v4l2 controller\n");
		goto err_fimc;
	}
	
	pdata = to_vip_plat(&pdev->dev);
	if (pdata->cfg_gpio)
		pdata->cfg_gpio();
	ctrl->gpio_reset = pdata->gpio_reset;

	/* vip source clock set? */

	/* things to initialize once */
	if (ctrl->id == 0) {
		ret = nx_vip_init_global(pdev);
		if (ret)
			goto err_global;
	}

	ret = video_register_device(ctrl->vd, VFL_TYPE_GRABBER, ctrl->id);
	if (ret) {
		err("cannot register video driver\n");
		goto err_video;
	}

	info("controller %d registered successfully\n", ctrl->id);

	return 0;

err_video:

err_global:

err_clk_io:
	nx_vip_unregister_controller(pdev);

err_fimc:
	return -EINVAL;
	
}

static int nx_vip_remove(struct platform_device *pdev)
{
	nx_vip_unregister_controller(pdev);

	return 0;
}

int nx_vip_suspend(struct platform_device *dev, pm_message_t state)
{
	return 0;
}

int nx_vip_resume(struct platform_device *dev)
{
	return 0;
}

static struct platform_driver nx_vip_driver = {
	.probe		= nx_vip_probe,
	.remove		= nx_vip_remove,
	.suspend	= nx_vip_suspend,
	.resume		= nx_vip_resume,
	.driver		= {
		.name	= "nx-v4l2",
		.owner	= THIS_MODULE,
	},
};

static int nx_vip_register(void)
{
	platform_driver_register(&nx_vip_driver);

	return 0;
}

static void nx_vip_unregister(void)
{
	platform_driver_unregister(&nx_vip_driver);
}

module_init(nx_vip_register);
module_exit(nx_vip_unregister);
	
MODULE_AUTHOR("Seungwoo Kim <ksw@mostitech.com>");
MODULE_DESCRIPTION("Nexell nxp2120 v4l2 driver");
MODULE_LICENSE("GPL");
