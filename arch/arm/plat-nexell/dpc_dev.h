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
#ifndef __DPC_DEV_H__
#define __DPC_DEV_H__

/*------------------------------------------------------------------------------
 * Macro:
 */
/* Layer number */
#define	RGB_0_LAYER				0	/* 0: RGB0 layer number */
#define	RGB_1_LAYER				1	/* 1: RGB1 layer number */
#define	RGB_2_LAYER				2	/* 2: RGB2 layer number */
#define	VIDEO_LAYER				3	/* 3: video layer number */

#define	RGB_LAYERS				2	/* number of RGB layer  : 0 = RGB0, 1 =RGB1 */
#define	VID_LAYERS				1	/* number of Video layer: 3 = VIDEO */
#define	ALL_LAYERS				(RGB_LAYERS + VID_LAYERS)

/* Device status */
#define	STAT_ENABLE				0
#define	STAT_DISABLE			1
#define	STAT_RESUME				2
#define	STAT_SUSPEND			4

/* others */
#define	VSYNC_WAIT_TIME			60	/* 60 msec */

/* For math func */
#define	PI 						3.141592
#define	DEGREE_RADIAN(dg)		(dg*PI/180)

typedef struct NX_MLC_RegisterSet	MLC_REG;
typedef struct NX_DPC_RegisterSet	DPC_REG;

/*------------------------------------------------------------------------------
 * struct:
 */
struct rgb_data {
	unsigned int type;
	unsigned int format;
	int			 srcw;
	int			 srch;
	int			 pixelbyte;
	/* color */
	struct {
	unsigned int	backgcolor;
	unsigned int 	transcolor;
	unsigned int	invertcolor;
	unsigned int	alpha;
	} color;
};

struct vid_data{
	unsigned int type;
	unsigned int fourcc;
	int			 srcw;
	int			 srch;
	int			 priority;
	unsigned int h_filter;
	unsigned int v_filter;
	int			 lscreen;	/* screen layer */
	/* color */
	struct {
		int		alpha;		/* Default= 15,    0 <= Range <= 16		*/
		int		bright;		/* Default= 0,  -128 <= Range <= 128	*/
		int		contrast;	/* Default= 0,     0 <= Range <= 8		*/
		double	hue;		/* Default= 0,     0 <= Range <= 360	*/
		double	saturation;	/* Default= 0,  -100 <= Range <= 100	*/
		int		satura;
		int		gamma;
	} color;
};

struct dpc_reg {
	unsigned int	  *	mlc_base;		/* MLC register base */
	unsigned int	  *	dpc_base;		/* DPC register base */
	unsigned int	  * mlc_save;		/* MLC register save ptr */
	unsigned int	  * dpc_save;		/* MLC register save ptr */
};

struct dpc_info {
	char			  * name;
	unsigned int		status;
	int					irq;
	wait_queue_head_t   wait;
	unsigned int 		wait_time;
	int 				cond;
	int					x_res;
	int					y_res;
	unsigned int		bgcolor;
	void			  *	ldata[ALL_LAYERS];	/* 0: RGB0, 1:RGB1, 3:VIDEO */
	struct dpc_reg	    regs;				/* dpc registers */
	struct dpc_blu_ops* bluops;				/* device operations */
	struct dpc_lcd_ops* lcdops;
};

/*------------------------------------------------------------------------------
 * VARS:
 */
/* to save register value when suspend mode */
static MLC_REG	mlc_reg_save[NUMBER_OF_MLC_MODULE];
static DPC_REG	dpc_reg_save[NUMBER_OF_DPC_MODULE];

/*------------------------------------------------------------------------------
 * to store layer RGB data
 */
static struct rgb_data rgb_data[NUMBER_OF_MLC_MODULE][RGB_LAYERS] = { { {0, }, }, };

/*------------------------------------------------------------------------------
 * to store layer VIDEO data
 */
static struct vid_data vid_data[NUMBER_OF_MLC_MODULE] = {
	{
		.priority 	= CFG_DISP_LAYER_VIDEO_PRIORITY,
		.lscreen  	= CFG_DISP_LAYER_SCREEN,
		.h_filter	= 0,	/* default horizontal filter */
		.v_filter	= 1,	/* default vertical filter   */
		.color		= {
			.alpha		= 15,
			.bright	 	=  0,
			.contrast	=  0,
			.hue		=  0,
			.saturation =  1,
			.satura	 	=  0,
		},
	},
};

/*------------------------------------------------------------------------------
 * Display device informatioin
 */
static struct dpc_info dpc_dev[NUMBER_OF_DPC_MODULE] = {
	 {
	 	.name 		= "1st dpc",
	 	.status		= STAT_DISABLE,
	 	.irq 		= IRQ_PHY_DPC_P,
	 	.x_res		= CFG_DISP_PRI_RESOL_WIDTH,
	 	.y_res		= CFG_DISP_PRI_RESOL_HEIGHT,
	 	.bgcolor	= CFG_DISP_BACK_GROUND_COLOR,
	 	.regs 		= {
	 		.mlc_base	= (u_int*)IO_ADDRESS(PHY_BASEADDR_MLC + (OFFSET_OF_MLC_MODULE * 0)),
	 		.dpc_base	= (u_int*)IO_ADDRESS(PHY_BASEADDR_DPC + (OFFSET_OF_DPC_MODULE * 0)),
	 		.mlc_save	= (u_int*)&mlc_reg_save[0],
	 		.dpc_save	= (u_int*)&dpc_reg_save[0],
	 	},
	 },
};

/*----------------------------------------------------------------------------*/
#define	DPC_INFO(_m)		((struct dpc_info *)&dpc_dev[_m])

#define	VID_DATA(_m)		((struct vid_data *)dpc_dev[_m].ldata[RGB_LAYERS])
#define	RGB_DATA(_m, _l)	((struct rgb_data *)dpc_dev[_m].ldata[_l])

#endif	/* __DPC_DEV_H__ */
