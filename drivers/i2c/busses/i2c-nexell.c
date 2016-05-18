/*
 * Copyright(c) 2011 STCube, co., ltd.
 * All right reserved by Seungwoo Kim <ksw@stcube.com>
 *
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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/i2c-id.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>
#include <mach/soc.h>

#define OLD_IMPLEMENTATION  1

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "i2c: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#if (0)
#define ERROUT(msg...)		{ printk(KERN_ERR "i2c: " msg); }
#else
#define ERROUT(msg...)		do {} while (0)
#endif

#define I2C_RETRY_COUNT		(10)
#define	ACK_WAIT_TIMEOUT	(50)	/* wait 50 msec */
#define	I2C_SOURCE_GPIO		(0)

#define NXP2120_ICCR_ACKGEN                 (1 << 7)
#define NXP2120_ICCR_CLKSRC_PCLK_DIV_256    (1 << 6)
#define NXP2120_ICCR_CLKSRC_PCLK_DIV_16     (0 << 6)
#define NXP2120_ICCR_IRQENB                 (1 << 5)
#define NXP2120_ICSR_ST_ENB                 (1 << 12)
#define NXP2120_ICSR_SLAVE_MATCH            (1 << 10)
#define NXP2120_ICSR_GENERAL_CALL           (1 << 9)
#define NXP2120_ICSR_SLV_RX_STOP            (1 << 8)
#define NXP2120_ICSR_MASTER                 (1 << 7)
#define NXP2120_ICSR_TX                     (1 << 6)
#define NXP2120_ICSR_BUSY                   (1 << 5)
#define NXP2120_ICSR_I2C_START              (1 << 5)
#define NXP2120_ICSR_TXRX_ENABLE            (1 << 4)
#define NXP2120_ICSR_ARBIT_FAIL             (1 << 3)
#define NXP2120_ICSR_ACK_STATUS             (1 << 0)
#define NXP2120_I2C_PEND_OP_HOLD            (1 << 1)
#define NXP2120_I2C_PEND_PEND               (1 << 0)
#define NXP2120_I2C_CLKENB_PCLKMODE         (1 << 3)

static unsigned int ack_time_out = ACK_WAIT_TIMEOUT;
/*------------------------------------------------------------------------------
 * 	Extern
 */
extern unsigned int cpu_get_clock_hz(int clk);

/*------------------------------------------------------------------------------
 * 	local data and macro
 */
struct i2c_hw {
	int 	port;
	int 	irq;
	u_int	io_scl;		/* scl io */
	u_int	io_sda;		/* scl io */
	u_int	clksrc;
	u_int	clkscale;
	u_int	cntmax;
	int		polling;
};

struct i2c_port  {
	struct i2c_adapter	adap;
	struct i2c_hw		hw;
	struct NX_I2C_RegisterSet *regs;
	struct mutex		lock;
	wait_queue_head_t	wait;
	unsigned int	 	cond;
	int					polling;
	int					gpio;
	bool				nostop;
	u_char				prev_data;
};

/*------------------------------------------------------------------------------
 * 	I2C interrupt handler, wait ACK
 */
static irqreturn_t i2c_irq_handler(int irqno, void *dev_id)
{
	struct i2c_port *i2c = dev_id;
	int poll = i2c->polling;
	DBGOUT("%s(i2c:%d, cond:%d, %s)\n",
		__func__, i2c->hw.port, i2c->cond, poll?"polling":"irq");

	/* set irq condition */
	i2c->cond = 1;

	if (! poll)
		wake_up(&i2c->wait);

#if OLD_IMPLEMENTATION
	NX_I2C_ClearInterruptPendingAll(i2c->hw.port);
#else
    i2c->regs->PEND = (i2c->regs->PEND & ~0x03) | NXP2120_I2C_PEND_PEND;
#endif

	return IRQ_HANDLED;
}

/*------------------------------------------------------------------------------
 * 	I2C local functions
 */
