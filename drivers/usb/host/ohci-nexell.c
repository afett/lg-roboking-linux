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
#include <linux/device.h>
#include <linux/signal.h>
#include <linux/platform_device.h>
#include <linux/pm.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>

/* Debugging stuff */
#if (0)
#define DBGOUT(msg...)	{ printk(KERN_INFO "ohci: " msg); }
#else
#define DBGOUT(msg...)
#endif

static bool usb_hc_init = false;

static int set_host_mode(void)
{
	u_int UDC_BASE = IO_ADDRESS(PHY_BASEADDR_UDC);
	u_int UDC_MODE = IO_ADDRESS(0xC0018848);

	/* CLOCK_GEN : clock div=0, 3:use ext_clk */
	*(volatile unsigned int *)(UDC_BASE + 0x8C4) = (
													(0<<15) |		// clk output enabled
													(0<< 5) |		// clock divide is 1
													(3<< 2) |		// ext clock
													(0<< 1)			// nomal clock output(falling edge)
													);


	/* CLOCK_ENB : clock always, clk enable */
	*(volatile unsigned int *)(UDC_BASE + 0x8C0) = (
													(1<<3) |	// PCLK Op mode is always
													(1<<2) |	// Clock Generation Enable
													(3<<0)		// usbd clock mode is always
													);

	/* PHY power disable */
	*(volatile unsigned short *)(UDC_BASE + 0x52) = 1<<0;

	*(volatile unsigned short *)(UDC_BASE + 0x840) = (
													(0<<9) |	// XO bias, and PLL blocks remain powered in suspend mode.
													(0<<7) |	// Ref Clk Freq is 12MHz
													(1<<5) |	// XO block uses an external clock supplied on the XO pin
													(1<<4) |	// POR Enable
													(0<<3) |	// POR
													(1<<2) |	// suspendm enable
													(1<<1) |	// suspend normal op mode
													(1<<0)		// ext VBUS enable
													);			// 12MHz,

	*(volatile unsigned short *)(UDC_BASE + 0x052) = (
													(1<<0) |
													(1<<7)
													);			// phy block off, utmi reset

	/* Dummy delay : required over 10 usec */
	udelay(10);

	*(volatile unsigned short *)(UDC_BASE + 0x052) = 0;			// phy block on

	*(volatile unsigned short *)(UDC_BASE + 0x848) = (
													(0<<9) |	// pull-down resistance on D+ is disabled
													(0<<8) |	// pull-down resistance on D- is disabled
													(0<<7) |	// this bit actived if opmode is 2b11
													(0<<6) |	// this bit actived if opmode is 2b11
													(1<<5) |	// OTG block is powered down
													(1<<4) |	// Ext VBus is enable
													(1<<3) |	// VBus valid comparator is enabled.
													(0<<2) |	// disabled charging VBus through the  SRP pull-up resistance
													(0<<1) |	// disable dischrging VBus through the SRP pull-down resistance
													(0<<0)		// ID pin sampling is disbled, and IDDDIG output is not valid.
													);

	/* set usb host mode */
	*(volatile unsigned int *)(UDC_MODE) = (*(volatile unsigned int *)(UDC_MODE) | (1<<10));

	return 0;
}

