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
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/clk.h>
#include <linux/log2.h>

#include <mach/hardware.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/irq.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>

#if (1)
#define DBGOUT(msg...)		{ printk(KERN_INFO "rtc: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

/*------------------------------------------------------------------------------
 * 	local data and macro
 */
#define	RTC_COUNT_BIT	(0)
#define	RTC_ALARM_BIT	(1)

static spinlock_t		rtc_lock;
static u_int			rtc_enable_irq = 0;
static u_int			alm_enable_irq = 0;

#define	RTC_TIME_YEAR 	(1970)	/* Jan 1 1970 00:00:00 */
static unsigned long	rtc_time_offs;


/*------------------------------------------------------------------------------
 * 	local functions
 */
static void init_rtc_base(void)
{
	DBGOUT("%s\n", __func__);
	NX_RTC_Initialize();
	NX_RTC_SetBaseAddress((U32)IO_ADDRESS(NX_RTC_GetPhysicalAddress()));
	NX_RTC_OpenModule();

	NX_RTC_ClearInterruptPendingAll();
	NX_RTC_SetInterruptEnableAll(CFALSE);

	rtc_time_offs = mktime(RTC_TIME_YEAR, 1, 1, 0, 0, 0);

	NX_RTC_SetRTCCounterWriteEnable(CTRUE);
	while(NX_RTC_IsBusyRTCCounter()) ;
	NX_RTC_SetRTCCounter(0);

	NX_RTC_SetRTCCounterWriteEnable(CFALSE);

	/* Confirm the write value. */
	while(NX_RTC_IsBusyRTCCounter()) ;
}

/*------------------------------------------------------------------------------
 * 	interrupt handler
 */