static int	init_i2c_func(struct i2c_port *i2c, struct i2c_plat_data * plat)
{
	int ret = 0;
	u_int pclk;

	/* set i2c hardware */
	i2c->hw.port	 = plat->port;
	i2c->hw.irq		 = plat->irq;
	i2c->hw.io_scl	 = plat->io_scl;
	i2c->hw.io_sda	 = plat->io_sda;
	i2c->hw.clksrc	 = plat->clksrc;
	i2c->hw.clkscale = plat->clkscale;
	i2c->hw.cntmax 	 = plat->cntmax;
	i2c->hw.polling	 = plat->polling;

	i2c->polling	 = plat->polling;
	i2c->gpio		 = plat->clksrc==I2C_SOURCE_GPIO ? 1 : 0;
	i2c->nostop		 = false;

	/* init i2c resource */
	mutex_init(&i2c->lock);

	if (i2c->gpio) {
		printk("%s.%d: gpio mode\n", I2C_DEV_NAME, i2c->hw.port);
		return ret;
	}

	init_waitqueue_head(&i2c->wait);

	ack_time_out = ACK_WAIT_TIMEOUT/(1000/msecs_to_jiffies(1000));

#if OLD_IMPLEMENTATION
	/* for I2C bus */
	NX_I2C_SetBaseAddress(i2c->hw.port, (U32)IO_ADDRESS(NX_I2C_GetPhysicalAddress(i2c->hw.port)));
#else
    NX_I2C_SetBaseAddress(i2c->hw.port, (U32)IO_ADDRESS(NX_I2C_GetPhysicalAddress(i2c->hw.port)));
    if (0 == i2c->hw.port)
        i2c->regs = (U32)IO_ADDRESS(PHY_BASEADDR_I2C0_MODULE);
    else
        i2c->regs = (U32)IO_ADDRESS(PHY_BASEADDR_I2C1_MODULE);
#endif

	/* 0=PLL0, 1=PLL1, 2=FCLK, 3=MCLK, 4=BCLK, 5=PCLK */
	pclk = cpu_get_clock_hz(5);
	printk("%s.%d: %8d hz [pclk=%d, src=%3d, scale=%2d][%s] \n",
		I2C_DEV_NAME, i2c->hw.port, (pclk/i2c->hw.clksrc)/(i2c->hw.clkscale),
		pclk, i2c->hw.clksrc, i2c->hw.clkscale, i2c->polling?"polling":"irq");

	if (! i2c->polling) {
		ret = request_irq(i2c->hw.irq, i2c_irq_handler, IRQF_DISABLED, I2C_DEV_NAME, i2c);
		if (ret)
			printk(KERN_ERR "fail, i2c:%d request irq %d ...\n", i2c->hw.port, i2c->hw.irq);
	}

	return ret;
}

static void	set_i2c_io_func(struct i2c_port * i2c)
{
	int	  pio  = i2c->gpio;
	int	  func = i2c->gpio ? NX_GPIO_PADFUNC_GPIO : NX_GPIO_PADFUNC_1;
	u_int scl  = i2c->hw.io_scl;
	u_int sda  = i2c->hw.io_sda;

	DBGOUT("%s(scl:%d, sda:%d, func:%s)\n", __func__, scl, sda, pio?"pio":"i2c");

	/*  set scl pad */
	if (func != soc_gpio_get_io_func(scl)) {
		soc_gpio_set_io_func(scl, func);
		soc_gpio_set_io_pullup(scl, 1);
		if (pio)
			soc_gpio_set_io_dir(scl, 0);	/* input */
	}

	/*  set sda pad */
	if (func != soc_gpio_get_io_func(sda)) {
		soc_gpio_set_io_func(sda, func);
		soc_gpio_set_io_pullup(sda, 1);
		if (pio)
			soc_gpio_set_io_dir(sda, 0);	/* input */
	}
}

/*------------------------------------------------------------------------------
 * 	Hardware I2C
 */
static inline void	set_i2c_clock(struct i2c_port *i2c, bool enable)
{
	int   port	 = i2c->hw.port;
	u_int src    = i2c->hw.clksrc;
	u_int scale  = i2c->hw.clkscale;
	u_int cntmax = i2c->hw.cntmax;

	DBGOUT("%s(i2c:%d, src:%d, scale:%d, cntmax:%d, %s)\n",
		__func__, port, src, scale, cntmax, enable?"on":"off");

#if OLD_IMPLEMENTATION
	if ( enable ||
		NX_PCLKMODE_DYNAMIC == NX_I2C_GetClockPClkMode(port)) {

		NX_I2C_OpenModule(port);
		NX_I2C_SetClockPClkMode(port, NX_PCLKMODE_ALWAYS);
		NX_I2C_SetClockPrescaler(port, src, scale);
		NX_I2C_SetDataDelay(port, cntmax);
		NX_I2C_SetInterruptEnableAll(port, CTRUE);

	} else {

		NX_I2C_SetInterruptEnableAll(port, CFALSE);
		NX_I2C_SetClockPClkMode(port, NX_PCLKMODE_DYNAMIC);
		NX_I2C_BusDisable(port);
	}
#else
    if ( enable || (0 == (i2c->regs->CLKENB & NXP2120_I2C_CLKENB_PCLKMODE))) {
        NX_I2C_OpenModule(port);
        i2c->regs->CLKENB = i2c->regs->CLKENB | NXP2120_I2C_CLKENB_PCLKMODE;
        i2c->regs->ICCR = i2c->regs->ICCR & ~0x6F;
        if (src) {
            i2c->regs->ICCR = i2c->regs->ICCR | NXP2120_ICCR_CLKSRC_PCLK_DIV_256;
        }
		i2c->regs->ICCR = i2c->regs->ICCR | scale;
		i2c->regs->QCNT_MAX = cntmax;
		i2c->regs->ICCR = i2c->regs->ICCR | NXP2120_ICCR_IRQENB;
	} else {
	    i2c->regs->ICCR = i2c->regs->ICCR & ~0x6F;
	    i2c->regs->CLKENB = i2c->regs->CLKENB & ~NXP2120_I2C_CLKENB_PCLKMODE;
	    i2c->regs->ICSR = i2c->regs->ICSR & ~NXP2120_ICSR_TXRX_ENABLE;
	}
#endif
}

