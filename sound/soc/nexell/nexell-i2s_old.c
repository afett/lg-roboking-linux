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

//------------------------------------------------------------------------------
// Nexell i2s control
//------------------------------------------------------------------------------
static void init_i2s_port(void)
{
	DBGOUT("%s\n", __func__);

	NX_AUDIO_SetBaseAddress(IO_ADDRESS(NX_AUDIO_GetPhysicalAddress()));
	NX_AUDIO_OpenModule();

	/* set I2S clock */
	NX_AUDIO_SetClockDivisorEnable(CFALSE);
	NX_AUDIO_SetClockPClkMode(NX_PCLKMODE_ALWAYS);

	NX_AUDIO_SetClockSource(0, CFG_AUDIO_I2S_CLK_SRC_0);
	NX_AUDIO_SetClockDivisor(0, CFG_AUDIO_I2S_CLK_DIV_0);
	NX_AUDIO_SetClockOutInv(0, CFG_AUDIO_I2S_CLK_INV_0);

	NX_AUDIO_SetClockSource(1, 7);	// sysnc clkgen 0 and 1
	NX_AUDIO_SetClockDivisor(1, CFG_AUDIO_I2S_CLK_DIV_1);
	NX_AUDIO_SetClockOutInv(1, CFG_AUDIO_I2S_CLK_INV_1);
	NX_AUDIO_SetClockOutEnb(1, CFG_AUDIO_I2S_MASTER_MODE);

	NX_AUDIO_SetClockDivisorEnable(CTRUE);
	msleep(1);

	NX_AUDIO_SetClockSource(1, CFG_AUDIO_I2S_CLK_SRC_1);	// set clkgen 1 source
	msleep(1);

	/* set mode(master/slave) */
	NX_AUDIO_SetI2SMasterMode(CFG_AUDIO_I2S_MASTER_MODE);
	NX_AUDIO_SetSyncPeriod(CFG_AUDIO_I2S_SYNC_PERIOD);

	/* set I2S misc controller */
	NX_AUDIO_SetI2SControllerReset(CFALSE);
	NX_AUDIO_SetInterfaceMode(CFG_AUDIO_I2S_TRANS_MODE);
	NX_AUDIO_SetPCMOUTDataWidth(CFG_AUDIO_I2S_PCM_IN_WIDTH);
	NX_AUDIO_SetPCMINDataWidth(CFG_AUDIO_I2S_PCM_OUT_WIDTH);

	NX_AUDIO_SetI2SLoopBackEnable(CFALSE);
	NX_AUDIO_SetI2SLinkOn();
	NX_AUDIO_SetAudioBufferPCMOUTEnable(CFALSE);
	NX_AUDIO_SetI2SOutputEnable(CFALSE);
	NX_AUDIO_SetAudioBufferPCMINEnable(CFALSE);
	NX_AUDIO_SetI2SInputEnable(CFALSE);
}

static void exit_i2s_port(void)
{
	DBGOUT("%s\n", __func__);

	NX_AUDIO_SetAudioBufferPCMOUTEnable(CFALSE);
	NX_AUDIO_SetI2SOutputEnable(CFALSE);
	NX_AUDIO_SetAudioBufferPCMINEnable(CFALSE);
	NX_AUDIO_SetI2SInputEnable(CFALSE);

	/* sysnc clkgen 0 and 1 */
	if (CFALSE == CFG_AUDIO_I2S_MASTER_MODE)	{
		NX_AUDIO_SetClockSource(1, 7);	/* set mclk source clock */
		msleep(1);
	}

	NX_AUDIO_SetClockDivisorEnable(CFALSE);
	NX_AUDIO_SetClockPClkMode(NX_PCLKMODE_DYNAMIC);
	NX_AUDIO_CloseModule();
}

static int start_i2s_port(struct snd_pcm_substream *substream)
{
	int ret = 0;
	int count = 10;

	DBGOUT("%s (%s)\n", __func__,
		SNDRV_PCM_STREAM_PLAYBACK == substream->stream ? "PLAY":"CAPT");

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		NX_AUDIO_SetI2SOutputEnable(CTRUE);
		NX_AUDIO_SetAudioBufferPCMOUTEnable(CTRUE);
	} else {
		NX_AUDIO_SetI2SInputEnable(CTRUE);
		NX_AUDIO_SetAudioBufferPCMINEnable(CTRUE);

		while ((CTRUE != NX_AUDIO_IsPCMInBufferReady()) && (count-- > 0))
			msleep(10);

		if (CTRUE != NX_AUDIO_IsPCMInBufferReady()) {
			printk(KERN_ERR "nexell-i2s: Error, I2S is not ready for capture\n");
			ret= -1;
		}
	}
	return ret;
}

static void stop_i2s_port(struct snd_pcm_substream *substream)
{
	DBGOUT("%s (%s)\n", __func__,
		SNDRV_PCM_STREAM_PLAYBACK == substream->stream ? "PLAY":"CAPT");

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		NX_AUDIO_SetI2SOutputEnable(CFALSE);
		NX_AUDIO_SetAudioBufferPCMOUTEnable(CFALSE);
	} else {
		NX_AUDIO_SetAudioBufferPCMINEnable(CFALSE);
		NX_AUDIO_SetI2SInputEnable(CFALSE);
	}
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

		ret = start_i2s_port(substream);
		break;

	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:

		stop_i2s_port(substream);
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
	DBGOUT("%s\n", __func__);
	return 0;
}

static int  nx_i2s_ops_set_sysclk(struct snd_soc_dai *dai, int clk_id, unsigned int freq, int dir)
{
	DBGOUT("%s\n", __func__);
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
	DBGOUT("%s\n", __func__);
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

	init_i2s_port();

	PM_DBGOUT("-%s\n", __func__);
	return 0;
}

struct snd_soc_dai nx_snd_i2s_dai = {
	.name 		= NEXELL_SND_SOC_DAI_NAME,
	.id 		= 0,
	.suspend	= nx_i2s_suspend,
	.resume 	= nx_i2s_resume,

	.playback	= {
		.channels_min 	= 2,
		.channels_max 	= 2,
		.rates 			= NEXELL_I2S_RATES,
		.formats 		= NEXELL_I2S_FORMATS,
		},

	.capture 	= {
		.channels_min 	= 2,
		.channels_max 	= 2,
		.rates 			= NEXELL_I2S_RATES,
		.formats 		= NEXELL_I2S_FORMATS,
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

	init_i2s_port();
	return snd_soc_register_dai(&nx_snd_i2s_dai);
}

static void __exit nx_snd_i2s_exit(void)
{
	DBGOUT("%s\n", __func__);

	exit_i2s_port();
	snd_soc_unregister_dai(&nx_snd_i2s_dai);
}

module_init(nx_snd_i2s_init);
module_exit(nx_snd_i2s_exit);

MODULE_AUTHOR("jhkim <jhkim@nexell.co.kr>");
MODULE_DESCRIPTION("Sound I2S driver for the Nexell");
MODULE_LICENSE("GPL");

