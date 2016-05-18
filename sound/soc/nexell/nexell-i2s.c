/*
 *  Copyright(C) 2011 MOSTiTECH co., ltd.
 *  All right reserved by Segunwoo Kim <ksw@mostitech.com>
 *  
 *  This code partialy based on NEXELL's i2s driver
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

#include <linux/init.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <linux/delay.h>

#include <sound/soc.h>

/* Nexell headers */
#include <mach/platform.h>

#include "nexell-pcm.h"
#include "nexell-i2s.h"

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "i2s: " msg); }
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

struct nx_audio_struc {
    struct	NX_AUDIO_RegisterSet *audio_regs;
    int debug_mask;
};

static struct nx_audio_struc nx_audio;

//------------------------------------------------------------------------------
// Nexell i2s control
//------------------------------------------------------------------------------

static void init_nx_i2s(void)
{
    struct	NX_AUDIO_RegisterSet *i2s_reg = nx_audio.audio_regs;
	DBGOUT("%s\n", __func__);

	/* set I2S clock */
#if 0	
	i2s_reg->CLKENB = 0x08; /* PCKALWAYS */
	i2s_reg->CLKGEN[0][0] = 4 | ((16-1) << 5); /* CLKGEN0 - CLKSEL PLL1 */

	i2s_reg->CLKENB |= 0x04; /* CLK Enable */
#endif
	printk("I2S audio clock enable = %x\n", i2s_reg->CLKENB); 
	msleep(1);

	i2s_reg->CLKGEN[1][0] = 0x8000 | 4; /* CLKGEN1 as CLKSEL PLL1 and PAD would be INPUT */
	msleep(1);

	i2s_reg->I2S_CTRL = NXP2120_I2S_CTRL_I2S_EN; /* I2S Enable */
	i2s_reg->AC97_CTRL = 0; /* AC97 Controller disable */
	i2s_reg->AUDIO_BUFF_CTRL = 0;
	i2s_reg->AUDIO_BUFF_CONFIG =  0;
	i2s_reg->I2S_CONFIG = i2s_reg->I2S_CONFIG & ~NXP2120_I2S_CONFIG_CTRL_MODE_MASK; /* Preserve Master mode */ 
	//i2s_reg->AC97_CONFIG |= 1; /* FRONT_EN should be 1 for USING I2S enable */
	i2s_reg->I2S_CTRL = NXP2120_I2S_CTRL_I2S_EN | NXP2120_I2S_CTRL_I2SLINK_RUN; /* I2S Enable & LINKON*/
}

static irqreturn_t nx_audio_buffer_irq(int irq, void *dev_id)
{
    struct nx_audio_struc *nx_audio = (struct nx_audio_struc *)dev_id;
    
    if (nx_audio->debug_mask) {
        if (nx_audio->debug_mask & 1) { /* PCM_OUT_UNDERRUN */
            nx_audio->audio_regs->AUDIO_IRQ_PEND = 1; /* Clear UNDERRUN */
            printk(KERN_ERR "PCM OUTPUT BUFFER UNDERRUN\n");
        }
        if (nx_audio->debug_mask & 4) { /* PCM_IN OVERRUN */
            nx_audio->audio_regs->AUDIO_IRQ_PEND = 4; /* Clear OVERRUN */
            printk(KERN_ERR "PCM INPUT BUFFER OVERRUN\n");
        }
        return IRQ_HANDLED;
    }
    return IRQ_NONE;
}


static int nx_i2s_probe(struct platform_device *pdev,
			      struct snd_soc_dai *dai)
{
	int ret;
	
