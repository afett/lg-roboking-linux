/*
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>

/*------------------------------------------------------------------------------
 * Serial platform device
 */
#if defined(CONFIG_SERIAL_NEXELL_UART)

#if	defined(CONFIG_SERIAL_NEXELL_UART_PORT0)
static struct platform_device uart_plat_device0 = {
	.name	= UART_DEV_NAME,
	.id		= 0,
};
#endif
#if	defined(CONFIG_SERIAL_NEXELL_UART_PORT1)
static struct platform_device uart_plat_device1 = {
	.name	= UART_DEV_NAME,
	.id		= 1,
};
#endif

static struct platform_device *uart_plat_devices[] __initdata = {
#if	defined(CONFIG_SERIAL_NEXELL_UART_PORT0)
	&uart_plat_device0,
#endif
#if	defined(CONFIG_SERIAL_NEXELL_UART_PORT1)
	&uart_plat_device1,
#endif
};
#endif /* CONFIG_SERIAL_NEXELL_UART */

/*------------------------------------------------------------------------------
 * USB EHCI platform device
 */
#if defined(CONFIG_USB_EHCI_HCD) ||	defined(CONFIG_USB_EHCI_HCD_MODULE)

static struct resource ehci_resources[] = {
	[0] = {
		.start  = PHY_BASEADDR_EHCI,
		.end    = PHY_BASEADDR_EHCI + 0x00000400,
		.flags  = IORESOURCE_IO,
	},
	[1] = {
		.start  = IRQ_PHY_EHCI,
		.end    = IRQ_PHY_EHCI,
		.flags  = IORESOURCE_IRQ,
	},
};

static u64 ehci_dmamask = 0xffffffffUL;

static struct platform_device ehci_plat_device = {
	.name	= EHCI_DEV_NAME,
	.id		= 0,
	.dev    = {
		.dma_mask = &ehci_dmamask,
		.coherent_dma_mask = 0xffffffffUL
	},
	.num_resources  = ARRAY_SIZE(ehci_resources),
	.resource       = ehci_resources,
};

#endif	/* CONFIG_USB_EHCI_HCD || CONFIG_USB_EHCI_HCD_MODULE */

/*------------------------------------------------------------------------------
 * USB OHCI platform device
 */
#if defined(CONFIG_USB_OHCI_HCD) ||	defined(CONFIG_USB_OHCI_HCD_MODULE)

static struct resource ohci_resources[] = {
	[0] = {
		.start  = PHY_BASEADDR_OHCI,
		.end    = PHY_BASEADDR_OHCI + 0x00000400,
		.flags  = IORESOURCE_IO,
	},
	[1] = {
		.start  = IRQ_PHY_OHCI,
		.end    = IRQ_PHY_OHCI,
		.flags  = IORESOURCE_IRQ,
	},
};

static u64 ohci_dmamask = 0xffffffffUL;

static struct platform_device ohci_plat_device = {
	.name	= OHCI_DEV_NAME,
	.id		= 0,
	.dev    = {
		.dma_mask = &ohci_dmamask,
		.coherent_dma_mask = 0xffffffffUL
	},
	.num_resources  = ARRAY_SIZE(ohci_resources),
	.resource       = ohci_resources,
};

#endif	/* CONFIG_USB_OHCI_HCD || CONFIG_USB_OHCI_HCD_MODULE */

/*------------------------------------------------------------------------------
 * USB Gadget platform device
 */
#if defined(CONFIG_USB_GADGET_NEXELL)

static struct resource gadget_resources[] = {
	[0] = {
		.start  = PHY_BASEADDR_UDC,
		.end    = PHY_BASEADDR_UDC + 0x00000C00,
		.flags  = IORESOURCE_IO,
	},
	[1] = {
		.start  = IRQ_PHY_UDC,
		.end    = IRQ_PHY_UDC,
		.flags  = IORESOURCE_IRQ,
	},
};

static u64 gadget_dmamask = 0xffffffffUL;

