/*
 * two_wm8960.c  --  WM8960 ALSA SoC Audio driver
 *
 * Author: Liam Girdwood
 * Modified by Seungwoo Kim, <ksw@stcube.com>
 * Heavily modified for two WM8960 uses.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <sound/tlv.h>

#include "wm8960.h"

#define AUDIO_NAME "two_wm8960"

struct snd_soc_codec *wm8960_0 = NULL;
struct snd_soc_codec *wm8960_1 = NULL;
struct snd_soc_codec_device soc_codec_dev_two_wm8960;

/* R25 - Power 1 */
#define WM8960_VREF      0x40

/* R28 - Anti-pop 1 */
#define WM8960_POBCTRL   0x80
#define WM8960_BUFDCOPEN 0x10
#define WM8960_BUFIOEN   0x08
#define WM8960_SOFT_ST   0x04
#define WM8960_HPSTBY    0x01

/* R29 - Anti-pop 2 */
#define WM8960_DISOP     0x40

/*
 * wm8960 register cache
 * We can't read the WM8960 register space when we are
 * using 2 wire for device control, so we cache them instead.
 */
static const u16 wm8960_reg[WM8960_CACHEREGNUM] = {
	0x0097, 0x0097, 0x0000, 0x0000,
	0x0000, 0x0008, 0x0000, 0x000a,
	0x01c0, 0x0000, 0x00ff, 0x00ff,
	0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x007b, 0x0100, 0x0032,
	0x0000, 0x00c3, 0x00c3, 0x01c0,
	0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000,
	0x0100, 0x0100, 0x0050, 0x0050,
	0x0050, 0x0050, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0040, 0x0000,
	0x0000, 0x0050, 0x0050, 0x0000,
	0x0002, 0x0037, 0x004d, 0x0080,
	0x0008, 0x0031, 0x0026, 0x00e9,
};

struct wm8960_priv {
	u16 reg_cache0[WM8960_CACHEREGNUM];
	u16 reg_cache1[WM8960_CACHEREGNUM];
	struct snd_soc_codec codec0;
	struct snd_soc_codec codec1;
};

#define wm8960_reset(c)	snd_soc_write(c, WM8960_RESET, 0)

static inline void two_wm8960_write_reg_cache(struct snd_soc_codec *codec,
	unsigned int reg, unsigned int value);

static int wm8960_cap_volume_info(struct snd_kcontrol *kcontrol,
				    struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 4;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = 63;
	return 0;
}

static int wm8960_cap_volume_get(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
    struct snd_soc_codec *codec0 = wm8960_0;
    struct snd_soc_codec *codec1 = wm8960_1;

	ucontrol->value.integer.value[0] = codec0->read(codec0, WM8960_LINVOL) & 63;
	ucontrol->value.integer.value[1] = codec0->read(codec0, WM8960_RINVOL) & 63;
	ucontrol->value.integer.value[2] = codec1->read(codec1, WM8960_LINVOL) & 63;
	ucontrol->value.integer.value[3] = codec1->read(codec1, WM8960_RINVOL) & 63;

	return 0;
}

//static int two_wm8960_write(struct snd_soc_codec *codec, unsigned int reg, unsigned int value)
static int wm8960_cap_volume_put(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec[2] = { wm8960_0, wm8960_1 };
	unsigned int input_regs[2] = { WM8960_LINVOL, WM8960_RINVOL};
	unsigned int reg;
	unsigned int value;
	unsigned int vol;
    int change = 0;
	int index;
	int channel;
	int i;
    u8 data[3];

	for ( i = 0 ; i < 4 ; i ++)
	{
		index   = i / 2;
		channel = i % 2;

		reg = input_regs[channel];

		vol   = codec[index]->read(codec[index], reg);
		value = clamp_t(int, ucontrol->value.integer.value[i], 0, 63);

		if ((vol & 63) == value)
			continue;

		value  = (vol & ~(63)) | value;

	    //printk("wm8960 write: reg=%x, val = %x\n", reg, value);
	    two_wm8960_write_reg_cache(codec[index], reg, value); /* write cache is enough to one */

	    data[0] = ((reg << 1) & 0xFE) | ((value >> 8) & 1); /* 7bit addr and 8th bit data */
	    data[1] = (value) & 0xFF;

        if (codec[index]->hw_write(codec[index]->control_data, data, 2) == 1)
            change += 1;
    }

	return change;
}

/* enumerated controls */
static const char *wm8960_deemph[] = {"None", "32Khz", "44.1Khz", "48Khz"};
static const char *wm8960_polarity[] = {"No Inversion", "Left Inverted",
	"Right Inverted", "Stereo Inversion"};
static const char *wm8960_3d_upper_cutoff[] = {"High", "Low"};
static const char *wm8960_3d_lower_cutoff[] = {"Low", "High"};
static const char *wm8960_alcfunc[] = {"Off", "Right", "Left", "Stereo"};
static const char *wm8960_alcmode[] = {"ALC", "Limiter"};
static const char *wm8960_recordmode[] = {"Stereo", "Left", "Right", "Swap Left&Right"};
static const char *wm8960_speakermode[] = {"All off","Left only","Right only", "Stereo"};
static const char *wm8960_micbias[] = {"High", "Low"};
static const char *wm8960_micboost[] = {"+0dB", "+13dB", "+20dB", "+29dB"};

