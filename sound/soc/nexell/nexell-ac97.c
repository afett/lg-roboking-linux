/*
 * nexell-ac97.c  --  ALSA Soc Audio Layer
 *
 * Copyright(c) 2011 STcube Inc. Seungwoo Kim <ksw@stcube.com>
 * Based on wolfson micro driver, modified for nxp2120 soc ac97.
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
#include <sound/ac97_codec.h>
#include <sound/initval.h>
#include <sound/soc.h>

/* Nexell headers */
#include <mach/platform.h>

#include "nexell-pcm.h"
#include "nexell-i2s.h"
#include "nexell-ac97.h"

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "ac97: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#define NXP2120_AC97_CTRL_WARM_RST             (1 << 3)
#define NXP2120_AC97_CTRL_ACLINK_RUN           (1 << 2)
#define NXP2120_AC97_CTRL_CTRL_RST             (1 << 1)
#define NXP2120_AC97_CTRL_COLD_RST             (1 << 0)

#define NXP2120_AC97_CONFIG_ADC2_EN            (1 << 7)
#define NXP2120_AC97_CONFIG_ADC1_EN            (1 << 6)
#define NXP2120_AC97_CONFIG_MIC_EN             (1 << 5)
#define NXP2120_AC97_CONFIG_PCMIN_EN           (1 << 4)
#define NXP2120_AC97_CONFIG_SPDIF_EN           (1 << 3)
#define NXP2120_AC97_CONFIG_CLFE_EN            (1 << 2)
#define NXP2120_AC97_CONFIG_REAR_EN            (1 << 1)
#define NXP2120_AC97_CONFIG_FRONT_EN           (1 << 0)

#define NXP2120_I2S_CTRL_I2SLINK_RUN           (1 << 1)
#define NXP2120_I2S_CTRL_I2S_EN                (1 << 0)

#define NXP2120_I2S_CONFIG_IF_MODE_MASK        (3 << 6)
#define NXP2120_I2S_CONFIG_IF_MODE_I2S         (0 << 6)
#define NXP2120_I2S_CONFIG_IF_MODE_LEFTJUSTIFIED  (2 << 6)
#define NXP2120_I2S_CONFIG_IF_MODE_RIGHTJUSTIFIED  (3 << 6)
#define NXP2120_I2S_CONFIG_SYNC_PERIOD_MASK    (3 << 4)
#define NXP2120_I2S_CONFIG_SYNC_PERIOD_32FS    (0 << 4)
#define NXP2120_I2S_CONFIG_SYNC_PERIOD_48FS    (1 << 4)
#define NXP2120_I2S_CONFIG_SYNC_PERIOD_64FS    (2 << 4)
#define NXP2120_I2S_CONFIG_LOOPBACK            (1 << 3)
#define NXP2120_I2S_CONFIG_I2S_IN_EN           (1 << 2)
#define NXP2120_I2S_CONFIG_I2S_OUT_EN          (1 << 1)
#define NXP2120_I2S_CONFIG_CTRL_MODE_MASK      (1 << 0)
#define NXP2120_I2S_CONFIG_CTRL_MODE_MASTER    (0 << 0)
#define NXP2120_I2S_CONFIG_CTRL_MODE_SLAVE     (1 << 0)

#define NXP2120_AUDIO_BUFF_CTRL_ADC2BUF_EN   (1 << 5)
#define NXP2120_AUDIO_BUFF_CTRL_ADC1BUF_EN   (1 << 4)
#define NXP2120_AUDIO_BUFF_CTRL_MICBUF_EN    (1 << 3)
#define NXP2120_AUDIO_BUFF_CTRL_PCMIBUF_EN   (1 << 2)
#define NXP2120_AUDIO_BUFF_CTRL_SPDIFBUF_EN  (1 << 1)
#define NXP2120_AUDIO_BUFF_CTRL_PCMOBUF_EN   (1 << 0)