static void start_usb_hc(struct device *dev)
{
	u_int HC_BASE = IO_ADDRESS(PHY_BASEADDR_EHCI);
	u_int IO_BASE = __PB_IO_MAP_REGS_VIRT;

#if defined(CONFIG_USB_GADGET_NXP2120) || defined(CONFIG_USB_GADGET_NXP2120_MODULE)
#error	"Not support usb device, when use usb host"
#endif

	if(true == usb_hc_init)
		return;

	DBGOUT("%s\n", __func__);

	/* set host mode */
	set_host_mode();

	/* set host module */
	*(volatile unsigned int *)(HC_BASE + 0x3C4) =
											0<<15 |	// Specifies the direction of the PADVCLK pad. You have to set this bit when CLKSRCSEL0/1 is 3 or 4. 0 : Enable(Output)                    1 : Reserved
											0<<5 |	// Specifies divider value of the source clock.
													//	Divider value = CLKDIV0 + 1
													//	Clock Divisor. For divide - by - N, enter an [N - 1] value.
													//	000000 ~ 111111(N-1) : Divide Value (N) = Divide by 1 to 64
													//	Ex) For divide-by-2 : [0001b]
											3<<2 |	// Clock Source Selection
													//	000 : Reserved			001 : Resrved
													//	010 : Reserved			011 : Ext Clock
													//	100 ~ 111 : Reserved
											0<<1;	// Specifies whether to invert the clock output.
													// 0 : Normal (Falling Edge)	1 : Invert (Rising Edge)


	*(volatile unsigned int *)(HC_BASE + 0x3C0) =
											1<<3 |	// PCLK Operating Mode 0 : Clock is enabled only when CPU accesses 1 : Always
											1<<2 |	// Clock Generation Enable 0 : Disable	1 : Enable
											3<<0;	// USBD Clock Enable
													// 00 : Disable		01 : Reserved		10 : Dynamic	11 : Always
	msleep(1);

	*(volatile unsigned int *)(HC_BASE + 0x0C0) =
											0<<9 |	// Common Block Power-Down Control :
														//	Function: This signal controls the power-down signals in the XO, Bias, and PLL blocks when the USB 2.0 nanoPHY is suspended.
														//	o 1: The XO, Bias, and PLL blocks are powered down in Suspend mode.
														//	o 0: The XO, Bias, and PLL blocks remain powered in Suspend mode.
											0<<7 |	// Reference Clock Frequency Select
													//	Function: This bus selects the USB 2.0 nanoPHY reference clock frequency.
													//	o 11: Reserved
													//	o 10: 48 MHz
													//	o 01: 24 MHz
													//	o 00: 12 MHz
											1<<5 |	// Reference Clock Select for PLL Block
													//	Function: This signal selects the reference clock source for the PLL block.
													//	o 11: The PLL uses CLKCORE as reference.
													//	o 10: The PLL uses CLKCORE as reference.
													//	o 01: The XO block uses an external clock supplied on the XO pin.
													//	o 00: The XO block uses the clock from a crystal.
											0<<4 |	// POR Enbale
													// 0 : Disable	1: Enable
											0<<3 |	// Power-On Reset
													// Function: This customer-specific signal resets all test registers and state machines in the USB 2.0 nanoPHY.
													// The POR signal must be asserted for a minimum of 10 ¥ìs.
											0<<2 |	// SuspendM Enbale
													// 0 : Disable	1: Enable
											0<<1;	// Suspend Assertion
													//	Function: Asserting this signal suspends the USB 2.0 nanoPHY by tristating the transmitter and powering down the USB 2.0 nanoPHY circuits.
													//	o 1: Normal operating mode
													//	o 0: Suspend mode
													//	USB 2.0 nanoPHY power-down behavior can be overridden using the test interface.
	msleep(1);

	*(volatile unsigned int *)(HC_BASE + 0x0C4) = 0;		// VBUS Interrupt Enbale
													// 0 : Disable	1: Enable
	msleep(1);

	*(volatile unsigned int *)(HC_BASE + 0x0C8) = 0;		// VBUS Interrupt Pending Enbale
													// 0 : Disable	1: Enable
	msleep(1);

	*(volatile unsigned int *)(HC_BASE + 0x0CC) =
											0<<9 |	// TermSel Enbale
													// 0 : Disable	1: Enable
											0<<8 |	// USB Termination Select
													//	Function: This controller signal sets FS or HS termination.
													//	o 1: Full-speed termination is enabled.
													//	o 0: High-speed termination is enabled.
											1<<7 |	// WordInterface Enbale
													// 0 : Disable	1: Enable
											1<<6 |	// UTMI Data Bus Width and Clock Select
													//	Function: This controller signal selects the data bus width of the UTMI data input and output paths.
													//	o 1: 16-bit data interface
													//	o 0: 8-bit data interface
													//	The USB 2.0 nanoPHY supports 8/16-bit data interfaces for all valid USB 2.0 nanoPHY speed modes.
											0<<5 |	// OPMode Enbale
													// 0 : Disable	1: Enable
											0<<3 |	// UTMI Operational Mode
													//	Function: This controller bus selects the UTMI operational mode.
													//	o 11: Normal operation without SYNC or EOP generation.
													//			If the XCVRSEL bus is not set to 2'b00 when OPMODE[1:0] is set to 2'b11,
													//			USB 2.0 nanoPHY behavior is undefined.
													//	o 10: Disable bit stuffing and NRZI encoding
													//	o 01: Non-driving
													//	o 00: Normal
											0<<2 |	// XCVRSel Enbale
													// 0 : Disable	1: Enable
											0<<0;	// Transceiver Select
													//	Function: This controller bus selects the HS, FS, or LS Transceiver.
													//	o 11: Sends an LS packet on an FS bus or receives an LS packet.
													//	o 10: LS Transceiver
													//	o 01: FS Transceiver
													//	o 00: HS Transceiver
	msleep(1);
	*(volatile unsigned int *)(HC_BASE + 0x0D0) = 0x320;	//

	msleep(1);
	*(volatile unsigned int *)(HC_BASE + 0x0D4) = 1<<15 | 1<<14 | 1<<13 | 0x20<<7 | 0x20<<1;	// default value

	msleep(1);
	*(volatile unsigned int *)(HC_BASE + 0x0D8) = 0x6D78;	// default value

	msleep(1);

	printk(KERN_INFO "%s: usb host OHCI version 0x%08x\n",
		OHCI_DEV_NAME, *(volatile unsigned int *)(IO_BASE + 0x00019800));

	usb_hc_init = true;
}


