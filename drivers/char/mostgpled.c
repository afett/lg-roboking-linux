/*
 *  GPLED driver for MOSTiTECH nxp2120 board.
 *  Copyright(c) 2011 MOSTiTECH co., ltd.
 *  All right reserved. Seungwoo Kim <ksw@mostitech.com>
 *  revision note :
 *           2011. 04. 04 : Ver 0.5  initially created.
 */
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/errno.h>
#include <linux/tty.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/mutex.h>
#include <linux/vt_kern.h>
#include <linux/selection.h>
#include <linux/kbd_kern.h>
#include <linux/console.h>
#include <linux/device.h>
#include <linux/spinlock.h>

#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/timer.h>

#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/leds.h>
#include <asm/irq.h>
#include <asm/mach/irq.h>
#include <asm/mach/time.h>
#include <asm/mach-types.h>

#include <mach/platform.h>

#define GPIO_DEBUG	0

#define GPLED_MAJOR	18
#define GPLED_MINOR	0

static unsigned int led_status;
static int boot_cnt = 1;
static int boot_end = HZ/10;
static int boot_first = 0;
static int boot_end;
static struct NX_GPIO_RegisterSet *base;
static struct NX_PWM_RegisterSet *pwm_base;

struct gpled_data {
    int hz;
    struct timer_list timer;
};

struct gpled_data gpdata;

int display(int val);

/* This rouitne is called after boot to measure/sign up boot process */
int gpled_bootled(void)
{
	if (boot_first) {
		if (boot_first < 10) {
			display(0x00000000 | 0x7FFFF);
		}
		boot_first++;
		if (boot_first > 20)
			boot_first = 0;
	} else {
		display(0x00000000| boot_cnt);
		boot_cnt <<= 1;
		if (boot_cnt > 0x800)
			boot_cnt = 1;
	}
	return boot_end;
}

#if defined(CONFIG_MODEL_RKSII_VI) || defined(CONFIG_MODEL_RK_HIT_V2) || defined(CONFIG_MODEL_RK_HIT_V4)
int display(int val)
{
	return 0;
}
#else
int display(int val)
{
    int led;

    led = base->GPIOxOUT & 0xFFC00000;
    if (val & 0x80000)
      led |= val & 0x37FFFF;
    else
      led |= (val & 0x37FFFF) | 0x80000;
#if defined(CONFIG_SND_NEXELL_SOC_AC97)
    base->GPIOxOUT = (led & 0xFFF00000) | ((~led) & 0xFFFFF);
#else
    base->GPIOxOUT = led;
#endif

#if GPIO_DEBUG
    printk("GPIOA_OUT val = %x\n", base->GPIOxOUT);
    printk("GPIOA_OUTENB val = %x\n", base->GPIOxOUTENB);
    printk("GPIOA_ALTFN0 = %x\n", base->GPIOxALTFN[0]);
    printk("GPIOA_ALTFN1 = %x\n", base->GPIOxALTFN[1]);
#endif
    return 0;
}
#endif	// CONFIG_MODEL_RKSII_VI || CONFIG_MODEL_RK_HIT_V2 || CONFIG_MODEL_RK_HIT_V4

static ssize_t
gpled_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	int ret=0;
	if (count > 0) {
		ret = copy_to_user(buf, &led_status, sizeof(led_status));
	}
	return ret;
}

static ssize_t
gpled_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	int ret=0;
	if (count > 0) {
		ret = copy_from_user(&led_status, buf, sizeof(led_status));
	}
	display(led_status);
	return ret;
}

static int
gpled_open(struct inode *inode, struct file *filp)
{
#if defined(CONFIG_NEXELL_FB)
#error "This drive should not use with LCD driver."
#endif

	boot_end = 0; /* This would stop the boot LED */
	del_timer(&gpdata.timer);

	return 0;
}

static int
gpled_release(struct inode *inode, struct file *filp)
{
	return 0;
}

#define GPLEDIOC_PWM_DUTY_CTRL_R	_IOWR('G',100, int)
#define GPLEDIOC_PWM_DUTY_CTRL_W	_IOW('G',101, int)
#define GPLEDIOC_PWM_PERIOD_CTRL_R	_IOWR('G',102, int)
#define GPLEDIOC_PWM_PERIOD_CTRL_W	_IOW('G',103, int)

static int gpled_ioctl(struct inode *node, struct file *fl, unsigned int com,
		    unsigned long arg)
{
    unsigned short int duty;
    void __user *argp = (void __user *)arg;
    int ret;

    switch (com) {
        case GPLEDIOC_PWM_DUTY_CTRL_R :
            duty = pwm_base->PWM2[0].PWM_DUTY[0];
            ret = copy_to_user(argp, &duty, sizeof(duty)); // ret : make compiler happy
            break;
        case GPLEDIOC_PWM_DUTY_CTRL_W :
            ret = copy_from_user(&duty, argp, sizeof(duty)); // ret : make compiler happy
            pwm_base->PWM2[0].PWM_DUTY[0] = duty;
            if (duty > 0) {
                pwm_base->PWM_CLKENB = 0x0C; /* PCLK always and GlockGeneration Disable */
            } else {
                if (duty == 0) {
                    /* We need to make some delay as update 1 period...*/
                    msleep(1);
                    pwm_base->PWM_CLKENB = 0x08; /* PCLK always and GlockGeneration Disable */
                }
            }
            break;
        case GPLEDIOC_PWM_PERIOD_CTRL_R :
            duty = pwm_base->PWM2[0].PWM_PERIOD[0];
            ret = copy_to_user(argp, &duty, sizeof(duty)); // ret : make compiler happy
            break;
        case GPLEDIOC_PWM_PERIOD_CTRL_W :
            ret = copy_from_user(&duty, argp, sizeof(duty)); // ret : make compiler happy
            pwm_base->PWM2[0].PWM_PERIOD[0] = duty;
            break;
    }
    return 0;
}