#define NXP2120_AUDIO_BUFF_CONFIG_PI_WIDTH_MASK (3 << 4)
#define NXP2120_AUDIO_BUFF_CONFIG_PI_WIDTH_16   (0 << 4)
#define NXP2120_AUDIO_BUFF_CONFIG_PI_WIDTH_18   (3 << 4)
#define NXP2120_AUDIO_BUFF_CONFIG_PI_WIDTH_20   (3 << 4)
#define NXP2120_AUDIO_BUFF_CONFIG_PI_WIDTH_24   (3 << 4)
#define NXP2120_AUDIO_BUFF_CONFIG_SP_WIDTH_MASK (3 << 4)
#define NXP2120_AUDIO_BUFF_CONFIG_SP_WIDTH_16   (0 << 4)
#define NXP2120_AUDIO_BUFF_CONFIG_SP_WIDTH_18   (3 << 4)
#define NXP2120_AUDIO_BUFF_CONFIG_SP_WIDTH_20   (3 << 4)
#define NXP2120_AUDIO_BUFF_CONFIG_PO_WIDTH_MASK (3 << 4)
#define NXP2120_AUDIO_BUFF_CONFIG_PO_WIDTH_16   (0 << 4)
#define NXP2120_AUDIO_BUFF_CONFIG_PO_WIDTH_18   (3 << 4)
#define NXP2120_AUDIO_BUFF_CONFIG_PO_WIDTH_20   (3 << 4)
#define NXP2120_AUDIO_BUFF_CONFIG_PO_WIDTH_24   (3 << 4)

#define NXP2120_AUDIO_IRQ_ENA_RDDONE_IRQ_EN     (1 << 6)
#define NXP2120_AUDIO_IRQ_ENA_ADC2OVR_IRQ_EN    (1 << 5)
#define NXP2120_AUDIO_IRQ_ENA_ADC1OVR_IRQ_EN    (1 << 4)
#define NXP2120_AUDIO_IRQ_ENA_MICOVR_IRQ_EN     (1 << 3)
#define NXP2120_AUDIO_IRQ_ENA_PIOVER_IRQ_EN     (1 << 2)
#define NXP2120_AUDIO_IRQ_ENA_SPUDR_IRQ_EN      (1 << 1)
#define NXP2120_AUDIO_IRQ_ENA_POUDR_IRQ_EN      (1 << 0)

#define NXP2120_AUDIO_IRQ_PEND_RDDONE_PEND      (1 << 6)
#define NXP2120_AUDIO_IRQ_PEND_ADC2OVR_PEND     (1 << 5)
#define NXP2120_AUDIO_IRQ_PEND_ADC1OVR_PEND     (1 << 4)
#define NXP2120_AUDIO_IRQ_PEND_MICOVR_PEND      (1 << 3)
#define NXP2120_AUDIO_IRQ_PEND_PIOVR_PEND       (1 << 2)
#define NXP2120_AUDIO_IRQ_PEND_SPUDR_PEND       (1 << 1)
#define NXP2120_AUDIO_IRQ_PEND_POUDR_PEND       (1 << 0)

#define NXP2120_AUDIO_STATUS0_CODEC_BUSY        (1 << 11)
#define NXP2120_AUDIO_STATUS0_CODEC_RDDONE      (1 << 10)
#define NXP2120_AUDIO_STATUS0_CODEC_WRDONE      (1 << 9)
#define NXP2120_AUDIO_STATUS0_CODEC_RDY         (1 << 8)
#define NXP2120_AUDIO_STATUS0_AC_FSM_MASK       (7 << 3)
#define NXP2120_AUDIO_STATUS0_I2S_FSM_MASK      (7 << 0)

#define NXP2120_AUDIO_STATUS1_ADC2BUF_RDY       (1 << 5)
#define NXP2120_AUDIO_STATUS1_ADC1BUF_RDY       (1 << 4)
#define NXP2120_AUDIO_STATUS1_MICBUF_RDY        (1 << 3)
#define NXP2120_AUDIO_STATUS1_PIBUF_RDY         (1 << 2)
#define NXP2120_AUDIO_STATUS1_SPBUF_RDY         (1 << 1)
#define NXP2120_AUDIO_STATUS1_POBUF_RDY         (1 << 0)