static void stop_usb_hc(struct device *dev)
{
	DBGOUT("%s\n", __func__);

	usb_hc_init = false;
}


/**
 * usb_hcd_nx_probe - initialize nexell-based HCDs
 * Context: !in_interrupt()
 *
 * Allocates basic resources for this USB host controller, and
 * then invokes the start() method for the HCD associated with it
 * through the hotplug entry's driver_data.
 *
 */
static int usb_hcd_nx_probe (const struct hc_driver *driver, struct platform_device *pdev)
{
	int retval;
	struct usb_hcd *hcd;

	DBGOUT("%s\n", __func__);

	/* allocate memory for usb_hcd struct. defined usb/ocre/hcd.c */
	hcd = usb_create_hcd (driver, &pdev->dev, EHCI_DEV_NAME);
	if (!hcd)
		return -ENOMEM;

	hcd->rsrc_start = pdev->resource[0].start;
	hcd->rsrc_len   = pdev->resource[0].end - pdev->resource[0].start + 1;

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len, pdev->name)) {
		pr_debug("request_mem_region failed");
		retval = -EBUSY;
		goto err_put;
	}

	hcd->regs = ioremap(hcd->rsrc_start, hcd->rsrc_len);
	if (!hcd->regs) {
		pr_debug("ioremap failed");
		retval = -ENOMEM;
		goto err_ioremap;
	}

	start_usb_hc(&pdev->dev);

	ohci_hcd_init(hcd_to_ohci(hcd));

	retval = usb_add_hcd(hcd, pdev->resource[1].start, IRQF_DISABLED);

	if (retval != 0)
		goto err_ioremap;

	return 0;

err_ioremap:
	stop_usb_hc(&pdev->dev);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);

err_put:
	usb_put_hcd(hcd);

	return retval;
}


/**
 * usb_hcd_nx_remove - shutdown processing for nexell-based HCDs
 * @dev: USB Host Controller being removed
 * Context: !in_interrupt()
 *
 * Reverses the effect of usb_hcd_nx_probe(), first invoking
 * the HCD's stop() method.  It is always called from a thread
 * context, normally "rmmod", "apmd", or something similar.
 *
 */