static const struct soc_enum wm8960_enum[] = {
	SOC_ENUM_SINGLE(WM8960_DACCTL1, 1, 4, wm8960_deemph),
	SOC_ENUM_SINGLE(WM8960_DACCTL1, 5, 4, wm8960_polarity),
	SOC_ENUM_SINGLE(WM8960_DACCTL2, 5, 4, wm8960_polarity),
	SOC_ENUM_SINGLE(WM8960_3D, 6, 2, wm8960_3d_upper_cutoff),
	SOC_ENUM_SINGLE(WM8960_3D, 5, 2, wm8960_3d_lower_cutoff),
	SOC_ENUM_SINGLE(WM8960_ALC1, 7, 4, wm8960_alcfunc),
	SOC_ENUM_SINGLE(WM8960_ALC3, 8, 2, wm8960_alcmode),
	SOC_ENUM_SINGLE(WM8960_ADDCTL1, 2, 4, wm8960_recordmode),
	SOC_ENUM_SINGLE(WM8960_CLASSD1, 6, 4, wm8960_speakermode),
	SOC_ENUM_SINGLE(WM8960_ADDCTL4, 0, 2, wm8960_micbias),
	SOC_ENUM_SINGLE(WM8960_LINPATH, 4, 4, wm8960_micboost),
	SOC_ENUM_SINGLE(WM8960_RINPATH, 4, 4, wm8960_micboost),
};

static const DECLARE_TLV_DB_SCALE(adc_tlv, -9700, 50, 1);
static const DECLARE_TLV_DB_SCALE(dac_tlv, -12700, 50, 1);
static const DECLARE_TLV_DB_SCALE(bypass_tlv, -2100, 300, 0);
static const DECLARE_TLV_DB_SCALE(out_tlv, -12100, 100, 1);
static const DECLARE_TLV_DB_SCALE(in_tlv, -1725, 75, 0);

static const struct snd_kcontrol_new wm8960_snd_controls[] = {
SOC_DOUBLE_R_TLV("Capture Volume", WM8960_LINVOL, WM8960_RINVOL,
		 0, 63, 0, in_tlv),
SOC_DOUBLE_R("Capture Volume ZC Switch", WM8960_LINVOL, WM8960_RINVOL,
	6, 1, 0),
SOC_DOUBLE_R("Capture Switch", WM8960_LINVOL, WM8960_RINVOL,
	7, 1, 0),

SOC_DOUBLE_R_TLV("Playback Volume", WM8960_LDAC, WM8960_RDAC,
		 0, 255, 0, dac_tlv),

SOC_DOUBLE_R_TLV("Headphone Playback Volume", WM8960_LOUT1, WM8960_ROUT1,
		 0, 127, 0, out_tlv),
SOC_DOUBLE_R("Headphone Playback ZC Switch", WM8960_LOUT1, WM8960_ROUT1,
	7, 1, 0),

SOC_DOUBLE_R_TLV("Speaker Playback Volume", WM8960_LOUT2, WM8960_ROUT2,
		 0, 127, 0, out_tlv),
SOC_DOUBLE_R("Speaker Playback ZC Switch", WM8960_LOUT2, WM8960_ROUT2,
	7, 1, 0),
SOC_ENUM("Speaker Playback Off Switch", wm8960_enum[8]),
SOC_SINGLE("Speaker DC Volume", WM8960_CLASSD3, 3, 5, 0),
SOC_SINGLE("Speaker AC Volume", WM8960_CLASSD3, 0, 5, 0),

SOC_SINGLE("PCM Playback -6dB Switch", WM8960_DACCTL1, 7, 1, 0),
SOC_ENUM("ADC Polarity", wm8960_enum[1]),
SOC_ENUM("Playback De-emphasis", wm8960_enum[0]),
SOC_SINGLE("ADC High Pass Filter Switch", WM8960_DACCTL1, 0, 1, 0),
SOC_ENUM("Record channels", wm8960_enum[7]),

SOC_ENUM("DAC Polarity", wm8960_enum[2]),

SOC_ENUM("3D Filter Upper Cut-Off", wm8960_enum[3]),
SOC_ENUM("3D Filter Lower Cut-Off", wm8960_enum[4]),
SOC_SINGLE("3D Volume", WM8960_3D, 1, 15, 0),
SOC_SINGLE("3D Switch", WM8960_3D, 0, 1, 0),

SOC_ENUM("ALC Function", wm8960_enum[5]),
SOC_SINGLE("ALC Max Gain", WM8960_ALC1, 4, 7, 0),
SOC_SINGLE("ALC Target", WM8960_ALC1, 0, 15, 1),
SOC_SINGLE("ALC Min Gain", WM8960_ALC2, 4, 7, 0),
SOC_SINGLE("ALC Hold Time", WM8960_ALC2, 0, 15, 0),
SOC_ENUM("ALC Mode", wm8960_enum[6]),
SOC_SINGLE("ALC Decay", WM8960_ALC3, 4, 15, 0),
SOC_SINGLE("ALC Attack", WM8960_ALC3, 0, 15, 0),

SOC_SINGLE("Noise Gate Threshold", WM8960_NOISEG, 3, 31, 0),
SOC_SINGLE("Noise Gate Switch", WM8960_NOISEG, 0, 1, 0),

SOC_DOUBLE_R_TLV("ADC PCM Capture Volume", WM8960_LADC, WM8960_RADC,
	0, 255, 0, adc_tlv),
SOC_ENUM("Left PGA Boost Gain", wm8960_enum[10]),
SOC_ENUM("Right PGA Boost Gain", wm8960_enum[11]),

SOC_SINGLE_TLV("Left Output Mixer Boost Bypass Volume",
	       WM8960_BYPASS1, 4, 7, 1, bypass_tlv),
SOC_SINGLE_TLV("Left Output Mixer LINPUT3 Volume",
	       WM8960_LOUTMIX, 4, 7, 1, bypass_tlv),
SOC_SINGLE_TLV("Right Output Mixer Boost Bypass Volume",
	       WM8960_BYPASS2, 4, 7, 1, bypass_tlv),
SOC_SINGLE_TLV("Right Output Mixer RINPUT3 Volume",
	       WM8960_ROUTMIX, 4, 7, 1, bypass_tlv),
SOC_ENUM("Microphone Bias", wm8960_enum[9]),

	{
		.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
		.name  = "Capture MIC Capture Volume",
		.info  = wm8960_cap_volume_info,
		.get   = wm8960_cap_volume_get,
		.put   = wm8960_cap_volume_put,
	},
};