#define NXP2120_AUDIO_CLKENB_PCLKMODE           (1 << 3)
#define NXP2120_AUDIO_CLKENB_CLKGENENB          (1 << 2)

#define NXP2120_AUDIO_CLKGEN0_CLKDIV0_MASK      (0xFF << 5)
#define NXP2120_AUDIO_CLKGEN0_CLKSRCSEL0_MASK   (7 << 2)
#define NXP2120_AUDIO_CLKGEN0_OUTCLKNV0         (1 << 1)

#define NXP2120_AUDIO_CLKGEN1_OUTCLKENB1_MASK   (1 << 15)
#define NXP2120_AUDIO_CLKGEN1_OUTCLKENB1_OUT    (0 << 15)
#define NXP2120_AUDIO_CLKGEN1_OUTCLKENB1_IN     (1 << 15)
#define NXP2120_AUDIO_CLKGEN1_CLKDIV1_MASK      (0xFF << 5)
#define NXP2120_AUDIO_CLKGEN1_CLKSRCSEL1_MASK   (7 << 2)
#define NXP2120_AUDIO_CLKGEN1_OUTCLKNV1         (1 << 1)

#define NXP2120_AUDIO_CLKGEN1_CLKSRCSEL1_MASK       (7 << 2)
#define NXP2120_AUDIO_CLKGEN1_CLKSRCSEL1_PLL0       (0 << 2)
#define NXP2120_AUDIO_CLKGEN1_CLKSRCSEL1_PLL1       (1 << 2)
#define NXP2120_AUDIO_CLKGEN1_CLKSRCSEL1_NONE       (2 << 2)
#define NXP2120_AUDIO_CLKGEN1_CLKSRCSEL1_BITCLK     (3 << 2)
#define NXP2120_AUDIO_CLKGEN1_CLKSRCSEL1_INVBITCLK  (4 << 2)
#define NXP2120_AUDIO_CLKGEN1_CLKSRCSEL1_AVCLK      (5 << 2)
#define NXP2120_AUDIO_CLKGEN1_CLKSRCSEL1_INVAVCLK   (6 << 2)
#define NXP2120_AUDIO_CLKGEN1_CLKSRCSEL1_CLKGEN0    (7 << 2)

struct nx_audio_struc {
    struct NX_AUDIO_RegisterSet *regs;
    int debug_mask;
};
static struct nx_audio_struc nx_audio;

static void init_nx_ac97(void)
{
    struct	NX_AUDIO_RegisterSet *ac97_reg = nx_audio.regs;
    int clksel1;

	DBGOUT("%s\n", __func__);

	ac97_reg->CLKENB = 0x08; /* PCKALWAYS */
	ac97_reg->CLKGEN[0][0] = 4 | ((16-1) << 5); /* CLKGEN0 - CLKSEL PLL1 */

	ac97_reg->CLKENB |= 0x04; /* CLK Enable */

    clksel1  = ac97_reg->CLKGEN[1][0];
    clksel1 &= ~NXP2120_AUDIO_CLKGEN1_OUTCLKENB1_MASK;
    clksel1 &= ~NXP2120_AUDIO_CLKGEN1_CLKSRCSEL1_MASK;
    clksel1 |= NXP2120_AUDIO_CLKGEN1_OUTCLKENB1_IN;
    clksel1 |= NXP2120_AUDIO_CLKGEN1_CLKSRCSEL1_BITCLK;
    clksel1 &= ~NXP2120_AUDIO_CLKGEN1_CLKDIV1_MASK;

    ac97_reg->CLKGEN[1][0] = clksel1;

    ac97_reg->AC97_CTRL = 0;

	ac97_reg->I2S_CTRL = 0;
	ac97_reg->AC97_CONFIG = 0;
	ac97_reg->AUDIO_BUFF_CTRL = 0;
	ac97_reg->AUDIO_BUFF_CONFIG =  0;
	ac97_reg->I2S_CONFIG = 0;
	ac97_reg->AC97_CTRL |= NXP2120_AC97_CTRL_CTRL_RST | NXP2120_AC97_CTRL_ACLINK_RUN | NXP2120_AC97_CTRL_COLD_RST; /* AC97 Controller enable */
}

