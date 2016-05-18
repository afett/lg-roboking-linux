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
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/pci.h>
#include <linux/random.h>
#include <linux/version.h>
#include <linux/mutex.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/freezer.h>
#include <linux/platform_device.h>
#include <media/v4l2-ioctl.h>

/* nexell soc headers */
#include "vip_drv.h"

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "vip-drv: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#define ERROUT(msg...)		{ 	\
		printk(KERN_ERR "ERROR: %s, %s line %d: \n",	\
			__FILE__, __FUNCTION__, __LINE__),			\
		printk(KERN_ERR msg); }

/*------------------------------------------------------------------------------
 * globals
 */
static LIST_HEAD(vip_devlist);
static struct vm_operations_struct mmap_ops;

/*----------------------------------------------------------------------------*/
struct v4l2_subdev * register_vip_subdev(int dev_id, const char *dev_name,
							int i2c_port, unsigned short i2c_addr, struct vip_sub_ops *ops)
{
	struct vip_dev	   *dev = NULL;
	struct v4l2_subdev *sub = NULL;
	struct list_head   *lh;
	int ret = -1;

	DBGOUT("%s\n", __func__);

	list_for_each(lh, &vip_devlist) {
		dev = list_entry(lh, struct vip_dev, dev_list);
		if (dev_id == dev->dev_id) {
			ret = 0;
			break;
		}
	}

	if (! dev || ret) {
		ERROUT("fail, not exist device id (%d) ...\n", dev_id);
		return NULL;
	}

	/* init sub device driver */
	if (dev_name) {
		dev->i2c_adap = i2c_get_adapter(i2c_port);
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
		sub = v4l2_i2c_new_subdev(&dev->v4l2_dev, dev->i2c_adap,
					dev_name, dev_name, i2c_addr, NULL);
	#else
		sub = v4l2_i2c_new_subdev(&dev->v4l2_dev, dev->i2c_adap,
					dev_name, dev_name, i2c_addr);
	#endif
		if (! sub) {
			ERROUT("fail, invalid sub device=%s ...\n", dev_name);
			return NULL;
		}
	}

	/* set decoder device info */
	dev->data->sub_dev = sub;
	dev->data->sub_ops = ops;

	return sub;
}
EXPORT_SYMBOL(register_vip_subdev);

void unregister_vip_subdev(int dev_id, struct v4l2_subdev *subdev)
{
	struct vip_dev   *dev = NULL;
	struct list_head *lh;
	int ret = -1;

	DBGOUT("%s\n", __func__);

	list_for_each(lh, &vip_devlist) {
		dev = list_entry(lh, struct vip_dev, dev_list);
		if (dev_id == dev->dev_id) {
			ret = 0;
			break;
		}
	}

	/* clear decoder device info */
	if (subdev != dev->data->sub_dev)	{
		ERROUT("fail, not exist sub device\n");
		return;
	}

	/* release i2c adapter */
	if (0 == ret && dev->i2c_adap) {
		i2c_del_adapter(dev->i2c_adap);
		dev->i2c_adap = NULL;
	}

	dev->data->sub_dev = NULL;
	dev->data->sub_ops = NULL;
}
EXPORT_SYMBOL(unregister_vip_subdev);

/*
 * supported tv norms, depend on decoder capability
 * ex>
 *		V4L2_STD_PAL    | V4L2_STD_PAL_N | \
 *		V4L2_STD_PAL_Nc | V4L2_STD_SECAM | \
 *	 	V4L2_STD_NTSC   | V4L2_STD_PAL_M | \
 * 		V4L2_STD_PAL_60
 */
v4l2_std_id	vip_dec_norm = 0;
v4l2_std_id	vip_cur_norm = V4L2_STD_NTSC;

static void verify_window(struct vip_dev *dev, struct v4l2_window *win)
{
	struct vip_data *dat = dev->data;

	if (win->w.width  > dat->max_w)
		win->w.width  = dat->max_w;
	if (win->w.height > dat->max_h)
		win->w.height = dat->max_h;

	/* align */
	win->w.width  = (win->w.width /16)*16;
	win->w.height = (win->w.height/16)*16;
}

/*----------------------------------------------------------------------------*/
static void mmap_open (struct vm_area_struct *vma);
static void mmap_close(struct vm_area_struct *vma);

static int mmap_check(void *priv)
{
	struct vip_fh 	*fh  = priv;
	struct vip_dev 	*dev = fh->dev;
	int i;

	DBGOUT("%s\n", __func__);

	/* check mmapped for muti task */
	for (i = 0; VIDEO_MAX_FRAME > i; i++) {
		if (dev->bufs[i]) {
			if (dev->bufs[i]->mmapped) {
				ERROUT("fail, already mmapped, index %d ...\n", i);
				return -EBUSY;
			}
		}
	}
	return 0;
}

/*
 * execute mmap_setup, before this function
 */
