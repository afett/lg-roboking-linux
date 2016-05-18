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

#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/init.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>

#include <asm/mach/irq.h>

/* nexell soc headers */
#include <mach/platform.h>

#if (0)
#define IRQ_DBGOUT(msg...)		{ printk(KERN_INFO "irq: " msg); }
#else
#define IRQ_DBGOUT(msg...)		do {} while (0)
#endif	/* irq debug */

#if (0)
#define PIO_DBGOUT(msg...)		{ printk(KERN_INFO "pio: " msg); }
#else
#define PIO_DBGOUT(msg...)		do {} while (0)
#endif 	/* gpio irq debug */

#if (0)
#define DMA_DBGOUT(msg...)		{ printk(KERN_INFO "dma: " msg); }
#else
#define DMA_DBGOUT(msg...)		do {} while (0)
#endif 	/* dma irq debug */

#if (0)
#define ALV_DBGOUT(msg...)		{ printk(KERN_INFO "alv: " msg); }
#else
#define ALV_DBGOUT(msg...)		do {} while (0)
#endif 	/* Alive irq debug */


/* local */
static void __init gpio_init_irq(void);
static void __init dma_init_irq(void);
static void __init alive_init_irq(void);

/*----------------------------------------------------------------------------
 *  irq handler
 *  start  -> enable
 *  do IRQ -> mask_ack -> handler -> unmask->
 *  do IRQ -> mask_ack -> handler -> unmask-> ...
 *  end    -> disable
 ----------------------------------------------------------------------------*/
#define	REG_INTBASE	IO_ADDRESS(PHY_BASEADDR_INTC_MODULE)

#define REG_INTMODE_0		0x08	/* Interrupt Mode Register */
#define REG_INTMODE_1		0x0C 	/* Interrupt Mode Register */
#define REG_INTMASK_0		0x10	/* Interrupt Mask Register */
#define REG_INTMASK_1		0x14 	/* Interrupt Mask Register */
#define REG_PRIORDER		0x18 	/* Priority Order Register */
#define REG_INTPEND_0		0x20	/* Interrupt Pending Register */
#define REG_INTPEND_1		0x24 	/* Interrupt Pending Register */

inline static void	soc_int_set_enable(int num)
{
	volatile u_int *reg = (u_int*)(REG_INTBASE + REG_INTMASK_0);
	 reg += num >> 5;
	*reg &= ~(1 << (num & 0x1f));
}

inline static void	soc_int_set_disable(int num)
{
	volatile u_int *reg = (u_int*)(REG_INTBASE + REG_INTMASK_0);
	 reg += num >> 5;
	*reg |= (1 << (num & 0x1f));
}

inline static void	soc_int_clr_pend(int num)
{
	volatile u_int *reg = (u_int*)(REG_INTBASE + REG_INTPEND_0);
	 reg += num >> 5;
	*reg  = 1UL << (num & 0x1f);
}

inline static int	soc_int_get_pend(int num)
{
	volatile u_int *reg = (u_int*)(REG_INTBASE + REG_INTPEND_0);
	reg += num >> 5;
	return (*reg >> num) & 1UL;
}

/*------------------------------------------------------------------------*/
static void cpu_irq_enable(unsigned int irqno)
{
	IRQ_DBGOUT("enable(%d)\n", irqno);
#if (1)
	NX_INTC_ClearInterruptPending(irqno);
	NX_INTC_SetInterruptEnable(irqno, CTRUE);
	NX_INTC_GetInterruptPending(irqno);
#else
	soc_int_clr_pend(irqno);	 	/* intc : clear irq pend */
	soc_int_set_enable(irqno);		/* intc : enable irq */
	soc_int_get_pend(irqno);		/* Guarantee that irq Pending is cleared */
#endif
}

static void cpu_irq_disable(unsigned int irqno)
{
	IRQ_DBGOUT("disable(%d)\n", irqno);
#if (1)
	NX_INTC_ClearInterruptPending(irqno);
	NX_INTC_SetInterruptEnable(irqno, CFALSE);
	NX_INTC_GetInterruptPending(irqno);
#else
	soc_int_clr_pend(irqno);	 	/* intc : clear irq pend */
	soc_int_set_disable(irqno);		/* intc : disable irq */
	soc_int_get_pend(irqno);		/* Guarantee that irq Pending is cleared */
#endif
}