	nx_audio.audio_regs =  (struct NX_AUDIO_RegisterSet *)IO_ADDRESS(PHY_BASEADDR_AUDIO);
	nx_audio.debug_mask = 0x5; /* Buf PCM_OUT and PCM_IN debug */
	/* Init nx i2s registers */
	init_nx_i2s();
	/* Before IRQ turn on, clear ALL PENDING IRQ,and disable ALL IRQ as no bootup bothering to many IRQs */
	nx_audio.audio_regs->AUDIO_IRQ_ENA = nx_audio.debug_mask;
	nx_audio.audio_regs->AUDIO_IRQ_PEND = 0x7F;
	/* Now enable IRQ for debugging Sound buffer over/underrun status */
	ret = request_irq(IRQ_PHY_AUDIO, nx_audio_buffer_irq, IRQF_DISABLED, "NX-AUDIO", &nx_audio);
	if (ret < 0) {
	    return -ENODEV;
	}
}


static void exit_nx_i2s(void)
{
    struct	NX_AUDIO_RegisterSet *i2s_reg = nx_audio.audio_regs;
	DBGOUT("%s\n", __func__);

	i2s_reg->AUDIO_BUFF_CTRL = 0;
	i2s_reg->AUDIO_BUFF_CONFIG =  0;
	i2s_reg->I2S_CONFIG = 0;
	i2s_reg->I2S_CTRL = 0;

	i2s_reg->CLKENB = 0;
}

static int start_nx_i2s(struct snd_pcm_substream *substream)
{
    struct	NX_AUDIO_RegisterSet *i2s_reg = nx_audio.audio_regs;
	int ret = 0;
	int count = 10;

	DBGOUT("%s (%s)\n", __func__,
		SNDRV_PCM_STREAM_PLAYBACK == substream->stream ? "PLAY":"CAPT");

	if (SNDRV_PCM_STREAM_PLAYBACK == substream->stream) {
	    i2s_reg->I2S_CONFIG = i2s_reg->I2S_CONFIG | NXP2120_I2S_CONFIG_I2S_OUT_EN; /* I2S Out Enable */ 
	    i2s_reg->AUDIO_BUFF_CTRL = i2s_reg->AUDIO_BUFF_CTRL | NXP2120_AUDIO_BUFF_CTRL_PCMOBUF_EN; /* PCMOUT buffer enable */
	    //i2s_reg->AC97_CONFIG |= 1;
	} else {
	    i2s_reg->I2S_CONFIG = i2s_reg->I2S_CONFIG | NXP2120_I2S_CONFIG_I2S_IN_EN; /* I2S In Enable */
	    i2s_reg->AUDIO_BUFF_CTRL = i2s_reg->AUDIO_BUFF_CTRL  | NXP2120_AUDIO_BUFF_CTRL_PCMIBUF_EN; /* PCMIN buffer enable */

		while ((0 == (i2s_reg->AUDIO_STATUS1 & NXP2120_AUDIO_STATUS1_PIBUF_RDY)) && (count-- > 0))
			msleep(10);

		if (0 == (i2s_reg->AUDIO_STATUS1 & NXP2120_AUDIO_STATUS1_PIBUF_RDY)) {
			printk(KERN_ERR "nexell-i2s: Error, I2S is not ready for capture\n");
			ret= -1;
		}
	}
	//printk("start I2Sconfig = %x %x %x\n", i2s_reg->I2S_CONFIG, i2s_reg->AC97_CONFIG, i2s_reg->AUDIO_BUFF_CTRL);
	return ret;
}

static void stop_nx_i2s(struct snd_pcm_substream *substream)
{
    struct	NX_AUDIO_RegisterSet *i2s_reg = nx_audio.audio_regs;
	DBGOUT("%s (%s)\n", __func__,
		SNDRV_PCM_STREAM_PLAYBACK == substream->stream ? "PLAY":"CAPT");

	if (SNDRV_PCM_STREAM_PLAYBACK == substream->stream) {
		i2s_reg->I2S_CONFIG = i2s_reg->I2S_CONFIG & ~NXP2120_I2S_CONFIG_I2S_OUT_EN; /* I2S Out Disable */ 
		i2s_reg->AUDIO_BUFF_CTRL = i2s_reg->AUDIO_BUFF_CTRL & ~NXP2120_AUDIO_BUFF_CTRL_PCMOBUF_EN;   /* PCMOUT buffer disable */
	} else {
	    i2s_reg->I2S_CONFIG = i2s_reg->I2S_CONFIG & ~NXP2120_I2S_CONFIG_I2S_IN_EN; /* I2S In Disable */ 
	    i2s_reg->AUDIO_BUFF_CTRL = i2s_reg->AUDIO_BUFF_CTRL & ~NXP2120_AUDIO_BUFF_CTRL_PCMIBUF_EN; /* PCMIN buffer disable */
	}
	//printk("stop I2Sconfig = %x\n", i2s_reg->I2S_CONFIG);
}

