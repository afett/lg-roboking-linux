/*------------------------------------------------------------------------------
 *
 *	Copyright (C) 2005 Nexell Co., Ltd All Rights Reserved
 *	Nexell Co. Proprietary & Confidential
 *
 *	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 *  AND	WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 *  BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
 *  FOR A PARTICULAR PURPOSE.
 *
 *	Module     : System memory config
 *	Description:
 *	Author     : Platform Team
 *	Export     :
 *	History    :
 *	   2009/05/13 first implementation
 ------------------------------------------------------------------------------*/
#ifndef __CFG_MAIN_H__
#define __CFG_MAIN_H__

#include <cfg_sys.h>

/*------------------------------------------------------------------------------
 * 	System Name
 */
#define	CFG_SYS_CPU_NAME						"nxp2120"
#define	CFG_SYS_BOARD_NAME						"nxp2120-dtk"

/*------------------------------------------------------------------------------
 * 	System PLL / BUS config
 */
/* cpu clock : fclk mclk bclk pclk */
#define	CFG_SYS_CLKPWR_UPDATE					CFALSE
#define	CFG_SYS_CLKPWR_SYNC_BUS					CFALSE

/* memory timing config : not stable */
#define CFG_SYS_MCUD_UPDATE						CFALSE
#define	CFG_SYS_MCUD_DLLRESET					CFALSE

/* memory bus arbiter config  */
#define CFG_SYS_UPDATE_FASTCH					CFALSE
#define CFG_SYS_UPDATE_SLOWCH					CFALSE

/*------------------------------------------------------------------------------
 * 	Debug Uart
 */
#define CFG_UART_DEBUG_CH						0
#define	CFG_UART_DEBUG_CLKSRC					CFG_SYS_CLKSRC_PLL1
#define	CFG_UART_DEBUG_CLKDIV					26 		// PLL1 147,500,000 Hz = 40 / PLL1 192,000,000 Hz = 26
#define	CFG_UART_DEBUG_CLKFREQ					(CFG_SYS_PLL1_FREQ/CFG_UART_DEBUG_CLKDIV)
#define CFG_UART_DEBUG_BAUDRATE					115200
#define CFG_UART_DEBUG_USE_UART					CTRUE

/*------------------------------------------------------------------------------
 * 	Timer List
 */
#define	CFG_TIMER_SYS_TICK_CH					0
#define	CFG_TIMER_SYS_TICK_CLKSRC				CFG_SYS_CLKSRC_PLL1
#define	CFG_TIMER_SYS_TICK_CLKDIV				64 		// (N divider:1~256)
#define	CFG_TIMER_SYS_TICK_CLKFREQ				(CFG_SYS_PLL1_FREQ/CFG_TIMER_SYS_TICK_CLKDIV)

#define	CFG_TIMER_SUB_TICK_CH					1		// For Timer driver
#define	CFG_TIMER_SUB_TICK_CLKSRC				CFG_SYS_CLKSRC_PLL1
#define	CFG_TIMER_SUB_TICK_CLKDIV				192		// (N divider:1~256)
#define	CFG_TIMER_SUB_TICK_CLKFREQ				(CFG_SYS_PLL1_FREQ/CFG_TIMER_SUB_TICK_CLKDIV)

#define	CFG_TIMER_WDT_TICK_CH					2		// For Timer driver
#define	CFG_TIMER_WDT_TICK_CLKSRC				CFG_SYS_CLKSRC_PLL1
#define	CFG_TIMER_WDT_TICK_CLKDIV				192		// (N divider:1~256)
#define	CFG_TIMER_WDT_TICK_CLKFREQ				(CFG_SYS_PLL1_FREQ/CFG_TIMER_SUB_TICK_CLKDIV)

/*------------------------------------------------------------------------------
 * 	Extern Ethernet
 */
#define CFG_EXT_PHY_BASEADDR_ETHER          	0x04000000	// DM9000: CS1
#define	CFG_EXT_IRQ_NUM_ETHER					(IRQ_GPIO_C_START + 12)

/*------------------------------------------------------------------------------
 * 	Nand
 */

/* nand interface */
#define CFG_NAND_ECC_MODE               		8			/* 4,8,16 */
#define CFG_NAND_ECC_LIMIT             			6			/* CFG_NAND_ECC_MODE - multi cell bit */
#define CFG_NAND_ECC_SIZE	              		512
#define CFG_NAND_ECC_BYTES               		13			/* 4=6.5 8=13, 16= 6 */