static int mmap_mapper(void *priv, struct vm_area_struct *vma)
{
	struct vip_fh  	  *fh  = priv;
	struct vip_dev	  *dev = fh->dev;
	struct vip_buffer *buf = NULL;

	int first, ret;
	unsigned long size;
	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;

	DBGOUT("%s\n", __func__);

	mutex_lock(&dev->b_lock);

	if (!(vma->vm_flags & VM_WRITE) || !(vma->vm_flags & VM_SHARED)) {
		mutex_unlock(&dev->b_lock);
		return -EINVAL;
	}

	/* look for first buffer to map */
	for (first = 0; VIDEO_MAX_FRAME > first; first++) {
		if (!dev->bufs[first])
			continue;

		if (V4L2_MEMORY_MMAP != dev->bufs[first]->memory)
			continue;
		if (dev->bufs[first]->boff == offset)
			break;
	}

	if (VIDEO_MAX_FRAME == first) {
		ERROUT("fail, invalid user space offset [offset=0x%lx]\n",
			offset);
		mutex_unlock(&dev->b_lock);
		return -EINVAL;
	}

	buf = dev->bufs[first];
	if (1 == buf->mmapped) {
		ERROUT("fail, already mmapped, index %d\n", first)
		mutex_unlock(&dev->b_lock);
		return -EINVAL;
	}

	buf->start 	 = vma->vm_start;
	buf->end   	 = vma->vm_end;
	buf->baddr	 = vma->vm_start;
	buf->mmapped = 1;

	/* remapping */
	buf->vaddr	= ioremap_nocache(buf->paddr, buf->bsize);
	if (!buf->vaddr) {
		ERROUT("ioremap_nocache size %ld failed\n",
			buf->bsize);
		goto error;
	}
	DBGOUT("[%d] ioremap_nocache at %p addr %p (size %ld)\n",
		first, (void*)buf->paddr, (void*)buf->vaddr, buf->bsize);

	/* Try to remap memory */
	size = vma->vm_end - vma->vm_start;
	size = (size < buf->bsize) ? size : buf->bsize;

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	ret = remap_pfn_range(vma, vma->vm_start,
				 buf->paddr >> PAGE_SHIFT,
				 size, vma->vm_page_prot);
	if (ret) {
		ERROUT("fail, remap failed with error %d. ", ret);
		iounmap(buf->vaddr);
		goto error;
	}

	/* set vma ops */
	vma->vm_ops          = &mmap_ops;
	vma->vm_flags       |= VM_DONTEXPAND;
	vma->vm_private_data = buf;

	mmap_open(vma);

	DBGOUT("%s, [%d] buf=0x%x: %08lx-%08lx (%lx) pgoff %08lx\n",
		__func__, first, (u_int)buf, vma->vm_start, vma->vm_end,
		(long int) buf->bsize, vma->vm_pgoff);

	mutex_unlock(&dev->b_lock);
	return 0;

error:
	buf->mmapped = 0;
	mutex_unlock(&dev->b_lock);
	return -ENOMEM;
}

static int mmap_unmapper(struct vm_area_struct *vma)
{
	struct vip_buffer *buf = vma->vm_private_data;
	struct vip_fh  	  *fh  = buf->priv_data;
	struct vip_dev	  *dev = fh->dev;
	int i;

	DBGOUT("%s\n", __func__);

	mutex_lock(&dev->b_lock);

	for (i = 0; VIDEO_MAX_FRAME > i; i++) {
		if (NULL == dev->bufs[i])
			continue;

		if (dev->bufs[i] != buf)
			continue;

		buf = dev->bufs[i];
		if (buf->mmapped) {
			DBGOUT("%s, %p [count=%u, mmapped=%d]\n",
				__func__, (void*)buf, buf->count, buf->mmapped);
			iounmap(buf->vaddr);
			buf->vaddr   = NULL;
			buf->start   = 0;
			buf->end     = 0;
			buf->baddr   = 0;
			buf->mmapped = 0;
		}
	}

	mutex_unlock(&dev->b_lock);
	return 0;
}

static int mmap_setup(void *priv, unsigned int bcount, unsigned int bsize,
			enum v4l2_memory memory)
{
	struct vip_fh 	  *fh  = priv;
	struct vip_dev 	  *dev = fh->dev;
	struct vip_buffer *buf = NULL;
	int ret, i;

	DBGOUT("%s, count=%d, size=%d, memory=0x%x\n",
		__func__, bcount, bsize, memory);

	if (memory != V4L2_MEMORY_MMAP     &&
	    memory != V4L2_MEMORY_USERPTR  &&
	    memory != V4L2_MEMORY_OVERLAY) {
		ERROUT("fail, reqbufs memory type invalid\n");
		return -EINVAL;
	}

	if ( bcount > VIDEO_MAX_FRAME) {
		ERROUT("fail, reqbufs memory out range %d , %d\n",
			VIDEO_MAX_FRAME , bcount);
		return -EINVAL;
	}

	mutex_lock(&dev->b_lock);

	ret = mmap_check(fh);
	if (0 != ret)
		goto _err;	/* already mmapped */

	/* Allocate and initialize buffers */
	for (i = 0; bcount > i; i++) {
		buf = kzalloc(sizeof(struct vip_buffer), GFP_KERNEL);
		if (!buf) {
			ret = -ENOMEM;
			goto _err;
		}
		dev->bufs[i] = buf;
		dev->bufs[i]->index     = i;
		dev->bufs[i]->memory    = memory;
		dev->bufs[i]->bsize     = bsize;
		dev->bufs[i]->mmapped   = 0;
		dev->bufs[i]->priv_data = priv;

		switch (memory) {
		case V4L2_MEMORY_MMAP:
			dev->bufs[i]->boff = bsize * i;
			break;
		case V4L2_MEMORY_USERPTR:
		case V4L2_MEMORY_OVERLAY:
			/* nothing */
			break;
		}
		DBGOUT("%s, [%d] buf=%p, %d bytes\n", __func__, i, (void*)buf, bsize);
	}

	/* allocate memory */
	if (! dev->overlay_run) {
		vip_dev_release_buf();	/* release */
		if (0 > vip_dev_alloc_buf(bcount)) {
			ret = -ENOMEM;
			goto _err;
		}
	}

	/* success */
	for (i = 0; bcount > i; i++) {
		dev->bufs[i]->vmb   = vip_buf_get_vm(i);
		dev->bufs[i]->paddr = vip_buf_get_phys(i, 0);
	}

	mutex_unlock(&dev->b_lock);
	return ret;

_err:
	for (i = 0; i < bcount; i++) {
		if (dev->bufs[i])
			kfree(dev->bufs[i]);
		dev->bufs[i] = NULL;
	}

	mutex_unlock(&dev->b_lock);
	return ret;
}

/*
 * execute mmap_unmapper, before this function
 */
