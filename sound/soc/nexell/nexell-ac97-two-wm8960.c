/*
 * (C) Copyright 2011
 * STcube Inc., Seungwoo Kim <ksw@stcube.com>
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
#include "nexell-ac97.h"
#include "../codecs/wm8960.h"

#define MOST2120_WM8960_DEBUG 0

#if (0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "codec: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

/*--------------------------------------------------------------------------------
 * sound soc ops
 */

#define WM8960_POBCTRL   0x80
#define WM8960_BUFDCOPEN 0x10
#define WM8960_BUFIOEN   0x08
#define WM8960_SOFT_ST   0x04
#define WM8960_HPSTBY    0x01

static int most2120_wm8960_init(struct snd_soc_codec *codec)
{
    u16 iface;
	DBGOUT("%s\n", __func__);

	/* Startup codec would need initialization...? */

	/* Enable anti-pop features */
	snd_soc_write(codec, WM8960_APOP1,
				     WM8960_POBCTRL | WM8960_SOFT_ST |
				     WM8960_BUFDCOPEN | WM8960_BUFIOEN);
    /* Now we should enable WM8960 and set WM8960 as correct status as we wanted. */
    
    snd_soc_write(codec, WM8960_POWER1, 0x1F2); //0x1F2
    /* Speaker Power Enable */
    snd_soc_write(codec, WM8960_POWER2, 0x1FF);
#if 1
    snd_soc_write(codec, WM8960_IFACE2, 0x040);  /* ADCLRC is not GPIO1 */
    snd_soc_write(codec, WM8960_ADDCTL2, 0x024); /* LRCM On/ADCRC input/HPSWEN off/HPSWPOL=1 */
    /* Set GPIO to GPIO JD */
    snd_soc_write(codec, WM8960_ADDCTL4, 0x000); /* GPIO1 is JD */
    /* Set LINVOL/RINVOL to set 0dB */
    snd_soc_write(codec, WM8960_LINVOL, 0x140 | 27); /* Set to 0dB */
    snd_soc_write(codec, WM8960_RINVOL, 0x140 | 27); /* Set to 0dB */
    /* Set ADCVOL to 0dB */
    snd_soc_write(codec, WM8960_LADC, 0x100 | 195); /* Set to 0dB */
    snd_soc_write(codec, WM8960_RADC, 0x100 | 195); /* Set to 0dB */
    /* Set MICBOOST to 20dB/no Mute */
    snd_soc_write(codec, WM8960_LINPATH, 0x100 | (0x02 << 4) | 0x08); /* Boost 20dB, Boost connect */
    snd_soc_write(codec, WM8960_RINPATH, 0x100 | (0x02 << 4) | 0x08); /* Boost 20dB, Boost connect */
    /* Set LOUT1/ROUT1 volume to set 0dB */
    snd_soc_write(codec, WM8960_LOUT1, 0x1F9);
    snd_soc_write(codec, WM8960_ROUT1, 0x1F9);
    /* Set output mixer DAC mute to off */
    snd_soc_write(codec, WM8960_LOUTMIX, 0x100);
    snd_soc_write(codec, 0x23, 0x100);
    snd_soc_write(codec, 0x24, 0x100);
    snd_soc_write(codec, WM8960_ROUTMIX, 0x100);
    /* Set LOUT2/ROUT2 volume to set 0dB */
    snd_soc_write(codec, WM8960_LOUT2, 0x1F9);
    snd_soc_write(codec, WM8960_ROUT2, 0x1F9);
    /* Power on Mixer */
    snd_soc_write(codec, WM8960_POWER3, 0x03C); // 0x03C for L,R micamp on
#endif
    snd_soc_write(codec, WM8960_CLOCK1, 0x04); /* internal SYSCLK to 1/2 of MCLK */
    snd_soc_write(codec, WM8960_DACCTL1, 0); /* Mute off */
#if 1
    //iface = snd_soc_read(codec, WM8960_IFACE1);
    iface = 0x02;
    snd_soc_write(codec, WM8960_IFACE1, iface); //  | 0x100 : ADC L,R channel would be reversed
#endif
    /* Speaker DC/AC gain to 1.5 (3.3V->5V) translate... */
    snd_soc_write(codec, WM8960_CLASSD3, 0x1B); /* DC gain to 1.52, AC gain to 1.52 */
    /* Speaker Output Driver enable */
    snd_soc_write(codec, WM8960_CLASSD1, 0xC0); /* Disables all speaker*/

	return 0;
}

static int most2120_codec_startup(struct snd_pcm_substream *substream)
{
	DBGOUT("%s\n", __func__);
	
	return 0;
}