static void exit_nx_ac97(void)
{
    struct	NX_AUDIO_RegisterSet *ac97_reg = nx_audio.regs;
	DBGOUT("%s\n", __func__);

	ac97_reg->AUDIO_BUFF_CTRL = 0;
	ac97_reg->AUDIO_BUFF_CONFIG =  0;
	ac97_reg->I2S_CONFIG = 0;
	ac97_reg->I2S_CTRL = 0;

	ac97_reg->CLKENB = 0;
}

static unsigned short nxp2120_ac97_read(struct snd_ac97 *ac97,
    unsigned short reg)
{
//    struct	NX_AUDIO_RegisterSet *ac97_reg = nx_audio.regs;
    u16 data=0;

    return (unsigned short)data;
}

static void nxp2120_ac97_write(struct snd_ac97 *ac97, unsigned short reg,
    unsigned short val)
{
//    struct	NX_AUDIO_RegisterSet *ac97_reg = nx_audio.regs;
}

static void nxp2120_ac97_warm_reset(struct snd_ac97 *ac97)
{
    struct	NX_AUDIO_RegisterSet *ac97_reg = nx_audio.regs;

    ac97_reg->AC97_CTRL |= NXP2120_AC97_CTRL_WARM_RST;
    udelay(5);
    ac97_reg->AC97_CTRL = ac97_reg->AC97_CTRL & ~NXP2120_AC97_CTRL_WARM_RST; 
}

static void nxp2120_ac97_cold_reset(struct snd_ac97 *ac97)
{
    struct	NX_AUDIO_RegisterSet *ac97_reg = nx_audio.regs;

    ac97_reg->AC97_CTRL = ac97_reg->AC97_CTRL & ~NXP2120_AC97_CTRL_COLD_RST;
    udelay(5);
    ac97_reg->AC97_CTRL = ac97_reg->AC97_CTRL | NXP2120_AC97_CTRL_COLD_RST;
}

struct snd_ac97_bus_ops soc_ac97_ops = {
    .read   = nxp2120_ac97_read,
    .write  = nxp2120_ac97_write,
    .warm_reset = nxp2120_ac97_warm_reset,
    .reset  = nxp2120_ac97_cold_reset,
};

static irqreturn_t nx_audio_buffer_irq(int irq, void *dev_id)
{
    struct nx_audio_struc *nx_audio = (struct nx_audio_struc *)dev_id;
    
    if (nx_audio->debug_mask) {
        short int pend;
        pend = nx_audio->regs->AUDIO_IRQ_PEND;
        if ((nx_audio->debug_mask & 1) && (pend & 1)) { /* PCM_OUT_UNDERRUN */
            nx_audio->regs->AUDIO_IRQ_PEND = 1; /* Clear UNDERRUN */
            printk(KERN_ERR "PCM OUTPUT BUFFER UNDERRUN\n");
        }
        if ((nx_audio->debug_mask & 4) && (pend & 4)) { /* PCM_IN OVERRUN */
            nx_audio->regs->AUDIO_IRQ_PEND = 4; /* Clear OVERRUN */
            printk(KERN_ERR "PCM INPUT BUFFER OVERRUN\n");
        }
        if ((nx_audio->debug_mask & 8) && (pend & 8)) { /* MIC_IN OVERRUN */
            nx_audio->regs->AUDIO_IRQ_PEND = 8; /* Clear OVERRUN */
            printk(KERN_ERR "MIC INPUT BUFFER OVERRUN\n");
        }
        if ((nx_audio->debug_mask & 0x10) && (pend & 0x10)) { /* ADC1_IN OVERRUN */
            nx_audio->regs->AUDIO_IRQ_PEND = 0x10; /* Clear OVERRUN */
            printk(KERN_ERR "ADC1 INPUT BUFFER OVERRUN\n");
        }
        return IRQ_HANDLED;
    }
    return IRQ_NONE;
}

static int nxp2120_ac97_probe(struct platform_device *pdev,
                  struct snd_soc_dai *dai)
{
    int ret;