/* disable irq: set mask bit & clear irq pend bit */
static void cpu_irq_mask_ack(unsigned int irqno)
{
	IRQ_DBGOUT("mask_ack(%d)\n", irqno);
#if (1)
	NX_INTC_ClearInterruptPending(irqno);
	NX_INTC_SetInterruptEnable(irqno, CFALSE);
	NX_INTC_GetInterruptPending(irqno);
#else
	soc_int_clr_pend(irqno);		/* intc : clear irq pend */
	soc_int_set_disable(irqno);		/* intc : disable irq */
	soc_int_get_pend(irqno);		/* Guarantee that irq Pending is cleared */
#endif
}

/* enable irq: set unmask bit & clear irq pend bit */
static void cpu_irq_unmask(unsigned int irqno)
{
	IRQ_DBGOUT("unmask(%d)\n", irqno);
#if (1)
	NX_INTC_ClearInterruptPending(irqno);
	NX_INTC_SetInterruptEnable(irqno, CTRUE);
	NX_INTC_GetInterruptPending(irqno);
#else
	soc_int_clr_pend(irqno);		/* intc : clear irq pend */
	soc_int_set_enable(irqno);		/* intc : enable irq */
	soc_int_get_pend(irqno);		/* Guarantee that irq Pending is cleared */
#endif
}

/* interrupt controller irq control */
static struct irq_chip cpu_irq_chip = {
	.name		= "IRQ",
	.enable		= cpu_irq_enable,
	.disable	= cpu_irq_disable,
	.mask_ack	= cpu_irq_mask_ack,
	.unmask		= cpu_irq_unmask,
};

void __init cpu_init_irq(void)
{
	int i = 0;
	IRQ_DBGOUT("%s\n", __func__);

	/* physical interrupt irq */
	for (i = 0; IRQ_PHY_MAX_COUNT > i; i++) {
		switch(i) {
		case IRQ_PHY_GPIO:	gpio_init_irq();
			break;
		case IRQ_PHY_DMA:	dma_init_irq();
			break;
		case IRQ_PHY_ALIVE: alive_init_irq();
			break;
		default:
			set_irq_chip(i, &cpu_irq_chip);
			set_irq_handler(i, handle_level_irq);
			set_irq_flags(i, IRQF_VALID);
			break;
		}
	}
}

/*----------------------------------------------------------------------------
 *  GPIO irq chain handler
 *  start  -> enable
 *  do IRQ -> gpio handler -> mask_ack -> handler -> unmask->
 *  do IRQ -> gpio handler -> mask_ack -> handler -> unmask->  ...
 *  end    -> disable
 ----------------------------------------------------------------------------*/
static void gpio_irq_enable(unsigned int irqno)
{
	int irq = IRQ_PHY_GPIO;
	int mod = (irqno - IRQ_GPIO_START) >> 5;
	int bit = (irqno - IRQ_GPIO_START) & 0x1F;

	PIO_DBGOUT("enable(irq:%d, grp:%d, bit:%d)\n", irqno, mod, bit);

	NX_GPIO_ClearInterruptPending(mod, bit);		/* ack   : gpio clear pend */
	NX_INTC_ClearInterruptPending(irq);				/* ack   : intc clear irq pend */

	NX_GPIO_SetInterruptEnable(mod, bit, CTRUE);	/* unmask: gpio enable irq */

	NX_INTC_GetInterruptPending(irq);				/* Guarantee that irq Pending is cleared */
}

static void gpio_irq_disable(unsigned int irqno)
{
	int irq = IRQ_PHY_GPIO;
	int mod = (irqno - IRQ_GPIO_START) >> 5;
	int bit = (irqno - IRQ_GPIO_START) & 0x1F;

	PIO_DBGOUT("disable(irq:%d, grp:%d, bit:%d)\n", irqno, mod, bit);

	NX_GPIO_ClearInterruptPending(mod, bit);		/* ack   : gpio clear pend */
	NX_INTC_ClearInterruptPending(irq);				/* ack : intc clear irq pend */

	NX_GPIO_SetInterruptEnable(mod, bit, CFALSE);	/* mask: gpio disalbe irq */

	NX_INTC_GetInterruptPending(irq);				/* Guarantee that irq Pending is cleared */
}

