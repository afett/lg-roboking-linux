/*
 *  Copyright(c) 2011 STCube co., ltd.
 *  All right reserved by Seungwoo Kim <ksw@stcube.com>
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
#include <linux/types.h>
#include <linux/platform_device.h>

#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <linux/vmalloc.h>
#include <asm/pgtable.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/map_desc.h>

#if defined(CONFIG_MODEL_RKSII_CI)
	#undef  CFG_SYS_CPU_NAME
	#define CFG_SYS_CPU_NAME "RKSII_CI"

#elif defined(CONFIG_MODEL_RKSII_VR)
	#undef  CFG_SYS_CPU_NAME
	#define CFG_SYS_CPU_NAME "RKSII_VR"

#elif defined(CONFIG_MODEL_RKSII_VR_FEMALE)
	#undef  CFG_SYS_CPU_NAME
	#define CFG_SYS_CPU_NAME "RKSII_VR_FEMALE"

#elif defined(CONFIG_MODEL_RKSII_VR_MALE)
	#undef  CFG_SYS_CPU_NAME
	#define CFG_SYS_CPU_NAME "RKSII_VR_MALE"

#elif defined(CONFIG_MODEL_RKSII_VI)
	#undef  CFG_SYS_CPU_NAME
	#define CFG_SYS_CPU_NAME "RKSII_VI"

#elif defined(CONFIG_MODEL_RKN)
	#undef  CFG_SYS_CPU_NAME
	#define CFG_SYS_CPU_NAME "RKN"

#elif defined(CONFIG_MODEL_RK_HIT_V2)
	#undef  CFG_SYS_CPU_NAME
	#define CFG_SYS_CPU_NAME "RK_HIT_V2"

#elif defined(CONFIG_MODEL_RK_HIT_V4)
	#undef  CFG_SYS_CPU_NAME
	#define CFG_SYS_CPU_NAME "RK_HIT_V4"

#elif defined(CONFIG_MODEL_RK_HIT_3DSENS)
	#undef  CFG_SYS_CPU_NAME
	#define CFG_SYS_CPU_NAME "RK_HIT_3DSENS"
#endif

#if (0)
#define DBGOUT(msg...)		{ printk("cpu: " msg); }
#define _IOMAP()		{							\
	int i = 0;										\
	for (; i<ARRAY_SIZE(cpu_iomap_desc); i++) 		\
		printk(KERN_INFO "cpu: iomap[%2d]: p 0x%08x -> v 0x%08x len=0x%x\n", i,	\
			(u_int)(cpu_iomap_desc[i].pfn<<12),		\
			(u_int)(cpu_iomap_desc[i].virtual),		\
			(u_int)(cpu_iomap_desc[i].length));		\
	}
#else
#define DBGOUT(msg...)		do {} while (0)
#define _IOMAP()
#endif

/* extern for cpu */
extern void __init 		cpu_base_init(void);
extern void __init 		cpu_init_irq(void);
extern void __init 		cpu_soc_init(void);
extern void __init 		cpu_device(void);
extern struct sys_timer cpu_sys_timer;

/* extern for board */
extern void __init 		board_init(void);
extern void __init 		board_device(void);

/* system memory info */
extern void * high_memory;
void *boot_alloc_mem;

#define	K_ALLOC_START	(PAGE_OFFSET)
#define	K_ALLOC_END		(high_memory)
#define	K_ALLOC_LEN		(K_ALLOC_END - K_ALLOC_START)>>20

#define	V_ALLOC_START	(VMALLOC_START)
#define	V_ALLOC_END		(VMALLOC_END)
#define	V_ALLOC_LEN		(VMALLOC_END - VMALLOC_START)>>20

#include <mach/iomap.h>

extern void pll_info(void);
extern void boot_mode(void);
extern u_int cpu_get_clock_hz(int clk);
/*------------------------------------------------------------------------------
 * Set prototype base address.
 */