    nx_audio.regs = (struct NX_AUDIO_RegisterSet *)IO_ADDRESS(PHY_BASEADDR_AUDIO);
    nx_audio.debug_mask = 0 ; // No debug 0x1D;

    init_nx_ac97();

    nx_audio.regs->AUDIO_IRQ_ENA = nx_audio.debug_mask;
	nx_audio.regs->AUDIO_IRQ_PEND = 0x7F;
	
    ret = request_irq(IRQ_PHY_AUDIO, nx_audio_buffer_irq, IRQF_DISABLED, "AC97", (void *)&nx_audio);
    if (ret < 0) {
        printk(KERN_ERR "nxp2120-ac97: interrupt request failed.\n");
    }
    return ret;
}

static void nxp2120_ac97_remove(struct platform_device *pdev,
                struct snd_soc_dai *dai)
{
    free_irq(IRQ_PHY_AUDIO, NULL);
}

#define INDEPENDENT_START 0

static int start_nx_ac97(struct snd_pcm_substream *substream)
{
    struct	NX_AUDIO_RegisterSet *ac97_reg = nx_audio.regs;
	int ret = 0;
	int count = 10;

	DBGOUT("%s (%s)\n", __func__,
		SNDRV_PCM_STREAM_PLAYBACK == substream->stream ? "PLAY":"CAPT");

	if (SNDRV_PCM_STREAM_PLAYBACK == substream->stream) {
	    ac97_reg->AC97_CONFIG = ac97_reg->AC97_CONFIG | NXP2120_AC97_CONFIG_FRONT_EN;
	    ac97_reg->AUDIO_BUFF_CTRL = ac97_reg->AUDIO_BUFF_CTRL | NXP2120_AUDIO_BUFF_CTRL_PCMOBUF_EN; /* PCMOUT buffer enable */
	} else { 
	    if (0 == substream->pcm->device) {
	        ac97_reg->AC97_CONFIG = ac97_reg->AC97_CONFIG | NXP2120_AC97_CONFIG_PCMIN_EN; /* AC97 In Enable */
	        /* Clear Pending IRQ */
	        ac97_reg->AUDIO_IRQ_PEND = 0x3F; /* All clear */
	        ac97_reg->AUDIO_BUFF_CTRL = ac97_reg->AUDIO_BUFF_CTRL  | NXP2120_AUDIO_BUFF_CTRL_PCMIBUF_EN | NXP2120_AUDIO_BUFF_CTRL_ADC1BUF_EN | NXP2120_AUDIO_BUFF_CTRL_MICBUF_EN; /* Start 4 channel concurrently. */

	        while ((0 == (ac97_reg->AUDIO_STATUS1 & NXP2120_AUDIO_STATUS1_PIBUF_RDY)) && (count-- > 0))
	            msleep(10);

	        if (0 == (ac97_reg->AUDIO_STATUS1 & NXP2120_AUDIO_STATUS1_PIBUF_RDY)) {
	            printk(KERN_ERR "nexell-ac97: Error, AC97 is not ready for capture PCMIN\n");
	            ret= -1;
	        }
		} else
		if (1 == substream->pcm->device) {
	        ac97_reg->AC97_CONFIG = ac97_reg->AC97_CONFIG | NXP2120_AC97_CONFIG_ADC1_EN; /* AC97 ADC1 In Enable */
	        /* Clear Pending IRQ */
	        ac97_reg->AUDIO_IRQ_PEND = 0x3F; /* All clear */
#if INDEPENDENT_START	        
	        ac97_reg->AUDIO_BUFF_CTRL = ac97_reg->AUDIO_BUFF_CTRL  | NXP2120_AUDIO_BUFF_CTRL_ADC1BUF_EN; /* ADC1 buffer enable */

	        while ((0 == (ac97_reg->AUDIO_STATUS1 & NXP2120_AUDIO_STATUS1_ADC1BUF_RDY)) && (count-- > 0))
	            msleep(10);

	        if (0 == (ac97_reg->AUDIO_STATUS1 & NXP2120_AUDIO_STATUS1_ADC1BUF_RDY)) {
	            printk(KERN_ERR "nexell-ac97: Error, AC97 is not ready for capture ADC1\n");
	            ret= -1;
	        }
#endif
		} else
		if (2 == substream->pcm->device) {
	        ac97_reg->AC97_CONFIG = ac97_reg->AC97_CONFIG | NXP2120_AC97_CONFIG_MIC_EN; /* AC97 Mic In Enable */
	        /* Clear Pending IRQ */
	        ac97_reg->AUDIO_IRQ_PEND = 0x3F; /* All clear */
#if INDEPENDENT_START
	        ac97_reg->AUDIO_BUFF_CTRL = ac97_reg->AUDIO_BUFF_CTRL  | NXP2120_AUDIO_BUFF_CTRL_MICBUF_EN; /* MIC buffer enable */

	        while ((0 == (ac97_reg->AUDIO_STATUS1 & NXP2120_AUDIO_STATUS1_MICBUF_RDY)) && (count-- > 0))
	            msleep(10);

	        if (0 == (ac97_reg->AUDIO_STATUS1 & NXP2120_AUDIO_STATUS1_MICBUF_RDY)) {
	            printk(KERN_ERR "nexell-ac97: Error, AC97 is not ready for capture MIC\n");
	            ret= -1;
	        }
#endif
		}
	}
	return ret;
}