static void usb_hcd_nx_remove (struct usb_hcd *hcd, struct platform_device *pdev)
{
	DBGOUT("%s\n", __func__);

	usb_remove_hcd(hcd);

	stop_usb_hc(&pdev->dev);

	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);
}

static int __devinit ohci_hc_start (struct usb_hcd *hcd)
{
	struct ohci_hcd	*ohci = hcd_to_ohci (hcd);
	int		ret;

	ohci_dbg (ohci, "ohci_hc_start, ohci:%p", ohci);

	/* The value of NDP in roothub_a is incorrect on this hardware */
	ohci->num_ports = 3;

	if ((ret = ohci_init(ohci)) < 0)
		return ret;

	if ((ret = ohci_run (ohci)) < 0) {
		err ("can't start %s", hcd->self.bus_name);
		ohci_stop (hcd);
		return ret;
	}

	return 0;
}

static const struct hc_driver ohci_hc_driver = {
	.description =		hcd_name,
	.product_desc =		"Nexell OHCI ",
	.hcd_priv_size =	sizeof(struct ohci_hcd),

	/*
	 * generic hardware linkage
	 */
	.irq =			ohci_irq,
	.flags =		HCD_USB11 | HCD_MEMORY,

	/*
	 * basic lifecycle operations
	 */
	.start =		ohci_hc_start,
	.stop =			ohci_stop,
	.shutdown =		ohci_shutdown,

	/*
	 * managing i/o requests and associated device resources
	 */
	.urb_enqueue =		ohci_urb_enqueue,
	.urb_dequeue =		ohci_urb_dequeue,
	.endpoint_disable =	ohci_endpoint_disable,

	/*
	 * scheduling support
	 */
	.get_frame_number =	ohci_get_frame,

	/*
	 * root hub support
	 */
	.hub_status_data =	ohci_hub_status_data,
	.hub_control =		ohci_hub_control,
#ifdef  CONFIG_PM
	.bus_suspend =		ohci_bus_suspend,
	.bus_resume =		ohci_bus_resume,
#endif
	.start_port_reset =	ohci_start_port_reset,
};


static int ohci_nx_driver_probe(struct platform_device *pdev)
{
	DBGOUT("%s\n", __func__);

	return usb_hcd_nx_probe(&ohci_hc_driver, pdev);
}

static int ohci_nx_driver_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);

	DBGOUT("%s\n", __func__);

	usb_hcd_nx_remove(hcd, pdev);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

#ifdef	CONFIG_PM
static int ohci_nx_driver_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);
	struct ohci_hcd *ohci = hcd_to_ohci(hcd);

	PM_DBGOUT("+%s\n", __func__);

	if (time_before(jiffies, ohci->next_statechange))
		msleep(5);
	ohci->next_statechange = jiffies;

	stop_usb_hc(&pdev->dev);

	hcd->state = HC_STATE_SUSPENDED;

	PM_DBGOUT("-%s\n", __func__);
	return 0;
}

static int ohci_nx_driver_resume(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);
	struct ohci_hcd *ohci = hcd_to_ohci(hcd);

	PM_DBGOUT("+%s\n", __func__);

	if (time_before(jiffies, ohci->next_statechange))
		msleep(5);

	ohci->next_statechange = jiffies;

	start_usb_hc(&pdev->dev);

	ohci_finish_controller_resume(hcd);

	PM_DBGOUT("-%s\n", __func__);
	return 0;
}
#endif


static struct platform_driver ohci_hcd_nexell_driver = {
	.probe		= ohci_nx_driver_probe,
	.remove		= ohci_nx_driver_remove,
	.shutdown	= usb_hcd_platform_shutdown,
#ifdef CONFIG_PM
	.suspend	= ohci_nx_driver_suspend,
	.resume		= ohci_nx_driver_resume,
#endif
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= OHCI_DEV_NAME,
	},
};

MODULE_ALIAS("platform:nexell-ohci");