static inline void wait_i2c_busy(struct i2c_port *i2c)
{
	int port = i2c->hw.port;
	int wait = 500;

	DBGOUT("%s(i2c:%d)\n", __func__, port);

	if (i2c->nostop)
		return;

#if OLD_IMPLEMENTATION
 	while (wait-- > 0 &&
		CTRUE == NX_I2C_IsBusy(port))
		udelay(1);
#else
 	while (wait-- > 0 && (i2c->regs->ICSR & NXP2120_ICSR_BUSY))
		udelay(1);
#endif

	if (0 > wait)
		printk(KERN_ERR "%s: i2c[%d] is busy ...\n", __func__, wait);
}

static inline bool wait_i2c_ack(struct i2c_port *i2c, bool wait_ack)
{
	int   port = i2c->hw.port;
	int   poll = (irqs_disabled() ? 1 : i2c->polling);
	int   i, tout;
	CBOOL ret = CFALSE;

	DBGOUT("%s(i2c:%d, wait:%s)\n", __func__, port, poll?"polling":"irq");

	/* wait polling */
	if (poll) {
		for (i = 0; (ACK_WAIT_TIMEOUT * 1000) > i; i++) {
#if OLD_IMPLEMENTATION
			ret = NX_I2C_GetInterruptPendingAll(port);
#else
            ret = i2c->regs->PEND & NXP2120_I2C_PEND_PEND ? CTRUE : CFALSE;
#endif
			if (CTRUE == ret || i2c->cond)
				break;
			udelay(1);
		}
	/* wait interrupt */
	} else {
		tout = wait_event_timeout(i2c->wait, i2c->cond, ack_time_out);
	}

	if (i2c->cond)
		ret = CTRUE;

	/* check arbitration */
#if OLD_IMPLEMENTATION
	if (NX_I2C_IsBusArbitFail(port)) {
		printk(KERN_ERR "fail, i2c:%d arbitration\n", port);
		ret = CFALSE;
	}
#else
	if (i2c->regs->ICSR & NXP2120_ICSR_ARBIT_FAIL) {
		printk(KERN_ERR "fail, i2c:%d arbitration\n", port);
		ret = CFALSE;
	}
#endif

	if (CFALSE == ret) {
		ERROUT("fail, i2c:%d wait ack '%s' timeout %dms\n",
			port, poll?"polling":"irq", ACK_WAIT_TIMEOUT);
#if OLD_IMPLEMENTATION
		ERROUT("fail, i2c:%d cond %d, pend %d, arbit %d\n", port, i2c->cond,
			NX_I2C_GetInterruptPendingAll(port), NX_I2C_IsBusArbitFail(port));
#else
		ERROUT("fail, i2c:%d cond %d, pend %d, arbit %d\n", port, i2c->cond,
			i2c->regs->PEND & NXP2120_I2C_PEND_PEND, i2c->regs->ICSR & NXP2120_ICSR_ARBIT_FAIL);
#endif
		goto err_ack;
	}

#if OLD_IMPLEMENTATION
	if (true == wait_ack && CFALSE == NX_I2C_IsACKReceived(port)) {
		ERROUT("fail, i2c:%d receive nack '%s'\n", port, poll?"polling":"irq");
		ret = CFALSE;	/* fail */
	}
#else
	if ((true == wait_ack) && (i2c->regs->ICSR & NXP2120_ICSR_ACK_STATUS)) {
		ERROUT("fail, i2c:%d receive nack '%s'\n", port, poll?"polling":"irq");
		ret = CFALSE;	/* fail */
	}
#endif

err_ack:
	/* clear irq condition */
	i2c->cond = 0;
#if OLD_IMPLEMENTATION
	NX_I2C_ClearInterruptPendingAll(port);
#else
    i2c->regs->PEND = (i2c->regs->PEND & ~0x03) | NXP2120_I2C_PEND_PEND;
#endif
	return ret;
}