static struct platform_device gadget_plat_device = {
	.name	= GADGET_DEV_NAME,
	.id		= -1,
	.dev    = {
		.dma_mask = &gadget_dmamask,
		.coherent_dma_mask = 0xffffffffUL
	},
	.num_resources  = ARRAY_SIZE(gadget_resources),
	.resource       = gadget_resources,
};
EXPORT_SYMBOL(gadget_plat_device);

#endif	/* CONFIG_USB_GADGET_NEXELL */

/*------------------------------------------------------------------------------
 * I2C Bus platform device
 */
#if defined(CONFIG_I2C_NEXELL)

#if	defined(CONFIG_I2C_NEXELL_PORT0)
static struct i2c_plat_data i2c0_platdata = {
	.port		= 0,
	.irq		= IRQ_PHY_I2C0,
	.io_scl		= CFG_PIO_I2C0_SCL,
	.io_sda		= CFG_PIO_I2C0_SDA,
	.clksrc 	= CFG_I2C0_CLOCK_SOURCE,
	.clkscale 	= CFG_I2C0_CLOCK_SCALER,
	.cntmax 	= CFG_I2C0_CLOCK_DELAY,
};

static struct platform_device i2c_plat_device0 = {
	.name	= I2C_DEV_NAME,
	.id		= 0,
	.dev    = {
		.platform_data	= &i2c0_platdata
	},
};
#endif
#if	defined(CONFIG_I2C_NEXELL_PORT1)
static struct i2c_plat_data i2c1_platdata = {
	.port		= 1,
	.irq		= IRQ_PHY_I2C1,
	.io_scl		= CFG_PIO_I2C1_SCL,
	.io_sda		= CFG_PIO_I2C1_SDA,
	.clksrc 	= CFG_I2C1_CLOCK_SOURCE,
	.clkscale 	= CFG_I2C1_CLOCK_SCALER,
	.cntmax 	= CFG_I2C1_CLOCK_DELAY,
};

static struct platform_device i2c_plat_device1 = {
	.name	= I2C_DEV_NAME,
	.id		= 1,
	.dev    = {
		.platform_data	= &i2c1_platdata
	},
};
#endif

static struct platform_device *i2c_plat_devices[] __initdata = {
#if	defined(CONFIG_I2C_NEXELL_PORT0)
	&i2c_plat_device0,
#endif
#if	defined(CONFIG_I2C_NEXELL_PORT1)
	&i2c_plat_device1,
#endif
};

#endif	/* CONFIG_I2C_NEXELL */

/*------------------------------------------------------------------------------
 * RTC (Real Time Clock) platform device
 */
#if defined(CONFIG_RTC_DRV_NEXELL)
static struct platform_device rtc_plat_device = {
	.name	= RTC_DEV_NAME,
	.id		= 0,
};
#endif	/* CONFIG_RTC_DRV_NEXELL */

/*------------------------------------------------------------------------------
 * Graphic VMEM platform device
 */
#if defined(CONFIG_GRP_NEXELL_VMEM)
static struct platform_device vmem_plat_device = {
	.name	= VMEM_DEV_NAME,
	.id		= 0,
};
#endif	/* CONFIG_GRP_NEXELL_VMEM */

/*------------------------------------------------------------------------------
 * Graphic DPC platform device
 */
#if defined(CONFIG_GRP_NEXELL_DPC)
static struct platform_device dpc_plat_device = {
	.name	= DPC_DEV_NAME,
	.id		= 0,
};
#endif	/* CONFIG_GRP_NEXELL_DPC */

/*------------------------------------------------------------------------------
 * Graphic OpenGL|ES platform device
 */
#if defined(CONFIG_GRP_NEXELL_OGL)
static struct platform_device ogl_plat_device = {
	.name	= OGL_DEV_NAME,
	.id		= 0,
};
#endif	/* CONFIG_GRP_NEXELL_OGL */