static int mmap_release(void *priv)
{
	struct vip_fh     *fh  = priv;
	struct vip_dev    *dev = fh->dev;
	struct vip_buffer *buf = NULL;
	int i, ret = 0;

	DBGOUT("%s\n", __func__);

	mutex_lock(&dev->b_lock);

	/* unmap */
	for (i = 0; VIDEO_MAX_FRAME > i; i++) {
		if (NULL == dev->bufs[i])
			continue;

		buf = dev->bufs[i];
		if (buf->mmapped || buf->vaddr) {
			DBGOUT("%s, %p [count=%u, mmapped=%d, virt=%p]\n",
				__func__, (void*)buf, buf->count, buf->mmapped, buf->vaddr);
			iounmap(buf->vaddr);
		}
	}

	/* release map */
	for (i = 0; VIDEO_MAX_FRAME > i; i++) {
		if (NULL == dev->bufs[i])
			continue;
		DBGOUT("%s, [%d] free buf=0x%x\n", __func__, i, (u_int)dev->bufs[i]);
		kfree(dev->bufs[i]);
		dev->bufs[i] = NULL;
	}

	/* release memory */
	vip_dev_release_buf();
	mutex_unlock(&dev->b_lock);
	return ret;
}

static void mmap_open(struct vm_area_struct *vma)
{
	struct vip_buffer *buf = vma->vm_private_data;

	DBGOUT("%s, %p [count=%u, vma=%08lx-%08lx]\n",
		__func__, (void*)buf, buf->count, vma->vm_start, vma->vm_end);

	buf->count++;
}

static void mmap_close(struct vm_area_struct *vma)
{
	struct vip_buffer *buf = vma->vm_private_data;

	DBGOUT("%s, %p [count=%u,vma=%08lx-%08lx]\n",
		__func__, (void*)buf, buf->count, vma->vm_start, vma->vm_end);

	if(0 != buf->count) {
		buf->count--;
		if (0 == buf->count)
			mmap_unmapper(vma);
	}
}

static struct vm_operations_struct mmap_ops = {
	.open     = mmap_open,
	.close    = mmap_close,
};

/*------------------------------------------------------------------------------
 *
 */
static int vip_vidioc_querycap(struct file *file, void *priv,
				struct v4l2_capability *cap)
{
	struct vip_fh  *fh  = priv;
	struct vip_dev *dev = fh->dev;

	DBGOUT("%s\n", __func__);

	strcpy(cap->driver	, "vip");
	strcpy(cap->card	, "vip");
	strlcpy(cap->bus_info, dev->v4l2_dev.name, sizeof(cap->bus_info));

	cap->version 	  = VIP_VERSION;
	cap->capabilities =	VIP_CAPT_CAPABILITY;

	return 0;
}

/*------------------------------------------------------------------------------
 * VIDIOC_ENUM_FMT handlers
 */
static int vip_vidioc_enum_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_fmtdesc *f)
{
	struct vip_fmt *fmt;

	DBGOUT("%s\n", __func__);

	fmt = vip_dev_get_format(0, f->index);

	strlcpy(f->description, fmt->name, sizeof(f->description));
	f->pixelformat = fmt->fourcc;
	f->type		   = fmt->type;	/* set capability type, CAPTURE or OVERLAY */

	return 0;
}

static int vip_vidioc_enum_fmt_vid_overlay(struct file *file, void *priv,
				struct v4l2_fmtdesc *f)
{
	struct vip_fmt *fmt;

	DBGOUT("%s\n", __func__);

	/* all format support overlay */
	fmt = vip_dev_get_format(0, f->index);

	strlcpy(f->description, fmt->name, sizeof(f->description));
	f->pixelformat = fmt->fourcc;
	f->type		   = fmt->type;	/* set capability type, CAPTURE or OVERLAY */

	return 0;
}

/*------------------------------------------------------------------------------
 * VIDIOC_TRY_FMT handlers
 */
static int vip_vidioc_try_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct vip_fh  	*fh  = priv;
	struct vip_dev  *dev = fh->dev;
	struct vip_data *dat = dev->data;

	struct vip_fmt 	*fmt;
	enum v4l2_field field;

	unsigned int format = f->fmt.pix.pixelformat;
	unsigned int minw, minh;
	unsigned int maxw, maxh;

	DBGOUT("%s\n", __func__);

	if (V4L2_BUF_TYPE_VIDEO_CAPTURE == f->type &&
		V4L2_PIX_FMT_YUYV != format) {
		ERROUT("fail, not support capture, fourcc (%c%c%c%c) \n",
			format&0xFF,(format>>8)&0xFF, (format>>16)&0xFF,(format>>24)&0xFF);
		return -EINVAL;
	}

	fmt = vip_dev_get_format(format, -1);
	if (! fmt)
		return -EINVAL;

	minw  = norm_minw();
	minh  = norm_minh();
	maxw  = dat->max_w;
	maxh  = dat->max_h;

	field = f->fmt.pix.field;

	if (field == V4L2_FIELD_ANY) {
		field = V4L2_FIELD_INTERLACED;
	} else if (V4L2_FIELD_INTERLACED != field) {
		ERROUT("fail, field type invalid\n");
		return -EINVAL;
	}

	f->fmt.pix.field = field;

	if (minw > f->fmt.pix.width)	f->fmt.pix.width  = minw;
	if (f->fmt.pix.width > maxw)	f->fmt.pix.width  = maxw;

	if (minw > f->fmt.pix.height)	f->fmt.pix.height = minw;
	if (f->fmt.pix.height > maxh)	f->fmt.pix.height = maxh;

	f->fmt.pix.bytesperline = (f->fmt.pix.width * fmt->depth) >> 3;
	f->fmt.pix.sizeimage 	= f->fmt.pix.height * f->fmt.pix.bytesperline;

	return 0;
}