static inline bool trans_i2c_start(struct i2c_port *i2c, struct i2c_msg *msg)
{
	int    port = i2c->hw.port;
	u_char addr;
	u_int  mode;
	int temp;
	int *ptr;

	if (msg->flags & I2C_M_TEN) {
		printk(KERN_ERR "fail, i2c:%d not support ten bit addr:0x%02x, flags:0x%x \n",
			port, msg->addr, msg->flags);
		return false;
	}

	addr = (msg->addr << 1);

	if (msg->flags & I2C_M_RD) {
		addr += 1;
		mode  = NX_I2C_TXRXMODE_MASTER_RX;
	} else {
		mode  = NX_I2C_TXRXMODE_MASTER_TX;
	}

	DBGOUT("%s(i2c:%d, addr:0x%02x, %s)\n", __func__, port, addr, msg->flags&I2C_M_RD?"R":"W");

 	/* clear irq condition */
	i2c->cond 		= 0;
	i2c->prev_data  = addr;

#if OLD_IMPLEMENTATION
	NX_I2C_ClearInterruptPendingAll(port);
	NX_I2C_SetAckGenerationEnable(port, CFALSE);

	/* start and trans address byte */
	if(! i2c->nostop)
		NX_I2C_ControlMode(port, NX_I2C_TXRXMODE_SLAVE_RX, NX_I2C_SIGNAL_STOP);

/*
	ptr = (int *)0xF000E000;
	printk("ICCR = %x\n", *ptr);
	ptr++;
	printk("ICSR = %x\n", *ptr);
*/

	NX_I2C_WriteByte(port, addr);
	NX_I2C_ControlMode(port, mode, NX_I2C_SIGNAL_START);

	if(msg->flags & I2C_M_RD)
		NX_I2C_ClearOperationHold(port);
#else
	i2c->regs->PEND = (i2c->regs->PEND & ~0x03) | NXP2120_I2C_PEND_PEND;
	i2c->regs->ICCR = i2c->regs->ICCR & ~NXP2120_ICCR_ACKGEN;

	/* start and trans address byte */
	if(! i2c->nostop)
	    i2c->regs->ICSR = (i2c->regs->ICSR & 0x1F0F) | NXP2120_ICSR_TXRX_ENABLE | NXP2120_ICSR_ST_ENB;

/*
	ptr = (int *)0xF000E000;
	printk("ICCR = %x\n", *ptr);
	ptr++;
	printk("ICSR = %x\n", *ptr);
*/
    i2c->regs->IDSR =  addr;
    //temp = i2c->regs->ICSR & 0x1F0F;

    i2c->regs->ICSR =  (i2c->regs->ICSR & 0x1F0F) | (mode<<6) | NXP2120_ICSR_I2C_START | NXP2120_ICSR_TXRX_ENABLE | NXP2120_ICSR_ST_ENB ;

    if(msg->flags & I2C_M_RD)
        i2c->regs->PEND = (i2c->regs->PEND & ~0x03) | NXP2120_I2C_PEND_OP_HOLD;
#endif

	/* wait ack for address */
	if (! wait_i2c_ack(i2c, true)) {
		ERROUT("fail, i2c:%d start wait ack, addr:0x%02x \n", port, addr);
		return false;
	}
	return true;
}