/*------------------------------------------------------------------------------
 * MISC ADC platform device
 */
#if defined(CONFIG_MISC_NEXELL_ADC)
static struct platform_device adc_plat_device = {
	.name	= ADC_DEV_NAME,
	.id		= 0,
};
#endif	/* CONFIG_MISC_NEXELL_ADC */

/*------------------------------------------------------------------------------
 * MISC PWM platform device
 */
#if defined(CONFIG_MISC_NEXELL_PWM)
static struct platform_device pwm_plat_device = {
	.name	= PWM_DEV_NAME,
	.id		= 0,
};
#endif	/* CONFIG_MISC_NEXELL_PWM */

/*------------------------------------------------------------------------------
 * MISC SPI platform device
 */
#if defined(CONFIG_MISC_NEXELL_SPI)
static struct platform_device spi_plat_device = {
	.name	= SPI_DEV_NAME,
	.id		= 0,
};
#endif	/* CONFIG_MISC_NEXELL_SPI */

/*------------------------------------------------------------------------------
 * MISC SPI platform device
 */
#if defined(CONFIG_MISC_NEXELL_GPIO)
static struct platform_device gpio_plat_device = {
	.name	= GPIO_DEV_NAME,
	.id		= 0,
};
#endif	/* CONFIG_MISC_NEXELL_GPIO */

struct poa030_i2c_platform_data poa030r0_data= {
    .id = 0,
    .i2c_name = "poa030",
    .device_name = "POA030",
};

struct poa030_i2c_platform_data poa030r1_data= {
    .id = 1,
    .i2c_name = "poa030_2",
    .device_name = "POA030_2",
};


static struct i2c_board_info i2c_devs0[] __initdata = {
	{ I2C_BOARD_INFO("wm8960", 0x1a),}, 
	{ I2C_BOARD_INFO("poa030", 0x6E),  .platform_data = &poa030r0_data},
	{ I2C_BOARD_INFO("hi161",  0x20),},
};

static struct i2c_board_info i2c_devs1[] __initdata = { 
	{ I2C_BOARD_INFO("poa030_2", 0x6E), .platform_data = &poa030r1_data},
	{ I2C_BOARD_INFO("wm8960_2", 0x1a),},
};


/*------------------------------------------------------------------------------
 * Network DM9000
 */
#if defined(CONFIG_DM9000) || defined(CONFIG_DM9000_MODULE)
#include <linux/dm9000.h>

static struct resource dm9000_resource[] = {
	[0] = {
		.start	= CFG_EXT_PHY_BASEADDR_ETHER,
		.end	= CFG_EXT_PHY_BASEADDR_ETHER + 3,
		.flags	= IORESOURCE_MEM
	},
	[1] = {
		.start	= CFG_EXT_PHY_BASEADDR_ETHER + 0x4,
		.end	= CFG_EXT_PHY_BASEADDR_ETHER + 0x4 + 500,
		.flags	= IORESOURCE_MEM
	},
	[2] = {
		.start	= CFG_EXT_IRQ_NUM_ETHER,
		.end	= CFG_EXT_IRQ_NUM_ETHER,
		.flags	= IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHLEVEL,
	}
};

static struct dm9000_plat_data dm9000_platdata = {
	.flags		= DM9000_PLATF_16BITONLY,
};

static struct platform_device dm9000_plat_device = {
	.name			= DM9000_DEV_NAME,
	.id				= 0,
	.num_resources	= ARRAY_SIZE(dm9000_resource),
	.resource		= dm9000_resource,
	.dev			= {
		.platform_data	= &dm9000_platdata,
	}
};
#endif	/* CONFIG_DM9000 || CONFIG_DM9000_MODULE */

/*------------------------------------------------------------------------------
 * V4L2 platform device
 */
#if defined(CONFIG_V4L2_DRIVER_NEXELL)

