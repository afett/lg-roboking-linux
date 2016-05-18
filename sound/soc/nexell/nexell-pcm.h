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

#ifndef __NEXELL_PCM_H
#define __NEXELL_PCM_H

#define	NX_SND_PCM_DAI_NAME	"nx-pcm"

/* platform data */
extern struct snd_soc_platform nx_snd_pcm_platform;

#if defined(CFG_DMA_AUDIO_PLAY)
#define PCM_DMA_PLAYBACK
#define	PCM_DMA_PLAYBACK_CH		CFG_DMA_AUDIO_PLAY
#endif

#if defined(CFG_DMA_AUDIO_REC)
#define PCM_DMA_CAPTURE
#define	PCM_DMA_CAPTURE_CH		CFG_DMA_AUDIO_REC
#endif


#endif	/* __NEXELL_PCM_H */