static void proto_init(void)
{
	U32 idx = 0;

	/* Alive pad */
	NX_ALIVE_Initialize();
	NX_ALIVE_SetBaseAddress((U32)IO_ADDRESS(NX_ALIVE_GetPhysicalAddress()));
	NX_ALIVE_OpenModule();

	/* Gpio */
	NX_GPIO_Initialize();
	for (idx = 0; NX_GPIO_GetNumberOfModule() > idx; idx++) {
		NX_GPIO_SetBaseAddress(idx, (U32)IO_ADDRESS(NX_GPIO_GetPhysicalAddress(idx)));
		NX_GPIO_OpenModule(idx);
	}

	/* Clock and Power for PLL */
	NX_CLKPWR_Initialize();
	NX_CLKPWR_SetBaseAddress((U32)IO_ADDRESS(NX_CLKPWR_GetPhysicalAddress()));
	NX_CLKPWR_OpenModule();

	/* MCUD For Dynamic Memory. */
	NX_MCUD_Initialize();
	NX_MCUD_SetBaseAddress((U32)IO_ADDRESS(NX_MCUD_GetPhysicalAddress()));
	NX_MCUD_OpenModule();

	/* MCUS for Static Memory. */
	NX_MCUS_Initialize();
	NX_MCUS_SetBaseAddress((U32)IO_ADDRESS(NX_MCUS_GetPhysicalAddress()));
	NX_MCUS_OpenModule();

	/* Interrupt controller */
	NX_INTC_Initialize();
	NX_INTC_SetBaseAddress((U32)IO_ADDRESS(NX_INTC_GetPhysicalAddress()));
	NX_INTC_OpenModule();

	/* Timer */
	NX_TIMER_Initialize();
	for (idx = 0; NX_TIMER_GetNumberOfModule() > idx; idx++) {
		NX_TIMER_SetBaseAddress(idx, (U32)IO_ADDRESS(NX_TIMER_GetPhysicalAddress(idx)));
		NX_TIMER_OpenModule(idx);
	}

	/* DMA */
	NX_DMA_Initialize();
	for (idx = 0; NX_DMA_GetNumberOfModule() > idx; idx++)	{
		NX_DMA_SetBaseAddress(idx, (U32)IO_ADDRESS(NX_DMA_GetPhysicalAddress(idx)));
		NX_DMA_OpenModule(idx);
	}

	/* RTC. */
	NX_RTC_Initialize();
	NX_RTC_SetBaseAddress((U32)IO_ADDRESS(NX_RTC_GetPhysicalAddress()));
	NX_RTC_OpenModule();

	/* DPC For Display Logo (Primary/Secondary). */
	NX_DPC_Initialize();
	for (idx = 0; NX_DPC_GetNumberOfModule() > idx; idx++) {
		NX_DPC_SetBaseAddress(idx, (U32)IO_ADDRESS(NX_DPC_GetPhysicalAddress(idx)));
		NX_DPC_OpenModule(idx);
	}

	/* MLC For Display Logo (Primary/Secondary). */
	NX_MLC_Initialize();
	for (idx = 0; NX_MLC_GetNumberOfModule() > idx; idx++) {
		NX_MLC_SetBaseAddress(idx, (U32)IO_ADDRESS(NX_MLC_GetPhysicalAddress(0)));
		NX_MLC_OpenModule(idx);
	}

	/* PWM For BLU(Back light unit). */
	NX_PWM_Initialize();
	NX_PWM_SetBaseAddress((U32)IO_ADDRESS(NX_PWM_GetPhysicalAddress()));
 	NX_PWM_OpenModule();

	/*
	 * NOTE> ALIVE Power Gate must enable for RTC register access.
	 */
	NX_ALIVE_SetWriteEnable(CTRUE);
}

/*------------------------------------------------------------------------------
 * intialize soc bus status.
 */