static const struct snd_kcontrol_new wm8960_lin_boost[] = {
SOC_DAPM_SINGLE("LINPUT2 Switch", WM8960_LINPATH, 6, 1, 0),
SOC_DAPM_SINGLE("LINPUT3 Switch", WM8960_LINPATH, 7, 1, 0),
SOC_DAPM_SINGLE("LINPUT1 Switch", WM8960_LINPATH, 8, 1, 0),
};

static const struct snd_kcontrol_new wm8960_lin[] = {
SOC_DAPM_SINGLE("Boost Switch", WM8960_LINPATH, 3, 1, 0),
};

static const struct snd_kcontrol_new wm8960_rin_boost[] = {
SOC_DAPM_SINGLE("RINPUT2 Switch", WM8960_RINPATH, 6, 1, 0),
SOC_DAPM_SINGLE("RINPUT3 Switch", WM8960_RINPATH, 7, 1, 0),
SOC_DAPM_SINGLE("RINPUT1 Switch", WM8960_RINPATH, 8, 1, 0),
};

static const struct snd_kcontrol_new wm8960_rin[] = {
SOC_DAPM_SINGLE("Boost Switch", WM8960_RINPATH, 3, 1, 0),
};

static const struct snd_kcontrol_new wm8960_loutput_mixer[] = {
SOC_DAPM_SINGLE("PCM Playback Switch", WM8960_LOUTMIX, 8, 1, 0),
SOC_DAPM_SINGLE("LINPUT3 Switch", WM8960_LOUTMIX, 7, 1, 0),
SOC_DAPM_SINGLE("Boost Bypass Switch", WM8960_BYPASS1, 7, 1, 0),
};

static const struct snd_kcontrol_new wm8960_routput_mixer[] = {
SOC_DAPM_SINGLE("PCM Playback Switch", WM8960_ROUTMIX, 8, 1, 0),
SOC_DAPM_SINGLE("RINPUT3 Switch", WM8960_ROUTMIX, 7, 1, 0),
SOC_DAPM_SINGLE("Boost Bypass Switch", WM8960_BYPASS2, 7, 1, 0),
};

static const struct snd_kcontrol_new wm8960_mono_out[] = {
SOC_DAPM_SINGLE("Left Switch", WM8960_MONOMIX1, 7, 1, 0),
SOC_DAPM_SINGLE("Right Switch", WM8960_MONOMIX2, 7, 1, 0),
};

static const struct snd_soc_dapm_widget wm8960_dapm_widgets[] = {
SND_SOC_DAPM_INPUT("LINPUT1"),
SND_SOC_DAPM_INPUT("RINPUT1"),
SND_SOC_DAPM_INPUT("LINPUT2"),
SND_SOC_DAPM_INPUT("RINPUT2"),
SND_SOC_DAPM_INPUT("LINPUT3"),
SND_SOC_DAPM_INPUT("RINPUT3"),

SND_SOC_DAPM_MICBIAS("MICB", WM8960_POWER1, 1, 0),

SND_SOC_DAPM_MIXER("Left Boost Mixer", WM8960_POWER1, 5, 0,
		   wm8960_lin_boost, ARRAY_SIZE(wm8960_lin_boost)),
SND_SOC_DAPM_MIXER("Right Boost Mixer", WM8960_POWER1, 4, 0,
		   wm8960_rin_boost, ARRAY_SIZE(wm8960_rin_boost)),

SND_SOC_DAPM_MIXER("Left Input Mixer", WM8960_POWER3, 5, 0,
		   wm8960_lin, ARRAY_SIZE(wm8960_lin)),
SND_SOC_DAPM_MIXER("Right Input Mixer", WM8960_POWER3, 4, 0,
		   wm8960_rin, ARRAY_SIZE(wm8960_rin)),

SND_SOC_DAPM_ADC("Left ADC", "Capture", WM8960_POWER1, 3, 0),
SND_SOC_DAPM_ADC("Right ADC", "Capture", WM8960_POWER1, 2, 0),

SND_SOC_DAPM_DAC("Left DAC", "Playback", WM8960_POWER2, 8, 0),
SND_SOC_DAPM_DAC("Right DAC", "Playback", WM8960_POWER2, 7, 0),

SND_SOC_DAPM_MIXER("Left Output Mixer", WM8960_POWER3, 3, 0,
	&wm8960_loutput_mixer[0],
	ARRAY_SIZE(wm8960_loutput_mixer)),
SND_SOC_DAPM_MIXER("Right Output Mixer", WM8960_POWER3, 2, 0,
	&wm8960_routput_mixer[0],
	ARRAY_SIZE(wm8960_routput_mixer)),

SND_SOC_DAPM_MIXER("Mono Output Mixer", WM8960_POWER2, 1, 0,
	&wm8960_mono_out[0],
	ARRAY_SIZE(wm8960_mono_out)),

SND_SOC_DAPM_PGA("LOUT1 PGA", WM8960_POWER2, 6, 0, NULL, 0),
SND_SOC_DAPM_PGA("ROUT1 PGA", WM8960_POWER2, 5, 0, NULL, 0),

SND_SOC_DAPM_PGA("Left Speaker PGA", WM8960_POWER2, 4, 0, NULL, 0),
SND_SOC_DAPM_PGA("Right Speaker PGA", WM8960_POWER2, 3, 0, NULL, 0),

SND_SOC_DAPM_PGA("Right Speaker Output", WM8960_CLASSD1, 7, 0, NULL, 0),
SND_SOC_DAPM_PGA("Left Speaker Output", WM8960_CLASSD1, 6, 0, NULL, 0),

SND_SOC_DAPM_OUTPUT("SPK_LP"),
SND_SOC_DAPM_OUTPUT("SPK_LN"),
SND_SOC_DAPM_OUTPUT("HP_L"),
SND_SOC_DAPM_OUTPUT("HP_R"),
SND_SOC_DAPM_OUTPUT("SPK_RP"),
SND_SOC_DAPM_OUTPUT("SPK_RN"),
SND_SOC_DAPM_OUTPUT("OUT3"),
};