/* disable irq: set mask bit & clear irq pend bit */
static void gpio_irq_mask_ack(unsigned int irqno)
{
	int irq = IRQ_PHY_GPIO;
	int mod = (irqno - IRQ_GPIO_START) >> 5;
	int bit = (irqno - IRQ_GPIO_START) & 0x1F;

	PIO_DBGOUT("mask_ack(irq:%d, grp:%d, bit:%d)\n", irqno, mod, bit);

	NX_GPIO_ClearInterruptPending(mod, bit);		/* clear irq pend */
	NX_INTC_ClearInterruptPending(irq);				/* ack : intc clear irq pend */

	NX_GPIO_SetInterruptEnable(mod, bit, CFALSE);	/* mask: gpio enable irq */

	NX_INTC_GetInterruptPending(irq);				/* check : Guarantee that irq Pending is cleared */
}

/* enable irq: set unmask bit & clear irq pend bit */
static void gpio_irq_unmask(unsigned int irqno)
{
	int irq = IRQ_PHY_GPIO;
	int mod = (irqno - IRQ_GPIO_START) >> 5;
	int bit = (irqno - IRQ_GPIO_START) & 0x1F;

	PIO_DBGOUT("unmask(irq:%d, grp:%d, bit:%d)\n", irqno, mod, bit);

	NX_GPIO_ClearInterruptPending(mod, bit);		/* clear irq pend */
	NX_INTC_ClearInterruptPending(irq);				/* ack : intc clear irq pend */

	NX_GPIO_SetInterruptEnable(mod, bit, CTRUE);	/* unmask: gpio enable irq */

	NX_INTC_GetInterruptPending(irq);				/* check : Guarantee that irq Pending is cleared */
}

/* ksw:
    GPIO mode settype is essential for HARD_IRQ/SOFT_IRQ of RT tasking.
    Because Default LOW LEVEL IRQ dispatches IRQ relentlessly hence shutdown the system.
    We should have mechanism for setting level High/Edge IRQ, so I implement set_type here.
*/
#define NXP2120_IRQ_LOW_LEVEL      0
#define NXP2120_IRQ_HIGH_LEVEL     1
#define NXP2120_IRQ_FALLING_EDGE   2
#define NXP2120_IRQ_RISING_EDGE    3
static int gpio_irq_set_type(unsigned int irq, unsigned int type) 
{
	int  mod = 0, bit = 0;
	u32 newvalue = 0;

	mod = (irq - IRQ_GPIO_START) >> 5;
	bit = (irq - IRQ_GPIO_START) & 0x1F;
	
	PIO_DBGOUT("IRQ set type : handle irq %x, type %x - mod %d bit %d\n", irq, type, mod, bit);	

	switch (type) {
	case IRQ_TYPE_NONE:
		printk(KERN_WARNING "No edge setting!\n");
		break;

	case IRQ_TYPE_EDGE_RISING:
		newvalue = NXP2120_IRQ_RISING_EDGE;
		break;

	case IRQ_TYPE_EDGE_FALLING:
		newvalue = NXP2120_IRQ_FALLING_EDGE;
		break;

	/*case IRQ_TYPE_EDGE_BOTH: // We can't handle both edge 
		newvalue = NXP2120_IRQ_BOTHEDGE;
		break; */

	case IRQ_TYPE_LEVEL_LOW:
		newvalue = NXP2120_IRQ_LOW_LEVEL;
		break;

	case IRQ_TYPE_LEVEL_HIGH:
		newvalue = NXP2120_IRQ_HIGH_LEVEL;
		break;

	default:
		printk(KERN_ERR "No such irq type %d", type);
		return -1;
	}

	/* We should set GPIO pins as is */
	NX_GPIO_SetInterruptMode(mod, bit, newvalue);
	/* Should I clear the pending bits or not? */

	return 0;
}

static struct irq_chip gpio_irq_chip = {
	.name		= "Gpio IRQ",
	.enable		= gpio_irq_enable,
	.disable	= gpio_irq_disable,
	.mask_ack	= gpio_irq_mask_ack,
	.unmask		= gpio_irq_unmask,
	.set_type	= gpio_irq_set_type,
};