static int vip_vidioc_try_fmt_vid_overlay(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct vip_fh   *fh  = priv;
	struct vip_data *dat = fh->dev->data;
	struct vip_fmt  *fmt;

	enum v4l2_field field;
	unsigned int minw, minh;
	unsigned int maxw, maxh;

	DBGOUT("%s\n", __func__);

	fmt = vip_dev_get_format(f->fmt.pix.pixelformat, -1);
	if (! fmt)
		return -EINVAL;

	minw  = norm_minw();
	minh  = norm_minh();
	maxw  = dat->max_w;
	maxh  = dat->max_h;

	field = f->fmt.pix.field;

	if (field == V4L2_FIELD_ANY) {
		field = V4L2_FIELD_INTERLACED;
	} else if (V4L2_FIELD_INTERLACED != field) {
		ERROUT("fail, field type invalid\n");
		return -EINVAL;
	}

	f->fmt.pix.field = field;

	if (minw > f->fmt.pix.width)	f->fmt.pix.width  = minw;
	if (f->fmt.pix.width > maxw)	f->fmt.pix.width  = maxw;

	if (minw > f->fmt.pix.height)	f->fmt.pix.height = minw;
	if (f->fmt.pix.height > maxh)	f->fmt.pix.height = maxh;

	f->fmt.pix.bytesperline = (f->fmt.pix.width * fmt->depth) >> 3;
	f->fmt.pix.sizeimage 	= f->fmt.pix.height * f->fmt.pix.bytesperline;

	return 0;
}

/*------------------------------------------------------------------------------
 * VIDIOC_G_FMT handlers
 */
static int vip_vidioc_g_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct vip_fh *fh = priv;

	DBGOUT("%s\n", __func__);

	f->fmt.pix.width        = fh->width;
	f->fmt.pix.height       = fh->height;
	f->fmt.pix.field        = fh->field;
	f->fmt.pix.pixelformat  = fh->fmt->fourcc;
	f->fmt.pix.bytesperline = (f->fmt.pix.width * fh->fmt->depth) >> 3;
	f->fmt.pix.sizeimage 	= f->fmt.pix.height * f->fmt.pix.bytesperline;

	return 0;
}

static int vip_vidioc_g_fmt_vid_overlay(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct vip_fh *fh = priv;

	DBGOUT("%s\n", __func__);

	f->fmt.win = fh->win;

	return 0;
}

/*------------------------------------------------------------------------------
 * VIDIOC_S_FMT handlers
 */
static int vip_vidioc_s_fmt_vid_cap(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct vip_fh   *fh  = priv;
	struct vip_data *dat = fh->dev->data;
	int crop_w, crop_h, iter_o;
	int ret = 0;

	DBGOUT("%s, type=0x%x\n", __func__, f->type);

	ret = vip_vidioc_try_fmt_vid_cap(file, fh, f);
	if (0 > ret)
		return ret;

	ret = vip_dev_set_format(f->fmt.pix.pixelformat);
	if (0 > ret)
		return ret;

	if (0 >= f->fmt.pix.width)
		return -EINVAL;

	if (0 >= f->fmt.pix.height)
		return -EINVAL;

	fh->fmt    = vip_dev_get_format(f->fmt.pix.pixelformat, -1);
	fh->width  = f->fmt.pix.width;
	fh->height = f->fmt.pix.height;
	fh->field  = f->fmt.pix.field;
	fh->type   = f->type;

	/* set crop */
	iter_o  = dat->scan_mode ? 2 : 1;	/* interlace */
	crop_w  = dat->c_right - dat->c_left;
	crop_h  = dat->c_bottom - dat->c_top;
	crop_h *= iter_o;

	if (crop_w != f->fmt.pix.width || crop_w != f->fmt.pix.height) {

		ret = vip_dev_set_clip(dat->c_left, dat->c_top,
				dat->c_left + f->fmt.pix.width,
				dat->c_top  + f->fmt.pix.height);
		if (0 > ret)
			return ret;
	}

	return ret;
}

static int vip_vidioc_s_fmt_vid_overlay(struct file *file, void *priv,
				struct v4l2_format *f)
{
	struct vip_fh  *fh  = priv;
	struct vip_dev *dev = fh->dev;

	DBGOUT("%s (fmt=0x%x)\n", __func__, (u_int)fh->fmt);

	verify_window(dev, &f->fmt.win);

	fh->win = f->fmt.win;

	if (! fh->fmt) {
		u_int fourcc = VIP_OUT_FOURCC;
		fh->fmt = vip_dev_get_format(fourcc, -1);
		if (! fh->fmt)
			return -EINVAL;
		vip_dev_set_format(fourcc);
	}
	return 0;
}

/*------------------------------------------------------------------------------
 * Buffer handlers
 */
static int vip_vidioc_reqbufs(struct file *file, void *priv,
				struct v4l2_requestbuffers *p)
{
	struct vip_fh   *fh  = priv;
	struct vip_dev  *dev = fh->dev;
	struct vip_data *dat = dev->data;
	int count, size;
	int ret;

	DBGOUT("%s\n", __func__);

	if (1 > (int)p->count) {
		ERROUT("fail, reqbufs count invalid (%u)\n", p->count);
		return -EINVAL;
	}
	if (dev->stream_on) {
		ERROUT("fail, current running ...\n");
		return -EBUSY;
	}

	/* already allocated */
	if (dev->overlay_run)
		count = dat->buffers;
	else
		count = p->count;
	DBGOUT("%s, count=%d, overlay=%d\n",
		__func__, count, dev->overlay_run);

	size = PAGE_ALIGN(fh->width * fh->height * 2);
	ret  = mmap_setup(fh, count, size, p->memory);
	if (0 > ret) {
		ERROUT("fail, reqbufs mmap setup \n");
		return ret;
	}

	/* save */
	dat->buffers = count;
	p->count	 = count;
	return ret;
}