static void bus_init(void)
{
	/*
	 * Update Fast Channel Arbiter
	 */
	#if (CFG_SYS_UPDATE_FASTCH == CTRUE)
	{
		NX_MCUD_SetFastArbiter(  0, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_00);
		NX_MCUD_SetFastArbiter(  1, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_01);
		NX_MCUD_SetFastArbiter(  2, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_02);
		NX_MCUD_SetFastArbiter(  3, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_03);
		NX_MCUD_SetFastArbiter(  4, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_04);
		NX_MCUD_SetFastArbiter(  5, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_05);
		NX_MCUD_SetFastArbiter(  6, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_06);
		NX_MCUD_SetFastArbiter(  7, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_07);
		NX_MCUD_SetFastArbiter(  8, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_08);
		NX_MCUD_SetFastArbiter(  9, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_09);
		NX_MCUD_SetFastArbiter( 10, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_10);
		NX_MCUD_SetFastArbiter( 11, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_11);
		NX_MCUD_SetFastArbiter( 12, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_12);
		NX_MCUD_SetFastArbiter( 13, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_13);
		NX_MCUD_SetFastArbiter( 14, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_14);
		NX_MCUD_SetFastArbiter( 15, (NX_MCUD_FAST)CFG_SYS_MCUD_FASTCH_15);
	}
	#endif

	/*
	 * Update Slow Channel Arbiter
	 */
	#if (CFG_SYS_UPDATE_SLOWCH == CTRUE)
	{
		NX_MCUD_SetSlowArbiter(  0, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_00);
		NX_MCUD_SetSlowArbiter(  1, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_01);
		NX_MCUD_SetSlowArbiter(  2, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_02);
		NX_MCUD_SetSlowArbiter(  3, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_03);
		NX_MCUD_SetSlowArbiter(  4, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_04);
		NX_MCUD_SetSlowArbiter(  5, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_05);
		NX_MCUD_SetSlowArbiter(  6, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_06);
		NX_MCUD_SetSlowArbiter(  7, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_07);
		NX_MCUD_SetSlowArbiter(  8, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_08);
		NX_MCUD_SetSlowArbiter(  9, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_09);
		NX_MCUD_SetSlowArbiter( 10, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_10);
		NX_MCUD_SetSlowArbiter( 11, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_11);
		NX_MCUD_SetSlowArbiter( 12, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_12);
		NX_MCUD_SetSlowArbiter( 13, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_13);
		NX_MCUD_SetSlowArbiter( 14, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_14);
		NX_MCUD_SetSlowArbiter( 15, (NX_MCUD_SLOW)CFG_SYS_MCUD_SLOWCH_15);
	}
	#endif

	/*
	 * NAND Bus config
	 */
	NX_MCUS_SetStaticBUSConfig
	(
		NX_MCUS_SBUSID_NAND,	// bus type 		: NAND
		0,						// bit width 		: Not used
		CFG_SYS_NAND_TACS,		// tACS  ( 0 ~ 3 )
		CFG_SYS_NAND_TCAH,		// tCAH  ( 0 ~ 3 )
		CFG_SYS_NAND_TCOS,		// tCOS  ( 0 ~ 3 )
		CFG_SYS_NAND_TCOH,		// tCOH  ( 0 ~ 3 )
		CFG_SYS_NAND_TACC,		// tACC  ( 1 ~ 16)
		0,						// tSACC ( 1 ~ 16) : Not used
		(NX_MCUS_WAITMODE)0,	// Wait mode		: Not used
		(NX_MCUS_BURSTMODE)0,	// Read  burst mode	: Not used
		(NX_MCUS_BURSTMODE)0	// Write burst mode : Not used
	);

	/*
	 * MCU-Static config: Static Bus #0 ~ #7
	 */
	#define STATIC_BUS_CONFIGUTATION( _n_ )								\
	NX_MCUS_SetStaticBUSConfig											\
	( 																	\
		NX_MCUS_SBUSID_STATIC ## _n_, 									\
		CFG_SYS_STATIC ## _n_ ## _BW, 									\
		CFG_SYS_STATIC ## _n_ ## _TACS, 								\
		CFG_SYS_STATIC ## _n_ ## _TCAH, 								\
		CFG_SYS_STATIC ## _n_ ## _TCOS, 								\
		CFG_SYS_STATIC ## _n_ ## _TCOH, 								\
		CFG_SYS_STATIC ## _n_ ## _TACC, 								\
		CFG_SYS_STATIC ## _n_ ## _TSACC,								\
		(NX_MCUS_WAITMODE ) CFG_SYS_STATIC ## _n_ ## _WAITMODE, 		\
		(NX_MCUS_BURSTMODE) CFG_SYS_STATIC ## _n_ ## _RBURST, 			\
		(NX_MCUS_BURSTMODE) CFG_SYS_STATIC ## _n_ ## _WBURST			\
	);

	STATIC_BUS_CONFIGUTATION( 0);
	STATIC_BUS_CONFIGUTATION( 1);
	STATIC_BUS_CONFIGUTATION( 2);
	STATIC_BUS_CONFIGUTATION( 3);
	STATIC_BUS_CONFIGUTATION( 4);
	STATIC_BUS_CONFIGUTATION( 5);
	STATIC_BUS_CONFIGUTATION( 6);
	STATIC_BUS_CONFIGUTATION( 7);
}

