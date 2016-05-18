/*
 * Driver for MT9M111 CMOS Image Sensor from Micron
 *
 * Copyright (C) 2008, Robert Jarzmik <robert.jarzmik@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * mt9m111 i2c address is 0x5d or 0x48 (depending on SAddr pin)
 * The platform has to define i2c_board_info and call i2c_register_board_info()
 */
#ifndef __MT9M111_H__
#define __MT9M111_H__
/* mt9m111: Sensor register addresses */
#define MT9M111_CHIP_VERSION		0x000
#define MT9M111_ROW_START		0x001
#define MT9M111_COLUMN_START		0x002
#define MT9M111_WINDOW_HEIGHT		0x003
#define MT9M111_WINDOW_WIDTH		0x004
#define MT9M111_HORIZONTAL_BLANKING_B	0x005
#define MT9M111_VERTICAL_BLANKING_B	0x006
#define MT9M111_HORIZONTAL_BLANKING_A	0x007
#define MT9M111_VERTICAL_BLANKING_A	0x008
#define MT9M111_SHUTTER_WIDTH		0x009
#define MT9M111_ROW_SPEED		0x00a
#define MT9M111_EXTRA_DELAY		0x00b
#define MT9M111_SHUTTER_DELAY		0x00c
#define MT9M111_RESET			0x00d
#define MT9M111_READ_MODE_B		0x020
#define MT9M111_READ_MODE_A		0x021
#define MT9M111_FLASH_CONTROL		0x023
#define MT9M111_GREEN1_GAIN		0x02b
#define MT9M111_BLUE_GAIN		0x02c
#define MT9M111_RED_GAIN		0x02d
#define MT9M111_GREEN2_GAIN		0x02e
#define MT9M111_GLOBAL_GAIN		0x02f
#define MT9M111_CONTEXT_CONTROL		0x0c8
#define MT9M111_PAGE_MAP		0x0f0
#define MT9M111_BYTE_WISE_ADDR		0x0f1

#define MT9M111_RESET_SYNC_CHANGES	(1 << 15)
#define MT9M111_RESET_RESTART_BAD_FRAME	(1 << 9)
#define MT9M111_RESET_SHOW_BAD_FRAMES	(1 << 8)
#define MT9M111_RESET_RESET_SOC		(1 << 5)
#define MT9M111_RESET_OUTPUT_DISABLE	(1 << 4)
#define MT9M111_RESET_CHIP_ENABLE	(1 << 3)
#define MT9M111_RESET_ANALOG_STANDBY	(1 << 2)
#define MT9M111_RESET_RESTART_FRAME	(1 << 1)
#define MT9M111_RESET_RESET_MODE	(1 << 0)

#define MT9M111_RMB_MIRROR_COLS		(1 << 1)
#define MT9M111_RMB_MIRROR_ROWS		(1 << 0)
#define MT9M111_CTXT_CTRL_RESTART	(1 << 15)
#define MT9M111_CTXT_CTRL_DEFECTCOR_B	(1 << 12)
#define MT9M111_CTXT_CTRL_RESIZE_B	(1 << 10)
#define MT9M111_CTXT_CTRL_CTRL2_B	(1 << 9)
#define MT9M111_CTXT_CTRL_GAMMA_B	(1 << 8)
#define MT9M111_CTXT_CTRL_XENON_EN	(1 << 7)
#define MT9M111_CTXT_CTRL_READ_MODE_B	(1 << 3)
#define MT9M111_CTXT_CTRL_LED_FLASH_EN	(1 << 2)
#define MT9M111_CTXT_CTRL_VBLANK_SEL_B	(1 << 1)
#define MT9M111_CTXT_CTRL_HBLANK_SEL_B	(1 << 0)
/*
 * mt9m111: Colorpipe register addresses (0x100..0x1ff)
 */
