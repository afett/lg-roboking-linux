/**********************************************************************
*  Copyright(c) 2010 MOSTiTECH co., ltd. 
*  All Rights Reserved. by Seungwoo Kim <ksw@mostitech.com>
* 
*  This file is based the definition by
**    Driver for MT9M111 CMOS Image Sensor from Micron
**
**   Copyright (C) 2008, Robert Jarzmik <robert.jarzmik@free.fr>
**   Copyright (C) 2010, Seungwoo Kim <ksw@mostitech.com>
*  But completely rewritten for most6410/LGE board
*
**********************************************************************/
/*!
 * @file mt9m111.c
 *
 * @brief Micron mt9m111 camera driver functions
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

#include "s3c_fimc.h"
#include "mt9m111.h"

enum mt9m111_context_t {
	HIGHPOWER = 0,
	LOWPOWER,
};

static int reset_frame_rate = 30;
static int mt9m111_powered =0;
static u32 mt9m111_pixfmt=0;
static enum mt9m111_context_t mt9m111_context=LOWPOWER;
static int mt9m111_flicker=0;
static int mt9m111_autoexposure=0;
static int mt9m111_autowhitebalance=0;
static int mt9m111_hflip=0;
static int mt9m111_vflip=0;
static int mt9m111_gain = 0;
static int sensor_init = 0;

static void mt9m111_set_ae(int active);
static void mt9m111_set_awb(int active);
static void mt9m111_flicker_control(int control);

#define COL_FMT(_name, _depth, _fourcc, _colorspace) \
	{ .name = _name, .depth = _depth, .fourcc = _fourcc, \
	.colorspace = _colorspace }
#define RGB_FMT(_name, _depth, _fourcc) \
	COL_FMT(_name, _depth, _fourcc, V4L2_COLORSPACE_SRGB)

#define I2C_FLAG_READ 0x01

static struct i2c_driver mt9m111_i2c_driver;

static struct i2c_client mt9m111_i2c_client = {
	.name = "mt9m111 i2c dev",
	.addr = MT9M111_I2C_ADDRESS,
	.driver = &mt9m111_i2c_driver,
};

/*
 * Function definitions
 */

static u16 mt9m111_endian_swap16(u16 data)
{
	u16 temp;

	temp = data;
	temp = ((data >> 8) & 0xff) | ((data << 8) & 0xff00);

	return temp;
}

static int mt9m111_i2c_client_xfer(unsigned int addr, char *reg, int reg_len,
				   char *buf, int num, int tran_flag)
{
	struct i2c_msg msg[2];
	int ret;
	u8 data[4];

	if (tran_flag & I2C_FLAG_READ) {
	    msg[0].addr = addr;
	    msg[0].len = reg_len;
	    msg[0].buf = reg;
	    msg[0].flags = tran_flag;
	    msg[0].flags &= ~I2C_M_RD;

	    msg[1].addr = addr;
	    msg[1].len = num;
	    msg[1].buf = buf;
	    msg[1].flags = tran_flag;

	    msg[1].flags |= I2C_M_RD;
	    
	    ret = i2c_transfer(mt9m111_i2c_client.adapter, msg, 2);
	} else {
	    data[0] = *reg;
	    data[1] = buf[0];
	    data[2] = buf[1];
	    
	    msg[0].addr = addr;
	    msg[0].len = 3;
	    msg[0].buf = data;
	    msg[0].flags = 0;
	    ret = i2c_transfer(mt9m111_i2c_client.adapter, msg, 1);
	}

	if (ret >= 0)
		return 0;

	return ret;
}


static int reg_page_map_set(const u16 reg)
{
	int ret;
	u8 temp1;
	u16 page,temp2;
	static int lastpage = -1;	/* PageMap cache value */

	page = (reg >> 8);
	if (page == lastpage)
		return 0;
	if (page > 2)
		return -EINVAL;
	
	temp2 = mt9m111_endian_swap16(page);
	temp1 = MT9M111_PAGE_MAP;
	ret = mt9m111_i2c_client_xfer(MT9M111_I2C_ADDRESS, &temp1, 1,
				       (u8 *) & temp2, 2, 0);
	if (ret >= 0)
		lastpage = page;
	return ret;
}