#define CFG_NAND_MLC_ECC_MODE              		16

#define CFG_NAND_AUTORESET             			CTRUE
#define CFG_NAND_WRITEPROTECT					CFALSE

#define	CFG_NAND_OPTIONS						NAND_NO_READRDY	/* For Hynix 64M (sector=16K, page=512) */
/*------------------------------------------------------------------------------
 * 	Display (DPC and MLC)
 */
/* oot status */
#define CFG_DISP_PRI_BOOT_LOGO                 	CTRUE
#define CFG_DISP_PRI_BOOT_ENB   	           	CTRUE

#define CFG_DISP_MAIN_SCREEN                 	MAIN_SCREEN_PRI	// FIX
#define CFG_DISP_HW_CURSOR_ENB              	CFALSE

/* MLC layer order */
#define CFG_DISP_LAYER_CURSOR                   0
#define CFG_DISP_LAYER_SCREEN                   1
#define CFG_DISP_LAYER_GRP3D                    1
#define CFG_DISP_LAYER_VIDEO                    3
#define CFG_DISP_LAYER_VIDEO_PRIORITY           2	// 0, 1, 2, 3

#define CFG_DISP_LAYER_CURSOR_NAME              "Cursor"
#define CFG_DISP_LAYER_SCREEN_NAME              "Window"
#define CFG_DISP_LAYER_GRP3D_NAME               "Grp3D"
#define CFG_DISP_LAYER_VIDEO_NAME               "Video"

/* MLC screen layer format */
#define CFG_DISP_SCREEN_RGB_FORMAT              MLC_RGBFMT_R5G6B5
#define CFG_DISP_SCREEN_PIXEL_BYTE	            2
#define CFG_DISP_SCREEN_COLOR_KEY	            0x090909
#define CFG_DISP_BACK_GROUND_COLOR	            0x0

/* Primary Display Module Sync */
#define CFG_DISP_PRI_RESOL_WIDTH          		800	// X Resolution
#define CFG_DISP_PRI_RESOL_HEIGHT				480	// Y Resolution

#define CFG_DISP_PRI_HSYNC_SYNC_WIDTH           2
#define CFG_DISP_PRI_HSYNC_FRONT_PORCH          8
#define CFG_DISP_PRI_HSYNC_BACK_PORCH           16
#define CFG_DISP_PRI_HSYNC_ACTIVE_HIGH          CFALSE
#define CFG_DISP_PRI_VSYNC_SYNC_WIDTH           1
#define CFG_DISP_PRI_VSYNC_FRONT_PORCH          7
#define CFG_DISP_PRI_VSYNC_BACK_PORCH           7
#define CFG_DISP_PRI_VSYNC_ACTIVE_HIGH 	        CFALSE
#define CFG_DISP_PRI_VSYNC_START_OFFSET			1
#define CFG_DISP_PRI_VSYNC_END_OFFSET			1

#define CFG_DISP_PRI_EVSYNC_ACTIVE_HEIGHT       1	/* Not used */
#define CFG_DISP_PRI_EVSYNC_SYNC_WIDTH          1	/* Not used */
#define CFG_DISP_PRI_EVSYNC_FRONT_PORCH         1 	/* Not used */
#define CFG_DISP_PRI_EVSYNC_BACK_PORCH          1	/* Not used */
#define CFG_DISP_PRI_EVSYNC_START_OFFSET		1	/* Not used */
#define CFG_DISP_PRI_EVSYNC_END_OFFSET			1	/* Not used */

#define CFG_DISP_PRI_SYNC_DELAY_RGB_PVD			0
#define CFG_DISP_PRI_SYNC_DELAY_HS_CP1			7
#define CFG_DISP_PRI_SYNC_DELAY_VS_FRAM			7
#define CFG_DISP_PRI_SYNC_DELAY_DE_CP2			7

