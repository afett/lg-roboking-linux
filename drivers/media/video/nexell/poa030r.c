/* linux/drivers/media/video/samsung/poa030r.c
 *
 * Toshiba POA030 CMOS Image Sensor driver
 *
 * Seungwoo Kim <ksw@mostitech.com>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#include <mach/devices.h>

#include "nx_vip.h"
#include "poa030r.h"


extern void nx_vip_reset_camera(void);

const static u16 ignore[] = { I2C_CLIENT_END };
const static u16 normal_addr[] = { POA030_I2C_ADDR, I2C_CLIENT_END };
const static u16 forces[] = { 0 };

static struct i2c_driver poa030_i2c_driver;

static struct nx_vip_camera poa030_data = {
	.id 		= 0,

	//.type		= CAM_TYPE_ITU,
	//.mode		= ITU_601_YCBCR422_8BIT,
	.order422	= CAM_ORDER422_8BIT_CBYCRY, //CAM_ORDER422_8BIT_YCBYCR,

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

struct i2c_poa030_data {
	struct nx_vip_camera    *poa030_data;
};

#define I2C_FLAG_READ   0x10

static int poa030_i2c_xfer(int reg, char *buf, int num, int tran_flag)
{
	struct i2c_msg msg[2];
	int ret;
	char xbuf[128];

	if (tran_flag & I2C_FLAG_READ) {
	    msg[0].addr = POA030_I2C_ADDR;
	    msg[0].len = 1;
	    msg[0].buf = (char *)&reg;
	    msg[0].flags = 0;

	    msg[1].addr = POA030_I2C_ADDR;
	    msg[1].len = num;
	    msg[1].buf = buf;
	    msg[1].flags = I2C_M_RD;

	    ret = i2c_transfer(poa030_data.client->adapter, msg, 2);
	} else {
	    xbuf[0] = reg;
	    memcpy(&xbuf[1], buf, num);
	    msg[0].addr = POA030_I2C_ADDR;
	    msg[0].len = 1 + num;
	    msg[0].buf = xbuf;
	    msg[0].flags = 0;

	    ret = i2c_transfer(poa030_data.client->adapter, msg, 1);
	}

	if (ret >= 0)
		return 0;

	return ret;
}

static int reg_page_map_set(const u16 reg)
{
	int ret;
	u8 temp1;
	u8 page;
	static int lastpage = -1;	/* PageMap cache value */

	page = (reg >> 8);
	if (page == lastpage)
		return 0;
	if (page > 3)
		return -EINVAL;

	temp1 = POA030_BANK;

	ret = poa030_i2c_xfer(temp1, (u8 *) & page, 1, 0);
	if (ret >= 0)
		lastpage = page;
	return ret;
}

static int poa030_read_reg8(u16  reg, u8 *val)
{
	int ret;
	u8 rval;

	ret = reg_page_map_set(reg);
	if (ret < 0) return ret;

	reg &= 0xFF;

	ret = poa030_i2c_xfer(reg, (u8 *) &rval, 1, I2C_FLAG_READ);
	if (!ret) {
		*val = rval;
		return 0;
	}
	return ret;
}

static int poa030_write_reg8(u16 reg, u8 val)
{
	u8 temp1;
	int ret;

	temp1 = reg & 0xFF;
	ret = reg_page_map_set(reg);
	if (ret < 0) return ret;
	//printk("write reg %x val %x.\n", reg, val);
	return poa030_i2c_xfer(temp1, (u8 *) & val, 1, 0);
}