static int vip_vidioc_querybuf(struct file *file, void *priv,
				struct v4l2_buffer *b)
{
	struct vip_fh     *fh  = priv;
	struct vip_dev    *dev = fh->dev;
	struct vip_fmt    *fmt = fh->fmt;
	struct vip_buffer *buf = dev->bufs[b->index];
	int count = dev->data->buffers;

	DBGOUT("%s\n", __func__);

	if ( fmt->fourcc != V4L2_PIX_FMT_YUYV && (
		b->type & V4L2_BUF_TYPE_VIDEO_CAPTURE ||
		b->type & V4L2_BUF_TYPE_VBI_CAPTURE ||
		b->type & V4L2_BUF_TYPE_SLICED_VBI_CAPTURE ))
	{
		ERROUT("fail, can't capture format %s\n", fmt->name);
		return -EINVAL;
	}

	if (b->index >= count) {
		ERROUT("fail, querybuf index %d out of range %d\n", b->index, count);
		return -EINVAL;
	}

	if (! buf) {
		ERROUT("fail, querybuf index %d not reqbuf\n", b->index);
		return -EINVAL;
	}

	/* set buffer params */
	b->type   = fh->type;
	b->memory = buf->memory;
	switch (b->memory) {
	case V4L2_MEMORY_MMAP:
		b->m.offset  = buf->boff;
		b->length    = buf->bsize;
		break;
	case V4L2_MEMORY_USERPTR:
		b->m.userptr = buf->baddr;
		b->length    = buf->bsize;
		break;
	case V4L2_MEMORY_OVERLAY:
		b->m.offset  = buf->boff;
		break;
	}

	b->flags = 0;
	if (buf->mmapped)
		b->flags |= V4L2_BUF_FLAG_MAPPED;

	/* set physical */
	b->reserved = vip_buf_get_phys(b->index, 0);

	return 0;
}

static int vip_vidioc_dqbuf(struct file *file, void *priv,
				struct v4l2_buffer *b)
{
	struct vip_fh    *fh  = priv;
	struct vip_dev   *dev = fh->dev;
	struct list_head *nod = NULL;
	wait_queue_t wait;
	int index;

	DBGOUT("%s\n", __func__);

	if (! dev->stream_on)
		return -EINVAL;

	nod = get_capt_buf_fill();
	if (! nod) {
		init_waitqueue_entry(&wait, current);
		current->state = TASK_INTERRUPTIBLE;
		add_wait_queue(&dev->wait_q, &wait);

		for ( ; ; ){
			set_current_state(TASK_INTERRUPTIBLE);
	      	if (! dev->c_cond)
	      		schedule();
	      	nod = get_capt_buf_fill();
	      	if(nod)
	      		break;
		}
		dev->c_cond = 0;
		current->state = TASK_RUNNING;
		remove_wait_queue(&dev->wait_q, &wait);
	}
	index = container_of(nod, struct vm_buffer, list)->index;
	b->index = index;

	DBGOUT("capt get -> [%2d] lu_a=0x%08x\n",
		b->index, (u_int)(struct vm_buffer*)container_of(nod, struct vm_buffer, list)
		->vm.LuMemory.Address);

	return 0;
}

static int vip_vidioc_qbuf(struct file *file, void *priv,
				struct v4l2_buffer *b)
{
	struct vip_fh    *fh  = priv;
	struct vip_dev   *dev = fh->dev;
	struct list_head *nod = NULL;
	int count = dev->data->buffers;

	DBGOUT("%s\n", __func__);

	if (b->index >= count) {
		ERROUT("fail, qbuf index %d out of range %d.n", b->index, count);
		return -EINVAL;
	}
	if (! dev->stream_on)
		return 0;

	nod = &dev->bufs[b->index]->vmb->list;
	set_capt_buf_free(nod);

	DBGOUT("capt rel -> [%2d] lu_a=0x%08x\n",
		b->index, (u_int)(struct vm_buffer*)container_of(nod, struct vm_buffer, list)
		->vm.LuMemory.Address);

	return 0;
}

#ifdef CONFIG_VIDEO_V4L1_COMPAT
static int vip_vidiocgmbuf(struct file *file, void *priv,
				struct video_mbuf *mbuf)
{
	ERROUT("not implemented\n");
	return -1;
}
#endif


static int vip_vidioc_overlay(struct file *file, void *priv,
				unsigned int on)
{
	struct vip_fh 	*fh  = priv;
	struct vip_dev  *dev = fh->dev;
	struct vip_data *dat = dev->data;

	int count = dat->buffers;
	int ret   = 0;

	DBGOUT("%s, on=%d\n", __func__, on);
	mutex_lock(&dev->mutex);

	if (on) {
		dev->overlay_run = 1;
		if (! dev->stream_on) {
			ret = vip_dev_alloc_buf(count);
			if (0 >ret)
				goto unlock;
		 	vip_dev_start();
	 	}

 	} else {
		dev->overlay_run = 0;
		if (! dev->stream_on) {
	 		vip_dev_stop();
 			vip_dev_release_buf();
 		}
 	}

unlock:
	mutex_unlock(&dev->mutex);

	return ret;
}

static int vip_vidioc_g_fbuf(struct file *file, void *priv,
				struct v4l2_framebuffer *fb)
{
	struct vip_fh 	  *fh = fh;
	struct vip_dev *dev = fh->dev;

	DBGOUT("%s\n", __func__);

	*fb = dev->fb_buf;
	fb->capability = V4L2_FBUF_CAP_LIST_CLIPPING;

	return 0;
}

static int vip_vidioc_s_fbuf(struct file *file, void *priv,
				struct v4l2_framebuffer *fb)
{
	struct vip_fh  *fh  = priv;
	struct vip_dev *dev = fh->dev;

	DBGOUT("%s\n", __func__);

