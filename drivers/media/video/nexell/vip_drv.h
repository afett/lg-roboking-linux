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
#ifndef __VIP_DRV_H__
#define __VIP_DRV_H__

#include <linux/i2c.h>
#include <linux/videodev2.h>
#include <media/v4l2-device.h>

#include "vip_dev.h"

/* buffer for one video frame */
struct vip_buffer {
	/* set when setup */
	int					index;
	enum v4l2_memory	memory;
 	unsigned long       bsize;
	size_t           	boff;		/* buffer offset (mmap + overlay) */
 	unsigned long       baddr;		/* buffer addr (userland ptr!) */
	int					mmapped;

	/* set when mmap */
	struct vm_buffer *	vmb;		/* nexell video memory info */
    void             *  vaddr;
	unsigned long       paddr;
 	unsigned long 		start;
 	unsigned long 		end;
   	unsigned int 		count;		/* mapping count for mutli task */

	void			*	priv_data;	/* set file's private_data */
};

struct vip_dev {
	int						dev_id;		/* vip device list num */
	struct list_head       	dev_list;	/* vip device list head */

	struct v4l2_device 	   	v4l2_dev;
	struct video_device	*	videv;
	struct device		*	device;

	struct mutex		   	mutex;
	int                     users;

	/* time stamp */
	int               		h, m, s, ms;

	/* i2c i/o */
	struct i2c_adapter 	*	i2c_adap;

	/* crop */
	struct v4l2_rect    	crop;

	/* thread for generating video stream*/
	struct task_struct	*	kthread;
	wait_queue_head_t		wait_q;
	unsigned int	 		p_cond;		/* preview condition */
	unsigned int	 		c_cond;		/* capture condition */
	unsigned int	 		w_time;

  	/* video overlay */
	struct v4l2_framebuffer	fb_buf;

	/* vip data */
	struct vip_data		*	data;
	struct vip_buffer	*	bufs[VIDEO_MAX_FRAME];
	struct mutex			b_lock;		/* buffer lock */
	int						overlay_run;
	int						stream_on;
};

struct vip_fmt {
	char *	name;
	u32   	fourcc;     /* v4l2 format id */
	int		linear;		/* 1D / 2D memory */
	int   	depth;		/* bit pre pixel */
	u_int   type;
};

struct vip_fh {
	struct vip_dev		*	dev;

	/* video capture */
	struct vip_fmt		*	fmt;
	unsigned int            width;
	unsigned int			height;
	enum v4l2_buf_type      type;
	enum v4l2_field     	field;

    /* video overlay */
	struct v4l2_window		win;
};

/*------------------------------------------------------------------------------
 * Device register function
 */
struct v4l2_subdev *register_vip_subdev(int dev_id, const char *dev_name, int i2c_port,
									unsigned short i2c_addr, struct vip_sub_ops *ops);
void 				unregister_vip_subdev(int dev_id, struct v4l2_subdev *subdev);

/*------------------------------------------------------------------------------
 * Device operation function
 */
int		vip_dev_init(struct vip_dev *, struct vip_plat_data *);
void 	vip_dev_exit(struct vip_dev *);

int 	vip_dev_open(void);
void 	vip_dev_close(void);

void 	vip_dev_start(void);
void 	vip_dev_stop(void);

void * 	vip_dev_get_format(u_int format, int index);
int 	vip_dev_set_format(u_int fourcc);		/* YUYV, 420, 422, 444 */
int 	vip_dev_set_clip(int left, int top, int right, int bottom);


int 	vip_dev_alloc_buf(int count);
void 	vip_dev_release_buf(void);

/*
void 	vip_dev_suspend(void);
void 	vip_dev_resume(void);
int 	vip_dev_reset(void);
int 	vip_dev_set_scale(int width, int height);
int 	vip_dev_set_rotate(u_int degree);
int 	vip_dec_set_input(int input);
int 	vip_dec_set_port(int port);
*/

#endif	/* __VIP_DRV_H__ */