static void poa030_start(struct i2c_client *client)
{
    u8 val;

    /* Do something for initialize POA030R */
    poa030_write_reg8(POA030_SOFTRESET, 0x01); /* Software reset */
    udelay(1000); /* wait some time */
    poa030_write_reg8(POA030_SOFTRESET, 0x00); /* Clear software reset */
    udelay(1000); /* wait some time */
    /* First, PAD control should be set */
    poa030_read_reg8(POA030_PAD_CONTROL2, &val);
    poa030_write_reg8(POA030_PAD_CONTROL, 0x00); /* Standby data hiz, normal not hiz */
    poa030_write_reg8(POA030_PAD_CONTROL2,  val & 0x7F); /* Clear Bit7 -> Disable OSC pad? */
    poa030_write_reg8(POA030_BAYER_CONTROL_01, 0x07); /* Mirror control : none */
    poa030_write_reg8(POA030_FORMAT, 0x00); /* Format control : Y only */ /* 0: CbYCrY, 1: CrYCbY, 2:YCbYCr, 3:YCrYCb */
    poa030_write_reg8(POA030_SYNC_CONTROL_1, 0x00); /* Polarity : none */
    poa030_write_reg8(POA030_AUTO_CONTROL_1, 0x98); /* Auto : AWB/AE enable */
    /* Now set VGA/QCGA*/
    /* VGA */ /*
    poa030_write_reg8(POA030_WINDOWX1_L, 0x05);
    poa030_write_reg8(POA030_WINDOWY1_L, 0x05);
    poa030_write_reg8(POA030_WINDOWX2_H, 0x02);
    poa030_write_reg8(POA030_WINDOWX2_L, 0x84);
    poa030_write_reg8(POA030_WINDOWY2_H, 0x01);
    poa030_write_reg8(POA030_WINDOWY2_L, 0xE4);
    poa030_write_reg8(POA030_SCALE_X,    0x20);
    poa030_write_reg8(POA030_SCALE_Y,    0x20);
    poa030_write_reg8(0x195,             0x00);
    poa030_write_reg8(0x196,             0x0A);
    poa030_write_reg8(POA030_AE_FWX1_H,  0x00);
    poa030_write_reg8(POA030_AE_FWX1_L,  0x05);
    poa030_write_reg8(POA030_AE_FWX2_H,  0x02);
    poa030_write_reg8(POA030_AE_FWX2_L,  0x84);
    poa030_write_reg8(POA030_AE_FWY1_H,  0x00);
    poa030_write_reg8(POA030_AE_FWY1_L,  0x05);
    poa030_write_reg8(POA030_AE_FWY2_H,  0x01);
    poa030_write_reg8(POA030_AE_FWY2_L,  0xE4);
    poa030_write_reg8(POA030_AE_CWX1_H,  0x00);
    poa030_write_reg8(POA030_AE_CWX1_L,  0xDA);
    poa030_write_reg8(POA030_AE_CWX2_H,  0x01);
    poa030_write_reg8(POA030_AE_CWX2_L,  0xAF);
    poa030_write_reg8(POA030_AE_CWY1_H,  0x00);
    poa030_write_reg8(POA030_AE_CWY1_L,  0xA5);
    poa030_write_reg8(POA030_AE_CWY2_H,  0x01);
    poa030_write_reg8(POA030_AE_CWY2_L,  0x44);
    */
    /* QVGA */
    poa030_write_reg8(POA030_WINDOWX1_L, 0x03);
    poa030_write_reg8(POA030_WINDOWY1_L, 0x03);
    poa030_write_reg8(POA030_WINDOWX2_H, 0x01);
    poa030_write_reg8(POA030_WINDOWX2_L, 0x42);
    poa030_write_reg8(POA030_WINDOWY2_H, 0x00);
    poa030_write_reg8(POA030_WINDOWY2_L, 0xF2);
    poa030_write_reg8(POA030_SCALE_X,    0x40);
    poa030_write_reg8(POA030_SCALE_Y,    0x40);
    poa030_write_reg8(0x195,             0x01);
    poa030_write_reg8(0x196,             0x40);
    poa030_write_reg8(POA030_AE_FWX1_H,  0x00);
    poa030_write_reg8(POA030_AE_FWX1_L,  0x03);
    poa030_write_reg8(POA030_AE_FWX2_H,  0x01);
    poa030_write_reg8(POA030_AE_FWX2_L,  0x42);
    poa030_write_reg8(POA030_AE_FWY1_H,  0x00);
    poa030_write_reg8(POA030_AE_FWY1_L,  0x03);
    poa030_write_reg8(POA030_AE_FWY2_H,  0x00);
    poa030_write_reg8(POA030_AE_FWY2_L,  0xF2);
    poa030_write_reg8(POA030_AE_CWX1_H,  0x00);
    poa030_write_reg8(POA030_AE_CWX1_L,  0x6D);
    poa030_write_reg8(POA030_AE_CWX2_H,  0x00);
    poa030_write_reg8(POA030_AE_CWX2_L,  0xD8);
    poa030_write_reg8(POA030_AE_CWY1_H,  0x00);
    poa030_write_reg8(POA030_AE_CWY1_L,  0x53);
    poa030_write_reg8(POA030_AE_CWY2_H,  0x00);
    poa030_write_reg8(POA030_AE_CWY2_L,  0xA2);
    /* Now Set CCIR 656 */
    poa030_write_reg8(POA030_SYNC_BLANKSAV, 0xAB); // 0xAB
    poa030_write_reg8(POA030_SYNC_BLANKEAV, 0xB6); // 0xB6
    poa030_write_reg8(POA030_SYNC_ACTIVSAV, 0x80);
    poa030_write_reg8(POA030_SYNC_ACTIVEAV, 0x9D);

    poa030_read_reg8(POA030_AUTO_CONTROL_1, &val);
    poa030_write_reg8(POA030_AUTO_CONTROL_1, val | 0x02);

    poa030_write_reg8(POA030_EXT_INTTIME_M, 0x80);

#if 0
    for (i=0; i<255; i++) {
        poa030_read_reg8(i + 0x0000, &val);
        printk("page A: %x => %x\n", i, val);
    }
    for (i=0; i<255; i++) {
        poa030_read_reg8(i + 0x0100, &val);
        printk("page B: %x => %x\n", i, val);
    }
    for (i=0; i<255; i++) {
        poa030_read_reg8(i + 0x0200, &val);
        printk("page C: %x => %x\n", i, val);
    }
#endif
}