static void stop_nx_ac97(struct snd_pcm_substream *substream)
{
    struct	NX_AUDIO_RegisterSet *ac97_reg = nx_audio.regs;
	DBGOUT("%s (%s)\n", __func__,
		SNDRV_PCM_STREAM_PLAYBACK == substream->stream ? "PLAY":"CAPT");

	if (SNDRV_PCM_STREAM_PLAYBACK == substream->stream) {
	    ac97_reg->AC97_CONFIG = ac97_reg->AC97_CONFIG & ~NXP2120_AC97_CONFIG_FRONT_EN;
		ac97_reg->AUDIO_BUFF_CTRL = ac97_reg->AUDIO_BUFF_CTRL & ~NXP2120_AUDIO_BUFF_CTRL_PCMOBUF_EN;   /* PCMOUT buffer disable */
	} else {
	    if (0 == substream->pcm->device) {
	        ac97_reg->AC97_CONFIG = ac97_reg->AC97_CONFIG & ~(NXP2120_AC97_CONFIG_PCMIN_EN);
	        ac97_reg->AUDIO_BUFF_CTRL = ac97_reg->AUDIO_BUFF_CTRL & ~(NXP2120_AUDIO_BUFF_CTRL_PCMIBUF_EN | NXP2120_AUDIO_BUFF_CTRL_ADC1BUF_EN | NXP2120_AUDIO_BUFF_CTRL_MICBUF_EN); /* PCMIN buffer disable */
	    }
	    if (1 == substream->pcm->device) {
	        ac97_reg->AC97_CONFIG = ac97_reg->AC97_CONFIG & ~(NXP2120_AC97_CONFIG_ADC1_EN);
	        ac97_reg->AUDIO_BUFF_CTRL = ac97_reg->AUDIO_BUFF_CTRL & ~NXP2120_AUDIO_BUFF_CTRL_ADC1BUF_EN; /* PCMIN buffer disable */
	    }
	    if (2 == substream->pcm->device) {
	        ac97_reg->AC97_CONFIG = ac97_reg->AC97_CONFIG & ~(NXP2120_AC97_CONFIG_MIC_EN);
	        ac97_reg->AUDIO_BUFF_CTRL = ac97_reg->AUDIO_BUFF_CTRL & ~NXP2120_AUDIO_BUFF_CTRL_MICBUF_EN; /* PCMIN buffer disable */
	    }
	}
}

/*--------------------------------------------------------------------------------
 * sound ac97 dai ops
 ---------------------------------------------------------------------------------*/
static int  nxp2120_ac97_startup(struct snd_pcm_substream *substream,	struct snd_soc_dai *dai)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static void nxp2120_ac97_shutdown(struct snd_pcm_substream *substream,struct snd_soc_dai *dai)
{
	DBGOUT("%s\n", __func__);
}

