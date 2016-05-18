/*
 * (C) Copyright 2011 STcube Inc., 
 * All right reserved by Seungwoo Kim <ksw@stcube.com>
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
#include <sound/soc-dapm.h>
#include <sound/initval.h>

#include <mach/platform.h>

#include "nexell-pcm.h"
#include "nexell-spdif.h"

#define NEXELL_SPDIF_RX_DEBUG 0

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "spdif-rx: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

struct nx_spdif_rx_priv {
	struct snd_soc_codec codec;
	int sample_offset;
	int enable_phase_detect;
};

/*--------------------------------------------------------------------------------
 * sound soc ops
 */

static int nx_spdif_rx_init(struct snd_soc_codec *codec)
{
	DBGOUT("%s\n", __func__);

	/* May there would not have any thing to initialize...? */


	return 0;
}

static int nx_spdif_rx_startup(struct snd_pcm_substream *substream)
{
	DBGOUT("%s\n", __func__);
	
	return 0;
}

static void nx_spdif_rx_shutdown(struct snd_pcm_substream *substream)
{
	DBGOUT("%s\n", __func__);
}

static int nx_spdif_rx_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{
	/* struct snd_soc_pcm_runtime *rtd = substream->private_data;
    struct snd_soc_dai *codec_dai = rtd->dai->codec_dai;
    struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai; */

    DBGOUT("Entered %s\n", __func__);
    // nxp2120-spdif rx driver don't need to implement anything.
    // Because it does not have anything to configure.
    
	return 0;
}

static int (*cpu_dai_resume)(struct snd_soc_dai *dai) = NULL;

static int nx_spdif_rx_resume_pre(struct platform_device *pdev)
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
/*****************************************************
 * Dummy codec dai
 *****************************************************/

static int nx_dummy_spdif_rx_hw_params(struct snd_pcm_substream *substream,
			    struct snd_pcm_hw_params *params,
			    struct snd_soc_dai *dai)
{
    /* if we should do something... */
    
    return 0;
}


static int nx_dummy_spdif_rx_set_dai_fmt(struct snd_soc_dai *codec_dai,
		unsigned int fmt)
{
    /* If we should do anything... */
    
    return 0;
}

#define NX_SPDIF_RX_RATES (SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_44100)

#define NX_SPDIF_RX_FORMATS (SNDRV_PCM_FMTBIT_S24_LE)

static struct snd_soc_dai_ops nx_spdif_dai_ops = {
	.hw_params = nx_dummy_spdif_rx_hw_params,
	.set_fmt = nx_dummy_spdif_rx_set_dai_fmt,
};

struct snd_soc_dai nx_spdif_rx_dai = {
	.name = "NX-SPDIF-RX",
	.capture = {
		.stream_name = "Capture",
		.channels_min = 2,
		.channels_max = 2,
		.rates = NX_SPDIF_RX_RATES,
		.formats = NX_SPDIF_RX_FORMATS,},
	.ops = &nx_spdif_dai_ops,
};

/***************************************************** 
 * Dummy codec device 
 *****************************************************/
static struct snd_soc_codec *spdif_rx_codec;

static int spdif_rx_codec_init()
{
    struct nx_spdif_rx_priv *nx_spdif_rx;
    struct snd_soc_codec *codec;
    int ret = 0;
    
    nx_spdif_rx = kzalloc(sizeof(struct nx_spdif_rx_priv), GFP_KERNEL);
	if (nx_spdif_rx == NULL)
		return -ENOMEM;

	codec = &nx_spdif_rx->codec;

    mutex_init(&codec->mutex);
	INIT_LIST_HEAD(&codec->dapm_widgets);
	INIT_LIST_HEAD(&codec->dapm_paths);

	codec->private_data = &nx_spdif_rx;
	codec->name = "nx-spdif-rx";
	codec->owner = THIS_MODULE;
	codec->bias_level = SND_SOC_BIAS_OFF;
	codec->set_bias_level = NULL;
	codec->dai = &nx_spdif_rx_dai;
	codec->num_dai = 1;

	ret = snd_soc_register_codec(codec);
	if (ret != 0) {
		dev_err(codec->dev, "Failed to register codec: %d\n", ret);
		goto err;
	}

	ret = snd_soc_register_dai(&nx_spdif_rx_dai);
	if (ret != 0) {
		dev_err(codec->dev, "Failed to register DAI: %d\n", ret);
		goto err_codec;
	}

	nx_spdif_rx_dai.dev = codec->dev;
	spdif_rx_codec = codec;
	
	return 0;

err_codec:
    snd_soc_unregister_codec(codec);
err:
    return ret;
}

