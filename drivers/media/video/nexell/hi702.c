/**********************************************************************
*  Copyright(c) 2010 MOSTiTECH co., ltd. 
*  All Rights Reserved. by Seungwoo Kim <ksw@mostitech.com>
* 
*  This file is based the definition by
*    Driver for HI702 CMOS Image Sensor from Hynix
*
*   Copyright (C) 2008, Robert Jarzmik <robert.jarzmik@free.fr>
*   Copyright (C) 2010, Seungwoo Kim <ksw@mostitech.com>
*  But completely rewritten for most6410/LGE board
*
**********************************************************************/
/*!
 * @file hi702.c
 *
 * @brief Hynix VGA camera driver functions
 *
 * @ingroup Camera
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/i2c-id.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/init.h>
#include <asm/io.h>

#include "nx_vip.h"
#include "hi702.h"

static int reset_frame_rate = 30;
static int hi702_powered =0;
static u32 hi702_pixfmt=0;
static int hi702_flicker=0;
static int hi702_autoexposure=0;
static int hi702_autowhitebalance=0;
static int hi702_hflip=0;
static int hi702_vflip=0;
static int hi702_gain = 0;
static int sensor_init = 0;

static void hi702_set_ae(int active);
static void hi702_set_awb(int active);
static void hi702_flicker_control(int control);

#define COL_FMT(_name, _depth, _fourcc, _colorspace) \
	{ .name = _name, .depth = _depth, .fourcc = _fourcc, \
	.colorspace = _colorspace }
#define RGB_FMT(_name, _depth, _fourcc) \
	COL_FMT(_name, _depth, _fourcc, V4L2_COLORSPACE_SRGB)

#define I2C_FLAG_READ 0x01

static struct i2c_driver hi702_i2c_driver;

static struct i2c_client hi702_i2c_client = {
	.name = "hi702 i2c dev",
	.addr = HI_702_I2C_ADDRESS,
	.driver = &hi702_i2c_driver,
};

static struct nx_vip_camera hi702_data = {
#if defined(DUAL_CAMERA)    
	.id 		= 1,
#else
    .id         = 0,
#endif
	.order422	= CAM_ORDER422_8BIT_YCBYCR,

	/*
	 * 20 fps: 44 MHz
	 * 12 fps: 26 MHz (more stable)
	 */
	.clockrate	= 26600000,

	.width		= 320,
	.height		= 240,
	.offset		= {
		.h1 = 0,
		.h2 = 0,
		.v1 = 0,
		.v2 = 0,
	},

	.polarity	= {
		.pclk	= 0,
		.vsync	= 1,
		.href	= 0,
		.hsync	= 0,
	},

	.initialized	= 0,
};

struct i2c_hi702_data {
	struct nx_vip_camera    *hi702_data;
};


/*
 * Function definitions
 */

static int hi702_i2c_client_xfer(unsigned int addr, char *reg, int reg_len,
				   char *buf, int num, int tran_flag)
{
	struct i2c_msg msg[2];
	int ret;
	u8 data[4];

	if (tran_flag & I2C_FLAG_READ) {
	    msg[0].addr = addr;
	    msg[0].len = 1;
	    msg[0].buf = reg;
	    msg[0].flags = tran_flag;
	    msg[0].flags &= ~I2C_M_RD;

	    msg[1].addr = addr;
	    msg[1].len = num;
	    msg[1].buf = buf;
	    msg[1].flags = tran_flag;

	    msg[1].flags |= I2C_M_RD;
	    
	    ret = i2c_transfer(hi702_data.client->adapter, msg, 2);
	} else {
	    data[0] = *reg;
	    data[1] = buf[0];
	    
	    msg[0].addr = addr;
	    msg[0].len = 2;
	    msg[0].buf = data;
	    msg[0].flags = 0;
	    ret = i2c_transfer(hi702_data.client->adapter, msg, 1);
	}

	if (ret >= 0)
		return 0;

	return ret;
}


static int reg_page_map_set(const u16 reg)
{
	int ret;
	u8 temp1;
	u8 page,temp2;
	static int lastpage = -1;	/* PageMap cache value */

	page = (reg >> 8);
	if (page == lastpage)
		return 0;
	switch  (page) {
        case 0x00:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x20:
        case 0x22:
            /* These are all valid address */
            break;
        default:
		  return -EINVAL;
    }
	
	temp1 = HI702_PAGE_MAP;
	temp2 = page;
	ret = hi702_i2c_client_xfer(HI_702_I2C_ADDRESS, &temp1, 1,
				       (u8 *) & temp2, 1, 0);
	if (ret >= 0)
		lastpage = page;
	return ret;
}

