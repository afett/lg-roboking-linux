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
#include <linux/platform_device.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dai.h>

#include <mach/platform.h>

#include "nexell-pcm.h"
#include "nexell-i2s.h"
#include "nexell-ps8xxx.h"
#include "../codecs/ps8738_8106.h"

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "codec: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

/*--------------------------------------------------------------------------------
 * sound soc ops
 */
static int ps8xxx_codec_startup(struct snd_pcm_substream *substream)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static void ps8xxx_codec_shutdown(struct snd_pcm_substream *substream)
{
	DBGOUT("%s\n", __func__);
}

static int ps8xxx_codec_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{
	DBGOUT("%s\n", __func__);
	return 0;
}

static int (*cpu_dai_resume)(struct snd_soc_dai *dai) = NULL;

static int ps8xxx_codec_resume_pre(struct platform_device *pdev)
{
	struct snd_soc_device *socdev  = platform_get_drvdata(pdev);
	struct snd_soc_card	  *card    = socdev->card;
	struct snd_soc_dai	  *cpu_dai = card->dai_link->cpu_dai;
	int ret = 0;

	PM_DBGOUT("+%s\n", __func__);

	/*
	 * first execute cpu(i2s) resume and execute codec resume.
	 */
	if (cpu_dai->resume && ! cpu_dai_resume) {
		cpu_dai_resume  = cpu_dai->resume;
		cpu_dai->resume = NULL;
	}

	if (cpu_dai_resume)
		ret = cpu_dai_resume(cpu_dai);

	PM_DBGOUT("-%s\n", __func__);
	return ret;
}

/*--------------------------------------------------------------------------------
 * sound soc struct
 */
static struct snd_soc_ops ps8xxx_codec_ops = {
	.startup 	= ps8xxx_codec_startup,
	.shutdown 	= ps8xxx_codec_shutdown,
	.hw_params 	= ps8xxx_codec_hw_params,
};

static struct snd_soc_dai_link ps8xxx_dai_link = {
	.name 			= NEXELL_SND_CODEC_DAI_NAME,
	.stream_name 	= NEXELL_SND_CODEC_DAI_NAME,
	.codec_dai 		= &ps8xxx_dai,			/* codecs/ps8738 */
	.cpu_dai 		= &nx_snd_i2s_dai,		/* nexell-i2s */
	.ops 			= &ps8xxx_codec_ops,
};

static struct snd_soc_card ps8xxx_card = {
	.name 		= NEXELL_SND_CODEC_DAI_NAME,
	.platform 	= &nx_snd_pcm_platform,			/* nexell-pcm */
	.dai_link 	= &ps8xxx_dai_link,
	.num_links 	= 1,
	.resume_pre	= &ps8xxx_codec_resume_pre,
};

static struct ps8xxx_setup_data ps8xxx_codec_data[] = {
	/* DAC: PS8738 */
	{
		.i2c_bus	 = CFG_AUDIO_I2C_CH,
		.i2c_address = (0x38),
	},
	/* ADC: PS8106 */
	{
		.i2c_bus	 = CFG_AUDIO_I2C_CH,
		.i2c_address = (0x3A),				/* ps8106: depend SA0 level */
	}
};

static struct snd_soc_device ps8xxx_soc_device = {
	.card 		= &ps8xxx_card,
	.codec_dev 	= &soc_codec_dev_ps8xxx,		/* codecs/ps8xxx */
	.codec_data = ps8xxx_codec_data,
};

/*--------------------------------------------------------------------------------
 * sound pcm platform
 ---------------------------------------------------------------------------------*/
static struct platform_device * ps8xxx_plat_device;

static int __init nx_snd_codec_init(void)
{
	struct snd_soc_device *socdev  = &ps8xxx_soc_device;
	struct snd_soc_card	  *card    = socdev->card;
	struct snd_soc_dai	  *cpu_dai = card->dai_link->cpu_dai;
	int ret = 0;

	DBGOUT("%s\n", __func__);

	ps8xxx_plat_device = platform_device_alloc("soc-audio", -1);
	if (! ps8xxx_plat_device) {
		printk(KERN_ERR "%s: fail platform_device_alloc for codec ...\n", __func__);
		return -ENOMEM;
	}

	platform_set_drvdata(ps8xxx_plat_device, socdev);
	socdev->dev = &ps8xxx_plat_device->dev;

	ret = platform_device_add(ps8xxx_plat_device);
	if (ret) {
		platform_device_put(ps8xxx_plat_device);
		return ret;
	}

	/* PLAY i2s rates, format */
	cpu_dai->playback.rates   = SNDRV_PCM_RATE_48000;	// NEXELL_I2S_RATES;
	cpu_dai->playback.formats = NEXELL_I2S_FORMATS;

	/* CAPT i2s rates, format */
	cpu_dai->capture.rates    = SNDRV_PCM_RATE_48000;	// NEXELL_I2S_RATES;
	cpu_dai->capture.formats  = NEXELL_I2S_FORMATS;

	return ret;
}

static void __exit nx_snd_codec_exit(void)
{
	DBGOUT("%s\n", __func__);
	platform_device_unregister(ps8xxx_plat_device);
}

module_init(nx_snd_codec_init);
module_exit(nx_snd_codec_exit);

MODULE_AUTHOR("jhkim <jhkim@nexell.co.kr>");
MODULE_DESCRIPTION("Sound codec-ps8xxx driver for the Nexell");
MODULE_LICENSE("GPL");
