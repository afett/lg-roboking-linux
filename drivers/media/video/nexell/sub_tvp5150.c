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
#include <linux/videodev2.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/highmem.h>
#include <linux/freezer.h>
#include <linux/platform_device.h>

#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-subdev.h>

#include <media/tvp5150.h>

#include "vip_drv.h"

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "tvp5150: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#define ERROUT(msg...)		{ 	\
		printk(KERN_ERR "ERROR: %s, %s line %d: \n",	\
			__FILE__, __FUNCTION__, __LINE__),			\
		printk(KERN_ERR msg); }

#define TVP5150A_IDENTYFY  	0x5051

#define DECODERSTATUS_MASK_READYFORCAPURE		(1<<0)
#define DECODERSTATUS_MASK_COPYPROTECTION		(1<<1)
#define DECODERSTATUS_MASK_60HZ					(1<<5)
#define DECODERSTATUS_MASK_LOOPLOCK				(1<<6)
#define DECODERSTATUS_MASK_INTERLACE			(1<<7)


static struct v4l2_subdev * tvp5150 = NULL;
static struct i2c_client  * i2c_client = NULL;

#ifndef CONFIG_VIDEO_TVP5150
static struct i2c_adapter * i2c_adap   = NULL;
#endif

static int i2c_read(unsigned char addr)
{
	struct i2c_client *c = i2c_client;
	unsigned char buffer[1];
	int rc;

	buffer[0] = addr;
	if (1 != (rc = i2c_master_send(c, buffer, 1))) {
		printk(KERN_ERR "tvp5150: i2c i/o error: rc == %d (should be 1)\n", rc);
		return -1;
	}

	if (1 != (rc = i2c_master_recv(c, buffer, 1))) {
		printk(KERN_ERR "tvp5150: i2c i/o error: rc == %d (should be 1)\n", rc);
		return -1;
	}

	DBGOUT("tvp5150: read 0x%02x = 0x%02x\n", addr, buffer[0]);

	return (buffer[0]);
}

#ifndef CONFIG_VIDEO_TVP5150
static int i2c_write(unsigned char addr, unsigned char value)
{
	struct i2c_client *c = i2c_client;
	unsigned char buffer[2];
	int rc;

	buffer[0] = addr;
	buffer[1] = value;

	DBGOUT("tvp5150: writing 0x%02x 0x%02x\n", buffer[0], buffer[1]);
	if (2 != (rc = i2c_master_send(c, buffer, 2))) {
		printk(KERN_ERR "tvp5150: i2c i/o error: rc == %d (should be 2)\n", rc);
		return -1;
	}

	return 0;
}

static int tvp5150_identify(void)
{
	/* Check Device id */
	u_int id[2]={0,};

	DBGOUT("%s\n", __func__);

	id[0] = i2c_read(0x80);
	id[1] = i2c_read(0x81);

	if (TVP5150A_IDENTYFY != ((id[1]<<8) | id[0])) {
		DBGOUT("%s: Fail, Can't to access with I2C !!!\n", __func__);
		return -1;
	}

	DBGOUT("%s: identify code = 0x%x\n", __func__, (id[1]<<8) | id[0]);
	return 0;
}

static int tvp5150_init_device(void)
{
	DBGOUT(" %s: InitDevice \n", __func__);

	// Operation Mode Control Register : Disable Power-down
	i2c_write(0x02, 0x00);

	// Some delay
	msleep(500);

	i2c_write(0x05, 0x01);		// software reset, all default set
	i2c_write(0x0F, 0x02);		// Select FID, GPCL
	i2c_write(0x00, 0x00);		// Composite = AIP1A

	// Misc Control Register : GPCL Output Enable & Low, YCbCr & Clock Enable
	i2c_write(0x03, 0x29);

	return 0;
}
#endif

static uint tvp5150_get_status(void)
{
	u_int stat[7]= {0,};
	u_int temp = 0;
	int   i;

	stat[0] = i2c_read(0x88);		// stat #1
	stat[1] = i2c_read(0x89);		// stat #2
	stat[6] = i2c_read(0xC0);		// stat A

	for (i=0; i < 7; i++)
		DBGOUT("%s: stat[%d]:0x%02X\r\n", __func__, i, stat[i]);

	temp = (DECODERSTATUS_MASK_60HZ | DECODERSTATUS_MASK_INTERLACE);

	if ( stat[6] & (1<<7))	temp |= DECODERSTATUS_MASK_READYFORCAPURE;	// |= 0x01 (ready)
	if ( stat[1] & 0x07)		temp |= DECODERSTATUS_MASK_COPYPROTECTION;	// |= 0x07 (macrovision)
	if ( stat[0] & (1<<5))	temp &= ~DECODERSTATUS_MASK_60HZ;			// &= 0xdf (50hz)
	if ((stat[0] & 0x06)==6)	temp |= DECODERSTATUS_MASK_LOOPLOCK;		// |= 0x40
	if ( stat[0] & (1<<6))	temp &= ~DECODERSTATUS_MASK_INTERLACE; 		// &= 0x7f (Non Interlace)

	DBGOUT("%s, stat = 0x%02x\n", __func__, temp);

	return temp;
}