static bool trans_i2c_data(struct i2c_port *i2c, struct i2c_msg *msg)
{
	int   port = i2c->hw.port;
	int	  len  = msg->len;
	CBOOL ack = CTRUE;
	int	  i;

	DBGOUT("%s(i2c:%d, %s, len:%d)\n", __func__, port, msg->flags&I2C_M_RD?"R":"W", len);

	/* i2c read */
	if (msg->flags & I2C_M_RD) {

		for (i = 0; len > i; i++) {

			if ((i + 1) == len)
				ack = CFALSE;

#if OLD_IMPLEMENTATION
			NX_I2C_SetAckGenerationEnable(port, ack);
			NX_I2C_ClearInterruptPendingAll(port);
 			NX_I2C_ClearOperationHold(port);

 			if (! wait_i2c_ack(i2c, false)) {
 				ERROUT("fail, i2c:%d Read  wait ack, len:%d   \n", port, i);
 				return false;
			}

			/* read byte */
			msg->buf[i] = NX_I2C_ReadByte(port);
#else
            if (CFALSE == ack) {
                i2c->regs->ICCR = i2c->regs->ICCR & ~NXP2120_ICCR_ACKGEN;
            } else {
                i2c->regs->ICCR = i2c->regs->ICCR | NXP2120_ICCR_ACKGEN;
            }
            i2c->regs->PEND = i2c->regs->PEND | NXP2120_I2C_PEND_PEND | NXP2120_I2C_PEND_OP_HOLD;

 			if (! wait_i2c_ack(i2c, false)) {
 				ERROUT("fail, i2c:%d Read  wait ack, len:%d   \n", port, i);
 				return false;
			}

			/* read byte */
			msg->buf[i] = i2c->regs->IDSR;
#endif
			DBGOUT("(R i2c:%d %d=0x%02x)\n", port, i, msg->buf[i]);
		}

	/* i2c write */
	} else {
		for (i = 0; len > i; i++) {

#if OLD_IMPLEMENTATION
			/* make SDA high */
			if (!(i2c->prev_data & 0x80) && (msg->buf[i] & 0x80)) {
				NX_I2C_WriteByte(port, msg->buf[i]);
				udelay(1);
			}

			/* set previos data */
			i2c->prev_data = msg->buf[i];

			/* write byte and trans */
			NX_I2C_SetAckGenerationEnable(port, CFALSE);
			NX_I2C_WriteByte(port, msg->buf[i]);
			NX_I2C_ClearInterruptPendingAll(port);
			NX_I2C_ClearOperationHold(port);
#else
			/* make SDA high */
			if (!(i2c->prev_data & 0x80) && (msg->buf[i] & 0x80)) {
			    i2c->regs->IDSR = msg->buf[i];
				udelay(1);
			}

			/* set previos data */
			i2c->prev_data = msg->buf[i];

			/* write byte and trans */
			i2c->regs->ICCR = i2c->regs->ICCR & ~NXP2120_ICCR_ACKGEN;
			i2c->regs->IDSR = msg->buf[i];
			i2c->regs->PEND = i2c->regs->PEND | NXP2120_I2C_PEND_PEND | NXP2120_I2C_PEND_OP_HOLD;
#endif
			DBGOUT("(W i2c:%d %2d=0x%02x)\n", port, i, msg->buf[i]);

			if (! wait_i2c_ack(i2c, true)) {
				ERROUT("fail, i2c:%d Write wait ack, data[%2d]=0x%02x\n", port, i, msg->buf[i]);
				for (i = 0; len > i; i++)
					ERROUT("w data[%2d]=0x%02x\n", i, msg->buf[i]);
				return false;
			}
		}
		/* make high level */
#if OLD_IMPLEMENTATION
		NX_I2C_WriteByte(port, 0xFF);
#else
        i2c->regs->IDSR = 0xFF;
#endif
	}
	return true;
}

static inline void trans_i2c_stop(struct i2c_port *i2c, struct i2c_msg *msg)
{
	int   port = i2c->hw.port;
	u_int mode = msg->flags & I2C_M_RD ? NX_I2C_TXRXMODE_MASTER_RX : NX_I2C_TXRXMODE_MASTER_TX;

	DBGOUT("%s(i2c:%d, %s, %s)\n",
		__func__, port, msg->flags&I2C_M_RD?"R":"W", msg->flags & I2C_M_NOSTART ?"nostop":"stop");

	if (msg->flags & I2C_M_RD)
		msg->flags &= ~I2C_M_NOSTART;

	i2c->nostop = msg->flags & I2C_M_NOSTART ? true : false;

	/* stop signal */
	if (! i2c->nostop) {
#if OLD_IMPLEMENTATION
		NX_I2C_ControlMode(port, mode, NX_I2C_SIGNAL_STOP);
		NX_I2C_ClearInterruptPendingAll(port);
		NX_I2C_ClearOperationHold(port);
#else
        i2c->regs->ICSR = (i2c->regs->ICSR & 0x1F0F) | (mode << 6) | NXP2120_ICSR_TXRX_ENABLE | NXP2120_ICSR_ST_ENB;
        i2c->regs->PEND = (i2c->regs->PEND & ~0x03) | NXP2120_I2C_PEND_PEND | NXP2120_I2C_PEND_OP_HOLD;
#endif
 	}
}

static int transfer_i2c_hw(struct i2c_port *i2c, struct i2c_msg *msg, int num)
{
	int ret = -EAGAIN;

	DBGOUT("%s(flags:0x%x, %c)\n", __func__, msg->flags, msg->flags&I2C_M_RD?'R':'W');

	/* lock i2c */
	mutex_lock(&i2c->lock);

	set_i2c_io_func(i2c);
	set_i2c_clock(i2c, true);

	wait_i2c_busy(i2c);

	/* transfer */
	if (! trans_i2c_start(i2c, msg))
		goto err_i2c;

	if (! trans_i2c_data(i2c, msg))
		goto err_i2c;

	ret = msg->len;

err_i2c:

	if (ret != msg->len)
		msg->flags &= ~I2C_M_NOSTART;

	trans_i2c_stop(i2c, msg);

	if (ret == msg->len && ! i2c->nostop)
		wait_i2c_busy(i2c);

	if (! i2c->nostop)
		set_i2c_clock(i2c, false);

	/* unlock i2c */
	mutex_unlock(&i2c->lock);

	return ret;
}