#define MT9M111_OPER_MODE_CTRL		0x106
#define MT9M111_OUTPUT_FORMAT_CTRL	0x108
#define MT9M111_COLOR_SATURATION_CTRL	0x125
#define MT9M111_REDUCER_XZOOM_B		0x1a0
#define MT9M111_REDUCER_XSIZE_B		0x1a1
#define MT9M111_REDUCER_YZOOM_B		0x1a3
#define MT9M111_REDUCER_YSIZE_B		0x1a4
#define MT9M111_REDUCER_XZOOM_A		0x1a6
#define MT9M111_REDUCER_XSIZE_A		0x1a7
#define MT9M111_REDUCER_YZOOM_A		0x1a9
#define MT9M111_REDUCER_YSIZE_A		0x1aa

#define MT9M111_OUTPUT_FORMAT_CTRL2_A	0x13a
#define MT9M111_OUTPUT_FORMAT_CTRL2_B	0x19b

#define MT9M111_OPMODE_AUTOWB_MAN	(1 << 15)
#define MT9M111_OPMODE_AUTOEXPO_EN	(1 << 14)
#define MT9M111_OPMODE_AUTO_FLK_DET	(1 << 7)
#define MT9M111_OPMODE_AUTOEXPO_W_L	(0 << 2)
#define MT9M111_OPMODE_AUTOEXPO_W_S	(1 << 2)
#define MT9M111_OPMODE_AUTOEXPO_WEIGHED	(2 << 2)
#define MT9M111_OPMODE_AUTOWB_EN	(1 << 1)


#define MT9M111_OUTFMT_PROCESSED_BAYER	(1 << 14)
#define MT9M111_OUTFMT_BYPASS_IFP	(1 << 10)
#define MT9M111_OUTFMT_INV_PIX_CLOCK	(1 << 9)
#define MT9M111_OUTFMT_RGB		(1 << 8)
#define MT9M111_OUTFMT_RGB565		(0x0 << 6)
#define MT9M111_OUTFMT_RGB555		(0x1 << 6)
#define MT9M111_OUTFMT_RGB444x		(0x2 << 6)
#define MT9M111_OUTFMT_RGBx444		(0x3 << 6)
#define MT9M111_OUTFMT_TST_RAMP_OFF	(0x0 << 4)
#define MT9M111_OUTFMT_TST_RAMP_COL	(0x1 << 4)
#define MT9M111_OUTFMT_TST_RAMP_ROW	(0x2 << 4)
#define MT9M111_OUTFMT_TST_RAMP_FRAME	(0x3 << 4)
#define MT9M111_OUTFMT_SHIFT_3_UP	(1 << 3)
#define MT9M111_OUTFMT_AVG_CHROMA	(1 << 2)
#define MT9M111_OUTFMT_SWAP_YCbCr_C_Y	(1 << 1)
#define MT9M111_OUTFMT_SWAP_RGB_EVEN	(1 << 1)
#define MT9M111_OUTFMT_SWAP_YCbCr_Cb_Cr	(1 << 0)
/*
 * mt9m111: Camera control register addresses (0x200..0x2ff not implemented)
 */
#define MT9M111_FLICKER_CONTROL		0x25B
#define MT9M111_AE_SPEED_SENSITIVIRY	0x29C
#define MT9M111_GLOBAL_CONTEXT_CTRL	0x2C8

#define reg_read(reg, val) mt9m111_read_reg(MT9M111_##reg, val)
#define reg_write(reg, val) mt9m111_write_reg(MT9M111_##reg, (val))
#define reg_set(reg, val) mt9m111_reg_set(MT9M111_##reg, (val))
#define reg_clear(reg, val) mt9m111_reg_clear(MT9M111_##reg, (val))

#define MT9M111_MIN_DARK_ROWS	8
#define MT9M111_MIN_DARK_COLS	24
#define MT9M111_MAX_HEIGHT	1024
#define MT9M111_MAX_WIDTH	1280

/*!
 * The flicker control setting 
 */
enum {
	MT9M111_FLICKER_DISABLE = 0,
	MT9M111_FLICKER_MANUAL_50,
	MT9M111_FLICKER_MANUAL_60,
	MT9M111_FLICKER_AUTO_DETECTION
};

/*! I2C Slave Address */
/* of SADDR high, it is 0x5D, if not,0x48 */
#define MT9M111_I2C_ADDRESS	0x5D

#endif