static int hi702_read_reg(u16  reg, u8 * val)
{
	int ret;
	u16 rval;

	ret = reg_page_map_set(reg);
	if (ret < 0) return ret;
	
	reg &= 0xFF;
	
	ret = hi702_i2c_client_xfer(HI_702_I2C_ADDRESS, (char *)&reg, 1,
				       (u8 *) &rval, 1, I2C_FLAG_READ);
	if (!ret) {
	    //printk("rval = %x\n", rval);
		*val = rval;
		return 0;
	}
	return ret;
}

static int hi702_write_reg(u16 reg, u8 val)
{
	u8 temp1;
	int ret;

	temp1 = reg & 0xFF;
	ret = reg_page_map_set(reg);
	if (ret < 0) return ret;
	//printk("write reg %x val %x.\n", reg, val);
	return hi702_i2c_client_xfer(HI_702_I2C_ADDRESS, &temp1, 1,
				       (u8 *) & val, 1, 0);
}


static int hi702_reg_set(const u16 reg, const u8 data)
{
	int ret;
	u8 val;

	ret = hi702_read_reg(reg, &val);
	if (ret >= 0)
		ret = hi702_write_reg(reg, val | data);
	return ret;
}

static int hi702_reg_clear(const u16 reg, const u8 data)
{
	int ret;
	u8 val;

	ret = hi702_read_reg(reg, &val);
	if (ret >= 0)
		ret = hi702_write_reg(reg, val & ~data); 
	return ret;
}

static int hi702_setfmt_rgb565(void)
{
    u8 val;
    reg_read(ISPCTL1, &val);
    val &= ~0xF0;
	return reg_write(ISPCTL1, val | HI_702_ISPCTL1_RGB565);
}

static int hi702_setfmt_rgb444(void)
{
    u8 val;
    reg_read(ISPCTL1, &val);
    val &= ~0xF0;
	return reg_write(ISPCTL1, HI_702_ISPCTL1_RGB444);
}

static int hi702_setfmt_yuv(int swap_yuv_cb_cr, int swap_yuv_y_chromas)
{
    u8 val;
    reg_read(ISPCTL1, &val);
    val &= ~0xF3;
    if (swap_yuv_cb_cr)
        val |= HI_702_ISPCTL1_YPHASE_YUYV;
    if (swap_yuv_y_chromas)
        val |= HI_702_ISPCTL1_UPHASE_VYUY;
    val |= HI_702_ISPCTL1_YUV422;
	return reg_write(ISPCTL1, val);
}

static int hi702_enable(void)
{
	int ret;

	ret = reg_clear(PWRCTL, HI_702_PWRCTL_SLEEP);
	if (ret >= 0)
		hi702_powered = 1;
	return ret;
}

static int hi702_disable(void)
{
	int ret;

	ret = reg_set(PWRCTL, HI_702_PWRCTL_SLEEP);
	if (ret >= 0)
		hi702_powered = 0;
	return ret;
}

static int hi702_sensor_reset(void)
{
	int ret;

	ret = reg_set(PWRCTL, HI_702_PWRCTL_SOFT_RESET);
	if (ret >= 0)
		ret = reg_clear(PWRCTL, HI_702_PWRCTL_SOFT_RESET);
	return ret;
}

static int hi702_set_pixfmt(u32 pixfmt)
{
	int ret = 0;

	switch (pixfmt) {
	case V4L2_PIX_FMT_RGB565:
		ret = hi702_setfmt_rgb565();
		break;
	case V4L2_PIX_FMT_YUYV:
		ret = hi702_setfmt_yuv(0, 0);
		break;
	default:
		//dev_err(&icd->dev, "Pixel format not handled : %x\n", pixfmt);
		ret = -EINVAL;
	}

	if (ret >= 0)
		hi702_pixfmt = pixfmt;

	return ret;
}

static int hi702_set_flip(int flip, int mask)
{
	u8 val;

	reg_read(VDOCTL2, &val);
	if (flip)
      val |=  mask;
    reg_write(VDOCTL2, val);

	return 0;
}

static int hi702_get_global_gain(void)
{
	unsigned int gain;

	gain = 0; /* Some gain value from registers, for what? */
	return gain;
}

static int hi702_set_global_gain(int gain)
{

	/* Set gain, for what ? */
	return 0;
}

int hi702_restore_state(void)
{
	hi702_set_pixfmt(hi702_pixfmt);
	hi702_set_flip(hi702_hflip, HI_702_VDOCTL2_X_FLIP);
	hi702_set_flip(hi702_vflip, HI_702_VDOCTL2_Y_FLIP);
	hi702_set_global_gain(hi702_gain);
	hi702_set_ae(hi702_autoexposure);
	return 0;
}