/*------------------------------------------------------------------------------
 * 	GPIO I2C
 */
#define SCL_HIGH(_io)		soc_gpio_set_out_value 	(_io, CTRUE )
#define SCL_LOW(_io)		soc_gpio_set_out_value 	(_io, CFALSE)
#define SCL_OUTPUT(_io)		soc_gpio_set_io_dir		(_io, CTRUE )
#define SCL_INPUT(_io)		soc_gpio_set_io_dir		(_io, CFALSE)
#define SCL_DATA(_io)		soc_gpio_get_in_value  	(_io)
#define SDA_HIGH(_io)		soc_gpio_set_out_value 	(_io, CTRUE )
#define SDA_LOW(_io)		soc_gpio_set_out_value 	(_io, CFALSE)
#define SDA_OUTPUT(_io)		soc_gpio_set_io_dir		(_io, CTRUE )
#define SDA_INPUT(_io)		soc_gpio_set_io_dir		(_io, CFALSE)
#define SDA_DATA(_io)		soc_gpio_get_in_value  	(_io)

#define I2C_DELAY(_n)		{ volatile u_int x=0; while ((0x200)*_n > x++); }

#define	SHT		(2)		/* start  hold  time */
#define	EST		(2)		/* Stop   setup time */

#define	DHT		(1)		/* data   hold  time */
#define	DST		(1)		/* data   setup time */
#define	CHT		(1)		/* clock  high  time */

/* 			________		 ___________		______
 *	<SDA>	 		|_______|			|_______|
 *			 <1>|<1>|SHT|DHT|DST|CHT|DHT|DST|EST|<1>
 *			____________		 ___		 __________
 *	<SCL>		 		|_______|	|_______|
 */

/*----------------------------------------------------------------------------*/
static inline void pio_start(struct i2c_hw *hw)
{
	u_int scl = hw->io_scl;
	u_int sda = hw->io_sda;

	/* SCL/SDA High */
	SDA_HIGH	(sda);
	SDA_OUTPUT	(sda);
	I2C_DELAY	(1);

	SCL_HIGH	(scl);
	SCL_OUTPUT	(scl);
	I2C_DELAY	(1);

	/* START signal */
	SDA_LOW		(sda);	/* Start condition */
	I2C_DELAY	(SHT);	/* Start hold */

	SCL_LOW		(scl);
	I2C_DELAY	(DHT);		/* Data  hold */
}

static inline void pio_stop(struct i2c_hw *hw)
{
	u_int scl = hw->io_scl;
	u_int sda = hw->io_sda;

	/* STOP signal */
	SDA_LOW		(sda);
	SDA_OUTPUT	(sda);
	I2C_DELAY	(DST);

	SCL_HIGH	(scl);
	I2C_DELAY	(EST);

	SDA_HIGH	(sda);
	I2C_DELAY	(1);

	SCL_INPUT	(scl);
	SDA_INPUT	(sda);
}

static inline bool pio_putbyte(struct i2c_hw *hw, unsigned char data)
{
	u_int scl = hw->io_scl;
	u_int sda = hw->io_sda;
	int i, nack = 0;

	SDA_OUTPUT	(sda);

	for (i=7 ; i >= 0 ; i--) {
		if (data & (1<<i))
			SDA_HIGH(sda);
		else
			SDA_LOW	(sda);

		I2C_DELAY	(DST);

		SCL_HIGH	(scl);
		I2C_DELAY	(CHT);

		SCL_LOW		(scl);
		I2C_DELAY	(DHT);
	}

	SDA_INPUT	(sda);
	I2C_DELAY	(DST);

	SCL_HIGH	(scl);
	I2C_DELAY	(CHT);

	/* Falling Edge */
#if (0)
	for (i = 0; (ACK_WAIT_TIMEOUT * 1000) > i; i++) {
		nack = SDA_DATA	(sda);
		if (! nack) break;
		udelay(1);
	}
#else
	nack = SDA_DATA(sda);
#endif

	SCL_LOW		(scl);
	I2C_DELAY	(DHT);

	SDA_INPUT	(sda);	/* END */

	return (nack ? false : true);
}