	dev->fb_buf = *fb;

	return 0;
}

/*------------------------------------------------------------------------------
 * Standard handling
 *	ENUMSTD is handled by videodev.c
 */
static int vip_vidioc_s_std(struct file *file, void *priv,
				v4l2_std_id *i)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

/*------------------------------------------------------------------------------
 * Output handling
 */
static int vip_vidioc_enum_output(struct file *file, void *priv,
				struct v4l2_output *a)
{
	DBGOUT("%s\n", __func__);
	if (a->index != 0)
		return -EINVAL;

	memset(a, 0, sizeof(*a));

	a->index = 0;
	a->type  = V4L2_OUTPUT_TYPE_ANALOGVGAOVERLAY;

	strncpy(a->name, "Autodetect", sizeof(a->name)-1);

	return 0;
}

static int vip_vidioc_g_output(struct file *file, void *priv,
				unsigned int *i)
{
	DBGOUT("%s\n", __func__);
	*i = 0;
	return 0;
}

static int vip_vidioc_s_output(struct file *file, void *priv,
				unsigned int i)
{
	DBGOUT("%s\n", __func__);
	if (i != 0)
		return -EINVAL;
	return 0;
}

/*------------------------------------------------------------------------------
 * Input handling
 */
static int vip_vidioc_enum_input(struct file *file, void *priv,
				struct v4l2_input *inp)
{
	struct vip_fh  	*fh  = priv;
	struct vip_data *dat = fh->dev->data;

	DBGOUT("%s\n", __func__);

	if (dat->sub_ops && dat->sub_ops->enum_input) {
		dat->sub_ops->enum_input(inp);
	} else {
		inp->type = V4L2_INPUT_TYPE_CAMERA;
		inp->std  = V4L2_STD_UNKNOWN;
		sprintf(inp->name, "Camera %u", inp->index);
	}

	return 0;
}

static int vip_vidioc_g_input(struct file *file, void *priv,
				unsigned int *i)
{
	struct vip_fh  	*fh  = priv;
	struct vip_data *dat = fh->dev->data;

	DBGOUT("%s\n", __func__);

	*i = 0;

	if (dat->sub_ops && dat->sub_ops->get_input)
		dat->sub_ops->get_input(i);

	return 0;
}

static int vip_vidioc_s_input(struct file *file, void *priv,
				unsigned int i)
{
	struct vip_fh  	*fh  = priv;
	struct vip_data *dat = fh->dev->data;

	DBGOUT("%s\n", __func__);

	if (dat->sub_ops && dat->sub_ops->set_input)
		return dat->sub_ops->set_input(i);

	return 0;
}

/*------------------------------------------------------------------------------
 * Control handling
 */
static int vip_vidioc_queryctrl(struct file *file, void *priv,
				struct v4l2_queryctrl *qc)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static int vip_vidioc_g_ctrl(struct file *file, void *priv,
				struct v4l2_control *ctrl)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static int vip_vidioc_s_ctrl(struct file *file, void *priv,
				struct v4l2_control *ctrl)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

/*------------------------------------------------------------------------------
 * Stream on/off
 */
static int vip_vidioc_streamon(struct file *file, void *priv,
				enum v4l2_buf_type i)
{
	struct vip_fh  *fh  = priv;
	struct vip_dev *dev = fh->dev;

	DBGOUT("%s\n", __func__);

	if (V4L2_BUF_TYPE_VIDEO_CAPTURE != fh->type)
		return -EINVAL;
	if (i != fh->type)
		return -EINVAL;

	mutex_lock(&dev->mutex);

	if (dev->stream_on) {
		ERROUT("fail, current running ...\n");
		return -EBUSY;
	}

	if (! dev->overlay_run)
		vip_dev_start();

	dev->stream_on = 1;

	mutex_unlock(&dev->mutex);
	return 0;
}

static int vip_vidioc_streamoff(struct file *file, void *priv,
				enum v4l2_buf_type i)
{
	struct vip_fh  *fh  = priv;
	struct vip_dev *dev = fh->dev;

	DBGOUT("%s\n", __func__);

	if (fh->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
		return -EINVAL;
	if (i != fh->type)
		return -EINVAL;

	mutex_lock(&dev->mutex);

	if (! dev->overlay_run)
		vip_dev_stop();

	dev->stream_on = 0;

	mutex_unlock(&dev->mutex);
	return 0;
}

/*------------------------------------------------------------------------------
 * Crop ioctls
 */
static int vip_vidioc_cropcap(struct file *file, void *priv,
					struct v4l2_cropcap *cap)
{
	struct vip_fh   *fh  = priv;
	struct vip_data *dat = fh->dev->data;

	DBGOUT("%s\n", __func__);

	if (cap->type != V4L2_BUF_TYPE_VIDEO_CAPTURE &&
	    cap->type != V4L2_BUF_TYPE_VIDEO_OVERLAY)
		return -EINVAL;

	cap->bounds.left 	= 0;
	cap->bounds.top  	= 0;
	cap->bounds.width  	= 1024;
	cap->bounds.height 	= 1024;

	cap->defrect.left 	= 0;
	cap->defrect.top  	= 0;
	cap->defrect.width  = dat->max_w;
	cap->defrect.height = dat->max_h;

	return 0;
}

static int vip_vidioc_g_crop(struct file *file, void *priv,
					struct v4l2_crop *crop)
{
	struct vip_fh  *fh  = priv;
	struct vip_dev *dev = fh->dev;

	DBGOUT("%s\n", __func__);

	if (crop->type != V4L2_BUF_TYPE_VIDEO_CAPTURE &&
	    crop->type != V4L2_BUF_TYPE_VIDEO_OVERLAY)
		return -EINVAL;

