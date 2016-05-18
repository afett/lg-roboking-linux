/*
 * nexell-spdif.c  --  ALSA Soc Audio Layer
 *
 * Copyright(c) 2011 STcube Inc. Seungwoo Kim <ksw@stcube.com>
 * Based on wolfson micro driver, modified for nxp2120 soc spdif rx port.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/clk.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/initval.h>
#include <sound/soc.h>

/* Nexell headers */
#include <mach/platform.h>

#include "nexell-pcm.h"
#include "nexell-spdif.h"

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "spdif: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#define NXP2120_SPDIF_CTRL_LOCK                (1 << 10)
#define NXP2120_SPDIF_CTRL_CLR_FIFO            (1 << 9)
#define NXP2120_SPDIF_CTRL_EN_PHA_DET          (1 << 8)
#define NXP2120_SPDIF_CTRL_SAMPLE_OFFSET_MASK  (0x0F << 4)
#define NXP2120_SPDIF_CTRL_SAMPLE_OFFSET_0     (8 << 4)
#define NXP2120_SPDIF_CTRL_SAMPLE_OFFSET_1     (7 << 4)
#define NXP2120_SPDIF_CTRL_SAMPLE_OFFSET_2     (6 << 4)
#define NXP2120_SPDIF_CTRL_SAMPLE_OFFSET_3     (5 << 4)
#define NXP2120_SPDIF_CTRL_SAMPLE_OFFSET_4     (4 << 4)
#define NXP2120_SPDIF_CTRL_SAMPLE_OFFSET_5     (3 << 4)
#define NXP2120_SPDIF_CTRL_SAMPLE_OFFSET_6     (2 << 4)
#define NXP2120_SPDIF_CTRL_SAMPLE_OFFSET_7     (1 << 4)
#define NXP2120_SPDIF_CTRL_SAMPLE_OFFSET_8     (0 << 4)
#define NXP2120_SPDIF_CTRL_EN_CAP_USER_STAT    (1 << 3)
#define NXP2120_SPDIF_CTRL_EN_CAP_DMA_DAT_ONLY (1 << 2)
#define NXP2120_SPDIF_CTRL_EN_CAP_DMA_SWAP     (1 << 1)
#define NXP2120_SPDIF_CTRL_EN_DECODE           (1 << 0)

#define NXP2120_SPDIF_PEND_LOCK                (1 << 7)
#define NXP2120_SPDIF_PEND_ERR                 (1 << 6)
#define NXP2120_SPDIF_PEND_PARITY              (1 << 5)
#define NXP2120_SPDIF_PEND_BLOCK               (1 << 4)
#define NXP2120_SPDIF_PEND_EN_IRQ_LOCK         (1 << 3)
#define NXP2120_SPDIF_PEND_EN_IRQ_ERR          (1 << 2)
#define NXP2120_SPDIF_PEND_EN_IRQ_PARITY       (1 << 1)
#define NXP2120_SPDIF_PEND_EN_IRQ_BLOCK        (1 << 0)

struct nx_spdif_struc {
    struct NX_SPDIF_RegisterSet *regs;
    int debug_mask;
};
static struct nx_spdif_struc nx_spdif;

static void init_nx_spdif(void)
{
    /* struct	NX_SPDIF_RegisterSet *reg = nx_spdif.regs; */
	DBGOUT("%s\n", __func__);

	// What would we do...
}

static void exit_nx_spdif(void)
{
    /* struct	NX_SPDIF_RegisterSet *reg = nx_spdif.regs; */
	DBGOUT("%s\n", __func__);

	// Do something....
}

static irqreturn_t nx_spdif_irq(int irq, void *dev_id)
{
    /* struct nx_spdif_struc *nx_spdif = (struct nx_spdif_struc *)dev_id; */
    
    return IRQ_NONE;
}

static int nxp2120_spdif_probe(struct platform_device *pdev,
                  struct snd_soc_dai *dai)
{
    int ret;

    nx_spdif.regs = (struct NX_SPDIF_RegisterSet *)IO_ADDRESS(PHY_BASEADDR_SPDIF);
    nx_spdif.debug_mask = 0 ; // No debug 0x1D;

    init_nx_spdif();

    nx_spdif.regs->SPDIF_PEND = nx_spdif.debug_mask;
	nx_spdif.regs->SPDIF_PEND = 0xF0 | nx_spdif.regs->SPDIF_PEND;
	