static inline u_char pio_getbyte(struct i2c_hw *hw, bool ack)
{
	u_int  scl = hw->io_scl;
	u_int  sda = hw->io_sda;
	u_char dat = 0;
	int   i;

	SDA_INPUT	(sda);

	for ( i=7; i >= 0; i-- ) {

		I2C_DELAY	(DST);
		SCL_HIGH	(scl);
		I2C_DELAY	(CHT);

		/* Falling Edge */
		if (SDA_DATA(sda))
			dat = (unsigned char)(dat | (1<<i));
		else
			dat = (unsigned char)(dat | (0<<i));

		SCL_LOW		(scl);
		I2C_DELAY	(DHT);
	}

	SDA_OUTPUT(sda);

	if (ack)
		SDA_LOW	(sda);
	else
		SDA_HIGH(sda);

	I2C_DELAY	(DST);

	SCL_HIGH	(scl);
	I2C_DELAY	(CHT);

	SCL_LOW		(scl);
	I2C_DELAY	(DHT);

	SDA_INPUT	(sda);	/* END */

	return dat;
}

static inline bool trans_pio_start(struct i2c_port *i2c, struct i2c_msg *msg)
{
	u_char addr = (msg->addr << 1);

	if (msg->flags & I2C_M_TEN) {
		printk(KERN_ERR "fail, i2c:%d not support ten bit addr:0x%02x, flags:0x%x \n",
			i2c->hw.port, msg->addr, msg->flags);
		return false;
	}

	if (msg->flags & I2C_M_RD)
		addr += 1;

	DBGOUT("%s(i2c:%d, addr:0x%02x, %s)\n", __func__,
		i2c->hw.port, addr, msg->flags&I2C_M_RD?"R":"W");

	pio_start(&i2c->hw);

	if (! pio_putbyte(&i2c->hw, addr)) {
		ERROUT("fail, i2c:%d start wait ack, addr:0x%02x \n", i2c->hw.port, addr);
		return false;
	}
	return true;
}

static bool trans_pio_data(struct i2c_port *i2c, struct i2c_msg *msg)
{
	int	  len  = msg->len;
	CBOOL ack  = CTRUE;
	int	  i;

	DBGOUT("%s(i2c:%d, %s, len:%d)\n",
		__func__, i2c->hw.port, msg->flags&I2C_M_RD?"R":"W", len);

	/* i2c read */
	if (msg->flags & I2C_M_RD) {

		for (i = 0; len > i; i++) {

			if ((i + 1) == len)
				ack = CFALSE;

			/* read byte */
			msg->buf[i] = pio_getbyte(&i2c->hw, ack);
			DBGOUT("(R i2c:%d %d=0x%02x)\n", i2c->hw.port, i, msg->buf[i]);
		}

	/* i2c write */
	} else {
		for (i = 0; len > i; i++) {
			/* write byte */
 			DBGOUT("(W i2c:%d %d=0x%02x)\n", i2c->hw.port, i, msg->buf[i]);
		 	if (! pio_putbyte(&i2c->hw,  msg->buf[i])) {
		 		ERROUT("fail, i2c:%d Write wait ack, data[%2d]=0x%02x\n", i2c->hw.port, i, msg->buf[i]);
		 		for (i = 0; len > i; i++)
		 			ERROUT("w data[%2d]=0x%02x\n", i, msg->buf[i]);
 				return false;
 			}

 		}
	}

	return true;
}

static inline void trans_pio_stop(struct i2c_port *i2c, struct i2c_msg *msg)
{
	bool stop = msg->flags & I2C_M_NOSTART ? false : true;

	DBGOUT("%s(i2c:%d, %s)\n", __func__, i2c->hw.port, msg->flags&I2C_M_RD?"R":"W");
	if (stop)
		pio_stop(&i2c->hw);
}

static int transfer_i2c_pio(struct i2c_port *i2c, struct i2c_msg *msg, int num)
{
	int ret = -EAGAIN;

	DBGOUT("%s(flags:0x%x, %c)\n", __func__, msg->flags, msg->flags&I2C_M_RD?'R':'W');

	/* lock i2c */
	mutex_lock(&i2c->lock);

	/* set i2c io */
	set_i2c_io_func(i2c);

	/* i2c start and trans address */
	if (! trans_pio_start(i2c, msg))
		goto err_i2c;

	/* trans data */
	if (! trans_pio_data(i2c, msg))
		goto err_i2c;

	/* OK */
	ret = msg->len;

err_i2c:
	/* i2c stop */
	trans_pio_stop(i2c, msg);

	/* unlock i2c */
	mutex_unlock(&i2c->lock);

	return ret;
}

/*------------------------------------------------------------------------------
 * 	I2C algorithm
 */