static int hi702_resume(void)
{
	int ret = 0;

	if (hi702_powered) {
		ret =  hi702_enable();
		if (ret >= 0) {
			hi702_sensor_reset();
			hi702_restore_state();
		}
	}
	return ret;
}

static int hi702_sensor_init(void)
{
	hi702_enable();
	hi702_sensor_reset();
	hi702_set_ae(hi702_autoexposure);
	hi702_set_awb(hi702_autowhitebalance);
	hi702_flicker_control(hi702_flicker);

	return 0;
}

static int hi702_release(void)
{
	hi702_disable();

	return 0;
}

/*
 * Interface active, can use i2c. If it fails, it can indeed mean, that
 * this wasn't our capture interface, so, we wait for the right one
 */
static int hi702_video_probe(void)
{
	u8 data;
	int ret;

	/*
	 * We must have a parent by now. And it cannot be a wrong one.
	 * So this entire test is completely redundant.
	 */

	ret = hi702_enable();
	if (ret)
		goto ei2c;
	ret = hi702_sensor_reset();
	if (ret)
		goto ei2c;

	reg_read(DEVID, &data);

	switch (data) {
	case 0x8C:
		// this is chip we want
		break;
	default:
		ret = -ENODEV;
		//dev_err(&icd->dev,"No HI702 chip detected, register read %x\n", data);
		goto ei2c;
	}

	//dev_info(&icd->dev, "Detected a HI702 chip ID 0x143a\n");

	if (ret)
		goto eisis;

	hi702_autoexposure = 1;
	hi702_autowhitebalance = 1;
	hi702_flicker = HI_702_FLICKER_AUTO_DETECTION;

	return 0;
eisis:
ei2c:
	return ret;
}

/*!
 * hi702 sensor set color configuration
 *
 * @param bright       int
 * @param saturation   int
 * @param red          int
 * @param green        int
 * @param blue         int
 * @return  None
 */
static void
hi702_set_color(int bright, int saturation, int red, int green, int blue)
{
	/* 
	   bright = -1, and/or saturation = -1 should set Auto birghtness 
	   and Auto Color saturation control,
	   otherwise, we should set birght, and saturation as manually.
	*/
	if (-1 == bright) {
	    reg_set(ISPCTL3, HI_702_ISPCTL3_AUTOBRIGHT);    
	} else {
	    reg_clear(ISPCTL3, HI_702_ISPCTL3_AUTOBRIGHT);
	    reg_write(DYOFS, bright & 0x3F); /* ksw : or 0~100 to 0~63? */   
	}
	if (-1 == saturation) {
	    reg_set(SATCTL, HI_702_SATCTL_AUTOCOLORSAT);
	    reg_clear(SATCTL, HI_702_SATCTL_MANCOLORSAT);
	} else {
	    reg_clear(SATCTL, HI_702_SATCTL_AUTOCOLORSAT);
	    reg_set(SATCTL, HI_702_SATCTL_MANCOLORSAT);
	    reg_write(SATB, blue);
	    reg_write(SATR, red);
	}
}

/*!
 * hi702 sensor get color configuration
 *
 * @param bright       int *
 * @param saturation   int *
 * @param red          int *
 * @param green        int *
 * @param blue         int *
 * @return  None
 */
static void
hi702_get_color(int *bright, int *saturation, int *red, int *green, int *blue)
{
}

/*!
 * hi702 sensor set AE measurement window mode configuration
 *
 * @param ae_mode      int
 * @return  None
 */
static void hi702_set_ae_mode(int ae_mode)
{
	
	if (ae_mode)
	    reg_set(AECTL1, HI_702_AECTL1_AE_ENABLE);
}

/*!
 * hi702 sensor get AE measurement window mode configuration
 *
 * @param ae_mode      int *
 * @return  None
 */
static void hi702_get_ae_mode(int *ae_mode)
{
	u8 data;

	reg_read(AECTL1, &data);
	*ae_mode = (data >> 7) & 0x01;
}

/*!
 * hi702 sensor enable/disable AE 
 *
 * @param active      int
 * @return  None
 */
static void hi702_set_ae(int active)
{
	if (active) {
		reg_set(AECTL1, HI_702_AECTL1_AE_ENABLE);
		
	} else {
		reg_clear(AECTL1, HI_702_AECTL1_AE_ENABLE);
	}
	hi702_autoexposure = active;
}

/*!
 * hi702 sensor enable/disable auto white balance
 *
 * @param active      int
 * @return  None
 */