static void gpio_irq_handler(unsigned int irqno, struct irq_desc *desc)
{
	int  mod = 0, bit = 0, irqnum = 0;
	int  ret = -1;
	uint pnd =  0;

	struct irq_desc *irqdesc;
	unsigned long flags  = 0;
	unsigned long detmod = 0;

	PIO_DBGOUT("handle(%d)\n", irqno);

	for (mod = 0; NX_GPIO_GetNumberOfModule() > mod; mod++)	{

		bit = NX_GPIO_GetInterruptPendingNumber(mod);		/* masked pend bit */
		pnd = *(U32*)(NX_GPIO_GetBaseAddress(mod) + 0x14);	/* pend bit */

		if (-1 == bit && ! pnd)
			continue;

		if (-1 == bit && pnd) {
			for(bit = 0 ; 32 > bit ; bit++ )
				if(0 != (pnd & (1UL<<bit)))
					break;
		}

		irqnum  = (bit + IRQ_GPIO_START + (mod<<5));
		irqdesc = irq_desc + irqnum;

		if (irqdesc && irqdesc->action)	{

			flags  = irqdesc->action->flags;
			detmod = NX_GPIO_GetInterruptMode(mod, bit);

			/* disable shared irq when level detect */
			if (!(detmod & 0x2))
				irqdesc->action->flags |= IRQF_DISABLED;

			desc_handle_irq(irqnum, irqdesc);

			irqdesc->action->flags = flags;
		} else {
			printk(KERN_ERR "Fail, not registered gpio irq:%d, mod:%d, bit:%d \n",
				irqno, mod, bit);
		}

		/* success & exit */
		ret = 0;
		break;
	}

	PIO_DBGOUT("handle(%d-%d), ret(%d)\n", irqno, irqnum, ret);

	if (-1 == ret) {
		printk(KERN_ERR "Unknown pio irq(A:0x%x-0x%x, B:0x%x-0x%x, C:0x%x-0x%x)\n",
			*(U32*)(NX_GPIO_GetBaseAddress(0) + 0x14), NX_GPIO_GetInterruptEnable32(0),
			*(U32*)(NX_GPIO_GetBaseAddress(1) + 0x14), NX_GPIO_GetInterruptEnable32(1),
			*(U32*)(NX_GPIO_GetBaseAddress(2) + 0x14), NX_GPIO_GetInterruptEnable32(2));

		for (mod = 0; NX_GPIO_GetNumberOfModule() > mod; mod++)
			NX_GPIO_ClearInterruptPendingAll(mod);

		NX_INTC_GetInterruptPending(IRQ_PHY_GPIO);			/* check : Guarantee that irq Pending is cleared */
	}
}

static void __init gpio_init_irq(void)
{
	int i = 0, irq = IRQ_PHY_GPIO;;

	/* regist physical gpio irq handler, shared irq handler*/
	set_irq_chained_handler(irq, gpio_irq_handler);

	/* regist virtual gpio irq info */
	for (i = IRQ_GPIO_START; IRQ_GPIO_END > i; i++) {
		set_irq_chip(i, &gpio_irq_chip);
		set_irq_handler(i, handle_level_irq);
		set_irq_flags(i, IRQF_VALID);
	}

	NX_INTC_ClearInterruptPending(irq);
	NX_INTC_SetInterruptEnable(irq, CTRUE);
}


/*----------------------------------------------------------------------------
 *  DMA irq chain handler
 *  start  -> enable
 *  do IRQ -> dma handler -> mask_ack -> handler -> unmask->
 *  do IRQ -> dma handler -> mask_ack -> handler -> unmask->  ...
 *  end    -> disable
 ----------------------------------------------------------------------------*/
static void dma_irq_enable(unsigned int irqno)
{
	int irq = IRQ_PHY_DMA;
	int ch  = (irqno - IRQ_DMA_START);

	DMA_DBGOUT("enable(irq:%d, ch:%d)\n", irqno, ch);

	NX_DMA_ClearInterruptPendingAll(ch);		/* ack   : dma clear irq pend */
	NX_INTC_ClearInterruptPending(irq);			/* ack   : intc clear irq pend */

	NX_DMA_SetInterruptEnableAll(ch, CTRUE);	/* unmask: enable irq */

	NX_INTC_GetInterruptPending(irq);			/* Guarantee that irq Pending is cleared */
}

static void dma_irq_disable(unsigned int irqno)
{
	int irq = IRQ_PHY_DMA;
	int ch  = (irqno - IRQ_DMA_START);

	DMA_DBGOUT("disable(irq:%d, ch:%d)\n", irqno, ch);

	NX_DMA_ClearInterruptPendingAll(ch);		/* ack  : dma clear irq pend */
	NX_INTC_ClearInterruptPending(irq);			/* ack  : intc clear irq pend */

	NX_DMA_SetInterruptEnableAll(ch, CFALSE);	/* mask : dma disable irq */

	NX_INTC_GetInterruptPending(irq);			/* Guarantee that irq Pending is cleared */
}