static int mt9m111_read_reg(u16  reg, u16 * val)
{
	int ret;
	u16 rval;

	ret = reg_page_map_set(reg);
	if (ret < 0) return ret;
	
	reg &= 0xFF;
	
	ret = mt9m111_i2c_client_xfer(MT9M111_I2C_ADDRESS, (char *)&reg, 1,
				       (u8 *) &rval, 2, I2C_FLAG_READ);
	if (!ret) {
	    //printk("rval = %x\n", rval);
		*val = mt9m111_endian_swap16(rval);
		return 0;
	}
	return ret;
}

static int mt9m111_write_reg(u16 reg, u16 val)
{
	u8 temp1;
	u16 temp2;
	int ret;

	temp1 = reg & 0xFF;
	ret = reg_page_map_set(reg);
	if (ret < 0) return ret;
	temp2 = mt9m111_endian_swap16(val);
	//printk("write reg %x val %x.\n", reg, val);
	return mt9m111_i2c_client_xfer(MT9M111_I2C_ADDRESS, &temp1, 1,
				       (u8 *) & temp2, 2, 0);
}


static int mt9m111_reg_set(const u16 reg, const u16 data)
{
	int ret;
	u16 val;

	ret = mt9m111_read_reg(reg, &val);
	if (ret >= 0)
		ret = mt9m111_write_reg(reg, val | data);
	return ret;
}

static int mt9m111_reg_clear(const u16 reg, const u16 data)
{
	int ret;
	u16 val;

	ret = mt9m111_read_reg(reg, &val);
	if (ret >= 0)
		ret = mt9m111_write_reg(reg, val & ~data); 
	return ret;
}

static int mt9m111_set_context(enum mt9m111_context_t ctxt)
{
	int valB = MT9M111_CTXT_CTRL_RESTART | MT9M111_CTXT_CTRL_DEFECTCOR_B
		| MT9M111_CTXT_CTRL_RESIZE_B | MT9M111_CTXT_CTRL_CTRL2_B
		| MT9M111_CTXT_CTRL_GAMMA_B | MT9M111_CTXT_CTRL_READ_MODE_B
		| MT9M111_CTXT_CTRL_VBLANK_SEL_B
		| MT9M111_CTXT_CTRL_HBLANK_SEL_B;
	int valA = MT9M111_CTXT_CTRL_RESTART;

	if (ctxt == HIGHPOWER)
		return reg_write(CONTEXT_CONTROL, valB);
	else
		return reg_write(CONTEXT_CONTROL, valA);
}

static int mt9m111_setup_rect(int pixfmt, int left, int top, int width, int height)
{
	int ret = 0, is_raw_format;

	if (pixfmt == V4L2_PIX_FMT_SBGGR8)
		is_raw_format = 1;
	else
		is_raw_format = 0;

	if (ret >= 0)
		ret = reg_write(COLUMN_START, left);
	if (ret >= 0)
		ret = reg_write(ROW_START, top);

	if (is_raw_format) {
		if (ret >= 0)
			ret = reg_write(WINDOW_WIDTH, width);
		if (ret >= 0)
			ret = reg_write(WINDOW_HEIGHT, height);
	} else {
		if (ret >= 0)
			ret = reg_write(REDUCER_XZOOM_B, MT9M111_MAX_WIDTH);
		if (ret >= 0)
			ret = reg_write(REDUCER_YZOOM_B, MT9M111_MAX_HEIGHT);
		if (ret >= 0)
			ret = reg_write(REDUCER_XSIZE_B, width);
		if (ret >= 0)
			ret = reg_write(REDUCER_YSIZE_B, height);
		if (ret >= 0)
			ret = reg_write(REDUCER_XZOOM_A, MT9M111_MAX_WIDTH);
		if (ret >= 0)
			ret = reg_write(REDUCER_YZOOM_A, MT9M111_MAX_HEIGHT);
		if (ret >= 0)
			ret = reg_write(REDUCER_XSIZE_A, width);
		if (ret >= 0)
			ret = reg_write(REDUCER_YSIZE_A, height);
	}

	return ret;
}

static int mt9m111_setup_pixfmt(u16 outfmt)
{
	int ret;

	ret = reg_write(OUTPUT_FORMAT_CTRL2_A, outfmt);
	if (ret >= 0)
		ret = reg_write(OUTPUT_FORMAT_CTRL2_B, outfmt);
	return ret;
}