static const struct snd_soc_dapm_route audio_paths[] = {
	{ "Left Boost Mixer", "LINPUT1 Switch", "LINPUT1" },
	{ "Left Boost Mixer", "LINPUT2 Switch", "LINPUT2" },
	{ "Left Boost Mixer", "LINPUT3 Switch", "LINPUT3" },

	{ "Left Input Mixer", "Boost Switch", "Left Boost Mixer", },
	{ "Left Input Mixer", NULL, "LINPUT1", },  /* Really Boost Switch */
	{ "Left Input Mixer", NULL, "LINPUT2" },
	{ "Left Input Mixer", NULL, "LINPUT3" },

	{ "Right Boost Mixer", "RINPUT1 Switch", "RINPUT1" },
	{ "Right Boost Mixer", "RINPUT2 Switch", "RINPUT2" },
	{ "Right Boost Mixer", "RINPUT3 Switch", "RINPUT3" },

	{ "Right Input Mixer", "Boost Switch", "Right Boost Mixer", },
	{ "Right Input Mixer", NULL, "RINPUT1", },  /* Really Boost Switch */
	{ "Right Input Mixer", NULL, "RINPUT2" },
	{ "Right Input Mixer", NULL, "LINPUT3" },

	{ "Left ADC", NULL, "Left Input Mixer" },
	{ "Right ADC", NULL, "Right Input Mixer" },

	{ "Left Output Mixer", "LINPUT3 Switch", "LINPUT3" },
	{ "Left Output Mixer", "Boost Bypass Switch", "Left Boost Mixer"} ,
	{ "Left Output Mixer", "PCM Playback Switch", "Left DAC" },

	{ "Right Output Mixer", "RINPUT3 Switch", "RINPUT3" },
	{ "Right Output Mixer", "Boost Bypass Switch", "Right Boost Mixer" } ,
	{ "Right Output Mixer", "PCM Playback Switch", "Right DAC" },

	{ "Mono Output Mixer", "Left Switch", "Left Output Mixer" },
	{ "Mono Output Mixer", "Right Switch", "Right Output Mixer" },

	{ "LOUT1 PGA", NULL, "Left Output Mixer" },
	{ "ROUT1 PGA", NULL, "Right Output Mixer" },

	{ "HP_L", NULL, "LOUT1 PGA" },
	{ "HP_R", NULL, "ROUT1 PGA" },

	{ "Left Speaker PGA", NULL, "Left Output Mixer" },
	{ "Right Speaker PGA", NULL, "Right Output Mixer" },

	{ "Left Speaker Output", NULL, "Left Speaker PGA" },
	{ "Right Speaker Output", NULL, "Right Speaker PGA" },

	{ "SPK_LN", NULL, "Left Speaker Output" },
	{ "SPK_LP", NULL, "Left Speaker Output" },
	{ "SPK_RN", NULL, "Right Speaker Output" },
	{ "SPK_RP", NULL, "Right Speaker Output" },

	{ "OUT3", NULL, "Mono Output Mixer", }
};

static int wm8960_add_widgets(struct snd_soc_codec *codec)
{
	snd_soc_dapm_new_controls(codec, wm8960_dapm_widgets,
				  ARRAY_SIZE(wm8960_dapm_widgets));

	snd_soc_dapm_add_routes(codec, audio_paths, ARRAY_SIZE(audio_paths));

	return 0;
}

static int wm8960_set_dai_fmt(struct snd_soc_dai *codec_dai,
		unsigned int fmt)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	u16 iface = snd_soc_read(codec, WM8960_IFACE1) & 0x1F0;

	/* set master/slave audio interface */
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		iface |= 0x0040;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		break;
	default:
		return -EINVAL;
	}

	/* interface format */
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		iface |= 0x0002;
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		iface |= 0x0001;
		break;
	case SND_SOC_DAIFMT_DSP_A:
		iface |= 0x0003;
		break;
	case SND_SOC_DAIFMT_DSP_B:
		iface |= 0x0013;
		break;
	default:
		return -EINVAL;
	}

	/* clock inversion */
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		break;
	case SND_SOC_DAIFMT_IB_IF:
		iface |= 0x0090;
		break;
	case SND_SOC_DAIFMT_IB_NF:
		iface |= 0x0080;
		break;
	case SND_SOC_DAIFMT_NB_IF:
		iface |= 0x0010;
		break;
	default:
		return -EINVAL;
	}

	/* set iface */
	snd_soc_write(codec, WM8960_IFACE1, iface);
	return 0;
}

static int wm8960_hw_params(struct snd_pcm_substream *substream,
			    struct snd_pcm_hw_params *params,
			    struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_device *socdev = rtd->socdev;
	struct snd_soc_codec *codec = socdev->card->codec;

	u16 iface = snd_soc_read(codec, WM8960_IFACE1) & 0xfff3;

	/* bit size */
	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16_LE:
		break;
	case SNDRV_PCM_FORMAT_S20_3LE:
		iface |= 0x0004;
		break;
	case SNDRV_PCM_FORMAT_S24_LE:
		iface |= 0x0008;
		break;
	}

	/* set iface */
	snd_soc_write(codec, WM8960_IFACE1, iface);

	return 0;
}

static int wm8960_mute(struct snd_soc_dai *dai, int mute)
{
	struct snd_soc_codec *codec = dai->codec;
	u16 mute_reg = snd_soc_read(codec, WM8960_DACCTL1) & 0xfff7;

	if (mute)
		snd_soc_write(codec, WM8960_DACCTL1, mute_reg | 0x8);
	else
		snd_soc_write(codec, WM8960_DACCTL1, mute_reg);
	return 0;
}

static int wm8960_set_bias_level(struct snd_soc_codec *codec,
				 enum snd_soc_bias_level level)
{
	struct wm8960_data *pdata = codec->dev->platform_data;
	u16 reg;