static int nxp2120_ac97_hw_params(struct snd_pcm_substream *substream,
                  struct snd_pcm_hw_params *params,
                  struct snd_soc_dai *dai)
{
	DBGOUT("%s\n", __func__);

    return 0;
}

static int nxp2120_ac97_trigger(struct snd_pcm_substream *substream, int cmd,
                struct snd_soc_dai *dai)
{
    switch (cmd) {
    case SNDRV_PCM_TRIGGER_START:
    case SNDRV_PCM_TRIGGER_RESUME:
    case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
        start_nx_ac97(substream);
        break;
    case SNDRV_PCM_TRIGGER_STOP:
    case SNDRV_PCM_TRIGGER_SUSPEND:
    case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
        stop_nx_ac97(substream);
        break;
    }

    return 0;
}

static int nxp2120_ac97_digital_mute(struct snd_soc_dai *dai, int mute)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static struct snd_soc_dai_ops nx_ac97_dai_ops = {
	.startup		= nxp2120_ac97_startup,
	.shutdown		= nxp2120_ac97_shutdown,
	.trigger		= nxp2120_ac97_trigger,
	.hw_params		= nxp2120_ac97_hw_params,
	.digital_mute 	= nxp2120_ac97_digital_mute,
};

static struct snd_soc_dai_ops nx_ac97_dai_adc1_ops = {
	.trigger		= nxp2120_ac97_trigger,
	.hw_params		= nxp2120_ac97_hw_params,
};
        
#define NXP2120_AC97_RATES (SNDRV_PCM_RATE_16000) // | SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_8000 | 

struct snd_soc_dai nxp2120_ac97_dai[] = {
{
    .name = "nxp2120-ac97",
    .id = 0,
    .ac97_control = 1,
    .probe = nxp2120_ac97_probe,
    .remove = nxp2120_ac97_remove,
    .playback = {
        .stream_name = "AC97 Playback",
        .channels_min = 2,
        .channels_max = 2,
        .rates = NXP2120_AC97_RATES,
        .formats = SNDRV_PCM_FMTBIT_S16_LE,},
    .capture = {
        .stream_name = "AC97 Capture",
        .channels_min = 2,
        .channels_max = 2,
        .rates = NXP2120_AC97_RATES,
        .formats = SNDRV_PCM_FMTBIT_S16_LE,},
    .ops = &nx_ac97_dai_ops,
},
{
	.name = "nxp2120-ac97-adc1",
	.id = 1,
	.ac97_control = 1,
	.capture = {
		.stream_name = "AC97 ADC1 Capture",
		.channels_min = 1,
		.channels_max = 1,
		.rates = NXP2120_AC97_RATES,
		.formats = SNDRV_PCM_FMTBIT_S16_LE,},
	.ops = &nx_ac97_dai_adc1_ops,
},
{
	.name = "nxp2120-ac97-mic",
	.id = 2,
	.ac97_control = 1,
	.capture = {
		.stream_name = "AC97 MIC Capture",
		.channels_min = 1,
		.channels_max = 1,
		.rates = NXP2120_AC97_RATES,
		.formats = SNDRV_PCM_FMTBIT_S16_LE,},
	.ops = &nx_ac97_dai_adc1_ops,
},
};

EXPORT_SYMBOL_GPL(nxp2120_ac97_dai);
EXPORT_SYMBOL_GPL(soc_ac97_ops);

static int __init nxp2120_ac97_init(void)
{
    return snd_soc_register_dais(nxp2120_ac97_dai,
                     ARRAY_SIZE(nxp2120_ac97_dai));
}
module_init(nxp2120_ac97_init);

static void __exit nxp2120_ac97_exit(void)
{
    exit_nx_ac97();
    
    snd_soc_unregister_dais(nxp2120_ac97_dai,
                ARRAY_SIZE(nxp2120_ac97_dai));
}
module_exit(nxp2120_ac97_exit);


MODULE_AUTHOR("Seungwoo Kim");
MODULE_DESCRIPTION("AC97 driver for the Nexell's nxp2120 chip");
MODULE_LICENSE("GPL");