static int mt9m111_setfmt_bayer8(void)
{
	return mt9m111_setup_pixfmt(MT9M111_OUTFMT_PROCESSED_BAYER);
}

static int mt9m111_setfmt_bayer10(void)
{

	return mt9m111_setup_pixfmt(MT9M111_OUTFMT_BYPASS_IFP);
}

static int mt9m111_setfmt_rgb565(int swap_rgb_red_blue, int swap_rgb_even_odd)
{
	int val = 0;

	if (swap_rgb_red_blue)
		val |= MT9M111_OUTFMT_SWAP_YCbCr_Cb_Cr;
	if (swap_rgb_even_odd)
		val |= MT9M111_OUTFMT_SWAP_RGB_EVEN;
	val |= MT9M111_OUTFMT_RGB | MT9M111_OUTFMT_RGB565;

	return mt9m111_setup_pixfmt(val);
}

static int mt9m111_setfmt_rgb555(int swap_rgb_red_blue, int swap_rgb_even_odd)
{
	int val = 0;

	if (swap_rgb_red_blue)
		val |= MT9M111_OUTFMT_SWAP_YCbCr_Cb_Cr;
	if (swap_rgb_even_odd)
		val |= MT9M111_OUTFMT_SWAP_RGB_EVEN;
	val |= MT9M111_OUTFMT_RGB | MT9M111_OUTFMT_RGB555;

	return mt9m111_setup_pixfmt(val);
}

static int mt9m111_setfmt_yuv(int swap_yuv_cb_cr, int swap_yuv_y_chromas)
{
	int val = 0;

	if (swap_yuv_cb_cr)
		val |= MT9M111_OUTFMT_SWAP_YCbCr_Cb_Cr;
	if (swap_yuv_y_chromas)
		val |= MT9M111_OUTFMT_SWAP_YCbCr_C_Y;

	return mt9m111_setup_pixfmt(val);
}

static int mt9m111_enable(void)
{
	int ret;

	ret = reg_set(RESET, MT9M111_RESET_CHIP_ENABLE);
	if (ret >= 0)
		mt9m111_powered = 1;
	return ret;
}

static int mt9m111_disable(void)
{
	int ret;

	ret = reg_clear(RESET, MT9M111_RESET_CHIP_ENABLE);
	if (ret >= 0)
		mt9m111_powered = 0;
	return ret;
}

static int mt9m111_sensor_reset(void)
{
	int ret;

	ret = reg_set(RESET, MT9M111_RESET_RESET_MODE);
	if (ret >= 0)
		ret = reg_set(RESET, MT9M111_RESET_RESET_SOC);
	if (ret >= 0)
		ret = reg_clear(RESET, MT9M111_RESET_RESET_MODE
				| MT9M111_RESET_RESET_SOC);
	return ret;
}

static int mt9m111_set_pixfmt(u32 pixfmt)
{
	int ret = 0;

	switch (pixfmt) {
	case V4L2_PIX_FMT_SBGGR8:
		ret = mt9m111_setfmt_bayer8();
		break;
//	case V4L2_PIX_FMT_SBGGR16:
//		ret = mt9m111_setfmt_bayer10();
//		break;
	case V4L2_PIX_FMT_RGB555:
		ret = mt9m111_setfmt_rgb555(0, 0);
		break;
	case V4L2_PIX_FMT_RGB565:
		ret = mt9m111_setfmt_rgb565(0, 0);
		break;
	case V4L2_PIX_FMT_YUYV:
		ret = mt9m111_setfmt_yuv(0, 0);
		break;
	default:
		//dev_err(&icd->dev, "Pixel format not handled : %x\n", pixfmt);
		ret = -EINVAL;
	}

	if (ret >= 0)
		mt9m111_pixfmt = pixfmt;

	return ret;
}

static int mt9m111_set_flip(int flip, int mask)
{
	int ret;

	if (mt9m111_context == HIGHPOWER) {
		if (flip)
			ret = reg_set(READ_MODE_B, mask);
		else
			ret = reg_clear(READ_MODE_B, mask);
	} else {
		if (flip)
			ret = reg_set(READ_MODE_A, mask);
		else
			ret = reg_clear(READ_MODE_A, mask);
	}

	return ret;
}