static void hi702_set_awb(int active)
{
	if (active) {
		reg_set(AWBCTL1, HI_702_AWBCTL1_AWB_ENABLE);
	} else {
		reg_clear(AWBCTL1, HI_702_AWBCTL1_AWB_ENABLE);
	}
	hi702_autowhitebalance = active;
}

/*!
 * hi702 sensor set the flicker control 
 *
 * @param control      int
 * @return  None
 */
static void hi702_flicker_control(int control)
{
	u8	data=0;

	reg_read(AECTL1, &data);
	
	switch (control) {
	case HI_702_FLICKER_DISABLE:
		data = 0;
		break;

	case HI_702_FLICKER_MANUAL_50:
		data = HI_702_AECTL1_100HZ;
		break;

	case HI_702_FLICKER_MANUAL_60:
		data = HI_702_AECTL1_120HZ;
		break;

	case HI_702_FLICKER_AUTO_DETECTION:
		data = 0;
		break;
	}
	
	if (control == HI_702_FLICKER_AUTO_DETECTION) {
		reg_set(AECTL1, HI_702_AECTL1_ANTIFLKR_ENABLE);
	} else {
		reg_clear(AECTL1, HI_702_AECTL1_ANTIFLKR_ENABLE);
	    reg_set(AECTL1, data);	
	}
	hi702_flicker = control;

	return;
}
/*!
 * hi702 Get mode&flicker control parameters 
 *
 * @return  None
 */
static void hi702_get_control_params(int *ae, int *awb, int *flicker)
{
	if ((ae != NULL) && (awb != NULL) && (flicker != NULL)) {
		*ae = hi702_autoexposure;
		*awb = hi702_autowhitebalance;
		*flicker = hi702_flicker;
		if (*flicker) {
			switch (*flicker) {
			case 1:
				*flicker = HI_702_FLICKER_MANUAL_50;
				break;
			case 2:
				*flicker = HI_702_FLICKER_MANUAL_60;
				break;
			default:
				*flicker = HI_702_FLICKER_AUTO_DETECTION;
				break;
			}
		} else
			*flicker = HI_702_FLICKER_DISABLE;
	}
}

/*!
 * hi702 get_i2c function
 *
 * @return  none
 */
int hi702_get_i2c(int addr, int *val)
{
    //printk("hi702 get i2c!, addr = 0x%x\n", addr);
	return hi702_read_reg(addr, (u8 *)val);
	//printk("hi702 get i2c!, val = 0x%x\n", *val);
}

/*!
 * hi702 set_i2c function
 *
 * @return  none
 */
int hi702_set_i2c(int addr, int val)
{
    //printk("hi702 set i2c!, addr = 0x%x val = 0x%x\n", addr, val);
	return hi702_write_reg(addr, val);
}

const static u16 ignore[] = { I2C_CLIENT_END };
const static u16 normal_addr[] = { (HI_702_I2C_ADDRESS), I2C_CLIENT_END };
const static u16 *forces[] = { NULL };
static struct i2c_driver hi702_i2c_driver;

static struct i2c_client_address_data addr_data = {
	.normal_i2c	= normal_addr,
	.probe		= ignore,
	.ignore		= ignore,
	.forces		= forces,
};

static void hi702_start(struct i2c_client *client)
{	
	/* Initialize data */
	hi702_sensor_init();
}

#if 0
static int hi702_attach(struct i2c_adapter *adap, int addr, int kind)
{
    hi702_i2c_client.adapter = adap;
    
    if (i2c_attach_client(&hi702_i2c_client)) {
        hi702_i2c_client.adapter = NULL;
        printk(KERN_ERR "hi702_attach: i2c_attach_client failed\n");
        return -1;
    }	    
	hi702_data.client = &hi702_i2c_client;
	/* probe ? */
	if(hi702_video_probe() != 0) {
	    printk(KERN_ERR "hi702 probe failed. Check Power or cables.\n");
	    return -1;
	}

	info("hi702 attached successfully\n");

	return 0;
}

static int hi702_attach_adapter(struct i2c_adapter *adap)
{
	int ret = 0;

	s3c_fimc_register_camera(&hi702_data);
	
	ret = i2c_probe(adap, &addr_data, hi702_attach);
	if (ret) {
		err("failed to attach hi702 driver\n");
		ret = -ENODEV;
	}

	return ret;
}

static int hi702_detach(struct i2c_client *client)
{
	i2c_detach_client(client);

	return 0;
}
#endif