static irqreturn_t do_rtc_irq_handler(int irq, void *id)
{
	struct rtc_device *rtc = (struct rtc_device *)id;
	u_int pend = NX_RTC_GetInterruptPending32();

	if(rtc_enable_irq && (pend & (1 << RTC_COUNT_BIT)))	{
		rtc_update_irq(rtc, 1, RTC_PF | RTC_UF | RTC_IRQF);
		NX_RTC_ClearInterruptPending(RTC_COUNT_BIT);
		DBGOUT("IRQ: RTC Count (PND:0x%x, ENB:%d)\n", pend, NX_RTC_GetInterruptEnable(RTC_COUNT_BIT));
		return IRQ_HANDLED;
	}

	if(alm_enable_irq && (pend & (1 << RTC_ALARM_BIT)))	{
		rtc_update_irq(rtc, 1, RTC_AF | RTC_IRQF);
		NX_RTC_ClearInterruptPending(RTC_ALARM_BIT);
		DBGOUT("IRQ: RTC Alarm (PND:0x%x, ENB:%d)\n", pend, NX_RTC_GetInterruptEnable(RTC_ALARM_BIT));
		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}

/*------------------------------------------------------------------------------
 * 	RTC OPS
 */
static int nx_rtc_ops_read_time(struct device *dev, struct rtc_time *tm)
{
	unsigned long rtc;

	spin_lock_irq(&rtc_lock);

	rtc = NX_RTC_GetRTCCounter();
	rtc_time_to_tm(rtc + rtc_time_offs, tm);
	DBGOUT("read time %04d.%02d.%02d %02d/%02d/%02d, rtc 0x%x\n",
		 tm->tm_year, tm->tm_mon, tm->tm_mday,
		 tm->tm_hour, tm->tm_min, tm->tm_sec, (u_int)rtc);

	spin_unlock_irq(&rtc_lock);

	return 0;
}

static int nx_rtc_ops_set_time(struct device *dev, struct rtc_time *tm)
{
	unsigned long rtc, curr_sec;

	spin_lock_irq(&rtc_lock);

	curr_sec = mktime(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
	             tm->tm_hour, tm->tm_min, tm->tm_sec);

	rtc = curr_sec - rtc_time_offs;

	DBGOUT("set time %02d.%02d.%02d %02d/%02d/%02d, rtc 0x%x\n",
		 tm->tm_year, tm->tm_mon, tm->tm_mday,
		 tm->tm_hour, tm->tm_min, tm->tm_sec, (u_int)rtc);

	/* set hw rtc */
	NX_RTC_SetRTCCounterWriteEnable(CTRUE);
	NX_RTC_SetRTCCounter(rtc);
	while(NX_RTC_IsBusyRTCCounter())
	{
		;
	}

	NX_RTC_SetRTCCounterWriteEnable(CFALSE);

	/* Confirm the write value. */
	while(NX_RTC_IsBusyRTCCounter())
	{
		;
	}

	spin_unlock_irq(&rtc_lock);

	return 0;
}

static int nx_rtc_ops_read_alarm(struct device *dev, struct rtc_wkalrm *wkalrm)
{
	struct rtc_time *tm = &wkalrm->time;
	unsigned long alarm;

	spin_lock_irq(&rtc_lock);

	alarm = NX_RTC_GetAlarmCounter();

	rtc_time_to_tm(alarm + rtc_time_offs, tm);

	wkalrm->enabled = (CTRUE == NX_RTC_GetInterruptEnable (RTC_ALARM_BIT) ? 1 : 0);
	wkalrm->pending = (CTRUE == NX_RTC_GetInterruptPending(RTC_ALARM_BIT) ? 1 : 0);

	DBGOUT("read alarm %04d.%02d.%02d %02d/%02d/%02d, alarm 0x%08x\n",
		 tm->tm_year, tm->tm_mon, tm->tm_mday,
		 tm->tm_hour, tm->tm_min, tm->tm_sec, (u_int)alarm);

	spin_unlock_irq(&rtc_lock);

	return 0;
}

static int nx_rtc_ops_set_alarm(struct device *dev, struct rtc_wkalrm *wkalrm)
{
	struct rtc_time *tm = &wkalrm->time;
	unsigned long alarm, alarm_sec;

	spin_lock_irq(&rtc_lock);

	alarm_sec = mktime(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
	                   tm->tm_hour, tm->tm_min, tm->tm_sec);

	alarm = alarm_sec - rtc_time_offs;
	DBGOUT("set  alarm %04d.%02d.%02d %02d/%02d/%02d, alarm 0x%08x\n",
		 tm->tm_year, tm->tm_mon, tm->tm_mday,
		 tm->tm_hour, tm->tm_min, tm->tm_sec, (u_int)alarm);

	/* set hw rtc */
	while(NX_RTC_IsBusyAlarmCounter())
	{
		;
	}
	NX_RTC_SetAlarmCounter(alarm);

	/* Confirm the write value. */
	while(NX_RTC_IsBusyAlarmCounter())
	{
		;
	}

	NX_RTC_ClearInterruptPending(RTC_ALARM_BIT);

	if (wkalrm->enabled)
		NX_RTC_SetInterruptEnable(RTC_ALARM_BIT, CTRUE);	// 0: RTC Counter, 1: RTC Alarm
	else
		NX_RTC_SetInterruptEnable(RTC_ALARM_BIT, CFALSE);	// 0: RTC Counter, 1: RTC Alarm

	spin_unlock_irq(&rtc_lock);

	return 0;
}

static int nx_rtc_ops_open(struct device *dev)
{
	DBGOUT("%s\n", __func__);

	return 0;
}

static void nx_rtc_ops_release(struct device *dev)
{
	DBGOUT("%s\n", __func__);
}


static int nx_rtc_count_irq_enable(struct device *rtc, unsigned int enabled)
{
	DBGOUT("%s (enb:%d)\n", __func__, enabled);
	if(enabled)
		NX_RTC_SetInterruptEnable(RTC_COUNT_BIT, CTRUE);
	else
		NX_RTC_SetInterruptEnable(RTC_COUNT_BIT, CFALSE);

	rtc_enable_irq = enabled;

	return 0;
}

static int nx_rtc_alarm_irq_enable(struct device *rtc, unsigned int enabled)
{
	DBGOUT("%s (enb:%d)\n", __func__, enabled);
	if(enabled)
		NX_RTC_SetInterruptEnable(RTC_ALARM_BIT, CTRUE);
	else
		NX_RTC_SetInterruptEnable(RTC_ALARM_BIT, CFALSE);

	alm_enable_irq = enabled;

	return 0;
}

static int nx_rtc_ops_ioctl(struct device *dev, unsigned int cmd, unsigned long arg)
{
	int ret = 0;

	spin_lock_irq(&rtc_lock);
	DBGOUT("%s cmd=%08x, arg=%08lx \n", __func__, cmd, arg);

	switch (cmd) {
	case RTC_AIE_OFF:
		nx_rtc_alarm_irq_enable(dev, 0);
		break;
	case RTC_AIE_ON:
		nx_rtc_alarm_irq_enable(dev, 1);
		break;
	case RTC_UIE_OFF:
		nx_rtc_count_irq_enable(dev, 0);
		break;
	case RTC_UIE_ON:
		nx_rtc_count_irq_enable(dev, 1);
		break;
	case RTC_IRQP_SET:
		ret = ENOTTY;
		break;
	default:
		ret = -ENOIOCTLCMD;
	}

	spin_unlock_irq(&rtc_lock);

	return ret;
}

/*
 * Provide additional RTC information in /proc/driver/rtc
 */
static int nx_rtc_ops_proc(struct device *dev, struct seq_file *seq)
{
	CBOOL birq = NX_RTC_GetInterruptEnable(RTC_COUNT_BIT);
	seq_printf(seq, "update_IRQ\t: %s\n", birq ? "yes" : "no");
	seq_printf(seq, "periodic_IRQ\t: %s\n", birq ? "yes" : "no");
	return 0;
}

static const struct rtc_class_ops nx_rtc_class_ops = {
	.open				= nx_rtc_ops_open,
	.release			= nx_rtc_ops_release,
	.ioctl 				= nx_rtc_ops_ioctl,
	.read_time			= nx_rtc_ops_read_time,
	.set_time			= nx_rtc_ops_set_time,
	.read_alarm			= nx_rtc_ops_read_alarm,
	.set_alarm			= nx_rtc_ops_set_alarm,
	.proc				= nx_rtc_ops_proc,
};

/*--------------------------------------------------------------------------------
 * RTC platform driver functions
 ---------------------------------------------------------------------------------*/
static int __devinit nx_rtc_driver_probe(struct platform_device *pdev)
{
	struct rtc_device *rtc;
	int ret;

	DBGOUT("probe rtc irq %d\n", IRQ_PHY_RTC);

	spin_lock_init(&rtc_lock);

	init_rtc_base();

	/* cpu init code should really have flagged this device as
	 * being wake-capable; if it didn't, do that here.
	 */
	if (!device_can_wakeup(&pdev->dev))
		device_init_wakeup(&pdev->dev, 1);

	/* register RTC and exit */
	rtc = rtc_device_register(pdev->name, &pdev->dev, &nx_rtc_class_ops, THIS_MODULE);
	if (IS_ERR(rtc)) {
		dev_err(&pdev->dev, "cannot attach rtc\n");
		ret = PTR_ERR(rtc);
		return ret;
	}

	platform_set_drvdata(pdev, rtc);

	/* set rtc frequency value */
	rtc->irq_freq 	   = 1;
	rtc->max_user_freq = 1;

	/* register disabled irq */
	ret = request_irq(IRQ_PHY_RTC, do_rtc_irq_handler, IRQF_DISABLED,  "rtc 1hz", rtc);
	if (ret) {
		dev_err(&pdev->dev, "IRQ%d error %d\n", (u_int)IRQ_PHY_RTC, ret);
		return ret;
	}
	DBGOUT("done: rtc probe ...\n");

	return 0;
}

static int __devexit nx_rtc_driver_remove(struct platform_device *pdev)
{
	struct rtc_device *rtc = platform_get_drvdata(pdev);

	DBGOUT("%s\n", __func__);

	free_irq(IRQ_PHY_RTC, rtc);

	rtc_device_unregister(rtc);
	platform_set_drvdata(pdev, NULL);

	kfree(rtc);

	return 0;
}

static int nx_rtc_driver_suspend(struct platform_device *pdev, pm_message_t state)
{
	PM_DBGOUT("+%s (rtc irq:%s, alarm irq:%s)\n",
		__func__, rtc_enable_irq?"on":"off", alm_enable_irq?"on":"off");

	if(rtc_enable_irq) {
		NX_RTC_ClearInterruptPending(RTC_COUNT_BIT);
		NX_RTC_SetInterruptEnable(RTC_COUNT_BIT, CFALSE);
	}

	PM_DBGOUT("-%s\n", __func__);
	return 0;
}

static int nx_rtc_driver_resume(struct platform_device *pdev)
{
	PM_DBGOUT("+%s (rtc irq:%s, alarm irq:%s)\n",
		__func__, rtc_enable_irq?"on":"off", alm_enable_irq?"on":"off");

	if(rtc_enable_irq) {
		NX_RTC_ClearInterruptPending(RTC_COUNT_BIT);
		NX_RTC_SetInterruptEnable(RTC_COUNT_BIT, CTRUE);
	}

	PM_DBGOUT("-%s\n", __func__);
	return 0;
}

static struct platform_driver rtc_plat_driver = {
	.probe		= nx_rtc_driver_probe,
	.remove		= __devexit_p(nx_rtc_driver_remove),
	.suspend	= nx_rtc_driver_suspend,
	.resume		= nx_rtc_driver_resume,
	.driver		= {
		.name	= RTC_DEV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init nx_rtc_driver_init(void)
{
	DBGOUT("%s\n", __func__);
	return platform_driver_probe(&rtc_plat_driver, nx_rtc_driver_probe);
}

static void __exit nx_rtc_driver_exit(void)
{
	DBGOUT("%s\n", __func__);
	platform_driver_unregister(&rtc_plat_driver);
}

module_init(nx_rtc_driver_init);
module_exit(nx_rtc_driver_exit);

MODULE_DESCRIPTION("RTC driver for the Nexell");
MODULE_AUTHOR("jhkim <jhkim@nexell.co.kr>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:rtc");