/* disable irq: set mask bit & clear irq pend bit */
static void dma_irq_mask_ack(unsigned int irqno)
{
	int irq = IRQ_PHY_DMA;
	int ch  = (irqno - IRQ_DMA_START);

	DMA_DBGOUT("mask_ack(irq:%d, ch:%d)\n", irqno, ch);

	NX_DMA_ClearInterruptPendingAll(ch);		/* clear irq pend */
	NX_INTC_ClearInterruptPending(irq);			/* intc : clear irq pend */

	NX_DMA_SetInterruptEnableAll(ch, CFALSE);	/* mask: dma disable irq */

	NX_INTC_GetInterruptPending(irq);			/* Guarantee that irq Pending is cleared */
}

/* enable irq: set unmask bit & clear irq pend bit */
static void dma_irq_unmask(unsigned int irqno)
{
	int irq = IRQ_PHY_DMA;
	int ch  = (irqno - IRQ_DMA_START);

	DMA_DBGOUT("unmask(irq:%d, ch:%d)\n", irqno, ch);

	NX_DMA_ClearInterruptPendingAll(ch);		/* clear irq pend */
	NX_INTC_ClearInterruptPending(irq);			/* intc : clear irq pend */

	NX_DMA_SetInterruptEnableAll(ch, CTRUE);	/* unmask: dma enable irq */

	NX_INTC_GetInterruptPending(irq);			/* Guarantee that irq Pending is cleared */
}

static struct irq_chip dma_irq_chip = {
	.name		= "DMA IRQ",
	.enable		= dma_irq_enable,
	.disable	= dma_irq_disable,
	.mask_ack	= dma_irq_mask_ack,
	.unmask		= dma_irq_unmask,
};

static void dma_irq_handler(unsigned int irqno, struct irq_desc *desc)
{
	int  ch = 0, irqnum = 0;
	int  ret = -1;

	struct irq_desc *irqdesc;

	for (ch = 0; NX_DMA_GetNumberOfModule() > ch; ch++) {

		if (NX_DMA_GetInterruptPendingAll(ch)) {

			irqnum  = (ch + IRQ_DMA_START);
			irqdesc = irq_desc + irqnum;

			if (irqdesc && irqdesc->action)
				desc_handle_irq(irqnum, irqdesc);
			else
				printk(KERN_ERR "Error, unknow dma irq(%d) \n", irqno);

			/* success & exit */
			ret = 0;
			break;
		}
	}

	DMA_DBGOUT("handle(%d-%d), ret(%d)\n", irqno, irqnum, ret);

	if (-1 == ret) {
		printk(KERN_ERR "Unknown dma irq(%d)\r\n", irqno);

		for (ch = 0; NX_DMA_GetNumberOfModule() > ch; ch++)
			NX_DMA_ClearInterruptPendingAll(ch);

		NX_INTC_ClearInterruptPending(IRQ_PHY_DMA);		/* ack   : intc clear irq pend */
		NX_INTC_GetInterruptPending(IRQ_PHY_DMA);		/* Guarantee that irq Pending is cleared */
	}
}

static void __init dma_init_irq(void)
{
	int i = 0, irq = IRQ_PHY_DMA;

	/* regist physical dma irq handler, shared irq handler*/
	set_irq_chained_handler(irq, dma_irq_handler);

	/* regist virtual dma irq info */
	for (i = IRQ_DMA_START; IRQ_DMA_END > i; i++) {
		set_irq_chip(i, &dma_irq_chip);
		set_irq_handler(i, handle_level_irq);
		set_irq_flags(i, IRQF_VALID);
	}

	NX_INTC_ClearInterruptPending(irq);
	NX_INTC_SetInterruptEnable(irq, CTRUE);
}

/*----------------------------------------------------------------------------
 *  ALIVE irq chain handler
 *  start  -> enable
 *  do IRQ -> alive handler -> mask_ack -> handler -> unmask->
 *  do IRQ -> alive handler -> mask_ack -> handler -> unmask->  ...
 *  end    -> disable
 ----------------------------------------------------------------------------*/