static const struct file_operations gpled_fops = {
	.read		= gpled_read,
	.write		= gpled_write,
	.ioctl      = gpled_ioctl,
	.open		= gpled_open,
	.release	= gpled_release,
};

static struct class *gpled_class;

extern void gpio_gpled_active(void);

static int __init nx_gpled_init(void)
{
    base = (struct NX_GPIO_RegisterSet *)IO_ADDRESS(PHY_BASEADDR_GPIO);
    pwm_base= (struct NX_PWM_RegisterSet *)IO_ADDRESS(PHY_BASEADDR_PWM);

#if defined(CONFIG_SND_NEXELL_SOC_AC97)
    base->GPIOxOUT = 0x00000;
#else
    base->GPIOxOUT = 0xFFFFF;
#endif
    base->GPIOxOUTENB |= 0xFFFFFFF;
/*  Now setting PWM as base period would be 10KHz or so.
    I want that PCLK divider generate about 40MHz, and we would set
    Prescaler to 10KHz by 1/4 and period set to 1000.
    If duty is bigger that period, then it would be always 0 or 1 state for
    output as all the period. So before first time setting to correct values for period and duty,
    I would set duty to 1023 and period to 1000 before enable these PWM registers.
*/
    pwm_base->PWM2[0].PWM_DUTY[0] = 0;
    pwm_base->PWM2[0].PWM_PERIOD[0] = 1000;
    /* I would set PLL1, and 192MHz to 48MHz as ClockDivider to 4 */
    pwm_base->PWM_CLKGEN = ((4-1) << 5) | (1 << 2) | 0;
    /* Then prescaler to 4 for 12MHz base, then with 1/1000 of period, It would be 12KHz base clock for PWM */
    pwm_base->PWM2[0].PWM_PREPOL = (1 << 7) | 4; /* for PWM0 */
    /* Now enable Clock for PWM unit */
    pwm_base->PWM_CLKENB = 0x08; /* PCLK always and GlockGeneration Disable */

    return 0;
}

void gpled_timer_handler(unsigned long arg)
{
    struct gpled_data *gp = (struct gpled_data *)arg;

    if (gpled_bootled()) {
        gp->timer.expires += gp->hz;
        add_timer(&gp->timer);
    }
}

static int __init gpled_init(void)
{
//    base = (struct NX_GPIO_RegisterSet *)IO_ADDRESS(PHY_BASEADDR_GPIO);
//    base->GPIOxOUTENB |= 0xFFFFFFF;
//    base->GPIOxOUT = 0x7FFFF;
    //base->GPIOxALTFN[0] = 0x00000000;
    //base->GPIOxALTFN[1] = 0x55000000;

	if (register_chrdev(GPLED_MAJOR, "gpled", &gpled_fops))
		panic("unable to get major %d for gpled device", GPLED_MAJOR);
	gpled_class = class_create(THIS_MODULE, "gpled");

	device_create(gpled_class, NULL, MKDEV(GPLED_MAJOR, GPLED_MINOR), NULL, "gpled");

#if GPIO_DEBUG
    printk("GPIOA_OUTENB = %x\n", base->GPIOxOUTENB);
    printk("GPIOA_OUT    = %x\n", base->GPIOxOUT);
    printk("GPIOA_ALTFN0 = %x\n", base->GPIOxALTFN[0]);
    printk("GPIOA_ALTFN1 = %x\n", base->GPIOxALTFN[1]);
#endif

    init_timer(&gpdata.timer);
    gpdata.hz = HZ / 10;
    gpdata.timer.expires = get_jiffies_64() + gpdata.hz;
    gpdata.timer.function = gpled_timer_handler;
    gpdata.timer.data = (unsigned long)&gpdata;

    add_timer(&gpdata.timer);

	display(0x0007FFFF);

	printk("GPLED device is registered and ready to use.\n");

	printk("hz = %d\n", gpdata.hz);

	return 0;
}

static void __exit gpled_cleanup(void)
{
	/* do nothing? */
	display(0); /* All led off */
}

core_initcall(nx_gpled_init);

module_init(gpled_init);
module_exit(gpled_cleanup);

MODULE_AUTHOR("Seungwoo Kim <ksw@mostitech.com>");
MODULE_DESCRIPTION("nxp2120 Mostitech/LGE GPLED Driver");
MODULE_LICENSE("GPL");