    ret = request_irq(IRQ_PHY_SPDIF, nx_spdif_irq, IRQF_DISABLED, "SPDIF", (void *)&nx_spdif);
    if (ret < 0) {
        printk(KERN_ERR "nxp2120-spdif: interrupt request failed.\n");
    }
    return ret;
}

static void nxp2120_spdif_remove(struct platform_device *pdev,
                struct snd_soc_dai *dai)
{
    free_irq(IRQ_PHY_SPDIF, NULL);
}

static int start_nx_spdif(struct snd_pcm_substream *substream)
{
    /* struct	NX_SPDIF_RegisterSet *reg = nx_spdif.regs; */
	int ret = 0;

	DBGOUT("%s (%s)\n", __func__,
		SNDRV_PCM_STREAM_PLAYBACK == substream->stream ? "PLAY":"CAPT");

	// do something

	return ret;
}

static void stop_nx_spdif(struct snd_pcm_substream *substream)
{
    /* struct	NX_SPDIF_RegisterSet *reg = nx_spdif.regs; */
	DBGOUT("%s (%s)\n", __func__,
		SNDRV_PCM_STREAM_PLAYBACK == substream->stream ? "PLAY":"CAPT");

	// Do something
}

/*--------------------------------------------------------------------------------
 * sound spdif dai ops
 ---------------------------------------------------------------------------------*/
static int  nxp2120_spdif_startup(struct snd_pcm_substream *substream,	struct snd_soc_dai *dai)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static void nxp2120_spdif_shutdown(struct snd_pcm_substream *substream,struct snd_soc_dai *dai)
{
	DBGOUT("%s\n", __func__);
}

static int nxp2120_spdif_hw_params(struct snd_pcm_substream *substream,
                  struct snd_pcm_hw_params *params,
                  struct snd_soc_dai *dai)
{
	DBGOUT("%s\n", __func__);

    return 0;
}

static int nxp2120_spdif_trigger(struct snd_pcm_substream *substream, int cmd,
                struct snd_soc_dai *dai)
{
    switch (cmd) {
    case SNDRV_PCM_TRIGGER_START:
    case SNDRV_PCM_TRIGGER_RESUME:
    case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
        start_nx_spdif(substream);
        break;
    case SNDRV_PCM_TRIGGER_STOP:
    case SNDRV_PCM_TRIGGER_SUSPEND:
    case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
        stop_nx_spdif(substream);
        break;
    }

    return 0;
}

static int nxp2120_spdif_digital_mute(struct snd_soc_dai *dai, int mute)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static struct snd_soc_dai_ops nx_spdif_dai_ops = {
	.startup		= nxp2120_spdif_startup,
	.shutdown		= nxp2120_spdif_shutdown,
	.trigger		= nxp2120_spdif_trigger,
	.hw_params		= nxp2120_spdif_hw_params,
	.digital_mute 	= nxp2120_spdif_digital_mute,
};
        
#define NXP2120_SPDIF_RATES (SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_44100)

struct snd_soc_dai nxp2120_spdif_dai = {
    .name = "nxp2120-spdif",
    .id = 0,
    .ac97_control = 0,
    .probe = nxp2120_spdif_probe,
    .remove = nxp2120_spdif_remove,
    .capture = {
        .stream_name = "SPDIF Capture",
        .channels_min = 2,
        .channels_max = 2,
        .rates = NXP2120_SPDIF_RATES,
        .formats = SNDRV_PCM_FMTBIT_S24_LE,}, // 24bit only?...
    .ops = &nx_spdif_dai_ops,
};

EXPORT_SYMBOL_GPL(nxp2120_spdif_dai);
EXPORT_SYMBOL_GPL(nx_spdif_dai_ops);

static int __init nxp2120_spdif_init(void)
{
    return snd_soc_register_dai(&nxp2120_spdif_dai);
}
module_init(nxp2120_spdif_init);

static void __exit nxp2120_spdif_exit(void)
{
    exit_nx_spdif();
    
    snd_soc_unregister_dai(&nxp2120_spdif_dai);
}
module_exit(nxp2120_spdif_exit);

MODULE_AUTHOR("Seungwoo Kim");
MODULE_DESCRIPTION("SPDIF driver for the Nexell's nxp2120 chip");
MODULE_LICENSE("GPL");