static int mt9m111_get_global_gain(void)
{
	unsigned int data, gain;

	reg_read(GLOBAL_GAIN, (u16 *)&data);
	if (data >= 0)
		gain = ((data & (1 << 10)) * 2)
			| ((data & (1 << 9)) * 2)
			| (data & 0x2f);
	else
		gain = data;

	return gain;
}
static int mt9m111_set_global_gain(int gain)
{
	u16 val;

	if (gain > 63 * 2 * 2)
		return -EINVAL;

	mt9m111_gain = gain;	
	if ((gain >= 64 * 2) && (gain < 63 * 2 * 2))
		val = (1 << 10) | (1 << 9) | (gain / 4);
	else if ((gain >= 64) && (gain < 64 * 2))
		val = (1<<9) | (gain / 2);
	else
		val = gain;

	return reg_write(GLOBAL_GAIN, val);
}

int mt9m111_restore_state(void)
{
	mt9m111_set_context(mt9m111_context);
	mt9m111_set_pixfmt(mt9m111_pixfmt);
	//mt9m111_setup_rect();
	mt9m111_set_flip(mt9m111_hflip, MT9M111_RMB_MIRROR_COLS);
	mt9m111_set_flip(mt9m111_vflip, MT9M111_RMB_MIRROR_ROWS);
	mt9m111_set_global_gain(mt9m111_gain);
	mt9m111_set_ae(mt9m111_autoexposure);
	return 0;
}

static int mt9m111_resume(void)
{
	int ret = 0;

	if (mt9m111_powered) {
		ret =  mt9m111_enable();
		if (ret >= 0) {
			mt9m111_sensor_reset();
			mt9m111_restore_state();
		}
	}
	return ret;
}

static int mt9m111_sensor_init(void)
{
	int ret;

	mt9m111_context = LOWPOWER;
	ret = mt9m111_enable();
	mt9m111_sensor_reset();
	mt9m111_set_context(mt9m111_context);
	mt9m111_set_ae(mt9m111_autoexposure);
	mt9m111_set_awb(mt9m111_autowhitebalance);
	mt9m111_flicker_control(mt9m111_flicker);

	if (ret < 0) {
		//dev_err(&icd->dev, "mt9m111 init failed: %d\n", ret);
	}
	return ret ? -EIO : 0;
}

static int mt9m111_release(void)
{
	int ret;

	ret = mt9m111_disable();
	if (ret < 0) {
		//dev_err(&icd->dev, "mt9m111 release failed: %d\n", ret);
	}
	return ret ? -EIO : 0;
}

/*
 * Interface active, can use i2c. If it fails, it can indeed mean, that
 * this wasn't our capture interface, so, we wait for the right one
 */
static int mt9m111_video_probe(void)
{
	u16 data;
	int ret;

	/*
	 * We must have a parent by now. And it cannot be a wrong one.
	 * So this entire test is completely redundant.
	 */

	ret = mt9m111_enable();
	if (ret)
		goto ei2c;
	ret = mt9m111_sensor_reset();
	if (ret)
		goto ei2c;

	reg_read(CHIP_VERSION, &data);

	switch (data) {
	case 0x143a:
		// this is chip we want
		break;
	default:
		ret = -ENODEV;
		//dev_err(&icd->dev,"No MT9M111 chip detected, register read %x\n", data);
		goto ei2c;
	}

	//dev_info(&icd->dev, "Detected a MT9M111 chip ID 0x143a\n");

	if (ret)
		goto eisis;

	mt9m111_autoexposure = 1;
	mt9m111_autowhitebalance = 1;
	mt9m111_flicker = MT9M111_FLICKER_AUTO_DETECTION;

	//mt9m111_swap_rgb_even_odd = 1;
	//mt9m111_swap_rgb_red_blue = 1;

	return 0;
eisis:
ei2c:
	return ret;
}

/*!
 * mt9m111 sensor set color configuration
 *
 * @param bright       int
 * @param saturation   int
 * @param red          int
 * @param green        int
 * @param blue         int
 * @return  None
 */