#define CFG_DISP_PRI_CLKGEN0_SOURCE             DPC_VCLK_SRC_PLL1
#define CFG_DISP_PRI_CLKGEN0_DIV                6
#define CFG_DISP_PRI_CLKGEN0_DELAY              0
#define CFG_DISP_PRI_CLKGEN1_SOURCE             DPC_VCLK_SRC_VCLK2
#define CFG_DISP_PRI_CLKGEN1_DIV                1
#define CFG_DISP_PRI_CLKGEN1_DELAY              0
#define CFG_DISP_PRI_PADCLKSEL                  DPC_PADCLKSEL_VCLK
#define CFG_DISP_PRI_OUT_CLK_INVERT             CFALSE

#define CFG_DISP_PRI_OUT_FORMAT                 DPC_FORMAT_RGB888
#define CFG_DISP_PRI_OUT_DUAL_VIEW              CFALSE
#define CFG_DISP_PRI_OUT_YCORDER                DPC_YCORDER_CbYCrY
#define CFG_DISP_PRI_OUT_RGB                    CTRUE
#define CFG_DISP_PRI_OUT_INTERLACE              CFALSE
#define CFG_DISP_PRI_OUT_POL_INVERT             CFALSE

#define CFG_DISP_PRI_MLC_INTERLACE              CFALSE
#define	CFG_DISP_PRI_MLC_LOCKSIZE				8

/* Primary out encoder */
#define CFG_CVBS_ENC_PRI_OUT_FORMAT		   		ENC_OUT_FMT_NONE
#define CFG_CVBS_ENC_PRI_OUT_TYPE               ENC_OUT_TYPE_RGB

/*------------------------------------------------------------------------------
 * 	I2C
 */
#define	CFG_I2C0_CLOCK_SOURCE					256	/* 0=GPIO, I2C=16 or 256 */
#define	CFG_I2C0_CLOCK_SCALER					1	/* 1 ~ 16 */
#define	CFG_I2C0_CLOCK_DELAY					10

#define	CFG_I2C1_CLOCK_SOURCE					256	/* 0=GPIO, I2C=16 or 256 */
#define	CFG_I2C1_CLOCK_SCALER					1
#define	CFG_I2C1_CLOCK_DELAY					10

/*------------------------------------------------------------------------------
 * 	Audio I2S
 */
#define	CFG_AUDIO_I2S_MASTER_MODE				CFALSE	// CTRUE
#define	CFG_AUDIO_I2S_CLK_SRC_0					PWM_CLK_SRC_PLL1
#define	CFG_AUDIO_I2S_CLK_DIV_0					16
#define	CFG_AUDIO_I2S_CLK_INV_0					CFALSE
#define	CFG_AUDIO_I2S_CLK_SRC_1					3		// 7
#define	CFG_AUDIO_I2S_CLK_DIV_1					1		// 4
#define	CFG_AUDIO_I2S_CLK_INV_1					CFALSE
#define	CFG_AUDIO_I2S_SYNC_PERIOD				48
#define	CFG_AUDIO_I2S_PCM_OUT_WIDTH				16
#define	CFG_AUDIO_I2S_PCM_IN_WIDTH				16
#define	CFG_AUDIO_I2S_TRANS_MODE				0 	/* 0 = I2S, 2 = Left-Justified, 3 = Right-Justified  */

#define	CFG_AUDIO_I2S_RATES_48000				// CFG_AUDIO_I2S_RATES_44100
#define	CFG_AUDIO_I2S_CLK_OUT_INV				CFALSE

/* i2c channel */
#define	CFG_AUDIO_I2C_CH						0
#define	CFG_AUDIO_I2C_ADDRESS					(0x38)
/*------------------------------------------------------------------------------
 * 	PWM
 */
#define CFG_PWM_CLK_SOURCE						PWM_CLK_SRC_PLL1
#define CFG_PWM_CLK_FREQ						CFG_SYS_PLL1_FREQ
#define CFG_PWM_CLK_DIV							64
#define CFG_PWM_PERIOD							100		/* must be 100 (%) */

#define CFG_LCD_PRI_BLU_ON						CTRUE	/* PWM */
#define CFG_LCD_PRI_LCD_ON						CTRUE	/* LCD controller */

#define CFG_LCD_PRI_PWM_CH						0
#define CFG_LCD_PRI_PWM_FREQ					300
#define CFG_LCD_PRI_PWM_DUTYCYCLE				50		/* (%) */

/*------------------------------------------------------------------------------
 * 	DMA List
 */
