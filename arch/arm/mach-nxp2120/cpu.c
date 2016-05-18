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

/*------------------------------------------------------------------------------
 * memory clock status.
 */
#define MEMCFG			*(volatile unsigned long *)IO_ADDRESS(0xc0014800)
#define MEMTIME0		*(volatile unsigned long *)IO_ADDRESS(0xc0014804)
#define MEMCTRL			*(volatile unsigned long *)IO_ADDRESS(0xc0014808)
#define MEMACTPWD		*(volatile unsigned long *)IO_ADDRESS(0xc001480C)
#define MEMTIME1		*(volatile unsigned long *)IO_ADDRESS(0xc0014810)

#define PHYDELAYCTRL	*(volatile unsigned long *)IO_ADDRESS(0xc0014894)
#define PHYDLLCTRL0		*(volatile unsigned long *)IO_ADDRESS(0xc0014898)

void mem_clk_info(void)
{
	unsigned int config = MEMCFG;
	unsigned int time_0 = MEMTIME0;
	unsigned int time_1 = MEMTIME0;

	int CASLAT  = (int)((config >> 21) & 0x7);
	int ADDLAT  = (int)((config >> 18) & 0x7);
	int DLYLAT  = (int)((config >> 24) & 0x7);

	int TRCD	= (int)(((time_0 >>  0) & 0x0F) + 1);
	int TRP		= (int)(((time_0 >>  4) & 0x0F) + 1);
	int TRAS	= (int)(((time_0 >> 12) & 0xFF) + 1);
	int TMRD	= (int)(((time_1 >> 16) & 0x0F) + 1);
	int TWR		= (int)(((time_1 >> 24) & 0x0F) + 1);
	int TWTR	= (int)(((time_1 >> 28) & 0x0F) + 1);
	int TRTP	= (int)(((time_1 >> 20) & 0x0F) + 1);
	int TRFC	= (int)(((time_0 >> 24) & 0xFF) + 1);

	DBGOUT("Memory Clock Configuration :\n");
	DBGOUT("CLAT =%4d, ALAT =%4d, DLAT =%4d \n", CASLAT, ADDLAT, DLYLAT);
	DBGOUT("TRCD =%4d, TRP  =%4d, TRAS =%4d, TMRD =%4d \n", TRCD, TRP , TRAS, TMRD);
	DBGOUT("TWR  =%4d, TWTR =%4d, TRTP =%4d, TRFC =%4d \n", TWR , TWTR, TRTP, TRFC);
}

/*------------------------------------------------------------------------------
 * cpu pll status functions.
 */
typedef struct	__NX_CLKPWR_RegSet
{
	volatile unsigned int CLKMODE[2]				; 	// 0x00, 0x04
	volatile unsigned int PLLSET[2]					; 	// 0x08, 0x0C
	volatile unsigned int __Reserved[(0x68-0x10)/4]	; 	// 0x10 ~ 0x64
	volatile unsigned int PWRMODE					; 	// 0x68
} NX_CLKPWR_REGSET;

#define NX_CLKPWR_BASEADDR		(0xC000F000)