static void alive_irq_enable(unsigned int irqno)
{
	int irq = IRQ_PHY_ALIVE;
	int bit = (irqno - IRQ_ALIVE_START);

	ALV_DBGOUT("enable(irq:%d, ch:%d)\n", irqno, bit);

	NX_ALIVE_ClearInterruptPending(bit);		/* ack   : alive clear irq pend */
	NX_INTC_ClearInterruptPending(irq);			/* ack   : intc clear irq pend */

	NX_ALIVE_SetInterruptEnable(bit, CTRUE);	/* unmask: enable irq */

	NX_INTC_GetInterruptPending(irq);			/* Guarantee that irq Pending is cleared */
}

static void alive_irq_disable(unsigned int irqno)
{
	int bit = (irqno - IRQ_ALIVE_START);
	int irq = IRQ_PHY_ALIVE;

	ALV_DBGOUT("disable(irq:%d, ch:%d)\n", irqno, bit);

	NX_ALIVE_ClearInterruptPending(bit);		/* ack  : alive clear irq pend */
	NX_INTC_ClearInterruptPending(irq);			/* ack  : intc clear irq pend */

	NX_ALIVE_SetInterruptEnable(bit, CFALSE);	/* mask : alive disable irq */

	NX_INTC_GetInterruptPending(irq);			/* Guarantee that irq Pending is cleared */
}

/* disable irq: set mask bit & clear irq pend bit */
static void alive_irq_mask_ack(unsigned int irqno)
{
	int irq = IRQ_PHY_ALIVE;
	int bit = (irqno - IRQ_ALIVE_START);

	ALV_DBGOUT("mask_ack(irq:%d, ch:%d)\n", irqno, bit);

	NX_ALIVE_ClearInterruptPending(bit);		/* clear irq pend */
	NX_INTC_ClearInterruptPending(irq);			/* intc : clear irq pend */

	NX_ALIVE_SetInterruptEnable(bit, CFALSE);	/* mask: alive disable irq */

	NX_INTC_GetInterruptPending(irq);			/* Guarantee that irq Pending is cleared */
}

/* enable irq: set unmask bit & clear irq pend bit */
static void alive_irq_unmask(unsigned int irqno)
{
	int irq = IRQ_PHY_ALIVE;
	int bit = (irqno - IRQ_ALIVE_START);

	ALV_DBGOUT("unmask(irq:%d, ch:%d)\n", irqno, bit);

	NX_ALIVE_ClearInterruptPending(bit);		/* clear irq pend */
	NX_INTC_ClearInterruptPending(irq);			/* intc : clear irq pend */

	NX_ALIVE_SetInterruptEnable(bit, CTRUE);	/* unmask: alive enable irq */

	NX_INTC_GetInterruptPending(irq);			/* Guarantee that irq Pending is cleared */
}

static struct irq_chip alive_irq_chip = {
	.name		= "Alive IRQ",
	.enable		= alive_irq_enable,
	.disable	= alive_irq_disable,
	.mask_ack	= alive_irq_mask_ack,
	.unmask		= alive_irq_unmask,
};

static void alive_irq_handler(unsigned int irqno, struct irq_desc *desc)
{
	int  bit = 0, irqnum = 0;
	int  ret = -1;

	struct irq_desc *irqdesc;

	bit = NX_ALIVE_GetInterruptPendingNumber();

	if (bit != -1) {

		irqnum  = (bit + IRQ_ALIVE_START);
		irqdesc = irq_desc + irqnum;

		if (irqdesc && irqdesc->action)
			desc_handle_irq(irqnum, irqdesc);
		else
			printk(KERN_ERR "Error, unknow alive irq(%d) \n", irqnum);

		/* success */
		ret = 0;
	}

	ALV_DBGOUT("handle(%d-%d), ret(%d)\n", irqno, irqnum, ret);

	if (-1 == ret) {
		printk(KERN_ERR "Unknown alive irq(%d)\r\n", irqno);
		NX_INTC_GetInterruptPending(IRQ_PHY_ALIVE);			/* check : Guarantee that irq Pending is cleared */
	}
}

static void __init alive_init_irq(void)
{
	int i = 0, irq = IRQ_PHY_ALIVE;

	/* regist physical alive irq handler, shared irq handler*/
	set_irq_chained_handler(irq, alive_irq_handler);

	/* regist virtual alive irq info */
	for (i = IRQ_ALIVE_START; IRQ_ALIVE_END > i; i++) {
		set_irq_chip	(i, &alive_irq_chip);
		set_irq_handler	(i, handle_level_irq);
		set_irq_flags	(i, IRQF_VALID);
	}

	NX_INTC_ClearInterruptPending(irq);
	NX_INTC_SetInterruptEnable(irq, CTRUE);
}