#define CFG_DMA_AUDIO_PLAY						0
#define CFG_DMA_AUDIO_REC						1
#define CFG_DMA_SDHC_RW							2

/*------------------------------------------------------------------------------
 * 	Touchscreen
 */
#define CFG_TOUCH_X_ADC_CH						0
#define CFG_TOUCH_Y_ADC_CH						1
#define CFG_TOUCH_X_REV_VAL						CFALSE
#define CFG_TOUCH_Y_REV_VAL						CTRUE
#define CFG_TOUCH_CALIBRATION					{ 55369, 128, -1209148, -227, 37772, -1851600, 65536 }

/*------------------------------------------------------------------------------
 * 	Keypad
 */
#define CFG_KEYPAD_KEY_BUTTON					{ PAD_GPIO_ALV + 0, PAD_GPIO_ALV + 1, PAD_GPIO_ALV + 2, PAD_GPIO_ALV + 3 }
#define CFG_KEYPAD_KEY_CODE						{ KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT }

/*------------------------------------------------------------------------------
 * 	Suspend mode and Reset
 */
#define CFG_PWR_SLEEP_MODE_ENB					CTRUE
#define CFG_PWR_SLEEP_SIGNATURE					0x50575200	/* PWR (ASCII) */

#define CFG_PWR_SLEEP_PAD_HOLD_GROUP0			CFALSE		/* IO Power Group 0 ( RX0 ~ RX4 )	*/
#define CFG_PWR_SLEEP_PAD_HOLD_GROUP1			CFALSE		/* IO Power Group 1 ( USB VBUS )	*/
#define CFG_PWR_SLEEP_PAD_HOLD_GROUP2			CFALSE		/* IO Power Group 2 ( GPIO )		*/

/* Wakeup Source : ALIVE [0~7] */
#define CFG_PWR_WAKEUP_SRC_ALIVE0				CFALSE
#define CFG_PWR_WAKEUP_MOD_ALIVE0				PWR_DECT_FALLINGEDGE
#define CFG_PWR_WAKEUP_SRC_ALIVE1				CFALSE
#define CFG_PWR_WAKEUP_MOD_ALIVE1				PWR_DECT_FALLINGEDGE
#define CFG_PWR_WAKEUP_SRC_ALIVE2				CFALSE
#define CFG_PWR_WAKEUP_MOD_ALIVE2				PWR_DECT_FALLINGEDGE
#define CFG_PWR_WAKEUP_SRC_ALIVE3				CTRUE
#define CFG_PWR_WAKEUP_MOD_ALIVE3				PWR_DECT_FALLINGEDGE

/* Wakeup Source : RTC / VDD power toggle */
#define CFG_PWR_WAKEUP_SRC_VDDTOGGLE			CFALSE
#define CFG_PWR_WAKEUP_SRC_RTC					CTRUE

/* Reset Source : ALIVE [0~7] */
#define CFG_PWR_RESET_SRC_ALIVE0				CFALSE
#define CFG_PWR_RESET_SRC_ALIVE1				CFALSE
#define CFG_PWR_RESET_SRC_ALIVE2				CFALSE
#define CFG_PWR_RESET_SRC_ALIVE3				CFALSE

/* Check clock power pad configure */
#if (CFG_PWR_WAKEUP_SRC_ALIVE0 == CTRUE && CFG_PWR_RESET_SRC_ALIVE0 == CTRUE)
	#error "Wake-up and Reset Enable are not allowed at the same time for the ALIVE 0."
#endif
#if (CFG_PWR_WAKEUP_SRC_ALIVE1 == CTRUE && CFG_PWR_RESET_SRC_ALIVE1 == CTRUE)
	#error "Wake-up and Reset Enable are not allowed at the same time for the ALIVE 1."
#endif
#if (CFG_PWR_WAKEUP_SRC_ALIVE2 == CTRUE && CFG_PWR_RESET_SRC_ALIVE2 == CTRUE)
	#error "Wake-up and Reset Enable are not allowed at the same time for the ALIVE 2."
#endif
#if (CFG_PWR_WAKEUP_SRC_ALIVE3 == CTRUE && CFG_PWR_RESET_SRC_ALIVE3 == CTRUE)
	#error "Wake-up and Reset Enable are not allowed at the same time for the ALIVE 3."
#endif

#endif /* __CFG_MAIN_H__ */