static void cpu_clk_info(void)
{
	unsigned int clkmode[2], pllreg[2];
	unsigned int pll[4], sel, dvo, div2, div3, fclk, mclk, bclk, pclk, sync;
	int i;

	NX_CLKPWR_REGSET *pReg = (NX_CLKPWR_REGSET *)IO_ADDRESS(NX_CLKPWR_BASEADDR);

	DBGOUT("%s: %s Frequency Configuration : \n", CFG_SYS_CPU_NAME, (CFG_SYS_CLKPWR_UPDATE==CTRUE?"DONE":"NONE"));

#if (0)
	DBGOUT( "\n" );
	DBGOUT( "< CLKPWR register dump >\n" );
	DBGOUT( "CLKMODE[0] = 0x%08X\n", pReg->CLKMODE[0]);
	DBGOUT( "CLKMODE[1] = 0x%08X\n", pReg->CLKMODE[1]);
	DBGOUT( "PLLSET [0] = 0x%08X\n", pReg->PLLSET[0]);
	DBGOUT( "PLLSET [1] = 0x%08X\n", pReg->PLLSET[1]);
	DBGOUT( "\n" );
#endif

	clkmode[0] 	= pReg->CLKMODE[0];
	clkmode[1] 	= pReg->CLKMODE[1];
	pllreg [0]	= pReg->PLLSET[0];
	pllreg [1]	= pReg->PLLSET[1];
	sync		= pReg->PWRMODE & (1<<28);

	for ( i=0 ; i<2 ; i++ )
	{
		unsigned int pdiv, mdiv, sdiv;
        unsigned int P_bitpos, M_bitpos, S_bitpos;
        unsigned int P_mask, M_mask, S_mask;

		const unsigned int PLL0_PDIV_BIT_POS 	= 18;
		const unsigned int PLL0_MDIV_BIT_POS 	=  8;
		const unsigned int PLL0_SDIV_BIT_POS 	=  0;
        const unsigned int PLL0_PDIV_MASK       = 0xFF;
        const unsigned int PLL0_MDIV_MASK       = 0x3FF;
        const unsigned int PLL0_SDIV_MASK       = 0xFF;

		const unsigned int PLL1_PDIV_BIT_POS 	= 18;
		const unsigned int PLL1_MDIV_BIT_POS 	=  8;
		const unsigned int PLL1_SDIV_BIT_POS 	=  0;
        const unsigned int PLL1_PDIV_MASK       = 0x3F;
        const unsigned int PLL1_MDIV_MASK       = 0x3FF;
        const unsigned int PLL1_SDIV_MASK       = 0xFF;

        if ( 0 == i ) {
            P_bitpos = PLL0_PDIV_BIT_POS;
            P_mask   = PLL0_PDIV_MASK;
            M_bitpos = PLL0_MDIV_BIT_POS;
            M_mask   = PLL0_MDIV_MASK;
            S_bitpos = PLL0_SDIV_BIT_POS;
            S_mask   = PLL0_SDIV_MASK;
        } else {
            P_bitpos = PLL1_PDIV_BIT_POS;
            P_mask   = PLL1_PDIV_MASK;
            M_bitpos = PLL1_MDIV_BIT_POS;
            M_mask   = PLL1_MDIV_MASK;
            S_bitpos = PLL1_SDIV_BIT_POS;
            S_mask   = PLL1_SDIV_MASK;
        }

		pdiv	= (pllreg[i] >> P_bitpos) & P_mask;
		mdiv	= (pllreg[i] >> M_bitpos) & M_mask;
		sdiv	= (pllreg[i] >> S_bitpos) & S_mask;

		pll[i]	= CFG_SYS_PLL_PMSFUNC( CFG_SYS_PLLFIN, pdiv, mdiv, sdiv );

		DBGOUT( "PLL%d [P = %2d, M=%2d, S=%2d] = %d Hz. \n", i, pdiv, mdiv, sdiv, pll[i] );
	}

	dvo 	= (clkmode[0]>>( 0+ 0)) & 0xF;
	sel		= (clkmode[0]>>( 4+ 0)) & 0x3;	//	NX_ASSERT( 2 > sel );
	div2	= (clkmode[0]>>( 8+ 0)) & 0xF;
	DBGOUT( "CPU : Core clock = PLL%d / %d = %d, AXI Bus Clock = Core clock / %d = %d, [%s]\n",
		sel, dvo+1, pll[sel] / (dvo+1), div2+1, (pll[sel] / (dvo+1)) / (div2+1),
		sync ? "SYNC" : "ASYNC");

	fclk = pll[sel] / (dvo+1);
	pll[3] = fclk;

	dvo	    = (clkmode[1]>>(  0+ 0)) & 0xF;
	sel		= (clkmode[1]>>(  4+ 0)) & 0x3;//	NX_ASSERT( 2 != sel );
	div2	= (clkmode[1]>>(  8+ 0)) & 0xF;
	div3	= (clkmode[1]>>( 12+ 0)) & 0xF;

	mclk = pll[sel] / (dvo  + 1);
	bclk = mclk     / (div2 + 1);
	pclk = bclk     / (div3 + 1);

	DBGOUT( "BUS : MCLK = %s / %d = %d, BCLK = MCLK / %d = %d, PCLK = BCLK / %d = %d\n",
						((sel==3) ? "FCLK" : ((sel==1) ? "PLL1" : "PLL0")),
						dvo+1, 	mclk,
                		div2+1, bclk,
                		div3+1, pclk );

	/* PLL1 Power down status */
	DBGOUT("PLL1 Power : %s \n", (clkmode[0] & (1 << 30)) ? "down" : "normal" );
	DBGOUT("FCLK: %d Hz -> %d Hz, change [%s]\n", pre_fclk, fclk, (pre_fclk == fclk) ? "NONE" : "DONE");
	DBGOUT("MCLK: %d Hz -> %d Hz, change [%s]\n", pre_mclk, mclk, (pre_mclk == mclk) ? "NONE" : "DONE");
}

void pll_info(void)
{
	cpu_clk_info();
	mem_clk_info();
}

/* check boot mode */
extern u_int cpu_get_reserve_pm_mem(int phys);

void	boot_mode(void)
{
	u32  base = 0; //cpu_get_reserve_pm_mem(0); ksw: We always On by ColdBoot
	U32  csum = 0, size = 0;
	U32 *data = NULL;

	int  val = 0, xor = 0, i = 0;
	bool coldboot = true;
	u32  scratch  = 0;

	if (! base) {
		DBGOUT("Boot mode : Cold boot ...\n");
		return;
	}

	csum 	 = *(U32*)(base + 0x0);
	size 	 = *(U32*)(base + 0x4);
	data 	 =  (U32*)(base + 0x8);
	scratch  = NX_ALIVE_GetScratchReg();

	if (CFG_PWR_SLEEP_SIGNATURE == scratch) {

		if (size <= 0 || size >= 0x100) {
			coldboot = true;
		} else {
			for (i = 0; i<size; i++, data++)
				xor = (*data ^ 0xAA), val ^= xor;

			if (csum != val)
				coldboot = true;
			else
				coldboot = false;
		}
	}

	DBGOUT("Boot mode : %s boot [0x%08x, 0x%08x, 0x%08x]...\n",
		coldboot==true?"Cold":"Warm", scratch, csum, val);
}