	crop->c = dev->crop;
	return 0;
}

static int vip_vidioc_s_crop(struct file *file, void *priv,
					struct v4l2_crop *crop)
{
	struct vip_fh   *fh  = priv;
	struct vip_dev  *dev = fh->dev;
	struct vip_data *dat = dev->data;
	int iter_o = dat->scan_mode ? 2 : 1;	/* interlace */
	int ret;

	DBGOUT("%s\n", __func__);

	if (crop->type != V4L2_BUF_TYPE_VIDEO_CAPTURE &&
	    crop->type != V4L2_BUF_TYPE_VIDEO_OVERLAY)
		return -EINVAL;

	if (0 >= crop->c.height)
		return -EINVAL;

	if (0 >= crop->c.width)
		return -EINVAL;

	ret = vip_dev_set_clip( crop->c.left, crop->c.top,
				crop->c.left + crop->c.width, crop->c.top + crop->c.height );
	if (0 > ret)
		return ret;

	crop->c.left    = dat->c_left;
	crop->c.top	    = dat->c_top;
	crop->c.width   = dat->c_right - dat->c_left;
	crop->c.height  = dat->c_bottom - dat->c_top;
	crop->c.height *= iter_o;	/* interlace mode */

	dev->crop = crop->c;

	return 0;
}

/*------------------------------------------------------------------------------
 * Stream type-dependent parameter ioctls
 */
static int vip_vidioc_g_parm(struct file *file, void *priv,
				struct v4l2_streamparm *a)
{
	ERROUT("not implemented\n");
	return 0;
}

static int vip_vidioc_s_parm(struct file *file, void *priv,
				struct v4l2_streamparm *a)
{
	ERROUT("not implemented\n");
	return 0;
}

/*------------------------------------------------------------------------------
 * VIP V4L2 file operatioins
 */
static int vip_ops_open(struct file *file)
{
	struct vip_dev *dev = video_drvdata(file);
	struct vip_fh  *fh  = NULL;

	int minor = video_devdata(file)->minor;
	int ret   = 0;

	DBGOUT("%s, users=%d\n", __func__, dev->users);

	/* lock */
	mutex_lock(&dev->mutex);

	dev->users++;
	if (dev->users > 1) {
		dev->users--;
		mutex_unlock(&dev->mutex);
		return -EBUSY;
	}

	ret = vip_dev_open();
	if (0 > ret)
		goto err_open;

	/* allocate + initialize per filehandle data */
	fh = kzalloc(sizeof(*fh), GFP_KERNEL);
	if (NULL == fh) {
		ret = -ENOMEM;
		goto err_open;
	}
	printk(KERN_INFO "open /dev/video%d type=%s users=%d minor=%d\n", dev->videv->num,
		v4l2_type_names[V4L2_BUF_TYPE_VIDEO_CAPTURE], dev->users, minor);

	/* set struct */
	dev->h  = 0;
	dev->m 	= 0;
	dev->s 	= 0;
	dev->ms = 0;
	dev->overlay_run = 0;
	dev->stream_on   = 0;

	fh->dev	   = dev;
	fh->type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fh->width  = dev->data->max_w;
	fh->height = dev->data->max_h;

	file->private_data = fh;

	/* unlock */
	mutex_unlock(&dev->mutex);
	return 0;

err_open:
	dev->users--;
	printk("fail, open /dev/video%d (user=%d, ret=%d) ...\n",
		dev->videv->num, dev->users, ret);

	/* unlock */
	mutex_unlock(&dev->mutex);
	return ret;
}

static int vip_ops_close(struct file *file)
{
	struct vip_fh  *fh  = file->private_data;
	struct vip_dev *dev = fh->dev;

	DBGOUT("%s called (minor=%d, users=%d)\n",
		__func__, video_devdata(file)->minor, dev->users);

	vip_dev_close();

	mmap_release((void *)fh);
	kfree(fh);

	mutex_lock(&dev->mutex);
	dev->users--;
	mutex_unlock(&dev->mutex);
	return 0;
}

static ssize_t vip_ops_read(struct file *file, char __user *data, size_t count, loff_t *ppos)
{
	ERROUT("not implemented\n");
	return -1;
}

static unsigned int vip_ops_poll(struct file *file, struct poll_table_struct *wait)
{
	unsigned int rc = 0;
	return rc = POLLIN|POLLRDNORM;
}

static int vip_ops_mmap(struct file *file, struct vm_area_struct *vma)
{
	void *fh = file->private_data;
	int   ret;

	DBGOUT("%s\n", __func__);

	ret = mmap_mapper(fh, vma);
	if (0 > ret)
		return ret;

	return ret;
}

static const struct v4l2_file_operations vip_v4l2_ops = {
	.owner		= THIS_MODULE,
	.open       = vip_ops_open,
	.release    = vip_ops_close,
	.read       = vip_ops_read,
	.poll		= vip_ops_poll,
	.ioctl      = video_ioctl2, /* V4L2 ioctl handler */
	.mmap       = vip_ops_mmap,
};

static const struct v4l2_ioctl_ops vip_ioc_ops = {
	.vidioc_querycap      		= vip_vidioc_querycap,
	.vidioc_enum_fmt_vid_cap  	= vip_vidioc_enum_fmt_vid_cap,
	.vidioc_enum_fmt_vid_overlay= vip_vidioc_enum_fmt_vid_overlay,
	.vidioc_g_fmt_vid_cap     	= vip_vidioc_g_fmt_vid_cap,
	.vidioc_g_fmt_vid_overlay  	= vip_vidioc_g_fmt_vid_overlay,
	.vidioc_s_fmt_vid_cap     	= vip_vidioc_s_fmt_vid_cap,
	.vidioc_s_fmt_vid_overlay 	= vip_vidioc_s_fmt_vid_overlay,
	.vidioc_try_fmt_vid_cap   	= vip_vidioc_try_fmt_vid_cap,
	.vidioc_try_fmt_vid_overlay = vip_vidioc_try_fmt_vid_overlay,
	.vidioc_reqbufs       		= vip_vidioc_reqbufs,
	.vidioc_querybuf      		= vip_vidioc_querybuf,
	.vidioc_qbuf          		= vip_vidioc_qbuf,
	.vidioc_dqbuf         		= vip_vidioc_dqbuf,
	.vidioc_overlay				= vip_vidioc_overlay,
#ifdef CONFIG_VIDEO_V4L1_COMPAT
	.vidiocgmbuf				= vip_vidiocgmbuf,
#endif
	.vidioc_g_fbuf				= vip_vidioc_g_fbuf,
	.vidioc_s_fbuf				= vip_vidioc_s_fbuf,
	.vidioc_s_std         		= vip_vidioc_s_std,
	.vidioc_enum_output			= vip_vidioc_enum_output,
	.vidioc_g_output            = vip_vidioc_g_output,
	.vidioc_s_output            = vip_vidioc_s_output,
	.vidioc_enum_input    		= vip_vidioc_enum_input,
	.vidioc_g_input       		= vip_vidioc_g_input,
	.vidioc_s_input       		= vip_vidioc_s_input,
	.vidioc_queryctrl     		= vip_vidioc_queryctrl,
	.vidioc_g_ctrl        		= vip_vidioc_g_ctrl,
	.vidioc_s_ctrl        		= vip_vidioc_s_ctrl,
	.vidioc_streamon      		= vip_vidioc_streamon,
	.vidioc_streamoff     		= vip_vidioc_streamoff,
	.vidioc_cropcap		 		= vip_vidioc_cropcap,
	.vidioc_g_crop		 		= vip_vidioc_g_crop,
	.vidioc_s_crop				= vip_vidioc_s_crop,
	.vidioc_g_parm				= vip_vidioc_g_parm,
	.vidioc_s_parm				= vip_vidioc_s_parm,
};

/*--------------------------------------------------------------------------------
 * VIP platform_driver functions
 ---------------------------------------------------------------------------------*/
static int __init vip_driver_probe(struct platform_device *pdev)
{
	struct vip_dev 	 	 *dev;
	struct video_device  *vid;
	struct vip_plat_data *plat = pdev->dev.platform_data;
	int ret;

	DBGOUT("%s\n", __func__);

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev) {
		ERROUT("fail, %s allcate device data ...\n", dev_name(&pdev->dev));
		return -ENOMEM;
	}
	snprintf(dev->v4l2_dev.name, sizeof(dev->v4l2_dev.name),
		"%s-%03d", V4L2_DEV_NAME, pdev->id);