/*------------------------------------------------------------------------------
 * intialize nexell soc frequency.
 */
static u_int  pre_fclk = 0;
static u_int  pre_mclk = 0;

static void pll_init(void)
{
	pre_fclk = cpu_get_clock_hz(2);
	pre_mclk = cpu_get_clock_hz(3);

	/* Update CPU PLL only */
	#if (CTRUE == CFG_SYS_CLKPWR_UPDATE && CTRUE != CFG_SYS_MCUD_UPDATE)
	{
		/* Bus mode */
		if (CFG_SYS_CPU_BUSDIV != (CFG_SYS_MCLK_DIV * CFG_SYS_BCLK_DIV))
			NX_CLKPWR_SetCPUBUSSyncMode(CFALSE);

		#if (CFG_SYS_CLKPWR_SYNC_BUS)
			if (CFG_SYS_CPU_BUSDIV == (CFG_SYS_MCLK_DIV * CFG_SYS_BCLK_DIV))
				NX_CLKPWR_SetCPUBUSSyncMode(CTRUE);
		#endif

		/* Pll change */
		NX_CLKPWR_SetClockCPU(CFG_SYS_CPU_CLKSRC, CFG_SYS_CPU_COREDIV, CFG_SYS_CPU_BUSDIV);
		NX_CLKPWR_SetClockMCLK(CFG_SYS_MCLK_CLKSRC, CFG_SYS_MCLK_DIV, CFG_SYS_BCLK_DIV, CFG_SYS_PCLK_DIV);
		NX_CLKPWR_SetPLLPMS(0, CFG_SYS_PLL0_P, CFG_SYS_PLL0_M, CFG_SYS_PLL0_S);
		NX_CLKPWR_SetPLLPowerOn(CFG_SYS_PLL1_USE);
		NX_CLKPWR_SetPLLPMS(1, CFG_SYS_PLL1_P, CFG_SYS_PLL1_M, CFG_SYS_PLL1_S);
		NX_CLKPWR_DoPLLChange();

		while (CFALSE == NX_CLKPWR_IsPLLStable()) { ; }
	}
	#endif

	/* Update CPU PLL and Memory Clock */
	#if (CTRUE == CFG_SYS_CLKPWR_UPDATE && CTRUE == CFG_SYS_MCUD_UPDATE)
	{
		/* When faster more than current */
		if (CFG_SYS_MCLK_FREQ > pre_mclk)
		{
			/* 1. SetMemCfg */
			NX_MCUD_SetCASLatency	(CFG_SYS_MCUD_CASLAT);
			NX_MCUD_SetDelayLatency	(CFG_SYS_MCUD_DLYLAT);

			/* 2. InitialMemConfig */
			NX_MCUD_ApplyModeSetting();

			while (NX_MCUD_IsBusyModeSetting()) { ; }

			/* 3. SetMemTime */
			NX_MCUD_SetMRD	(CFG_SYS_MCUD_TMRD);
			NX_MCUD_SetRP	(CFG_SYS_MCUD_TRP);
			NX_MCUD_SetRCD	(CFG_SYS_MCUD_TRCD);
			NX_MCUD_SetRAS	(CFG_SYS_MCUD_TRAS);
			NX_MCUD_SetWR	(CFG_SYS_MCUD_TWR);
			NX_MCUD_SetRFC	(CFG_SYS_MCUD_TRFC);
			NX_MCUD_SetWTR	(CFG_SYS_MCUD_TWTR);
			NX_MCUD_SetRTP	(CFG_SYS_MCUD_TRTP);
			NX_MCUD_SetActivePowerDownEnable(CFG_SYS_MCUD_ACTPDENB);
			NX_MCUD_SetActivePowerDownPeriod(CFG_SYS_MCUD_ACTPDPRED);

			/* 4. SetDLL */
			#if 	(CFG_SYS_MCLK_FREQ >= 400000000)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x14, 0x14);
			#elif 	(CFG_SYS_MCLK_FREQ >= 333333333)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x18, 0x18);
			#elif 	(CFG_SYS_MCLK_FREQ >= 266666666)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x1E, 0x1E);
			#elif 	(CFG_SYS_MCLK_FREQ >= 200000000)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x28, 0x28);
			#elif 	(CFG_SYS_MCLK_FREQ >= 166666666)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x2F, 0x2F);
			#elif 	(CFG_SYS_MCLK_FREQ >= 133333333)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x3B, 0x3B);
			#elif	(CFG_SYS_MCLK_FREQ >= 100000000)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x4F, 0x4F);
			#else
			NX_MCUD_PHYDLL_SetConfig(CTRUE , 0x7F, 0x7F);
			#endif

			NX_MCUD_SetDLLReset(CFG_SYS_MCUD_DLLRESET);
			NX_MCUD_SetMCLKConfig((CFG_SYS_BCLK_DIV == 2) ? CTRUE : CFALSE, (CFG_SYS_MCLK_FREQ >= 200000000) ? CTRUE : CFALSE);

			/* 5. Pll change */
			if (CFG_SYS_CPU_CORE_FREQ != pre_fclk)
			{
				NX_CLKPWR_SetClockCPU(CFG_SYS_CPU_CLKSRC, CFG_SYS_CPU_COREDIV, CFG_SYS_CPU_BUSDIV);
				NX_CLKPWR_SetClockMCLK(CFG_SYS_MCLK_CLKSRC, CFG_SYS_MCLK_DIV, CFG_SYS_BCLK_DIV, CFG_SYS_PCLK_DIV);
				NX_CLKPWR_SetPLLPMS(0, CFG_SYS_PLL0_P, CFG_SYS_PLL0_M, CFG_SYS_PLL0_S);
				NX_CLKPWR_SetPLLPowerOn(CFG_SYS_PLL1_USE);
				NX_CLKPWR_SetPLLPMS(1, CFG_SYS_PLL1_P, CFG_SYS_PLL1_M, CFG_SYS_PLL1_S);
				NX_CLKPWR_DoPLLChange();

				while (CFALSE == NX_CLKPWR_IsPLLStable()) { ; }
			}

			/* 6. SetMemRefresh */
			NX_MCUD_SetRefreshPeriod(CFG_SYS_MCUD_REFPRED);
		}
		else if (pre_mclk > CFG_SYS_MCLK_FREQ)
		{
			/* 1. SetMemCfg */
			NX_MCUD_SetDelayLatency(CFG_SYS_MCUD_DLYLAT);

			/* 2. SetMemRefresh */
			NX_MCUD_SetRefreshPeriod(CFG_SYS_MCUD_REFPRED);

			/* 3. SetDLL */
			#if 	(CFG_SYS_MCLK_FREQ >= 400000000)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x14, 0x14);
			#elif 	(CFG_SYS_MCLK_FREQ >= 333333333)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x18, 0x18);
			#elif 	(CFG_SYS_MCLK_FREQ >= 266666666)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x1E, 0x1E);
			#elif 	(CFG_SYS_MCLK_FREQ >= 200000000)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x28, 0x28);
			#elif 	(CFG_SYS_MCLK_FREQ >= 166666666)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x2F, 0x2F);
			#elif 	(CFG_SYS_MCLK_FREQ >= 133333333)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x3B, 0x3B);
			#elif	(CFG_SYS_MCLK_FREQ >= 100000000)
			NX_MCUD_PHYDLL_SetConfig(CFALSE, 0x4F, 0x4F);
			#else
			NX_MCUD_PHYDLL_SetConfig(CTRUE , 0x7F, 0x7F);
			#endif

			NX_MCUD_SetDLLReset(CFG_SYS_MCUD_DLLRESET);
			NX_MCUD_SetMCLKConfig((CFG_SYS_BCLK_DIV == 2) ? CTRUE : CFALSE, (CFG_SYS_MCLK_FREQ >= 200000000) ? CTRUE : CFALSE);

			/* 4. Pll change */
			if (CFG_SYS_CPU_CORE_FREQ != pre_fclk)
			{
				NX_CLKPWR_SetClockCPU(CFG_SYS_CPU_CLKSRC, CFG_SYS_CPU_COREDIV, CFG_SYS_CPU_BUSDIV);
				NX_CLKPWR_SetClockMCLK(CFG_SYS_MCLK_CLKSRC, CFG_SYS_MCLK_DIV, CFG_SYS_BCLK_DIV, CFG_SYS_PCLK_DIV);
				NX_CLKPWR_SetPLLPMS(0, CFG_SYS_PLL0_P, CFG_SYS_PLL0_M, CFG_SYS_PLL0_S);
				NX_CLKPWR_SetPLLPowerOn(CFG_SYS_PLL1_USE);
				NX_CLKPWR_SetPLLPMS(1, CFG_SYS_PLL1_P, CFG_SYS_PLL1_M, CFG_SYS_PLL1_S);
				NX_CLKPWR_DoPLLChange();

				while (CFALSE == NX_CLKPWR_IsPLLStable()) { ; }
			}

			/* 5. SetMemTime */
			NX_MCUD_SetMRD	(CFG_SYS_MCUD_TMRD);
			NX_MCUD_SetRP	(CFG_SYS_MCUD_TRP);
			NX_MCUD_SetRCD	(CFG_SYS_MCUD_TRCD);
			NX_MCUD_SetRAS	(CFG_SYS_MCUD_TRAS);
			NX_MCUD_SetWR	(CFG_SYS_MCUD_TWR);
			NX_MCUD_SetRFC	(CFG_SYS_MCUD_TRFC);
			NX_MCUD_SetWTR	(CFG_SYS_MCUD_TWTR);
			NX_MCUD_SetRTP	(CFG_SYS_MCUD_TRTP);
			NX_MCUD_SetActivePowerDownEnable(CFG_SYS_MCUD_ACTPDENB);
			NX_MCUD_SetActivePowerDownPeriod(CFG_SYS_MCUD_ACTPDPRED);

			/* 6. SetMemCfg */
			NX_MCUD_SetCASLatency(CFG_SYS_MCUD_CASLAT);

			/* 7. InitialMemConfig */
			NX_MCUD_ApplyModeSetting();

			while (NX_MCUD_IsBusyModeSetting()) { ; }
		}
		else
		{
			if (CFG_SYS_CPU_CORE_FREQ != pre_fclk)
			{
				NX_CLKPWR_SetClockCPU(CFG_SYS_CPU_CLKSRC, CFG_SYS_CPU_COREDIV, CFG_SYS_CPU_BUSDIV);
				NX_CLKPWR_SetClockMCLK(CFG_SYS_MCLK_CLKSRC, CFG_SYS_MCLK_DIV, CFG_SYS_BCLK_DIV, CFG_SYS_PCLK_DIV);
				NX_CLKPWR_SetPLLPMS(0, CFG_SYS_PLL0_P, CFG_SYS_PLL0_M, CFG_SYS_PLL0_S);
				NX_CLKPWR_SetPLLPowerOn(CFG_SYS_PLL1_USE);
				NX_CLKPWR_SetPLLPMS(1, CFG_SYS_PLL1_P, CFG_SYS_PLL1_M, CFG_SYS_PLL1_S);
				NX_CLKPWR_DoPLLChange();

				while (CFALSE == NX_CLKPWR_IsPLLStable()) { ; }
			}
		}
	}
	#endif	// CFG_SYS_CLKPWR_UPDATE
}