/*------------------------------------------------------------------------------
 * 	cpu clock interface.
 */
u_int cpu_get_clock_hz(int clk)
{
	unsigned int clkmode[2], pllreg[2], clock_hz = 0;
	unsigned int pll[4], sel, dvo, div2, div3, fclk, mclk, bclk, pclk;
	int i;

	NX_CLKPWR_REGSET *pReg = (NX_CLKPWR_REGSET *)IO_ADDRESS(NX_CLKPWR_BASEADDR);

	clkmode[0] 	= pReg->CLKMODE[0];
	clkmode[1] 	= pReg->CLKMODE[1];
	pllreg [0]	= pReg->PLLSET[0];
	pllreg [1]	= pReg->PLLSET[1];

	for ( i=0 ; i<2 ; i++ ) {
		unsigned int 	pdiv, mdiv, sdiv;
        unsigned int     P_bitpos, M_bitpos, S_bitpos;
        unsigned int     P_mask, M_mask, S_mask;

		const unsigned int PLL0_PDIV_BIT_POS 	= 18;
		const unsigned int PLL0_MDIV_BIT_POS 	=  8;
		const unsigned int PLL0_SDIV_BIT_POS 	=  0;
        const unsigned int PLL0_PDIV_MASK       = 0xFF;
        const unsigned int PLL0_MDIV_MASK       = 0x3FF;
        const unsigned int PLL0_SDIV_MASK       = 0xFF;

		const unsigned int PLL1_PDIV_BIT_POS 	= 18;
		const unsigned int PLL1_MDIV_BIT_POS 	=  8;
		const unsigned int PLL1_SDIV_BIT_POS 	=  0;
        const unsigned int PLL1_PDIV_MASK       = 0x3F;
        const unsigned int PLL1_MDIV_MASK       = 0x3FF;
        const unsigned int PLL1_SDIV_MASK       = 0xFF;

        if ( 0 == i ) {
            P_bitpos = PLL0_PDIV_BIT_POS;
            P_mask   = PLL0_PDIV_MASK;
            M_bitpos = PLL0_MDIV_BIT_POS;
            M_mask   = PLL0_MDIV_MASK;
            S_bitpos = PLL0_SDIV_BIT_POS;
            S_mask   = PLL0_SDIV_MASK;
        } else {
            P_bitpos = PLL1_PDIV_BIT_POS;
            P_mask   = PLL1_PDIV_MASK;
            M_bitpos = PLL1_MDIV_BIT_POS;
            M_mask   = PLL1_MDIV_MASK;
            S_bitpos = PLL1_SDIV_BIT_POS;
            S_mask   = PLL1_SDIV_MASK;
        }

		pdiv	= (pllreg[i] >> P_bitpos) & P_mask;
		mdiv	= (pllreg[i] >> M_bitpos) & M_mask;
		sdiv	= (pllreg[i] >> S_bitpos) & S_mask;

		pll[i]	= CFG_SYS_PLL_PMSFUNC( CFG_SYS_PLLFIN, pdiv, mdiv, sdiv );
	}

	dvo 	= (clkmode[0]>>( 0+ 0)) & 0xF;
	sel		= (clkmode[0]>>( 4+ 0)) & 0x3;	//	NX_ASSERT( 2 > sel );
	div2	= (clkmode[0]>>( 8+ 0)) & 0xF;

	fclk = pll[sel] / (dvo+1);
	pll[3] = fclk;

	dvo	    = (clkmode[1]>>(  0+ 0)) & 0xF;
	sel		= (clkmode[1]>>(  4+ 0)) & 0x3;//	NX_ASSERT( 2 != sel );
	div2	= (clkmode[1]>>(  8+ 0)) & 0xF;
	div3	= (clkmode[1]>>( 12+ 0)) & 0xF;

	mclk = pll[sel] / (dvo  + 1);
	bclk = mclk     / (div2 + 1);
	pclk = bclk     / (div3 + 1);

	switch(clk) {
	case 0:	clock_hz =	pll[0];	break;
	case 1:	clock_hz =	pll[1];	break;
	case 2:	clock_hz =	fclk;	break;
	case 3:	clock_hz =	mclk;	break;
	case 4:	clock_hz =	bclk;	break;
	case 5:	clock_hz =	pclk;	break;
	default:
		DBGOUT("error: unknown clock [%d], (0~6)\n", clk);
		break;
	}

	return clock_hz;
}
EXPORT_SYMBOL(cpu_get_clock_hz);