/*!
 * poa030 get_i2c function
 *
 * @return  none
 */
static int poa030_get_i2c(int addr, int *val)
{
    //printk("poa030 get i2c!, addr = 0x%x\n", addr);
	return poa030_read_reg8(addr, (u8 *)val);
	//printk("poa030 get i2c!, val = 0x%x\n", *val);
}

/*!
 * poa030 set_i2c function
 *
 * @return  none
 */
static int poa030_set_i2c(int addr, int val)
{
    //printk("poa030 set i2c!, addr = 0x%x val = 0x%x\n", addr, val);
	return poa030_write_reg8(addr, val);
}

#if 0
static int poa030_detect_client(struct i2c_adapter *adap, int addr, int kind)
{
    poa030_i2c_client.adapter = adap;
    if (i2c_attach_client(&poa030_i2c_client)) {
        poa030_i2c_client.adapter = NULL;
        printk(KERN_ERR "POA030_attach: i2c_attach_client failed\n");
        return -1;
    }
    poa030_data.client = &poa030_i2c_client;

    info("POA030 attached successfully\n");

	return 0;
}

static int poa030_attach(struct i2c_adapter *adap)
{
	int ret = 0;
	int id;

	s3c_fimc_register_camera(&poa030_data); /* We need to turn MCLK first, to detect I2C client */

	s3c_fimc_reset_camera();
	msleep(5);
	ret = i2c_probe(adap, &addr_data, poa030_detect_client);
	if (ret) {
		err("failed to attach POA030 driver\n");
		ret = -ENODEV;
	}
	/* It s time to start initialize....? */
	/* We have to check that driver exist or not, check adaptor? */
	if (poa030_i2c_client.adapter == NULL) { /* Then we don't have any client so we should return faile. */
	    err("failed to detect POA030.\n");
	    return -ENODEV;
	}
	/* get device IDL */
	poa030_read_reg8(POA030_DEVICEID_H, &id);
	printk("ID H = %x ", id & 0xFF);
	poa030_read_reg8(POA030_DEVICEID_L, &id);
	printk("ID L = %x\n", id & 0xFF);
	poa030_start(&poa030_i2c_client);
	poa030_data.initialized = 1;

	return ret;
}

static int poa030_detach(struct i2c_client *client)
{
    int err;

    if (!poa030_i2c_client.adapter)
        return -1;

    err = i2c_detach_client(&poa030_i2c_client);
    poa030_i2c_client.adapter = NULL;

    return err;
}
#endif

static void poa030_i2c_register(struct i2c_client *client, struct poa030_i2c_platform_data *pdata)
{
    poa030_data.client = client;
    poa030_data.i2c_read = poa030_get_i2c;
    poa030_data.i2c_write = poa030_set_i2c;

    nx_vip_register_camera(&poa030_data); /* We need to turn MCLK first, to detect I2C client */

	nx_vip_reset_camera();

	msleep(5);
}