	/* register the device instance */
	ret = v4l2_device_register(NULL, &dev->v4l2_dev);
	if (ret)
		goto free_dev;

	/* kzalloc */
	vid = video_device_alloc();
	if (!vid) {
		ret = -ENOMEM;
		goto unreg_dev;
	}

	/* set video_device */
	vid->fops			= &vip_v4l2_ops;
	vid->ioctl_ops		= &vip_ioc_ops;
	vid->release		= video_device_release;
	vid->minor			= -1;
	vid->tvnorms		=  0; 			//vip_dec_norm,
	vid->current_norm	= V4L2_STD_PAL;	//vip_cur_norm,

	snprintf(vid->name, sizeof(vid->name), "%s (%i)", V4L2_DEV_NAME, vid->num);

	/* create the character device */
	ret = video_register_device(vid, VFL_TYPE_GRABBER, -1);
	if (0 > ret)
		goto rel_vdev;

	video_set_drvdata(vid, dev);

	/* init hw device */
	ret = vip_dev_init(dev, plat);
	if (0 > ret)
		goto rel_vdev;

	/* set vip_data   */
	dev->dev_id = pdev->id;
	dev->videv 	= vid;
	dev->device = &pdev->dev;

	/* initialize locks */
	mutex_init(&dev->mutex);
	mutex_init(&dev->b_lock);

	/* Now that everything is fine, let's add it to device list */
	list_add_tail(&dev->dev_list, &vip_devlist);

	platform_set_drvdata(pdev, dev);

	printk(KERN_INFO "V4L2 device registered as /dev/video%d\n", vid->num);

	return 0;

rel_vdev:
	video_device_release(vid);
unreg_dev:
	v4l2_device_unregister(&dev->v4l2_dev);
free_dev:
	kfree(dev);

	return ret;
}

static int vip_driver_remove(struct platform_device *pdev)
{
	struct vip_dev      *dev = platform_get_drvdata(pdev);
	struct video_device *vid  = dev->videv;

	DBGOUT("%s\n", __func__);

	vip_dev_exit(dev);

	video_device_release(vid);
	v4l2_device_unregister(&dev->v4l2_dev);

	kfree(dev);

	return 0;
}

static int vip_driver_suspend(struct platform_device *dev, pm_message_t state)
{
	PM_DBGOUT("%s\n", __func__);
	return 0;
}

static int vip_driver_resume(struct platform_device *dev)
{
	PM_DBGOUT("%s\n", __func__);
	return 0;
}

static struct platform_driver vip_plat_driver = {
	.probe		= vip_driver_probe,
	.remove		= vip_driver_remove,
	.suspend	= vip_driver_suspend,
	.resume		= vip_driver_resume,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= V4L2_DEV_NAME,
	},
};

static int __init vip_driver_init(void)
{
	DBGOUT("%s\n", __func__);
	return platform_driver_register(&vip_plat_driver);
}

static void __exit vip_driver_exit(void)
{
	DBGOUT("%s\n", __func__);
	platform_driver_unregister(&vip_plat_driver);
}

module_init(vip_driver_init);
module_exit(vip_driver_exit);

MODULE_AUTHOR("jhkim <jhkin@nexell.co.kr>");
MODULE_DESCRIPTION("VIP(Video Input Processor) Camera driver for the Nexell");
MODULE_LICENSE("GPL");