static int nx_i2c_algo_xfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
{
	struct i2c_port *i2c  = adap->algo_data;
	struct i2c_msg  *tmsg = msgs;
	int i   = adap->retries;
	int ret = -EAGAIN;
	int len = 0;
	int  (*transfer_i2c)(struct i2c_port *, struct i2c_msg *, int) = NULL;

	if (i2c->gpio)
		transfer_i2c = transfer_i2c_pio;
	else
		transfer_i2c = transfer_i2c_hw;

	DBGOUT("\n");
	DBGOUT("%s(msg num:%d)\n", __func__, num);

	for ( ; num > 0; num--, tmsg++) {

		len = tmsg->len;

		/* transfer */
		for (i = adap->retries; i > 0; i--) {
			ret = transfer_i2c(i2c, tmsg, num);
			if (ret == len) {
				break;
			}
			ERROUT("i2c:%d (try:%d)\n", i2c->hw.port, adap->retries-i+1);
		}

		/* Error */
		if (ret != len)
			break;
	}

	if (ret != len)
		printk(KERN_ERR "Error: i2c-%d, msg len:%d,%d, try:%d, ret:%d\n",
			i2c->hw.port, ret, len, adap->retries, ret);

	return ret;
}

static u32 nx_i2c_algo_func (struct i2c_adapter *adap)
{
	DBGOUT("%s\n", __func__);
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

static struct i2c_algorithm nx_i2c_algo = {
	.master_xfer 	= nx_i2c_algo_xfer,
	.functionality 	= nx_i2c_algo_func,
};

/*--------------------------------------------------------------------------------
 * I2C platform_driver functions
 ---------------------------------------------------------------------------------*/
static int nx_i2c_driver_probe(struct platform_device *pdev)
{
	struct i2c_port 	 * i2c  = NULL;
	struct i2c_plat_data * plat = pdev->dev.platform_data;
	int ret = 0;

	DBGOUT("%s (device name:%s, id:%d)\n", __func__, pdev->name, pdev->id);

	/*	allocate i2c_port data */
	i2c = kzalloc(sizeof(struct i2c_port), GFP_KERNEL);
	if (!i2c) {
		printk(KERN_ERR "fail, %s allocate driver info ...\n", pdev->name);
		return -ENOMEM;
	}

	/* init i2c data struct */
	ret = init_i2c_func(i2c, plat);
	if (ret)
		goto err_mem;

	/*	init i2c adapter */
	strlcpy(i2c->adap.name, I2C_DEV_NAME, I2C_NAME_SIZE);

	i2c->adap.owner 	= THIS_MODULE;
	i2c->adap.nr 		= i2c->hw.port;
	i2c->adap.class 	= I2C_CLASS_HWMON | I2C_CLASS_SPD;
	i2c->adap.algo 		= &nx_i2c_algo;
	i2c->adap.algo_data = i2c;
	i2c->adap.dev.parent= &pdev->dev;
	i2c->adap.retries 	= I2C_RETRY_COUNT;

	ret = i2c_add_numbered_adapter(&i2c->adap);
	if (ret) {
		printk(KERN_ERR "fail, i2c:%d add to adapter ...\n", i2c->hw.port);
		goto err_irq;
	}

	/* set i2c io */
	set_i2c_io_func(i2c);

	/* set driver data */
	platform_set_drvdata(pdev, i2c);
	return ret;

err_irq:
	if (! i2c->gpio && ! i2c->polling)
		free_irq(i2c->hw.irq, i2c);

err_mem:
	kfree(i2c);
	return ret;
}

static int nx_i2c_driver_remove(struct platform_device *pdev)
{
	struct i2c_port *i2c = platform_get_drvdata(pdev);

	DBGOUT("%s (irq:%d)\n", __func__, i2c->hw.irq);

	if (! i2c->gpio && ! i2c->polling)
		free_irq(i2c->hw.irq, i2c);

	i2c_del_adapter(&i2c->adap);
	kfree(i2c);

	platform_set_drvdata(pdev, NULL);
	return 0;
}

static struct platform_driver i2c_plat_driver = {
	.probe		= nx_i2c_driver_probe,
	.remove		= nx_i2c_driver_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= I2C_DEV_NAME,
	},
};

static int __init nx_i2c_driver_init(void)
{
	DBGOUT("%s\n", __func__);
	return platform_driver_register(&i2c_plat_driver);
}

static void __exit nx_i2c_driver_exit(void)
{
	DBGOUT("%s\n", __func__);
	platform_driver_unregister(&i2c_plat_driver);
}

module_init(nx_i2c_driver_init);
module_exit(nx_i2c_driver_exit);

MODULE_AUTHOR("jhkim <jhkim@nexell.co.kr>");
MODULE_DESCRIPTION("I2C driver for the Nexell");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:nx-i2c");