static int __devinit poa030_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    struct poa030_i2c_platform_data  *pdata = client->dev.platform_data;
    struct i2c_poa030_data           *ppoa030_data;

    printk("POA030_i2c_probe client->addr=%X\n", client->addr);
    /* Check I2C address that the device realy exist */
    //if (!i2c_check_functionality(client->adapter,
    //                 I2C_FUNC_SMBUS_BYTE_DATA))
    //    return -EIO;

	ppoa030_data = kzalloc(sizeof(struct i2c_poa030_data), GFP_KERNEL);
	if (ppoa030_data == NULL) {
		dev_err(&client->dev, "failed to create T8EJ0 data\n");
		return -ENOMEM;
	}

	ppoa030_data->poa030_data   = &poa030_data;

	i2c_set_clientdata(client, ppoa030_data);

    poa030_i2c_register(client, pdata);

	poa030_start(client);
	poa030_data.initialized = 1;

    return 0;
}

static int __devexit poa030_i2c_remove(struct i2c_client *client)
{
    struct i2c_poa030_data *ppoa030_data = i2c_get_clientdata(client);

	kfree(ppoa030_data);

    return  0;
}

static int poa030_change_resolution(struct i2c_client *client, int res)
{
	switch (res) {
	case CAM_RES_QSVGA:
		// Do something
		break;

	case CAM_RES_MAX:	/* fall through */
	case CAM_RES_DEFAULT:	/* fall through */
	case CAM_RES_SVGA:
 		// DO something...
		break;

	default:
		err("unexpect value\n");
	}

	return 0;
}

static int poa030_change_whitebalance(struct i2c_client *client, enum nx_vip_wb_t type)
{

	return 0;
}

static int poa030_set_brightness(struct i2c_client *client, int val)
{
    int maxval;
    /* Value would be 0 to 100 */
    /*  AE mode 00 01 10 11 */
#if 0
    maxval = 0x4F;
    maxval = maxval * val / 100;
    poa030_write_reg8(POA030_GLOBALGAIN, maxval);
#else
    maxval = 0x1000 - 0x0100;
    maxval = maxval * val / 100 + 0x0100;
    poa030_write_reg8(POA030_EXT_GLBG_L, maxval&0xFF);
    poa030_write_reg8(POA030_EXT_GLBG_H, (maxval>>8)&0xFF);
#endif

	return 0;
}

static int poa030_set_set_power_save(struct i2c_client *client, int val)
{
    u8 padval;

    poa030_read_reg8(POA030_PAD_CONTROL, &padval);

    if (val == 0)
        poa030_write_reg8(POA030_PAD_CONTROL, padval&0x7F);
    else
        poa030_write_reg8(POA030_PAD_CONTROL, padval|0x80);

	return 0;
}

static int poa030_command(struct i2c_client *client, u32 cmd, void *arg)
{
	switch (cmd) {
	case I2C_CAM_INIT:
		poa030_start(client);
		info("POA030 : external camera initialized\n");
		break;

	case I2C_CAM_RESOLUTION:
		poa030_change_resolution(client, (int) arg);
		break;

	case I2C_CAM_WB:
		poa030_change_whitebalance(client, (enum nx_vip_wb_t) arg);
        break;
    case I2C_CAM_BRIGHTNESS:
        poa030_set_brightness(client, (int) arg);
        break;

    case I2C_CAM_POWER_SAVE:
        poa030_set_set_power_save(client, (int) arg);
        break;

	default:
		err("unexpect command\n");
		break;
	}

	return 0;
}

static const struct i2c_device_id poa030_i2c_id[] = {
    { "poa030", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, poa030_i2c_id);

static struct i2c_driver poa030_i2c_driver = {
	.driver = {
		.name = "POA030",
		.owner = THIS_MODULE,
	},
	.id_table     = poa030_i2c_id,
	.probe        = poa030_i2c_probe,
    .remove       = __devexit_p(poa030_i2c_remove),
	.command      = poa030_command,
};

static __init int poa030_init(void)
{
    printk("PixelPlus POA030 driver initialized\n");
	return i2c_add_driver(&poa030_i2c_driver);
}

static __init void poa030_exit(void)
{
	i2c_del_driver(&poa030_i2c_driver);
}

module_init(poa030_init)
module_exit(poa030_exit)

MODULE_AUTHOR("Seungwoo Kim<ksw@mostitech.com>");
MODULE_DESCRIPTION("POA030 Camera Driver");
MODULE_LICENSE("GPL");