static void
mt9m111_set_color(int bright, int saturation, int red, int green, int blue)
{
	u16 data, dd;

	reg_read(COLOR_SATURATION_CTRL, &data);
	data &= ~(0x07 << 3);

	switch (saturation) {
	case 100:
		dd  = 0;
		break;
	case 75:
		dd = 1;
		break;
	case 50:
		dd = 2;
		break;
	case 37:
	case 38:
		dd = 3;
		break;
	case 25:
		dd = 4;
		break;
	case 150:
		dd = 5;
		break;
	case 0:
		dd = 6; // This is black and white
		break;
	default:
		dd = 6; 
	}
	data |= dd << 3;
	reg_write(COLOR_SATURATION_CTRL, data);
}

/*!
 * mt9m111 sensor get color configuration
 *
 * @param bright       int *
 * @param saturation   int *
 * @param red          int *
 * @param green        int *
 * @param blue         int *
 * @return  None
 */
static void
mt9m111_get_color(int *bright, int *saturation, int *red, int *green, int *blue)
{
	u16 data;

	reg_read(COLOR_SATURATION_CTRL, &data);
	data >>= 3;
	data &= 0x07;
	switch (data) {
	case 0:
		*saturation = 100;
		break;
	case 1:
		*saturation = 75;
		break;
	case 2:
		*saturation = 50;
		break;
	case 3:
		*saturation = 38;
		break;
	case 4:
		*saturation = 25;
		break;
	case 5:
		*saturation = 150;
		break;
	case 6:
		*saturation = 0; // This is black and white
		break;
	default:
		*saturation = 0; 
	}
}

/*!
 * mt9m111 sensor set AE measurement window mode configuration
 *
 * @param ae_mode      int
 * @return  None
 */
static void mt9m111_set_ae_mode(int ae_mode)
{
	u16 data;
	
	data = ae_mode;
	reg_read(OPER_MODE_CTRL, &data);
	data &= ~(0x03 << 2);
	data |= ae_mode << 2;
	reg_write(OPER_MODE_CTRL, data);
}

/*!
 * mt9m111 sensor get AE measurement window mode configuration
 *
 * @param ae_mode      int *
 * @return  None
 */
static void mt9m111_get_ae_mode(int *ae_mode)
{
	u16 data;

	reg_read(OPER_MODE_CTRL, &data);
	*ae_mode = (data >> 2) & 0x03;
}

/*!
 * mt9m111 sensor enable/disable AE 
 *
 * @param active      int
 * @return  None
 */
static void mt9m111_set_ae(int active)
{
	if (active) {
		reg_set(OPER_MODE_CTRL, MT9M111_OPMODE_AUTOEXPO_EN);
		
	} else {
		reg_clear(OPER_MODE_CTRL, MT9M111_OPMODE_AUTOEXPO_EN);
	}
	mt9m111_autoexposure = active;
}

/*!
 * mt9m111 sensor enable/disable auto white balance
 *
 * @param active      int
 * @return  None
 */
static void mt9m111_set_awb(int active)
{
	if (active) {
		reg_set(OPER_MODE_CTRL, MT9M111_OPMODE_AUTOWB_EN);
	} else {
		reg_clear(OPER_MODE_CTRL, MT9M111_OPMODE_AUTOWB_EN);
	}
	mt9m111_autowhitebalance = active;
}

/*!
 * mt9m111 sensor set the flicker control 
 *
 * @param control      int
 * @return  None
 */
static void mt9m111_flicker_control(int control)
{
	u16	data=0;

	switch (control) {
	case MT9M111_FLICKER_DISABLE:
		data = 0;
		break;

	case MT9M111_FLICKER_MANUAL_50:
		data = 3;
		break;

	case MT9M111_FLICKER_MANUAL_60:
		data = 1;
		break;

	case MT9M111_FLICKER_AUTO_DETECTION:
		data = 0;
		break;
	}
	reg_write(FLICKER_CONTROL, data);
	if (control == MT9M111_FLICKER_AUTO_DETECTION) {
		reg_set(OPER_MODE_CTRL, MT9M111_OPMODE_AUTO_FLK_DET);
	} else {
		reg_clear(OPER_MODE_CTRL, MT9M111_OPMODE_AUTO_FLK_DET);
	}
	mt9m111_flicker = control;

	return;
}
/*!
 * mt9m111 Get mode&flicker control parameters 
 *
 * @return  None
 */
