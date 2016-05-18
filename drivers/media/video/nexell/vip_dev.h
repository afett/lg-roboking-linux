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
#ifndef __VIP_DEV_H__
#define __VIP_DEV_H__

#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/fourcc.h>

#include "vip_buf.h"

/* vip scan mode */
enum {
	SCAN_PROGRESS	= 0,
	SCAN_INTERLACE	= 1,
};

/* vip out device */
enum {
	VIP_CLIPPER 	= 0,
	VIP_DECIMATOR 	= 1,
};

#define	VIP_ROTATE_ENABLE	0x0001
#define	VIP_V_FLIP_ENABLE	0x0002
#define	VIP_H_FLIP_ENABLE	0x0004

struct vip_output
{
	int			out_dev;	/* output device, 0 = clipper, 1 = decimator. */
	int			scan_mode;	/* output scan mode type, 0 = progressive, 1 = Interlaced. */
	u_int		fourcc;		/* output video format.	YUV420, YUV422, YUV444, YUYV. */
	int			c_left;		/* clipper's left coord, must be multiple of 16. */
	int			c_top;		/* clipper's top coord, must be multiple of 16. */
	int			c_right;	/* clipper's right coord, must be multiple of 16. */
	int			c_bottom;	/* clipper's bottom coord, must be multiple of 16. */
	int			s_width;	/* decimator's scale down image width, must be multiple of 16 */
	int			s_height;	/* decimator's scale down image height, must be multiple of 16 */
	int			width;		/* out image width */
	int			height;		/* out image height */
	u_int		rotate;		/* rotate function (Rotate/Vertical flip/Horizontal flip) */
};

struct vip_signal {
	u_int		ex_fd_sig;		/* External field signal, default 0, 0: not use, 1: use */
	u_int		ex_fd_sel;		/* External field select, default 0, Bypass[Low odd] (1:Invert[Low even], 2: Fix 0[odd], 3: Fix 1[even]) */
	u_int		ex_vd_sig;		/* External valid signal, default 0 , 0: not use, 1: use */
	u_int		ex_vd_pol;		/* External valid pol, default 0 , 0: disable, 1: enable) */
};

struct vip_sync {
   	int 		external_sync;	/* 	specifies whether external H/V Sync signals are used or not.
   									1 = External H/V Sync, 0 = H/V Sync in Embedded Sync. */
 	int 		interlace;   	/* 	specifies whether the scan mode is interlace or not.
 		              				1 = Interlace mode, 0 = Progressive mode. */
   	int			h_active;		/* 	specifies the horizontal active pixel, 0 ~ 4096.
   									this value must be multiple of 2. */
   	int			h_sync_width; 	/*	specifies the horizontal sync width in pixel.
   									this argument is only valid when 'external_sync' is '0'. */
   	int			h_front_porch; 	/*	specifies the horizontal sync front porch in pixel.
   									this value must be greater than 7.
   									this argument is only valid when 'external_sync' is '0'. */
   	int			h_back_porch; 	/*  specifies the horizontal sync back porch in pixel.
   							    	this value must be greater than 0.
   							    	this argument is only valid when 'external_sync' is '1'. */
   	int			v_active;		/*  specifies the vertical active pixel, 0 ~ 4096.
   							    	this value must be multiple of 2. */
   	int			v_sync_width; 	/*  specifies the vertical sync width in lines.
   							    	this argument is only valid when 'external_sync' is '0'. */
   	int			v_front_porch; 	/*  specifies the vertical sync front porch in lines.
   							    	this argument is only valid when 'external_sync' is '0'. */
   	int			v_back_porch; 	/*  specifies the vertical sync back porch in lines.
   							    	this argument is only valid when 'external_sync' is '1'. */
	int			framepersec;	/*	input video image frame per second.	*/
};

struct vip_sub_ops {
	int		(*open) 		(void);
	int		(*close)		(void);
	int		(*start)		(void);
	int		(*stop)			(void);
	int		(*get_sync)  	(struct vip_sync *);
	int		(*set_input) 	(int);
	int		(*get_input) 	(int *);
	int		(*enum_input) 	(struct v4l2_input *);
};

struct vip_data {
	/* output type */
	int					in_port;	/* vip input port 0, 1 */
	int					out_dev;	/* output mdoule, 0 = clipper, 1 = decimator. */
	int					dpc_dev;	/* display out module, 0=primary, 1=secondary */

	int					scan_mode;	/* output scan mode type, 0 = progressive, 1 = Interlaced. */
	u_int				fourcc;		/* output video fourcc.	YUV420, YUV422, YUV444, YUYV. */
	u_int				format;		/* output video format.	420, 422, 444, YUYV. */
	int					width;
	int					height;
	int					stride;

	/* clipper's coord, must be multiple of 16. */
	int					c_left;
	int					c_top;
	int					c_right;
	int					c_bottom;

	/* decimator's scale down, must be multiple of 16 */
	int					s_width;
	int					s_height;

	/* rotate */
	int					rotate;
	int					v_flip;
	int					h_flip;

	/* etc */
	u_int				d_order;	/* Default 0: CBY0CRY1 (1:CRY1CBY0, 2:Y0CBY1CR, 3:Y1CRY0CB) */
	u_int				invert;		/* Field invert, default 0, use when Interlaced mode for clipper or decimator. */

	/* image and buffer info */
	unsigned int        max_w;
	unsigned int		max_h;
	int					halign;
	int					valign;
	int					buffers;

	/* sub device: decoder info */
	struct v4l2_subdev*	sub_dev;	/* sub device: decoders */
	struct vip_sub_ops*	sub_ops;	/* sub device: decoder ops */
};

/*----------------------------------------------------------------------------*/
#define VIP_MAJOR_VERSION 	0
#define VIP_MINOR_VERSION 	6
#define VIP_RELEASE 		0
#define VIP_VERSION \
	KERNEL_VERSION(VIP_MAJOR_VERSION, VIP_MINOR_VERSION, VIP_RELEASE)

#define norm_minw() 		16
#define norm_minh() 		16

#define	VIP_CAPT_CAPABILITY			\
	V4L2_CAP_VIDEO_CAPTURE 	| 		\
	V4L2_CAP_STREAMING 		| 		\
	V4L2_CAP_READWRITE		|		\
	V4L2_CAP_VIDEO_OVERLAY	|		\
	V4L2_CAP_VIDEO_OUTPUT_OVERLAY

#define	VIP_VMEM_H_ALIGN		1
#define	VIP_VMEM_V_ALIGN		1

#define	VIP_VID_CAPT_FMT_NUM	1				/* suport only FOURCC_YUYV */

#define	VIP_MAX_SUBDEVS			4

#define	VIP_OUT_MODULE			VIP_CLIPPER
#define	VIP_OUT_FOURCC			FOURCC_YUYV		/* FOURCC_YUYV, FOURCC_MVS0, FOURCC_MVS2, FOURCC_MVS4 */
#define	VIP_OVERLAY_OUT			0

#endif	/* __VIP_DEV_H__ */