static int spdif_rx_codec_exit()
{
    struct nx_spdif_rx_priv *nx_spdif_rx;

    nx_spdif_rx = nx_spdif_rx_dai.codec->private_data; 
    snd_soc_unregister_dai(&nx_spdif_rx_dai);
	snd_soc_unregister_codec(nx_spdif_rx_dai.codec);

	kfree(nx_spdif_rx);
	
	return 0;
}

static int spdif_rx_probe(struct platform_device *pdev)
{
    struct snd_soc_device *socdev = platform_get_drvdata(pdev);
    struct nx_spdif_rx_priv *nx_spdif_rx;
    struct snd_soc_codec *codec;
    int ret = 0;
    
    codec = spdif_rx_codec;
    if (NULL == codec) {
        dev_err(&pdev->dev, "Codec device not registered\n");
		return -ENODEV;
    }
	socdev->card->codec = spdif_rx_codec;

    ret = snd_soc_new_pcms(socdev, SNDRV_DEFAULT_IDX1, SNDRV_DEFAULT_STR1);
	if (ret < 0) {
		dev_err(codec->dev, "failed to create pcms: %d\n", ret);
		goto pcm_err;
	}
	return 0;

pcm_err:
	snd_soc_free_pcms(socdev);
	snd_soc_dapm_free(socdev);
	
	return ret;
}

static int spdif_rx_remove(struct platform_device *pdev)
{
    struct snd_soc_device *socdev = platform_get_drvdata(pdev);

	snd_soc_free_pcms(socdev);
	snd_soc_dapm_free(socdev);
	
	return 0;
}

struct snd_soc_codec_device soc_codec_dev_spdif_rx = {
	.probe = 	spdif_rx_probe,
	.remove = 	spdif_rx_remove,
	.suspend = 	NULL,
	.resume =	NULL,
};
EXPORT_SYMBOL_GPL(soc_codec_dev_spdif_rx);
/*--------------------------------------------------------------------------------
 * sound soc struct
 */
static struct snd_soc_ops nx_spdif_rx_ops = {
	.startup 	= nx_spdif_rx_startup,
	.shutdown 	= nx_spdif_rx_shutdown,
	.hw_params 	= nx_spdif_rx_hw_params,
};


static struct snd_soc_dai_link nxp2120_dai_link = {
	.name 			= "nexell-spdif",
	.stream_name 	= "nexll spdif HiFi",
	.codec_dai 		= &nx_spdif_rx_dai,			/* codecs(none: spdif-rx) */
	.cpu_dai 		= &nxp2120_spdif_dai,		/* nexell-spdif */
	.init           = nx_spdif_rx_init,
	.ops 			= &nx_spdif_rx_ops,
};

static struct snd_soc_card nxp2120_spdif_rx_card = {
	.name 		= "nexell-spdif",
	.platform 	= &nx_snd_pcm_platform,			/* nexell-pcm */
	.dai_link 	= &nxp2120_dai_link,
	.num_links 	= 1,
	.resume_pre	= &nx_spdif_rx_resume_pre,
};

static struct snd_soc_device spdif_rx_soc_device = {
	.card 		= &nxp2120_spdif_rx_card,
	.codec_dev 	= &soc_codec_dev_spdif_rx,
};

/*--------------------------------------------------------------------------------
 * sound pcm platform
 ---------------------------------------------------------------------------------*/
static struct platform_device * spdif_rx_plat_device;

static int __init nxp2120_spdif_rx_init(void)
{
	struct snd_soc_device *socdev  = &spdif_rx_soc_device;
	int ret = 0;

	DBGOUT("%s\n", __func__);

	/* register codec first */
	spdif_rx_codec_init();
	spdif_rx_plat_device = platform_device_alloc("soc-audio", 1);
	if (! spdif_rx_plat_device) {
		printk(KERN_ERR "%s: fail platform_device_alloc for codec ...\n", __func__);
		return -ENOMEM;
	}

	platform_set_drvdata(spdif_rx_plat_device, socdev);
	socdev->dev = &spdif_rx_plat_device->dev;

	ret = platform_device_add(spdif_rx_plat_device);
	if (ret) {
		platform_device_put(spdif_rx_plat_device);
		return ret;
	}

	return ret;
}

static void __exit nxp2120_spdif_rx_exit(void)
{
	DBGOUT("%s\n", __func__);
	platform_device_unregister(spdif_rx_plat_device);
	spdif_rx_codec_exit();
}

module_init(nxp2120_spdif_rx_init);
module_exit(nxp2120_spdif_rx_exit);

MODULE_AUTHOR("Seungwoo Kim <ksw@stcube.com>");
MODULE_DESCRIPTION("SPDIF rx driver for nxp2120 chip");
MODULE_LICENSE("GPL");