/*------------------------------------------------------------------------------
 * 	cpu interface.
 */
void __init cpu_base_init(void)
{
	/* Make sure the peripheral register window is closed, since
	 * we will use PTE flags (TEX[1]=1,B=0,C=1) to determine which
	 * pages are peripheral interface or not.
	 */
	asm("mcr p15, 0, %0, c15, c2, 4" : : "r" (0xC0000000 | 0x15));
	DBGOUT("\n");

	proto_init();
	bus_init();
	pll_init();

	pll_info();
	boot_mode();

	DBGOUT("Prototype : %s mode \n\n", "Release");
}

/*------------------------------------------------------------------------------
 * 	cpu initialize and io/memory map.
 * 	procedure: fixup -> map_io -> init_irq -> timer init -> init_machine
 */
static void __init cpu_fixup(
	struct machine_desc *desc, struct tag *tags, char **cmdline, struct meminfo *mi
	)
{
	DBGOUT("%s\n", __func__);

	/*
	 * system momory  = system_size + linear_size
	 */
    mi->nr_banks     	= 1;
	mi->bank[0].start 	= CFG_MEM_PHY_SYSTEM_BASE;
    mi->bank[0].size	= CFG_MEM_PHY_SYSTEM_SIZE; // + CFG_MEM_PHY_LINEAR_SIZE;
    mi->bank[0].node  	= 0;
}