static int tvp5150_ops_open(void)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static int tvp5150_ops_set_input(int port)
{
	struct v4l2_subdev *sd = tvp5150;
	DBGOUT("%s\n", __func__);

	return v4l2_subdev_call(sd, video, s_routing, TVP5150_COMPOSITE0, TVP5150_NORMAL, 0);
}

static int tvp5150_ops_get_sync(struct vip_sync *sync)
{
	u_int stat;
	DBGOUT("%s\n", __func__);

	if (! sync)
		return -1;

	stat = tvp5150_get_status();

	if (! (stat & DECODERSTATUS_MASK_READYFORCAPURE))
		DBGOUT("%s: no signal stat=0x%02x \n", __func__, stat);

	DBGOUT("%s: signal %d HZ \n", __func__, stat&DECODERSTATUS_MASK_60HZ?60:50);

	if (stat & DECODERSTATUS_MASK_60HZ) {
		sync->external_sync =   0;
		sync->h_active     	= 720; 	/* active width */
		sync->h_front_porch =   8; 	/* horizontal blank : front porch */
		sync->h_sync_width  =  64; 	/* horizontal blank : sync width */
		sync->h_back_porch  =  66; 	/* horizontal blank : back porch */
		sync->v_active     	= 240; 	/* active height */
		sync->v_front_porch =   0; 	/* vertical blank : front porch (4) */
		sync->v_sync_width  =   6; 	/* vertical blank : sync width */
		sync->v_back_porch  =  12; 	/* vertical blank : back porch */
	} else {
		sync->external_sync =   0;
		sync->h_active     	= 720; 	/* active width */
		sync->h_front_porch =  10; 	/* horizontal blank : front porch */
		sync->h_sync_width  =  64; 	/* horizontal blank : sync width */
		sync->h_back_porch  =  70; 	/* horizontal blank : back porch */
		sync->v_active     	= 288; 	/* active height */
		sync->v_front_porch =   2; 	/* vertical blank : front porch (4) */
		sync->v_sync_width  =   6; 	/* vertical blank : sync width */
		sync->v_back_porch  =  12; 	/* vertical blank : back porch */
	}

	sync->framepersec =  30;

	if (DECODERSTATUS_MASK_INTERLACE & stat)
		sync->interlace = 1;
	else
		sync->interlace = 0;

	return 0;
}

static struct vip_sub_ops tvp5150_ops = {
	.open	  	= tvp5150_ops_open,
	.set_input	= tvp5150_ops_set_input,
	.get_sync	= tvp5150_ops_get_sync,
};

#if defined (CONFIG_PLAT_NXC1000_VTK)
#define	TVP5151_ID		0xB8
#else
#define	TVP5151_ID		0xBA
#endif

static int __init tvp5150_subdev_init(void)
{
	int 	i2c_port = 2;
	u_short i2c_addr = (TVP5151_ID>>1);

#ifdef CONFIG_VIDEO_TVP5150
	tvp5150 = register_vip_subdev(0, "tvp5150", i2c_port, i2c_addr, &tvp5150_ops);
	if (! tvp5150)
		return -1;
	i2c_client = v4l2_get_subdevdata(tvp5150);

	return 0;
#else
	struct i2c_board_info info;
	int ret = 0;

	i2c_adap = i2c_get_adapter(i2c_port);
	if (!i2c_adap) {
		ERROUT("can't get i2c adapter %d\n", i2c_port);
		return -1;
	}

	info.addr = i2c_addr;
	strlcpy(info.type, "tvp5150", I2C_NAME_SIZE);

	i2c_client = i2c_new_device(i2c_adap, &info);
	if (!i2c_client) {
		ERROUT("can't add i2c device at 0x%x\n", (unsigned int)info.addr);
		return -1;
	}

	tvp5150 = register_vip_subdev(0, NULL, i2c_port, i2c_addr, &tvp5150_ops);

	ret = tvp5150_identify();
	if (0 > ret)
		return ret;

	return tvp5150_init_device();
#endif
}

static void __exit tvp5150_subdev_exit(void)
{
	unregister_vip_subdev(0, tvp5150);

#ifndef CONFIG_VIDEO_TVP5150
	if (i2c_adap) {
		i2c_del_adapter(i2c_adap);
		i2c_adap   = NULL;
		i2c_client = NULL;
	}
#endif
}


module_init(tvp5150_subdev_init);
module_exit(tvp5150_subdev_exit);

MODULE_AUTHOR("jhkim <jhkin@nexell.co.kr>");
MODULE_DESCRIPTION("Register VIP sub device for the Nexell");
MODULE_LICENSE("GPL");