#if defined(CONFIG_V4L2_DRIVER_NEXELL_MODULE0)
static struct resource vip_resource0[] = {
	[0] = {
		.start	= PHY_BASEADDR_VIP0,
		.end	= PHY_BASEADDR_VIP0 + 0x800,
		.flags	= IORESOURCE_MEM
	},
	[1] = {
		.start	= IRQ_PHY_VIP,
		.end	= IRQ_PHY_VIP,
		.flags	= IORESOURCE_IRQ,
	}
};
#endif

#if defined(CONFIG_V4L2_DRIVER_NEXELL_MODULE1)
static struct resource vip_resource1[] = {
	[0] = {
		.start	= PHY_BASEADDR_VIP1,
		.end	= PHY_BASEADDR_VIP1 + 0x800,
		.flags	= IORESOURCE_MEM
	},
	[1] = {
		.start	= IRQ_PHY_VIP1,
		.end	= IRQ_PHY_VIP1,
		.flags	= IORESOURCE_IRQ,
	}
};
#endif

#if defined(CONFIG_V4L2_DRIVER_NEXELL_MODULE0)
int vip0_cfg_gpio()
{
    struct	NX_GPIO_RegisterSet *gpioa;
    
    gpioa = (struct NX_GPIO_RegisterSet *)(IO_ADDRESS(PHY_BASEADDR_GPIO + 0x00));
    
    gpioa->GPIOxALTFN[1] = gpioa->GPIOxALTFN[1] & 0xFCFFFFFF; /* GPIOA 28  to GPIO*/
    gpioa->GPIOxOUT = gpioa->GPIOxOUT & ~0x10000000;
    
    return 0;
}
#endif

#if defined(CONFIG_V4L2_DRIVER_NEXELL_MODULE1)
int vip1_cfg_gpio()
{
    struct	NX_GPIO_RegisterSet *gpioa, *gpiob, *gpioc;
    
    gpioa = (struct NX_GPIO_RegisterSet *)(IO_ADDRESS(PHY_BASEADDR_GPIO + 0x00));
    gpiob = (struct NX_GPIO_RegisterSet *)(IO_ADDRESS(PHY_BASEADDR_GPIO + 0x40));
    gpioc = (struct NX_GPIO_RegisterSet *)(IO_ADDRESS(PHY_BASEADDR_GPIO + 0x80));
    
//    printk("GPIOB FN0 = %x\n", gpiob->GPIOxALTFN[0]);
//    printk("GPIOB FN1 = %x\n", gpiob->GPIOxALTFN[1]);
//    printk("GPIOA FN1 = %x\n", gpioa->GPIOxALTFN[1]);
    gpiob->GPIOxALTFN[0] &= 0x03FFFFFF; /* GPIOB 2, 13,14,15 */
    gpiob->GPIOxALTFN[1] &= 0xFFFFFFC0; /* GPIOB 16,17,18 */
    gpiob->GPIOxALTFN[0] |= 0xFC000000; /* ALT2:2, ALT3 13,14,15 */
    gpiob->GPIOxOUTENB &= 0xFFFFFFFB;
    gpiob->GPIOxALTFN[1] |= 0x0000003F; /* ALT3 16,17,18 */
    gpioa->GPIOxALTFN[1] &= 0x03FFFFFF; /* GPIOA 29,30, 31 */
    gpioa->GPIOxALTFN[1] |= 0xFC000000; /* ALT3 29, 30 , 31*/
    gpioc->GPIOxALTFN[0] &= 0xFFCFFFFF; /* GPIO 10 to GPIO */
    gpioc->GPIOxOUT = gpioc->GPIOxOUT & ~0x00000400;
//    printk("GPIOB FN0 = %x\n", gpiob->GPIOxALTFN[0]);
//    printk("GPIOB FN1 = %x\n", gpiob->GPIOxALTFN[1]);
//    printk("GPIOA FN1 = %x\n", gpioa->GPIOxALTFN[1]);
    
    return 0;
}
#endif