static void most2120_codec_shutdown(struct snd_pcm_substream *substream)
{
	DBGOUT("%s\n", __func__);
}

extern int cpu_get_clock_hz(int clk); /* From Nexell's core freq module */

static int most2120_codec_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
    struct snd_soc_dai *codec_dai = rtd->dai->codec_dai;
    struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;
    unsigned int pll_out = 0, ddiv = 0, adiv = 0, bdiv;
    int ret = 0;
    int iis_clk;
#if MOST2120_WM8960_DEBUG
    int i;
#endif

    DBGOUT("Entered %s\n", __func__);
    // most2120-4ch ac97 driver don't need to implement anything.
    // Because it has fixed 16KHz record/playback capability.
    
#if MOST2120_WM8960_DEBUG
    for (i=0; i<56;i++) {
        printk("codec addr = 0x%x(%d) val= %x\n", i, i, snd_soc_read(codec_dai->codec, i));
    }
#endif
	return 0;
}

static int (*cpu_dai_resume)(struct snd_soc_dai *dai) = NULL;

static int most2120_codec_resume_pre(struct platform_device *pdev)
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
static struct snd_soc_ops most2120_codec_ops = {
	.startup 	= most2120_codec_startup,
	.shutdown 	= most2120_codec_shutdown,
	.hw_params 	= most2120_codec_hw_params,
};

static struct snd_soc_ops most2120_codec_ops2 = {
	.startup 	= most2120_codec_startup,
	.shutdown 	= most2120_codec_shutdown,
	.hw_params 	= most2120_codec_hw_params,
};


static struct snd_soc_dai_link most2120_dai_link[] = {
{
	.name 			= "most-ac97-wm8960",
	.stream_name 	= "most-ac97 HiFi",
	.codec_dai 		= &wm8960_dai,			/* codecs wm8960 */
	.cpu_dai 		= &nxp2120_ac97_dai[0],		/* nexell-ac97 */
	.init           = most2120_wm8960_init,
	.ops 			= &most2120_codec_ops,
},
{
	.name 			= "most-ac97-wm8960_2",
	.stream_name 	= "most-wm8960 ADC1",
	.codec_dai 		= &wm8960_2_dai,			/* codecs wm8960 */
	.cpu_dai 		= &nxp2120_ac97_dai[1],		/* nexell-ac97 */
	.ops 			= &most2120_codec_ops2,
},
{
	.name 			= "most-ac97-wm8960_3",
	.stream_name 	= "most-wm8960 MIC",
	.codec_dai 		= &wm8960_3_dai,			/* codecs wm8960 */
	.cpu_dai 		= &nxp2120_ac97_dai[2],		/* nexell-ac97 */
	.ops 			= &most2120_codec_ops2,
},
};

static struct snd_soc_card most2120_wm8960_card = {
	.name 		= "most-ac97",
	.platform 	= &nx_snd_pcm_platform,			/* nexell-pcm */
	.dai_link 	= &most2120_dai_link,
	.num_links 	= 3,
	.resume_pre	= &most2120_codec_resume_pre,
};

static struct wm8960_data wm8960_codec_data = {
	.dres = 0,
};

static struct snd_soc_device wm8960_soc_device = {
	.card 		= &most2120_wm8960_card,
	.codec_dev 	= &soc_codec_dev_two_wm8960,		/* codecs/wm8960 */
	.codec_data = &wm8960_codec_data,
};

/*--------------------------------------------------------------------------------
 * sound pcm platform
 ---------------------------------------------------------------------------------*/
static struct platform_device * wm8960_plat_device;

static int __init most2120_ac97_init(void)
{
	struct snd_soc_device *socdev  = &wm8960_soc_device;
	int ret = 0;

	DBGOUT("%s\n", __func__);

	wm8960_plat_device = platform_device_alloc("soc-audio", -1);
	if (! wm8960_plat_device) {
		printk(KERN_ERR "%s: fail platform_device_alloc for codec ...\n", __func__);
		return -ENOMEM;
	}

	platform_set_drvdata(wm8960_plat_device, socdev);
	socdev->dev = &wm8960_plat_device->dev;

	ret = platform_device_add(wm8960_plat_device);
	if (ret) {
		platform_device_put(wm8960_plat_device);
		return ret;
	}

	return ret;
}

static void __exit most2120_ac97_exit(void)
{
	DBGOUT("%s\n", __func__);
	platform_device_unregister(wm8960_plat_device);
}

module_init(most2120_ac97_init);
module_exit(most2120_ac97_exit);

MODULE_AUTHOR("Seungwoo Kim <ksw@stcube.com>");
MODULE_DESCRIPTION("Sound codec-ac97 TWO WM8960 driver for most2120 board");
MODULE_LICENSE("GPL");