static void mt9m111_get_control_params(int *ae, int *awb, int *flicker)
{
	if ((ae != NULL) && (awb != NULL) && (flicker != NULL)) {
		*ae = mt9m111_autoexposure;
		*awb = mt9m111_autowhitebalance;
		*flicker = mt9m111_flicker;
		if (*flicker) {
			switch (*flicker) {
			case 1:
				*flicker = MT9M111_FLICKER_MANUAL_50;
				break;
			case 3:
				*flicker = MT9M111_FLICKER_MANUAL_60;
				break;
			default:
				*flicker = MT9M111_FLICKER_AUTO_DETECTION;
				break;
			}
		} else
			*flicker = MT9M111_FLICKER_DISABLE;
	}
}

#if 0
sensor_interface *mt9m111_config(int *frame_rate, int high_quality)
{
	int out_width, out_height;

#if 0	
	if (high_quality) {
		out_width = 1280;
		out_height = 1024;
	} else {
		out_width = 640; //320
		out_height = 480; //240
	}
#else
    /* ksw : Just I want to set interface as camera's i2c read from 1a7(width), 1aa(height) */
    if (high_quality) {
        printk("high quality!\n");
        reg_read(REDUCER_XSIZE_B, (u16 *)&out_width);
	    reg_read(REDUCER_YSIZE_B, (u16 *)&out_height);
	    mt9m111_context = HIGHPOWER;
	    mt9m111_set_context(mt9m111_context);
    } else {
        printk("low quality!\n");
	    reg_read(REDUCER_XSIZE_A, (u16 *)&out_width);
	    reg_read(REDUCER_YSIZE_A, (u16 *)&out_height);
	    mt9m111_context = LOWPOWER;
	    mt9m111_set_context(mt9m111_context);
	}
#endif
	mt9m111_interface(interface_param, out_width, out_height);

	if (0 == sensor_init) {
		mt9m111_sensor_init();
		sensor_init = 1;
	}
	return interface_param;
}
#endif

/*!
 * mt9m111 get_i2c function
 *
 * @return  none
 */
void mt9m111_get_i2c(int addr, int *val)
{
    //printk("mt9m111 get i2c!, addr = 0x%x\n", addr);
	mt9m111_read_reg(addr, (u16 *)val);
	//printk("mt9m111 get i2c!, val = 0x%x\n", *val);
}

/*!
 * mt9m111 set_i2c function
 *
 * @return  none
 */
void mt9m111_set_i2c(int addr, int val)
{
    //printk("mt9m111 set i2c!, addr = 0x%x val = 0x%x\n", addr, val);
	mt9m111_write_reg(addr, val);
}

const static u16 ignore[] = { I2C_CLIENT_END };
const static u16 normal_addr[] = { (MT9M111_I2C_ADDRESS), I2C_CLIENT_END };
const static u16 *forces[] = { NULL };
static struct i2c_driver mt9m111_i2c_driver;