#if defined(CONFIG_V4L2_DRIVER_NEXELL_MODULE0)
int vip0_gpio_reset(int reset)
{
    struct	NX_GPIO_RegisterSet *gpioa;
    
    gpioa = (struct NX_GPIO_RegisterSet *)(IO_ADDRESS(PHY_BASEADDR_GPIO + 0x00));
    
    if (reset)
        gpioa->GPIOxOUT = gpioa->GPIOxOUT | 0x10000000;
    else
        gpioa->GPIOxOUT = gpioa->GPIOxOUT & ~0x10000000;
    
    return 0;
}
#endif

#if defined(CONFIG_V4L2_DRIVER_NEXELL_MODULE1)
int vip1_gpio_reset(int reset)
{
    struct	NX_GPIO_RegisterSet *gpioc;

    gpioc = (struct NX_GPIO_RegisterSet *)(IO_ADDRESS(PHY_BASEADDR_GPIO + 0x80));
    
    if (reset)
        gpioc->GPIOxOUT = gpioc->GPIOxOUT | 0x00000400;
    else
        gpioc->GPIOxOUT = gpioc->GPIOxOUT & ~0x00000400;
    
    return 0;
}
#endif

#if defined(CONFIG_V4L2_DRIVER_NEXELL_MODULE0)
static struct nx_platform_vip v4l2_platdata0 = {
	.in_port		= 1,
	.dpc_device		= MAIN_SCREEN_PRI,
	.hor_align		= 1,
	.ver_align		= 1,
	.buff_count		= 4,
	.skip_count		= 3,
	.cfg_gpio       = vip0_cfg_gpio,
	.gpio_reset     = vip0_gpio_reset,
	
};
#endif

#if defined(CONFIG_V4L2_DRIVER_NEXELL_MODULE1)
static struct nx_platform_vip v4l2_platdata1 = {
	.in_port		= 1,
	.dpc_device		= MAIN_SCREEN_PRI,
	.hor_align		= 1,
	.ver_align		= 1,
	.buff_count		= 4,
	.skip_count		= 3,
	.cfg_gpio       = vip1_cfg_gpio,
	.gpio_reset     = vip1_gpio_reset,
};
#endif

#if defined(CONFIG_V4L2_DRIVER_NEXELL_MODULE0)
static struct platform_device v4l2_plat_device0 = {
	.name	= V4L2_DEV_NAME,
	.id		= 0,	/* 0, 1, ... */
	.num_resources	= ARRAY_SIZE(vip_resource0),
	.resource		= vip_resource0,
	.dev    = {
		.platform_data	= &v4l2_platdata0,
	},
};
#endif

#if defined(CONFIG_V4L2_DRIVER_NEXELL_MODULE1)
static struct platform_device v4l2_plat_device1 = {
	.name	= V4L2_DEV_NAME,
	.id		= 1,	/* 0, 1, ... */
	.num_resources	= ARRAY_SIZE(vip_resource1),
	.resource		= vip_resource1,
	.dev    = {
		.platform_data	= &v4l2_platdata1,
	},
};
#endif

static struct platform_device *v4l2_plat_devices[] = {
#if defined(CONFIG_V4L2_DRIVER_NEXELL_MODULE0)
    &v4l2_plat_device0,
#endif
#if defined(CONFIG_V4L2_DRIVER_NEXELL_MODULE1)
    &v4l2_plat_device1,
#endif
};
#endif	/* CONFIG_VIDEO_NEXELL */

/*------------------------------------------------------------------------------
 * VIP platform device
 */
#if defined(CONFIG_GRP_NEXELL_VIP) || defined(CONFIG_GRP_NEXELL_VIP_MODULE)

#if	defined(CONFIG_GRP_NEXELL_VIP_MODULE0)
static struct platform_device vip_plat_device0 = {
	.name	= VIP_DEV_NAME,
	.id		= 0,
};
#endif