	switch (level) {
	case SND_SOC_BIAS_ON:
		break;

	case SND_SOC_BIAS_PREPARE:
		/* Set VMID to 2x50k */
		reg = snd_soc_read(codec, WM8960_POWER1);
		reg &= ~0x180;
		reg |= 0x80;
		snd_soc_write(codec, WM8960_POWER1, reg);
		break;

	case SND_SOC_BIAS_STANDBY:
		if (codec->bias_level == SND_SOC_BIAS_OFF) {
			/* Enable anti-pop features */
			snd_soc_write(codec, WM8960_APOP1,
				     WM8960_POBCTRL | WM8960_SOFT_ST |
				     WM8960_BUFDCOPEN | WM8960_BUFIOEN);

			/* Discharge HP output */
			reg = WM8960_DISOP;
			if (pdata)
				reg |= pdata->dres << 4;
			snd_soc_write(codec, WM8960_APOP2, reg);

			msleep(400);

			snd_soc_write(codec, WM8960_APOP2, 0);

			/* Enable & ramp VMID at 2x50k */
			reg = snd_soc_read(codec, WM8960_POWER1);
			reg |= 0x80;
			snd_soc_write(codec, WM8960_POWER1, reg);
			msleep(100);

			/* Enable VREF */
			snd_soc_write(codec, WM8960_POWER1, reg | WM8960_VREF);

			/* Disable anti-pop features */
			snd_soc_write(codec, WM8960_APOP1, WM8960_BUFIOEN);
		}

		/* Set VMID to 2x250k */
		reg = snd_soc_read(codec, WM8960_POWER1);
		reg &= ~0x180;
		reg |= 0x100;
		snd_soc_write(codec, WM8960_POWER1, reg);
		break;

	case SND_SOC_BIAS_OFF:
		/* Enable anti-pop features */
		snd_soc_write(codec, WM8960_APOP1,
			     WM8960_POBCTRL | WM8960_SOFT_ST |
			     WM8960_BUFDCOPEN | WM8960_BUFIOEN);

		/* Disable VMID and VREF, let them discharge */
		snd_soc_write(codec, WM8960_POWER1, 0);
		msleep(600);

		snd_soc_write(codec, WM8960_APOP1, 0);
		break;
	}

	codec->bias_level = level;

	return 0;
}

/* PLL divisors */
struct _pll_div {
	u32 pre_div:1;
	u32 n:4;
	u32 k:24;
};

/* The size in bits of the pll divide multiplied by 10
 * to allow rounding later */
#define FIXED_PLL_SIZE ((1 << 24) * 10)

static int pll_factors(unsigned int source, unsigned int target,
		       struct _pll_div *pll_div)
{
	unsigned long long Kpart;
	unsigned int K, Ndiv, Nmod;

	pr_debug("WM8960 PLL: setting %dHz->%dHz\n", source, target);

	/* Scale up target to PLL operating frequency */
	target *= 8;

	Ndiv = target / source;
	//printk("Ndiv = %d\n", Ndiv);
	if (Ndiv < 6) {
		source >>= 1;
		pll_div->pre_div = 1;
		Ndiv = target / source;
	} else
		pll_div->pre_div = 0;

	if ((Ndiv < 6) || (Ndiv > 12)) {
		pr_err("WM8960 PLL: Unsupported N=%d\n", Ndiv);
		return -EINVAL;
	}

	pll_div->n = Ndiv;
	Nmod = target % source;
	Kpart = FIXED_PLL_SIZE * (long long)Nmod;

	//printk("Kpart = %d\n", Kpart);

	do_div(Kpart, source);

	K = Kpart & 0xFFFFFFFF;

	/* Check if we need to round */
	if ((K % 10) >= 5)
		K += 5;

	/* Move down to proper range now rounding is done */
	K /= 10;

	pll_div->k = K;

	//printk("K = %d\n", K);

	pr_debug("WM8960 PLL: N=%x K=%x pre_div=%d\n",
		 pll_div->n, pll_div->k, pll_div->pre_div);

	return 0;
}

static int wm8960_set_dai_pll(struct snd_soc_dai *codec_dai, int pll_id,
		int source, unsigned int freq_in, unsigned int freq_out)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	u16 reg;
	static struct _pll_div pll_div;
	int ret;

    //printk("WM8960 PLL: in =%d out= %d \n", freq_in, freq_out);

	if (freq_in && freq_out) {
		ret = pll_factors(freq_in, freq_out, &pll_div);
		if (ret != 0)
			return ret;
	}

	/* Disable the PLL: even if we are changing the frequency the
	 * PLL needs to be disabled while we do so. */
	snd_soc_write(codec, WM8960_CLOCK1,
		     snd_soc_read(codec, WM8960_CLOCK1) & ~7);
	snd_soc_write(codec, WM8960_POWER2,
		     snd_soc_read(codec, WM8960_POWER2) & ~1);

	if (!freq_in || !freq_out)
		return 0;

	reg = snd_soc_read(codec, WM8960_PLL1) & ~0x3f;
	reg |= pll_div.pre_div << 4;
	reg |= pll_div.n;

	if (pll_div.k) {
		reg |= 0x20;

#if 0
		snd_soc_write(codec, WM8960_PLL2, (pll_div.k >> 18) & 0x3f);
		snd_soc_write(codec, WM8960_PLL3, (pll_div.k >> 9) & 0x1ff);
		snd_soc_write(codec, WM8960_PLL4, pll_div.k & 0x1ff);
#else
		snd_soc_write(codec, WM8960_PLL2, (pll_div.k >> 16) & 0xff);
		snd_soc_write(codec, WM8960_PLL3, (pll_div.k >> 8) & 0x0ff);
		snd_soc_write(codec, WM8960_PLL4, pll_div.k & 0xff);
#endif
	}
	snd_soc_write(codec, WM8960_PLL1, reg);

	/* Turn it on */
	snd_soc_write(codec, WM8960_POWER2,
		     snd_soc_read(codec, WM8960_POWER2) | 1);
	msleep(250);
	snd_soc_write(codec, WM8960_CLOCK1,
		     snd_soc_read(codec, WM8960_CLOCK1) | 1 | (0x02 << 1));

	return 0;
}