static struct s3c_fimc_camera mt9m111_data = {
	.id 		= CONFIG_VIDEO_FIMC_CAM_CH + 1,
	.type		= CAM_TYPE_ITU,
	.mode		= ITU_601_YCBCR422_8BIT,
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

static struct i2c_client_address_data addr_data = {
	.normal_i2c	= normal_addr,
	.probe		= ignore,
	.ignore		= ignore,
	.forces		= forces,
};

static void mt9m111_start(struct i2c_client *client)
{	
	/* Initialize data */
	mt9m111_sensor_init();
}

static int mt9m111_attach(struct i2c_adapter *adap, int addr, int kind)
{
    mt9m111_i2c_client.adapter = adap;
    
    if (i2c_attach_client(&mt9m111_i2c_client)) {
        mt9m111_i2c_client.adapter = NULL;
        printk(KERN_ERR "mt9m111_attach: i2c_attach_client failed\n");
        return -1;
    }	    
	mt9m111_data.client = &mt9m111_i2c_client;
	/* probe ? */
	if(mt9m111_video_probe() != 0) {
	    printk(KERN_ERR "mt9m111 probe failed. Check Power or cables.\n");
	    return -1;
	}

	info("mt9m111 attached successfully\n");

	return 0;
}

static int mt9m111_attach_adapter(struct i2c_adapter *adap)
{
	int ret = 0;

	s3c_fimc_register_camera(&mt9m111_data);
	
	ret = i2c_probe(adap, &addr_data, mt9m111_attach);
	if (ret) {
		err("failed to attach mt9m111 driver\n");
		ret = -ENODEV;
	}

	return ret;
}

static int mt9m111_detach(struct i2c_client *client)
{
	i2c_detach_client(client);

	return 0;
}

static int mt9m111_change_resolution(struct i2c_client *client, int res)
{
    int out_width, out_height;
    /* MT9M111 has two power mode.
       When High Power mode, It can set resolution up to 1280X1024 through higher than 640X512.
       When Low Power mode, It can set resoution up to 640X512
       So We should set power mode first prior to set resoution.
     */
    if (res == CAM_RES_QVGA ||
        res == CAM_RES_QSVGA ||
        res == CAM_RES_VGA ||
        res == CAM_RES_DEFAULT) {
        mt9m111_context = LOWPOWER;
	} else {
	    mt9m111_context = HIGHPOWER;
	}
	mt9m111_set_context(mt9m111_context);

	switch (res) {
	case CAM_RES_QVGA:
	case CAM_RES_QSVGA:
	case CAM_RES_VGA:
	case CAM_RES_DEFAULT:	/* fall through */	    
		/* Set camera resolution */
		if (res == CAM_RES_VGA) {
		    out_width == 640;
		    out_width == 480;
		} else
		if (res == CAM_RES_QSVGA) {
		    out_width == 400;
		    out_width == 300;
		} else { /* QVGA is DEFAULT */
		    out_width == 320;
		    out_width == 240;
		}
		reg_write(REDUCER_XSIZE_A, out_width);
	    reg_write(REDUCER_YSIZE_A, out_height);
		break;

	case CAM_RES_MAX:	/* fall through */
	case CAM_RES_XGA:
	case CAM_RES_SXGA:
	case CAM_RES_SVGA:
 		/* Set camera resolution to MAX, Maxpower */
 		if (res == CAM_RES_SVGA) {
		    out_width == 800;
		    out_width == 600;
		} else
		if (res == CAM_RES_XGA) {
		    out_width == 1024;
		    out_width == 768;
		}
		else {
		    out_width == 1280;
		    out_width == 1024;
		}
 		reg_write(REDUCER_XSIZE_B, out_width);
	    reg_write(REDUCER_YSIZE_B, out_height);
		break;

	default:
		err("unexpect value\n");
	}

	return 0;
}

static int mt9m111_change_whitebalance(struct i2c_client *client, enum s3c_fimc_wb_t type)
{
	// Some registers or just auto wb or manual wb
	if (type == WB_AUTO)     
	    mt9m111_set_awb(true);
	else
	    mt9m111_set_awb(false);

	return 0;
}

static int mt9m111_command(struct i2c_client *client, u32 cmd, void *arg)
{
	switch (cmd) {
	case I2C_CAM_INIT:
		mt9m111_start(client);
		info("external camera initialized\n");
		break;

	case I2C_CAM_RESOLUTION:
		mt9m111_change_resolution(client, (int) arg);
		break;

	case I2C_CAM_WB:
		mt9m111_change_whitebalance(client, (enum s3c_fimc_wb_t) arg);
        	break;

	default:
		err("unexpect command\n");
		break;
	}

	return 0;
}

static struct i2c_driver mt9m111_i2c_driver = {
	.driver = {
		.name = "mt9m111",
	},
	.id = I2C_DRIVERID_MT9M111,
	.attach_adapter = mt9m111_attach_adapter,
	.detach_client = mt9m111_detach,
	.command = mt9m111_command,
};

static __init int mt9m111_init(void)
{
    printk("mt9m111 init\n");
	return i2c_add_driver(&mt9m111_i2c_driver);
}

static __init void mt9m111_clean(void)
{
	i2c_del_driver(&mt9m111_i2c_driver);
}

module_init(mt9m111_init);
module_exit(mt9m111_clean);

MODULE_DESCRIPTION("Micron MT9M111 Camera driver");
MODULE_AUTHOR("Seungwoo Kim");
MODULE_LICENSE("GPL");

