#
# Copyright 2010 Nexell Co.
#
# Makefile for the Nexell Sound

obj-$(CONFIG_SND_NEXELL_SOC)		+= nexell-snd-pcm.o
nexell-snd-pcm-objs					:= nexell-pcm.o

obj-$(CONFIG_SND_NEXELL_SOC_I2S)	+= nexell-snd-i2s.o
nexell-snd-i2s-objs 				:= nexell-i2s.o

obj-$(CONFIG_SND_NEXELL_SOC_AC97) 	+= nexell-snd-ac97.o
nexell-snd-ac97-objs 				:= nexell-ac97.o

obj-$(CONFIG_SND_NEXELL_SOC_SPDIF) 	+= nexell-snd-spdif.o
nexell-snd-spdif-objs 				:= nexell-spdif.o

obj-$(CONFIG_SND_CODEC_CS42L52) 	+= nexell-snd-cs42l52.o
nexell-snd-cs42l52-objs 			:= nexell-cs42l52.o

obj-$(CONFIG_SND_CODEC_RT5623) 		+= nexell-snd-rt5623.o
nexell-snd-rt5623-objs 				:= nexell-rt5623.o

obj-$(CONFIG_SND_CODEC_PS8738_8106) += nexell-snd-ps8xxx.o
nexell-snd-ps8xxx-objs 				:= nexell-ps8xxx.o

obj-$(CONFIG_SND_MOST2120_WM8960) += nexell-snd-wm8960.o
nexell-snd-wm8960-objs 				:= nexell-wm8960.o

obj-$(CONFIG_SND_MOST2120_AC97_TWO_WM8960) += nexell-snd-ac97-two-wm8960.o
nexell-snd-ac97-two-wm8960-objs    := nexell-ac97-two-wm8960.o

obj-$(CONFIG_SND_NEXELL_SPDIF_RX) += nexell-snd-spdif-rx.o
nexell-snd-spdif-rx-objs                  := nexell-spdif-rx.o

################################################################################
# Module build:
# 	Link Nexell prototype function
################################################################################

ifeq ($(CONFIG_ARCH_NXP2120), y)
CPU	:= nxp2120
endif

PROTODIR := ../../../arch/arm/mach-$(CPU)/prototype/module

ifeq ($(CONFIG_SND_NEXELL_SOC_I2S),m)
nexell-snd-i2s-objs		+= $(PROTODIR)/nx_audio.o
endif

ifeq ($(CONFIG_SND_NEXELL_SOC_AC97),m)
nexell-snd-ac97-objs	+= $(PROTODIR)/nx_audio.o
endif