static int wm8960_set_dai_clkdiv(struct snd_soc_dai *codec_dai,
		int div_id, int div)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	u16 reg;

	switch (div_id) {
	case WM8960_SYSCLKSEL:
		reg = snd_soc_read(codec, WM8960_CLOCK1) & 0x1fe;
		snd_soc_write(codec, WM8960_CLOCK1, reg | div);
		break;
	case WM8960_SYSCLKDIV:
		reg = snd_soc_read(codec, WM8960_CLOCK1) & 0x1f9;
		snd_soc_write(codec, WM8960_CLOCK1, reg | div);
		break;
	case WM8960_DACDIV:
		reg = snd_soc_read(codec, WM8960_CLOCK1) & 0x1c7;
		snd_soc_write(codec, WM8960_CLOCK1, reg | div);
		break;
	case WM8960_ADCDIV:
		reg = snd_soc_read(codec, WM8960_CLOCK1) & 0x03F;
		snd_soc_write(codec, WM8960_CLOCK1, reg | div);
		break;
	case WM8960_OPCLKDIV:
		reg = snd_soc_read(codec, WM8960_PLL1) & 0x03f;
		snd_soc_write(codec, WM8960_PLL1, reg | div);
		break;
	case WM8960_DCLKDIV:
		reg = snd_soc_read(codec, WM8960_CLOCK2) & 0x03f;
		snd_soc_write(codec, WM8960_CLOCK2, reg | div);
		break;
	case WM8960_TOCLKSEL:
		reg = snd_soc_read(codec, WM8960_ADDCTL1) & 0x1fd;
		snd_soc_write(codec, WM8960_ADDCTL1, reg | div);
		break;
	case WM8960_BCLKDIV:
		reg = snd_soc_read(codec, WM8960_CLOCK2) & 0x1F0;
		snd_soc_write(codec, WM8960_CLOCK2, reg | div);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

#define WM8960_RATES SNDRV_PCM_RATE_8000_48000

#define WM8960_FORMATS \
	(SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE | \
	SNDRV_PCM_FMTBIT_S24_LE)

static struct snd_soc_dai_ops wm8960_dai_ops = {
	.hw_params = wm8960_hw_params,
	.digital_mute = wm8960_mute,
	.set_fmt = wm8960_set_dai_fmt,
	.set_clkdiv = wm8960_set_dai_clkdiv,
	.set_pll = wm8960_set_dai_pll,
};

struct snd_soc_dai wm8960_dai = {
	.name = "WM8960",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 1,
		.channels_max = 2,
		.rates = WM8960_RATES,
		.formats = WM8960_FORMATS,},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 1,
		.channels_max = 2,
		.rates = WM8960_RATES,
		.formats = WM8960_FORMATS,},
	.ops = &wm8960_dai_ops,
	.symmetric_rates = 1,
};
EXPORT_SYMBOL_GPL(wm8960_dai);

struct snd_soc_dai wm8960_2_dai = {
	.name = "WM8960_2",
	/*.playback = {
		.stream_name = "Playback",
		.channels_min = 1,
		.channels_max = 2,
		.rates = WM8960_RATES,
		.formats = WM8960_FORMATS,}, */
	.capture = {
		.stream_name = "Capture",
		.channels_min = 1,
		.channels_max = 2,
		.rates = WM8960_RATES,
		.formats = WM8960_FORMATS,},
	.ops = &wm8960_dai_ops,
	.symmetric_rates = 1,
};
EXPORT_SYMBOL_GPL(wm8960_2_dai);

struct snd_soc_dai wm8960_3_dai = {
	.name = "WM8960_3",
	/*.playback = {
		.stream_name = "Playback",
		.channels_min = 1,
		.channels_max = 2,
		.rates = WM8960_RATES,
		.formats = WM8960_FORMATS,}, */
	.capture = {
		.stream_name = "Capture",
		.channels_min = 1,
		.channels_max = 2,
		.rates = WM8960_RATES,
		.formats = WM8960_FORMATS,},
	.ops = &wm8960_dai_ops,
	.symmetric_rates = 1,
};
EXPORT_SYMBOL_GPL(wm8960_3_dai);

static int wm8960_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;

	wm8960_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

static int wm8960_resume(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;
	struct snd_soc_codec *codec1 = wm8960_1;
	int i;
	u8 data[2];
	u16 *cache = codec->reg_cache;

	/* Sync reg_cache with the hardware */
	for (i = 0; i < ARRAY_SIZE(wm8960_reg); i++) {
		data[0] = (i << 1) | ((cache[i] >> 8) & 0x0001);
		data[1] = cache[i] & 0x00ff;
		codec->hw_write(codec->control_data, data, 2);
		codec->hw_write(codec1->control_data, data, 2);
	}

	wm8960_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
	wm8960_set_bias_level(codec, codec->suspend_bias_level);
	return 0;
}

/*
 * read wm89690 register cache
 */
static inline unsigned int two_wm8960_read_reg_cache(struct snd_soc_codec *codec,
	unsigned int reg)
{
    u16 *cache = codec->reg_cache;
    BUG_ON(reg > (ARRAY_SIZE(wm8960_reg)) - 1);
    return cache[reg];
}

/*
 * write wm8960 register cache
 */
static inline void two_wm8960_write_reg_cache(struct snd_soc_codec *codec,
	unsigned int reg, unsigned int value)
{
    u16 *cache = codec->reg_cache;

    /* Reset register and reserved registers are uncached */
    if (reg < 0 || reg > ARRAY_SIZE(wm8960_reg) - 1)
        return;

    cache[reg] = value;
}

/*
 * write to the wm8990 register space
 */