static void hi702_i2c_register(struct i2c_client *client, void *pdata)
{
    int i;
    hi702_data.client = client;
    hi702_data.i2c_read = hi702_get_i2c;
    hi702_data.i2c_write = hi702_set_i2c;

#if 0    
    for (i=0x0; i<0x40; i++) {
        int val;
        hi702_get_i2c(i + 0x1000, &val);
        printk("reg(%x)=%x\n", i, val);
    }
#endif

    nx_vip_register_camera(&hi702_data); /* We need to turn MCLK first, to detect I2C client */

	//nx_vip_reset_camera();

	msleep(5);
}


static int __devinit hi702_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    struct hi702_i2c_platform_data  *pdata = client->dev.platform_data;
    struct i2c_hi702_data           *phi702_data;

    printk("Hi702_i2c_probe client->addr=%X\n", client->addr);
    /* Check I2C address that the device realy exist */
    //if (!i2c_check_functionality(client->adapter,
    //                 I2C_FUNC_SMBUS_BYTE_DATA))
    //    return -EIO;

	phi702_data = kzalloc(sizeof(struct i2c_hi702_data), GFP_KERNEL);
	if (phi702_data == NULL) {
		dev_err(&client->dev, "failed to create T8EJ0 data\n");
		return -ENOMEM;
	}

	phi702_data->hi702_data   = &hi702_data;

	i2c_set_clientdata(client, phi702_data);

    hi702_i2c_register(client, pdata);

	hi702_start(client);
	hi702_data.initialized = 1;

    return 0;
}

static int __devexit hi702_i2c_remove(struct i2c_client *client)
{
    struct i2c_hi702_data *phi702_data = i2c_get_clientdata(client);

	kfree(phi702_data);

    return  0;
}


static int hi702_change_resolution(struct i2c_client *client, int res)
{
    int out_width;
    /* 
      HI702 is VGA camera with 2 sub sampling feature.
     */
    if (res == CAM_RES_QVGA ||
        res == CAM_RES_QQVGA ||
        res == CAM_RES_VGA ||
        res == CAM_RES_DEFAULT) {
     } else {
         err("Invalid resolution\n");
         return -1;
     }

	switch (res) {
	case CAM_RES_QVGA:
	case CAM_RES_QQVGA:
	case CAM_RES_VGA:
	case CAM_RES_DEFAULT:	/* fall through */	    
		/* Set camera resolution */
		if (res == CAM_RES_VGA) {
		    out_width = 640;
		    out_width = 480;
		    reg_write(VDOCTL1, HI_702_VDOCTL1_SUB_VGA);
		} else
		if (res == CAM_RES_QQVGA) {
		    out_width = 160;
		    out_width = 120;
		    reg_write(VDOCTL1, HI_702_VDOCTL1_SUB_QQVGA);
		} else { /* QVGA is DEFAULT */
		    out_width = 320;
		    out_width = 240;
		    reg_write(VDOCTL1, HI_702_VDOCTL1_SUB_QVGA);
		}
		break;
	}

	return 0;
}

static int hi702_change_whitebalance(struct i2c_client *client, enum nx_vip_wb_t type)
{
	// Some registers or just auto wb or manual wb
	if (type == WB_AUTO)     
	    hi702_set_awb(true);
	else
	    hi702_set_awb(false);

	return 0;
}

static int hi702_command(struct i2c_client *client, u32 cmd, void *arg)
{
	switch (cmd) {
	case I2C_CAM_INIT:
		hi702_start(client);
		info("hi702 : external camera initialized\n");
		break;

	case I2C_CAM_RESOLUTION:
		hi702_change_resolution(client, (int) arg);
		break;

	case I2C_CAM_WB:
		hi702_change_whitebalance(client, (enum nx_vip_wb_t) arg);
        	break;

	default:
		err("unexpect command\n");
		break;
	}

	return 0;
}

static const struct i2c_device_id hi702_i2c_id[] = {
    { "hi702", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, hi702_i2c_id);

static struct i2c_driver hi702_i2c_driver = {
	.driver = {
		.name = "hi702",
	},
	.id_table     = hi702_i2c_id,
	.probe        = hi702_i2c_probe,
    .remove       = __devexit_p(hi702_i2c_remove),
	.command = hi702_command,
};

static __init int hi702_init(void)
{
    printk("hi702 init\n");
	return i2c_add_driver(&hi702_i2c_driver);
}

static __init void hi702_clean(void)
{
	i2c_del_driver(&hi702_i2c_driver);
}

module_init(hi702_init);
module_exit(hi702_clean);

MODULE_DESCRIPTION("Hynix HI702 Camera driver");
MODULE_AUTHOR("Seungwoo Kim");
MODULE_LICENSE("GPL");