static void __init cpu_map_io(void)
{
	/* debug */
	_IOMAP();

	/* make iotable */
	iotable_init(cpu_iomap_desc, ARRAY_SIZE(cpu_iomap_desc));

	/* init cpu */
	cpu_base_init();

	boot_alloc_mem = CFG_MEM_PHY_SYSTEM_BASE + CFG_MEM_PHY_SYSTEM_SIZE;

	printk("k_alloc: 0x%08x ~ 0x%08x, %dMB\n",
		(u_int)K_ALLOC_START, (u_int)K_ALLOC_END, (u_int)K_ALLOC_LEN);
	printk("v_alloc: 0x%08x ~ 0x%08x, %dMB\n",
		(u_int)V_ALLOC_START, (u_int)V_ALLOC_END, (u_int)V_ALLOC_LEN);
}

static void __init cpu_init_machine(void)
{
    struct	NX_AUDIO_RegisterSet *i2s_reg = (struct NX_AUDIO_RegisterSet *)IO_ADDRESS(PHY_BASEADDR_AUDIO);
	DBGOUT("%s\n", __func__);

	board_init();
	cpu_soc_init();

	/*
	 * register platform device
	 */
	cpu_device();
	//board_device(); // ksw: We don't have board device...
	/* We should set CLKENB for SND DEVICE */
	i2s_reg->CLKENB = 0x0C; /* PCKALWAYS & enable CLK */
	i2s_reg->CLKGEN[0][0] = 4 | ((16-1) << 5);
	i2s_reg->I2S_CONFIG = 1; /* Set slave mode */
}

/*------------------------------------------------------------------------------
 * Maintainer: STCube Co., Ltd.
 */

MACHINE_START(NXP2120, CFG_SYS_CPU_NAME)
 	.phys_io		=  (__PB_IO_MAP_REGS_PHYS),
	.io_pg_offst	=  (__PB_IO_MAP_REGS_VIRT >> 18) & 0xfffc,
	.boot_params	=  CFG_MEM_PHY_SYSTEM_BASE + 0x00000100,
	.fixup			=  cpu_fixup,
	.map_io			=  cpu_map_io,
	.init_irq		=  cpu_init_irq,
	.timer			= &cpu_sys_timer,
	.init_machine	=  cpu_init_machine,
MACHINE_END