static int two_wm8960_write(struct snd_soc_codec *codec, unsigned int reg,
	unsigned int value)
{
    struct snd_soc_codec *codec0 = wm8960_0;
    struct snd_soc_codec *codec1 = wm8960_1;
    int success = 0;
    u8 data[3];

    data[0] = ((reg << 1) & 0xFE) | ((value >> 8) & 1); /* 7bit addr and 8th bit data */
    data[1] = (value) & 0xFF;

    two_wm8960_write_reg_cache(codec0, reg, value); /* write cache is enough to one */
    two_wm8960_write_reg_cache(codec1, reg, value); /* write cache is enough to one */

    //printk("wm8960 write: reg=%x, val = %x\n", reg, value);
    if (reg == WM8960_RESET) {
        codec->hw_write(codec0->control_data, data, 1);
        codec->hw_write(codec1->control_data, data, 1);
        return 0;
    } else {
        if (codec->hw_write(codec0->control_data, data, 2) == 1)
            success = 1;
        if (codec->hw_write(codec1->control_data, data, 2) == 1)
            success += 2;
        if (3 == success)
            return 0;
        else
            return -EIO;
    }
}

static int wm8960_probe(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec;
	int ret = 0;

	if ((wm8960_0 == NULL) || (wm8960_1 == NULL)) {
		dev_err(&pdev->dev, "Codec device not registered\n");
		return -ENODEV;
	}

	socdev->card->codec = wm8960_0;
	codec = wm8960_0;

	/* register pcms */
	ret = snd_soc_new_pcms(socdev, SNDRV_DEFAULT_IDX1, SNDRV_DEFAULT_STR1);
	if (ret < 0) {
		dev_err(codec->dev, "failed to create pcms: %d\n", ret);
		goto pcm_err;
	}

	snd_soc_add_controls(codec, wm8960_snd_controls,
			     ARRAY_SIZE(wm8960_snd_controls));
	wm8960_add_widgets(codec);

	//snd_soc_add_controls(wm8960_1, wm8960_snd_controls,
	//		     ARRAY_SIZE(wm8960_snd_controls));
	//wm8960_add_widgets(wm8960_1);


	return ret;

pcm_err:
	return ret;
}

/* power down chip */
static int wm8960_remove(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);

	snd_soc_free_pcms(socdev);
	snd_soc_dapm_free(socdev);

	return 0;
}

struct snd_soc_codec_device soc_codec_dev_two_wm8960 = {
	.probe = 	wm8960_probe,
	.remove = 	wm8960_remove,
	.suspend = 	wm8960_suspend,
	.resume =	wm8960_resume,
};
EXPORT_SYMBOL_GPL(soc_codec_dev_two_wm8960);

static int wm8960_register(struct wm8960_priv *wm8960,
			   enum snd_soc_control_type control)
{
	struct wm8960_data *pdata = wm8960->codec0.dev->platform_data;
	struct snd_soc_codec *codec0 = &wm8960->codec0;
	struct snd_soc_codec *codec1 = &wm8960->codec1;
	int ret;
	u16 reg;

	if (wm8960_0 || wm8960_1) {
		dev_err(codec0->dev, "Another WM8960 0/1 is registered\n");
		ret = -EINVAL;
		goto err;
	}

	if (!pdata) {
		dev_warn(codec0->dev, "No platform data supplied for codec0 and codec1\n");
	} else {
		if (pdata->dres > WM8960_DRES_MAX) {
			dev_err(codec0->dev, "Invalid DRES: %d\n", pdata->dres);
			pdata->dres = 0;
		}
	}

	mutex_init(&codec0->mutex);
	INIT_LIST_HEAD(&codec0->dapm_widgets);
	INIT_LIST_HEAD(&codec0->dapm_paths);
	mutex_init(&codec1->mutex);
	INIT_LIST_HEAD(&codec1->dapm_widgets);
	INIT_LIST_HEAD(&codec1->dapm_paths);

	codec0->private_data = wm8960;
	codec0->name = "WM8960";
	codec0->owner = THIS_MODULE;
	codec0->bias_level = SND_SOC_BIAS_OFF;
	codec0->set_bias_level = wm8960_set_bias_level;
	codec0->dai = &wm8960_dai;
	codec0->num_dai = 1;
	codec0->reg_cache_size = WM8960_CACHEREGNUM;
	codec0->reg_cache = &wm8960->reg_cache0;

	codec1->private_data = wm8960;
	codec1->name = "WM8960_2";
	codec1->owner = THIS_MODULE;
	codec1->bias_level = SND_SOC_BIAS_OFF;
	codec1->set_bias_level = wm8960_set_bias_level;
	codec1->dai = &wm8960_2_dai;
	codec1->num_dai = 1;
	codec1->reg_cache_size = WM8960_CACHEREGNUM;
	codec1->reg_cache = &wm8960->reg_cache1;

	memcpy(codec0->reg_cache, wm8960_reg, sizeof(wm8960_reg));
	memcpy(codec1->reg_cache, wm8960_reg, sizeof(wm8960_reg));
#if 0
	memcpy(codec->reg_cache, wm8960_reg, sizeof(wm8960_reg));

	ret = snd_soc_codec_set_cache_io(codec, 7, 9, control);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set cache I/O: %d\n", ret);
		goto err;
	}
#endif
	codec0->read = two_wm8960_read_reg_cache;
	codec0->write = two_wm8960_write;

#if defined(CONFIG_I2C) || defined(CONFIG_I2C_MODULE)
    codec0->hw_write = (hw_write_t)i2c_master_send;