/*--------------------------------------------------------------------------------
 * sound i2s dai ops
 ---------------------------------------------------------------------------------*/
static int  nx_i2s_ops_startup(struct snd_pcm_substream *substream,	struct snd_soc_dai *dai)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static void nx_i2s_ops_shutdown(struct snd_pcm_substream *substream,struct snd_soc_dai *dai)
{
	DBGOUT("%s\n", __func__);
}

static int nx_i2s_ops_trigger(struct snd_pcm_substream *substream, int cmd, struct snd_soc_dai *dai)
{
	int ret = 0;

	DBGOUT("%s(cmd:0x%x)\n", __func__, cmd);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_START:

		ret = start_nx_i2s(substream);
		break;

	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:

		stop_nx_i2s(substream);
		break;

	default:
		ret = -EINVAL;
	}
	return ret;
}

static int nx_i2s_ops_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static int  nx_i2s_ops_set_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
    struct	NX_AUDIO_RegisterSet *i2s_reg = (struct NX_AUDIO_RegisterSet *)IO_ADDRESS(PHY_BASEADDR_AUDIO);
    int config;
	DBGOUT("%s\n", __func__);
	/* We can Handle Format as I2S / LeftJustify / RightJustify */
	config = i2s_reg->I2S_CONFIG;
	config &= ~(NXP2120_I2S_CONFIG_IF_MODE_MASK | NXP2120_I2S_CONFIG_CTRL_MODE_MASK); /* Maskout IFMODE, Master/Slave mode */
	
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
    case SND_SOC_DAIFMT_CBM_CFM:
        config |= NXP2120_I2S_CONFIG_CTRL_MODE_MASTER;
        break;
    case SND_SOC_DAIFMT_CBS_CFS:
        config |= NXP2120_I2S_CONFIG_CTRL_MODE_SLAVE;
        break;
    default:
        DBGOUT("unknwon master/slave format\n");
        return -EINVAL;
    }

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
    case SND_SOC_DAIFMT_RIGHT_J:
        config |= NXP2120_I2S_CONFIG_IF_MODE_RIGHTJUSTIFIED;
        break;
    case SND_SOC_DAIFMT_LEFT_J:
        config |= NXP2120_I2S_CONFIG_IF_MODE_LEFTJUSTIFIED;
        break;
    case SND_SOC_DAIFMT_I2S:
        config |= NXP2120_I2S_CONFIG_IF_MODE_I2S;
        break;
    default:
        DBGOUT("Unknown data format. Fall back to I2S\n");
        config |= NXP2120_I2S_CONFIG_IF_MODE_I2S;
    }
    
    i2s_reg->I2S_CONFIG = config;

	return 0;
}