#if	defined(CONFIG_GRP_NEXELL_VIP_MODULE1)
static struct platform_device vip_plat_device1 = {
	.name	= VIP_DEV_NAME,
	.id		= 1,
};
#endif

static struct platform_device *vip_plat_devices[] __initdata = {
#if	defined(CONFIG_GRP_NEXELL_VIP_MODULE0)
	&vip_plat_device0,
#endif
#if	defined(CONFIG_GRP_NEXELL_VIP_MODULE1)
	&vip_plat_device1,
#endif
};
#endif	/* CONFIG_GRP_NEXELL_VIP || CONFIG_GRP_NEXELL_VIP_MODULE */

/*------------------------------------------------------------------------------
 * register cpu platform devices
 */
void __init cpu_device(void)
{
#if defined(CONFIG_SERIAL_NEXELL_UART)
	printk("plat: add device serial (array:%d)\n", ARRAY_SIZE(uart_plat_devices));
	platform_add_devices(uart_plat_devices, ARRAY_SIZE(uart_plat_devices));
#endif

#if defined(CONFIG_USB_EHCI_HCD) ||	defined(CONFIG_USB_EHCI_HCD_MODULE)
	printk("plat: add device usb ehci\n");
	platform_device_register(&ehci_plat_device);
#endif

#if defined(CONFIG_USB_OHCI_HCD) ||	defined(CONFIG_USB_OHCI_HCD_MODULE)
	printk("plat: add device usb ohci\n");
	platform_device_register(&ohci_plat_device);
#endif

#if defined(CONFIG_USB_GADGET_NEXELL)
	printk("plat: add device usb gadget\n");
	platform_device_register(&gadget_plat_device);
#endif

#if defined(CONFIG_I2C_NEXELL)
	printk("plat: add device i2c bus (array:%d) \n", ARRAY_SIZE(i2c_plat_devices));
	platform_add_devices(i2c_plat_devices, ARRAY_SIZE(i2c_plat_devices));
#endif

#if defined(CONFIG_RTC_DRV_NEXELL)
	printk("plat: add device Real Time Clock  \n");
	platform_device_register(&rtc_plat_device);
#endif

/* Graphic */
#if defined(CONFIG_GRP_NEXELL_VMEM)
	printk("plat: add device graphic memory allocator\n");
	platform_device_register(&vmem_plat_device);
#endif

#if defined(CONFIG_GRP_NEXELL_DPC)
	printk("plat: add device display control\n");
	platform_device_register(&dpc_plat_device);
#endif

#if defined(CONFIG_GRP_NEXELL_OGL)
	printk("plat: add device graphic opengl|es\n");
	platform_device_register(&ogl_plat_device);
#endif

/* MISC */
#if defined(CONFIG_MISC_NEXELL_ADC)
	printk("plat: add device misc adc\n");
	platform_device_register(&adc_plat_device);
#endif

#if defined(CONFIG_MISC_NEXELL_PWM)
	printk("plat: add device misc pwm\n");
	platform_device_register(&pwm_plat_device);
#endif

#if defined(CONFIG_MISC_NEXELL_SPI)
	printk("plat: add device misc spi\n");
	platform_device_register(&spi_plat_device);
#endif

#if defined(CONFIG_MISC_NEXELL_GPIO)
	printk("plat: add device misc gpio\n");
	platform_device_register(&gpio_plat_device);
#endif

#if defined(CONFIG_DM9000) || defined(CONFIG_DM9000_MODULE)
	printk("plat: add device dm9000 net\n");
	platform_device_register(&dm9000_plat_device);
#endif

#if defined(CONFIG_V4L2_DRIVER_NEXELL)
	printk("plat: add device v4l2 camera\n");
	platform_add_devices(v4l2_plat_devices, ARRAY_SIZE(v4l2_plat_devices));
#endif
    i2c_register_board_info(0, i2c_devs0, ARRAY_SIZE(i2c_devs0));
    i2c_register_board_info(1, i2c_devs1, ARRAY_SIZE(i2c_devs1));
}