#endif
	codec1->read = two_wm8960_read_reg_cache;
	codec1->write = two_wm8960_write;
	codec1->hw_write = codec0->hw_write;

	wm8960_0 = codec0;
	wm8960_1 = codec1;

	ret = wm8960_reset(codec0);
	if (ret < 0) {
		dev_err(codec0->dev, "Failed to issue reset\n");
		goto err;
	}

	wm8960_dai.dev = codec0->dev;

	wm8960_set_bias_level(codec0, SND_SOC_BIAS_STANDBY);

	/* Latch the update bits */
	reg = snd_soc_read(codec0, WM8960_LINVOL);
	snd_soc_write(codec0, WM8960_LINVOL, reg | 0x100);
	reg = snd_soc_read(codec0, WM8960_RINVOL);
	snd_soc_write(codec0, WM8960_RINVOL, reg | 0x100);
	reg = snd_soc_read(codec0, WM8960_LADC);
	snd_soc_write(codec0, WM8960_LADC, reg | 0x100);
	reg = snd_soc_read(codec0, WM8960_RADC);
	snd_soc_write(codec0, WM8960_RADC, reg | 0x100);
	reg = snd_soc_read(codec0, WM8960_LDAC);
	snd_soc_write(codec0, WM8960_LDAC, reg | 0x100);
	reg = snd_soc_read(codec0, WM8960_RDAC);
	snd_soc_write(codec0, WM8960_RDAC, reg | 0x100);
	reg = snd_soc_read(codec0, WM8960_LOUT1);
	snd_soc_write(codec0, WM8960_LOUT1, reg | 0x100);
	reg = snd_soc_read(codec0, WM8960_ROUT1);
	snd_soc_write(codec0, WM8960_ROUT1, reg | 0x100);
	reg = snd_soc_read(codec0, WM8960_LOUT2);
	snd_soc_write(codec0, WM8960_LOUT2, reg | 0x100);
	reg = snd_soc_read(codec0, WM8960_ROUT2);
	snd_soc_write(codec0, WM8960_ROUT2, reg | 0x100);

	ret = snd_soc_register_codec(wm8960_0);
	if (ret != 0) {
		dev_err(codec0->dev, "Failed to register codec: %d\n", ret);
		goto err;
	}

	ret = snd_soc_register_codec(wm8960_1);
	if (ret != 0) {
		dev_err(codec0->dev, "Failed to register codec: %d\n", ret);
		goto err;
	}

	ret = snd_soc_register_dai(&wm8960_dai);
	if (ret != 0) {
		dev_err(codec0->dev, "Failed to register DAI: %d\n", ret);
		goto err_codec;
	}

	ret = snd_soc_register_dai(&wm8960_2_dai);
	if (ret != 0) {
		dev_err(codec0->dev, "Failed to register DAI: %d\n", ret);
		goto err_codec;
	}

	return 0;

err_codec:
	snd_soc_unregister_codec(codec0);
	snd_soc_unregister_codec(codec1);
err:
	return ret;
}

static void wm8960_unregister(struct wm8960_priv *wm8960)
{
	wm8960_set_bias_level(&wm8960->codec0, SND_SOC_BIAS_OFF);
	snd_soc_unregister_dai(&wm8960_dai);
	snd_soc_unregister_dai(&wm8960_2_dai);
	snd_soc_unregister_codec(&wm8960->codec0);
	snd_soc_unregister_codec(&wm8960->codec1);
	wm8960_0 = NULL;
	wm8960_1 = NULL;
}

static struct wm8960_priv *wm8960;

static int wm8960_i2c_probe(struct i2c_client *i2c,
				      const struct i2c_device_id *id)
{

	struct snd_soc_codec *codec0;

	codec0 = &wm8960->codec0;

	i2c_set_clientdata(i2c, wm8960);
	codec0->control_data = i2c;

	codec0->dev = &i2c->dev;

	return 0;
}

static int wm8960_i2c_remove(struct i2c_client *client)
{
	return 0;
}

static int wm8960_i2c_probe2(struct i2c_client *i2c,
				      const struct i2c_device_id *id)
{
	struct snd_soc_codec *codec1;

	codec1 = &wm8960->codec1;

	i2c_set_clientdata(i2c, wm8960);
	codec1->control_data = i2c;

	codec1->dev = &i2c->dev;

	return wm8960_register(wm8960, SND_SOC_I2C);
}

static int wm8960_i2c_remove2(struct i2c_client *client)
{
	struct wm8960_priv *wm8960 = i2c_get_clientdata(client);
	wm8960_unregister(wm8960);
	return 0;
}

static const struct i2c_device_id wm8960_i2c_id[] = {
	{ "wm8960", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, wm8960_i2c_id);

static const struct i2c_device_id wm8960_i2c_id_2[] = {
	{ "wm8960_2", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, wm8960_i2c_id_2);

static struct i2c_driver wm8960_i2c_driver = {
	.driver = {
		.name = "WM8960 I2C Codec",
		.owner = THIS_MODULE,
	},
	.probe =    wm8960_i2c_probe,
	.remove =   __devexit_p(wm8960_i2c_remove),
	.id_table = wm8960_i2c_id,
};

static struct i2c_driver wm8960_i2c_driver_2 = {
	.driver = {
		.name = "WM8960 I2C Codec(2nd)",
		.owner = THIS_MODULE,
	},
	.probe =    wm8960_i2c_probe2,
	.remove =   __devexit_p(wm8960_i2c_remove2),
	.id_table = wm8960_i2c_id_2,
};

static int __init wm8960_modinit(void)
{
	int ret;

	wm8960 = kzalloc(sizeof(struct wm8960_priv), GFP_KERNEL);
	if (wm8960 == NULL)
		return -ENOMEM;

	ret = i2c_add_driver(&wm8960_i2c_driver);
	if (ret != 0) {
		printk(KERN_ERR "Failed to register WM8960 I2C driver: %d\n",
		       ret);
	}
	ret = i2c_add_driver(&wm8960_i2c_driver_2);
	if (ret != 0) {
		printk(KERN_ERR "Failed to register 2nd WM8960 I2C driver: %d\n",
		       ret);
	}

	return ret;
}
module_init(wm8960_modinit);

static void __exit wm8960_exit(void)
{
	i2c_del_driver(&wm8960_i2c_driver);
	i2c_del_driver(&wm8960_i2c_driver_2);
	kfree(wm8960);
}
module_exit(wm8960_exit);


MODULE_DESCRIPTION("ASoC two WM8960 driver");
MODULE_AUTHOR("Seungwoo Kim <ksw@stcube.com>");
MODULE_LICENSE("GPL");