static int  nx_i2s_ops_set_sysclk(struct snd_soc_dai *dai, int clk_id, unsigned int freq, int dir)
{
    struct	NX_AUDIO_RegisterSet *i2s_reg = (struct NX_AUDIO_RegisterSet *)IO_ADDRESS(PHY_BASEADDR_AUDIO);
    int clksel0, clksel1;
	DBGOUT("%s\n", __func__);
	
	switch (clk_id) {
	  case NX_I2S_CLKSRC_0 :
	      clksel0 = i2s_reg->CLKGEN[0][0];
	      clksel0 &= ~NXP2120_AUDIO_CLKGEN0_CLKSRCSEL0_MASK;
	      switch (freq) {
	        case NX_I2S_CLKSRC0_PLL0 :
	            clksel0 |= (0 << 2);
	            break;
	        case NX_I2S_CLKSRC0_PLL1 :
	            clksel0 |= (1 << 2);
	            break;
	        default :
	            clksel0 |= (1 << 2);
	      }
	      i2s_reg->CLKGEN[0][0] = clksel0;
	      break;
      case NX_I2S_CLKSRC_1 :
          clksel1 = i2s_reg->CLKGEN[1][0];
          clksel1 &= ~NXP2120_AUDIO_CLKGEN1_CLKSRCSEL1_MASK;
          clksel1 &= ~NXP2120_AUDIO_CLKGEN1_OUTCLKENB1_MASK; /* CLKSEL1 output enable as default */
          switch (freq) {
            case NX_I2S_CLKSRC1_PLL0 :
                clksel1 |= (0 << 2);
                break;
            case NX_I2S_CLKSRC1_PLL1 :
                clksel1 |= (1 << 2);
	            break;
            case NX_I2S_CLKSRC1_EXT_BIT :
                clksel1 |= (3 << 2);
                clksel1 |= NXP2120_AUDIO_CLKGEN1_OUTCLKENB1_IN; /* CLKSEL1 output disable for EXT BITCLK */
                break;
            case NX_I2S_CLKSRC1_EXT_INVBIT :
                clksel1 |= (4 << 2);
                clksel1 |= NXP2120_AUDIO_CLKGEN1_OUTCLKENB1_IN; /* CLKSEL1 output disable for EXT INV_BITCLK */
                break;
            case NX_I2S_CLKSRC1_MCLK :
                clksel1 |= (7 << 2);
                break;
            default :
                clksel1 |= (1 << 2);
          }
          i2s_reg->CLKGEN[1][0] = clksel1;
          break;
      default:
          ; /* default is do nothing */
    }
	
	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
static int nx_i2s_ops_set_pll(struct snd_soc_dai *codec_dai, int pll_id,
			    int source, unsigned int freq_in,
			    unsigned int freq_out)
#else
static int nx_i2s_ops_set_pll(struct snd_soc_dai *dai,	int pll_id,
				unsigned int freq_in, unsigned int freq_out)
#endif
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static int nx_i2s_ops_set_clkdiv(struct snd_soc_dai *dai, int div_id, int div)
{
    struct	NX_AUDIO_RegisterSet *i2s_reg = (struct NX_AUDIO_RegisterSet *)IO_ADDRESS(PHY_BASEADDR_AUDIO);
    int clksel0, clksel1, config;
	DBGOUT("%s\n", __func__);
	
	switch (div_id) {
	  case NX_I2S_CLKDIV_0 :
	      clksel0 = i2s_reg->CLKGEN[0][0];
	      clksel0 &= ~NXP2120_AUDIO_CLKGEN0_CLKDIV0_MASK;
	      clksel0 |= div << 5;
	      i2s_reg->CLKGEN[0][0] = clksel0;
	      break;
      case NX_I2S_CLKDIV_1 :
          clksel1 = i2s_reg->CLKGEN[1][0];
	      clksel1 &= ~NXP2120_AUDIO_CLKGEN1_CLKDIV1_MASK;
	      clksel1 |= div << 5;
	      i2s_reg->CLKGEN[1][0] = clksel1;
	      break;
	  case NX_I2S_CLKDIV_SYNC_PERIOD :
	      config = i2s_reg->I2S_CONFIG;
	      config &= ~NXP2120_I2S_CONFIG_SYNC_PERIOD_MASK;
	      switch (div) {
	        case NX_I2S_PERDIV_32:
	            config |= NXP2120_I2S_CONFIG_SYNC_PERIOD_32FS;
	            break;
	        case NX_I2S_PERDIV_48:
	            config |= NXP2120_I2S_CONFIG_SYNC_PERIOD_48FS;
	            break;
	        case NX_I2S_PERDIV_64:
	            config |= NXP2120_I2S_CONFIG_SYNC_PERIOD_64FS;
	            break;
	        default:
	            ;
	      }
	      i2s_reg->I2S_CONFIG = config;
	      break;	          
	  default:
	      ; /* Do nothing for Not correct ID */
	}
	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
static int nx_i2s_ops_set_tdm_slot(struct snd_soc_dai *dai, unsigned int tx_mask, unsigned int rx_mask,
				int slots, int slot_width)
#else
static int nx_i2s_ops_set_tdm_slot(struct snd_soc_dai *cpu_dai,	unsigned int mask, int slots)
#endif
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static int nx_i2s_ops_set_tristate(struct snd_soc_dai *dai, int tristate)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static int nx_i2s_ops_digital_mute(struct snd_soc_dai *dai, int mute)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static struct snd_soc_dai_ops nx_i2s_dai_ops = {
	.startup		= nx_i2s_ops_startup,
	.shutdown		= nx_i2s_ops_shutdown,
	.trigger		= nx_i2s_ops_trigger,
	.hw_params		= nx_i2s_ops_hw_params,
	.set_fmt		= nx_i2s_ops_set_fmt,
	.set_sysclk		= nx_i2s_ops_set_sysclk,
	.set_pll 		= nx_i2s_ops_set_pll,
	.set_clkdiv 	= nx_i2s_ops_set_clkdiv,
	.set_tdm_slot 	= nx_i2s_ops_set_tdm_slot,
	.set_tristate 	= nx_i2s_ops_set_tristate,
	.digital_mute 	= nx_i2s_ops_digital_mute,
};

/*--------------------------------------------------------------------------------
 * sound i2s dai suspend/resume
 ---------------------------------------------------------------------------------*/
static int  nx_i2s_suspend(struct snd_soc_dai *cpu_dai)
{
	PM_DBGOUT("%s\n", __func__);
	return 0;
}

static int  nx_i2s_resume(struct snd_soc_dai *cpu_dai)
{
	PM_DBGOUT("+%s\n", __func__);

	init_nx_i2s();

	PM_DBGOUT("-%s\n", __func__);
	return 0;
}

#define NX_I2S_RATES \
	(SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 | SNDRV_PCM_RATE_16000 | \
	SNDRV_PCM_RATE_22050 | SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 | \
	SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_88200 | SNDRV_PCM_RATE_96000)

//#define NX_I2S_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S24_LE)
#define NX_I2S_FORMATS (SNDRV_PCM_FMTBIT_S16_LE)

struct snd_soc_dai nx_snd_i2s_dai = {
	.name 		= NEXELL_SND_SOC_DAI_NAME,
	.id 		= 0,
	.probe      = nx_i2s_probe,
	.suspend	= nx_i2s_suspend,
	.resume 	= nx_i2s_resume,

	.playback	= {
		.channels_min 	= 2,
		.channels_max 	= 2,
		.rates 			= NX_I2S_RATES,
		.formats 		= NX_I2S_FORMATS,
		},

	.capture 	= {
		.channels_min 	= 2,
		.channels_max 	= 2,
		.rates 			= NX_I2S_RATES,
		.formats 		= NX_I2S_FORMATS,
		},

	.ops = &nx_i2s_dai_ops,
};
EXPORT_SYMBOL_GPL(nx_snd_i2s_dai);

/*--------------------------------------------------------------------------------
 * sound i2s platform
 ---------------------------------------------------------------------------------*/
static int __init nx_snd_i2s_init(void)
{
	DBGOUT("%s\n", __func__);

	//init_nx_i2s();
	return snd_soc_register_dai(&nx_snd_i2s_dai);
}

static void __exit nx_snd_i2s_exit(void)
{
	DBGOUT("%s\n", __func__);

	exit_nx_i2s();
	snd_soc_unregister_dai(&nx_snd_i2s_dai);
}

module_init(nx_snd_i2s_init);
module_exit(nx_snd_i2s_exit);

MODULE_AUTHOR("Seungwoo Kim <ksw@mostitech.com>");
MODULE_DESCRIPTION("Sound I2S driver for the Nexell");
MODULE_LICENSE("GPL");

