/*
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#if (0)
#include <common.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <malloc.h>
#include <linux/mtd/nand.h>
#include <nand.h>
#include <platform.h>
#else
#include <linux/kernel.h>
#include <linux/mtd/nand.h>
#include <mach/platform.h>
#include <asm/io.h>
#endif

#if defined (CONFIG_MTD_NAND_NEXELL_HWECC) || defined (CONFIG_SYS_NAND_HW_ECC)

#include "nand_ecc.h"

#if	(0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "ecc: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#define ERROUT(msg...)		{ 					\
		printk(KERN_ERR "ERROR: %s, %s line %d: \n",		\
			__FILE__, __FUNCTION__, __LINE__),	\
		printk(KERN_ERR msg); }

/*
 * 0x05	= Bad marker (256/512 byte pages)
 * 0x00 = Bad marker (2048/4096   byte pages)
 * 0x01 = Reserved   (2048/4096   byte pages)
 */
static struct nand_ecclayout nx_nand_oob = {
	.eccbytes 	=   0,
	.eccpos 	= { 0, },
	.oobfree 	= { {.offset = 0, .length = 0} }
};

#define	ECC_HW_MAX_BYTES		(44) 	//  4 bit ecc ( 7byte),  8 bit ecc (13byte),
										// 16 bit ecc (26byte), 24 bit ecc (42byte),

/*------------------------------------------------------------------------------
 * u-boot nand hw ecc interface
 */

static int nand_sw_ecc_verify_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	int i;
	struct nand_chip *chip = mtd->priv;

	for (i = 0; i < len; i++)
		if (buf[i] != readb(chip->IO_ADDR_R))
			return -EFAULT;
	return 0;
}

static int nand_hw_ecc_read_subpage(struct mtd_info *mtd, struct nand_chip *chip,
				uint32_t data_offs, uint32_t readlen, uint8_t *bufpoi)

{
	int start_step, end_step, num_steps;
	int i, n, ret = 0;
	int pageoffs, eccoffs;

	int eccbytes = chip->ecc.bytes;
	int eccsize  = chip->ecc.size;
	int ecctotal = chip->ecc.total + 3;	// Bad marker

	uint32_t  ecc_buff[ECC_HW_MAX_BYTES/4];
	uint8_t  *ecc_code = (uint8_t*)ecc_buff;
	uint32_t *oobpoi = (uint32_t *)chip->oob_poi;
	uint8_t  *p;

	int err = 0;
	int	o_syn[HW_ECC_MODE], e_pos[HW_ECC_MODE];
	uint32_t *e_dat;

	/* Column address wihin the page aligned to ECC size (256bytes). */
	start_step = data_offs / chip->ecc.size;
	end_step   = (data_offs + readlen - 1) / chip->ecc.size;
	num_steps  = end_step - start_step + 1;

	/* Data size aligned to ECC ecc.size*/
	eccoffs  = mtd->writesize;
	pageoffs = start_step * eccsize;
	p = bufpoi + pageoffs;		/* buffer position */
	n = start_step * eccbytes + (!start_step ? 2 : 3); // Bad marker

	DBGOUT("%s, readlen=%6d, start=%4d, ene=%4d, num=%4d, eccoffs=%4d, pageoffs=%6d, eccpoi=%d\n",
		__func__, readlen, start_step, end_step, num_steps, eccoffs, pageoffs, n);

	/* read oob */
	chip->cmdfunc(mtd, NAND_CMD_RNDOUT, eccoffs, -1);
	chip->read_buf(mtd, chip->oob_poi, ecctotal);

	/* read data */
	chip->cmdfunc(mtd, NAND_CMD_RNDOUT, pageoffs, -1);

	for ( ; num_steps; num_steps--, p += eccsize) {

		for (i = 0; i < eccbytes; i++, n++) {
			if (5 == n) n += 1;	// Bad marker
			ecc_code[i] = chip->oob_poi[n];
		}

		/* write ecc data to orignal ecc register */
		NX_NAND_SetResetECC(HW_ECC_MODE);
		NX_NAND_SetOriECC((uint32_t*)ecc_code, HW_ECC_MODE);

		/* read data */
		chip->read_buf(mtd, p, eccsize);

		/* wait decoding  and error correct */
		NX_NAND_WaitForDecoding();

		if (NX_NAND_GetErrorStatus()) {
#if 1
			/* check nand intialzie status */
			for (n = 0; mtd->oobsize/4 > n; n++) {
				if (0xFFFFFFFF != oobpoi[n]) break;
			}
			if (mtd->oobsize/4 == n)
				continue;
#endif
			NX_NAND_GetOddSyndrome(&o_syn[0]);
			ret = NX_NAND_GetErrorLocation(&o_syn[0], &e_pos[0], &err);

			if (0 > ret) {
				ERROUT("nand detect ecc errs, can't correct (step:%d, pageoffs:%d, errcnt:%d)\n",
					(end_step - start_step + 1 - num_steps), pageoffs, err);
				ret = -EIO;
				break;
			} else {
				printk(KERN_INFO "nand pageoffs %d, errors %2d, bit: ", pageoffs, err);
				for (n=0 ; err > n; n++) {
					printk(KERN_INFO "%d", e_pos[n]);
					if (n != err-1)
						printk(KERN_INFO ", ");

					if (4096 > e_pos[n]) {
						e_dat = (uint32_t*)p;
						e_dat[e_pos[n] / 32] ^= 1U<<(e_pos[n] % 32);
					}
				}
				ret = err > CFG_NAND_ECC_LIMIT ? -EBADMSG: 0;
				printk(KERN_INFO "\n");
			}
		}
	}

	DBGOUT("DONE %s, ret=%d\n", __func__, ret);
	mtd->ecc_stats.failed    = ret;
	mtd->ecc_stats.corrected = ret;
	return ret;
}

static int nand_hw_ecc_read_page(struct mtd_info *mtd, struct nand_chip *chip,
				uint8_t *buf, int page)
{
	int i, n, ret = 0;

	int eccsteps = chip->ecc.steps;
	int eccbytes = chip->ecc.bytes;
	int eccsize  = chip->ecc.size;
	int ecctotal = chip->ecc.total + 3;	// Bad marker

	uint32_t  ecc_buff[ECC_HW_MAX_BYTES/4];
	uint8_t  *ecc_code = (uint8_t*)ecc_buff;
	uint8_t  *p = buf;
	uint32_t *oobpoi = (uint32_t *)chip->oob_poi;

	int err = 0;
	int	o_syn[HW_ECC_MODE], e_pos[HW_ECC_MODE];
	uint32_t *e_dat;

	DBGOUT("%s, page=%d, ecc mode=%d, bytes=%d\n", __func__, page, HW_ECC_MODE, eccbytes);

	if (512 >= mtd->writesize) {
		/* read oob */
		chip->ecc.read_oob(mtd, chip, page, 1);
		/* read data */
		chip->cmdfunc(mtd, NAND_CMD_READ0, 0x00, page);
	} else {
		/* read oob */
		chip->cmdfunc(mtd, NAND_CMD_RNDOUT, mtd->writesize, -1);
		chip->read_buf(mtd, chip->oob_poi, ecctotal);
		/* read data */
		chip->cmdfunc(mtd, NAND_CMD_RNDOUT, 0, -1);
	}

	for (n = 2; eccsteps; eccsteps--, p += eccsize) {

		for (i = 0; i < eccbytes; i++, n++) {
			if (5 == n) n += 1;	// Bad marker
			ecc_code[i] = chip->oob_poi[n];
		}

		/* write ecc data to orignal ecc register */
		NX_NAND_SetResetECC(HW_ECC_MODE);
		NX_NAND_SetOriECC((uint32_t*)ecc_code, HW_ECC_MODE);

		/* read data */
		chip->read_buf(mtd, p, eccsize);

		/* wait decoding  and error correct */
		NX_NAND_WaitForDecoding();

		if (NX_NAND_GetErrorStatus()) {
#if 1
			/* check nand intialzie status */
			for (n = 0; mtd->oobsize/4 > n; n++) {
				if (0xFFFFFFFF != oobpoi[n]) break;
			}
			if (mtd->oobsize/4 == n)
				continue;
#endif
			NX_NAND_GetOddSyndrome(&o_syn[0]);
			ret = NX_NAND_GetErrorLocation(&o_syn[0], &e_pos[0], &err);

			if (0 > ret) {
				ERROUT("nand detect ecc errs, can't correct (step:%d, page:%d, errcnt:%d)\n",
					(chip->ecc.steps - eccsteps), page, err);
				ret = -EIO;
				break;
			} else {
				printk(KERN_INFO "nand page %d, ecc %2d err, bit: ", page, err);
				for (n=0 ; err > n; n++) {
					printk(KERN_INFO "%d", e_pos[n]);
					if (n != err-1)
						printk(KERN_INFO ", ");

					if (4096 > e_pos[n]) {
						e_dat = (uint32_t*)p;
						e_dat[e_pos[n] / 32] ^= 1U<<(e_pos[n] % 32);
					}
				}
				ret = err > CFG_NAND_ECC_LIMIT ? -EBADMSG: 0;
				printk(KERN_INFO "\n");
			}
		}
	}

	DBGOUT("DONE %s, ret=%d\n", __func__, ret);
	mtd->ecc_stats.failed    = ret;
	mtd->ecc_stats.corrected = ret;
	return ret;
}

#define DEBUG_WRITE_PAGE	(0)
#if (DEBUG_WRITE_PAGE)
static uint8_t page_buf[4096] = { 0, };
static uint8_t page_oob[128]  = { 0, };
extern int write_page_num;
#endif

static void nand_hw_ecc_write_page(struct mtd_info *mtd, struct nand_chip *chip,
				  const uint8_t *buf)
{
	int i, n;
	int eccsteps = chip->ecc.steps;
	int eccbytes = chip->ecc.bytes;
	int eccsize  = chip->ecc.size;

	uint32_t  ecc_buff[ECC_HW_MAX_BYTES/4];
	uint8_t  *ecc_code = (uint8_t*)ecc_buff;
	uint8_t  *p = (uint8_t *)buf;

	DBGOUT("%s\n", __func__);

	/* write data and get ecc */
	for (n = 2; eccsteps; eccsteps--, p += eccsize) {

		NX_NAND_SetResetECC(HW_ECC_MODE);

		/* write page */
		chip->write_buf(mtd, p, eccsize);

		/* get ecc code from ecc register */
		NX_NAND_WaitForEncoding();
		NX_NAND_GetGenECC((uint32_t *)ecc_code, HW_ECC_MODE);

		/* set oob with ecc */
		for (i = 0; i < eccbytes; i++, n++) {
			if (5 == n) n += 1;	// Bad marker
			chip->oob_poi[n] = ecc_code[i];
		}
	}

	/* write oob */
	chip->write_buf(mtd, chip->oob_poi, mtd->oobsize);

#if (DEBUG_WRITE_PAGE)
	memcpy(page_buf, buf, mtd->writesize);
	memcpy(page_oob, chip->oob_poi, mtd->oobsize);
#endif
}

static int nand_hw_ecc_verify_byf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
#if defined (CONFIG_MTD_NAND_VERIFY_WRITE)
	struct nand_chip *chip = mtd->priv;
	int eccsteps = chip->ecc.steps;
	int eccbytes = chip->ecc.bytes;
	int eccsize  = chip->ecc.size;
	int ecctotal = chip->ecc.total + 3;	// Bad marker

	uint32_t  ecc_buff[ECC_HW_MAX_BYTES/4];
	uint8_t  *ecc_code = (uint8_t*)ecc_buff;
	uint8_t  *p = (uint8_t *)buf;

	int eccoffs;
	int err, ret = 0;
	int	o_syn[HW_ECC_MODE], e_pos[HW_ECC_MODE];
	int i, n;

	if (512 >= mtd->writesize)
		return nand_sw_ecc_verify_buf(mtd, buf, len);

	eccoffs  = mtd->writesize;
	eccsteps = chip->ecc.steps;
	p = (uint8_t *)buf;

	/* note> chip->oob_poi is saved when write_page */
#if (1)
	/* read oob */
	chip->cmdfunc(mtd, NAND_CMD_RNDOUT, eccoffs, -1);
	chip->read_buf(mtd, chip->oob_poi, ecctotal);

	/* read data */
	chip->cmdfunc(mtd, NAND_CMD_RNDOUT, 0, -1);
#endif

	for (n = 2; eccsteps; eccsteps--, p += eccsize) {

		for (i = 0; i < eccbytes; i++, n++) {
			if (5 == n) n += 1;	// Bad marker
			ecc_code[i] = chip->oob_poi[n];
		}

		/* set orignal ecc register */
		NX_NAND_SetResetECC(HW_ECC_MODE);
		NX_NAND_SetOriECC((uint32_t *)ecc_code, HW_ECC_MODE);

		/* read data */
		chip->read_buf(mtd, p, eccsize);

		/* wait decoding  and error correct */
		NX_NAND_WaitForDecoding();

		if (NX_NAND_GetErrorStatus()) {
			NX_NAND_GetOddSyndrome(&o_syn[0]);
			ret = NX_NAND_GetErrorLocation(&o_syn[0], &e_pos[0], &err);
			if (0   > ret ||
				err > CFG_NAND_ECC_LIMIT) {
				ERROUT("nand detect ecc errs, can't correct (step:%d, errcnt:%d, ret:%d)\n",
					(chip->ecc.steps - eccsteps), err, ret);
				#if (DEBUG_WRITE_PAGE)
				{
					int i = 0;
					printk("<PAGE:%6d>", write_page_num);
					for (i=0; mtd->writesize > i; i++) {
						if (0 == (i%8)) printk("\n" "%4d ", i);
						printk("[%c %02x:%02x]",
							page_buf[i]!=buf[i] ? 'X':'O', page_buf[i], buf[i]);
					}
					printk("\n<00B >");
					for (i=0; mtd->oobsize > i; i++) {
						if (0 == (i%8)) printk("\n" "%4d ", i);
						printk("[%c %02x:%02x]",
							page_oob[i]!=chip->oob_poi[i] ? 'X':'O', page_oob[i], chip->oob_poi[i]);
					}
					printk("\n<COPY>\n");
					memcpy((void *)buf, page_buf, mtd->writesize);
					memcpy(chip->oob_poi, page_oob, mtd->oobsize);
					printk("\n<DONE>\n");
					return 0;
				}
				#endif
				return -EIO;
			}
		}
	}
#endif /* CONFIG_MTD_NAND_VERIFY_WRITE */
	return 0;
}

int nand_hw_ecc_check(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	int ret = 0;

	if (512 > mtd->writesize) {
		printk(KERN_INFO "NAND ecc: page size %d not support hw ecc\n",
			mtd->writesize);
		chip->ecc.mode 			= NAND_ECC_SOFT;
		chip->ecc.read_page 	= NULL;
		chip->ecc.read_subpage 	= NULL;
		chip->ecc.write_page 	= NULL;
		chip->ecc.layout		= NULL;
		chip->verify_buf		= nand_sw_ecc_verify_buf;

		if ( chip->buffers &&
			!(chip->options & NAND_OWN_BUFFERS)) {
			kfree(chip->buffers);
			chip->buffers = NULL;
		}
		ret = nand_scan_tail(mtd);
		printk(KERN_INFO "NAND ecc: Software \n");
	}
	return ret;
}

int nand_hw_ecc_init(struct nand_chip *chip)
{
	int eccbytes = 0;
	DBGOUT("%s\n", __func__);

	/*
	 * ecc.bytes:
	 *  4 bit ecc need  52 bit (6.5B) ecc code per 512 Byte
	 *  8 bit ecc need 104 bit (13 B) ecc code per 512 Byte
	 * 16 bit ecc need 208 bit (26 B) ecc code per 512 Byte
	 * 24 bit ecc need 336 bit (42 B) ecc code per 512 Byte
	 *
	 *  Page  512 Byte +  16 Byte
	 *  Page 2048 Byte +  64 Byte
	 *  Page 4096 Byte + 128 Byte
	 */
	switch (HW_ECC_MODE) {
	case  4: 	eccbytes =  7; 	break;
	case  8: 	eccbytes = 13; 	break;
	case 16: 	eccbytes = 26; 	break;
	case 24: 	eccbytes = 42; 	break;
	default:
		ERROUT("not supoort ecc %d mode !!!\n", HW_ECC_MODE);
		return -1;
	}

	chip->ecc.mode 			= NAND_ECC_HW;
	chip->ecc.read_page 	= nand_hw_ecc_read_page;
	chip->ecc.read_subpage 	= nand_hw_ecc_read_subpage;
	chip->ecc.write_page 	= nand_hw_ecc_write_page;
	chip->ecc.size 			= CFG_NAND_ECC_SIZE;	/* per 512 byte */
	chip->ecc.bytes 		= eccbytes;				/* 8 bit ecc */
	chip->ecc.layout		= &nx_nand_oob;
	chip->verify_buf		= nand_hw_ecc_verify_byf;

	NX_NAND_CreateLookupTable();
	return 0;
}


//------------------------------------------------------------------------------
// NAND CTRL REGISGER:
//------------------------------------------------------------------------------
#define BASEADDR_NFREG			IO_ADDRESS(PHY_BASEADDR_NAND_MODULE)
#define BASEADDR_NFCTRL			(BASEADDR_NFREG)			// 0xC001587C
#define BASEADDR_NFECC			(BASEADDR_NFREG + 0x04)		// 0xC0015880
#define BASEADDR_NFORGECC		(BASEADDR_NFREG + 0x20)		// 0xC001589C
#define BASEADDR_NFECCSTATUS	(BASEADDR_NFREG + 0x40)		// 0xC00158BC
#define BASEADDR_NFSYNDROME		(BASEADDR_NFREG + 0x44)		// 0xC00158C0

#define REG_NFCTRL				(*(volatile unsigned int *)(BASEADDR_NFREG))
#define REG_NFECCSTATUS			(*(volatile unsigned int *)(BASEADDR_NFECCSTATUS))

#define NX_NFECCSTATUS_ERROR	(1U<< 2)
#define NX_NFECCSTATUS_DECDONE	(1U<< 1)
#define NX_NFECCSTATUS_ENCDONE	(1U<< 0)
#define NX_NFCTRL_ECCRST		(1U<<11)

static char BASEADDR_POLY[64*1024] = { 0, };
//------------------------------------------------------------------------------
// BCH variables:
//------------------------------------------------------------------------------
//	k : number of information
//	m : dimension of Galois field.
//	t : number of error that can be corrected.
//	n : length of codeword = 2^m - 1
//	r : number of parity bit = m * t
//------------------------------------------------------------------------------
#define NX_BCH_VAR_K		(512 * 8)
#define NX_BCH_VAR_M		(13)
#define NX_BCH_VAR_T		(HW_ECC_MODE)		// 4 or 8 or 16

#define NX_BCH_VAR_N		(((1<<NX_BCH_VAR_M)-1))
#define NX_BCH_VAR_R		(NX_BCH_VAR_M * NX_BCH_VAR_T)

#define NX_BCH_VAR_TMAX		(16)
#define NX_BCH_VAR_RMAX		(NX_BCH_VAR_M * NX_BCH_VAR_TMAX)

#define NX_BCH_VAR_R32		((NX_BCH_VAR_R   +31)/32)
#define NX_BCH_VAR_RMAX32	((NX_BCH_VAR_RMAX+31)/32)


typedef struct tag_POLYNOMIALS
{
	short BCH_AlphaToTable[8192];
	short BCH_IndexOfTable[8192];
	int elp[(NX_BCH_VAR_TMAX*2)+1][(NX_BCH_VAR_TMAX*2)*2]; 	// Error locator polynomial (ELP)
	int B  [(NX_BCH_VAR_TMAX*2)+1][(NX_BCH_VAR_TMAX*2)*2];	// Scratch polynomial
} POLYNOMIALS;


//------------------------------------------------------------------------------
// Generate GF(2**NX_BCH_VAR_M) from the primitive polynomial p(X) in p[0]..p[NX_BCH_VAR_M]
// The lookup table looks like:
// index -> polynomial form:   pAlphaTo[ ] contains j = alpha**i;
// polynomial form -> index form:  pIndexOf[j = alpha**i] = i
// pAlphaTo[1] = 2 is the primitive element of GF(2**NX_BCH_VAR_M)
//------------------------------------------------------------------------------
void NX_NAND_CreateLookupTable(void)
{
	int i;
	int mask;	// Register states
	unsigned int p = 0x25AF;	// Primitive polynomials

	POLYNOMIALS *pPoly = (POLYNOMIALS *)BASEADDR_POLY;
	short *   pAlphaTo = &(pPoly->BCH_AlphaToTable[0]);
	short *   pIndexOf = &(pPoly->BCH_IndexOfTable[0]);

	// Galois field implementation with shift registers
	// Ref: L&C, Chapter 6.7, pp. 217
	mask = 1;
	pAlphaTo[NX_BCH_VAR_M] = 0;
	for ( i=0 ; i<NX_BCH_VAR_M ; i++ )	{
		pAlphaTo[ i ] = mask;
		pIndexOf[ pAlphaTo[i] ] = i;

		if ( p & (1U<<i) )
			pAlphaTo[NX_BCH_VAR_M] ^= mask;

		mask <<= 1 ;
	}

	pIndexOf[ pAlphaTo[NX_BCH_VAR_M] ] = NX_BCH_VAR_M;
	mask >>= 1;
	for ( i=NX_BCH_VAR_M+1 ; i<NX_BCH_VAR_N ; i++ ) {
		if ( pAlphaTo[i-1] >= mask )
			pAlphaTo[i] = pAlphaTo[NX_BCH_VAR_M] ^ ((pAlphaTo[i-1] ^ mask) << 1);
		else
			pAlphaTo[i] = pAlphaTo[i-1] << 1;

		pIndexOf[pAlphaTo[i]] = i;
	}
	pIndexOf[0] = -1;
}

//------------------------------------------------------------------------------
#define NX_BCH_AlphaTo( _i_ )		((int)(pPoly->BCH_AlphaToTable[ (_i_) ]))
#define NX_BCH_IndexOf( _i_ )		((int)(pPoly->BCH_IndexOfTable[ (_i_) ]))

int	NX_BCH_MODULAR(int index)
{
	register int modular = NX_BCH_VAR_N;

	while ( index >= modular )
		index -= modular;

	return index;
}

//------------------------------------------------------------------------------
void NX_NAND_SetResetECC(int EccMode)
{
	const U32 BIT_SIZE	= 2;
	const U32 BIT_POS	= 28;
	const U32 BIT_MASK	= ((1<<BIT_SIZE)-1) << BIT_POS;

	register U32 regval;

	EccMode /= 8;	// NFECCMODE[1:0] = 0(4), 1(8), 2(16)

	regval  = REG_NFCTRL;
	regval &= ~(BIT_MASK);	// Unmask bits.
	regval |= (EccMode << BIT_POS);

	// Reset H/W BCH decoder.
	REG_NFCTRL = regval | NX_NFCTRL_ECCRST;
}

//------------------------------------------------------------------------------
void NX_NAND_WaitForDecoding(void)
{
	while (0==(REG_NFECCSTATUS & NX_NFECCSTATUS_DECDONE)) { ; }
}

//------------------------------------------------------------------------------
void NX_NAND_WaitForEncoding(void)
{
	while (0==(REG_NFECCSTATUS & NX_NFECCSTATUS_ENCDONE)) { ; }
}

//------------------------------------------------------------------------------
int	NX_NAND_GetErrorStatus(void)
{
	if (REG_NFECCSTATUS & NX_NFECCSTATUS_ERROR)
		return 1;
	return 0;
}
//------------------------------------------------------------------------------
int	NX_NAND_GetErrorLocation(int *pOddSyn, int *pLocation, int *ErrCnt)
{
	register int i, j, elp_sum ;
	int count;
	int r;				// Iteration steps
	int Delta; 			// Discrepancy value

//	int elp[(NX_BCH_VAR_TMAX*2)+1][(NX_BCH_VAR_TMAX*2)+2]; 	// Error locator polynomial (ELP)
//	int B  [(NX_BCH_VAR_TMAX*2)+1][(NX_BCH_VAR_TMAX*2)+2];	// Scratch polynomial
	POLYNOMIALS *pPoly = (POLYNOMIALS *)BASEADDR_POLY;

	int L[(NX_BCH_VAR_TMAX*2)+1];		// Degree of ELP
	int reg[(NX_BCH_VAR_TMAX*1)+1];	// Register state
	int	s[(NX_BCH_VAR_TMAX*2)];


	for( i=0 ; i<NX_BCH_VAR_T ; i++ )
		s[i*2] = pOddSyn[i];

	// Even syndrome = (Odd syndrome) ** 2
	for( i=1,j=0 ; i<(NX_BCH_VAR_T*2) ; i+=2, j++ )
	{
		if( s[j] == 0 )		s[i] = 0;
		else				s[i] = NX_BCH_AlphaTo( NX_BCH_MODULAR( 2 * NX_BCH_IndexOf(s[j]) ) );
	}

	// Initialization of pPoly->elp, pPoly->B and register
	for( i=0 ; i<=(NX_BCH_VAR_T*2) ; i++ )
	{
		L[i] = 0 ;
		for( j=0 ; j<=(NX_BCH_VAR_T*2) ; j++ )
		{
			pPoly->elp[i][j] = 0 ;
			pPoly->B[i][j] = 0 ;
		}
	}

	for( i=0 ; i<=NX_BCH_VAR_T ; i++ )
	{
		reg[i] = 0 ;
	}

	pPoly->elp[1][0] = 1 ;
	pPoly->elp[1][1] = s[0] ;

	L[1] = 1 ;
	if( s[0] != 0 )
		pPoly->B[1][0] = NX_BCH_AlphaTo( NX_BCH_MODULAR( NX_BCH_VAR_N - NX_BCH_IndexOf(s[0]) ) );
	else
		pPoly->B[1][0] = 0;

	for( r=3 ; r<=(NX_BCH_VAR_T*2)-1 ; r=r+2 )
	{
		// Compute discrepancy
		Delta = s[r-1] ;
		for( i=1 ; i<=L[r-2] ; i++ )
		{
			if( (s[r-i-1] != 0) && (pPoly->elp[r-2][i] != 0) )
				Delta ^= NX_BCH_AlphaTo( NX_BCH_MODULAR( NX_BCH_IndexOf(s[r-i-1]) + NX_BCH_IndexOf(pPoly->elp[r-2][i]) ) );
		}

		if( Delta == 0 )
		{
			L[r] = L[r-2] ;
			for( i=0 ; i<=L[r-2] ; i++ )
			{
				pPoly->elp[r][i] = pPoly->elp[r-2][i];
				pPoly->B[r][i+2] = pPoly->B[r-2][i] ;
			}
		}
		else
		{
			// Form new error locator polynomial
			for( i=0 ; i<=L[r-2] ; i++ )
			{
				pPoly->elp[r][i] = pPoly->elp[r-2][i] ;
			}

			for( i=0 ; i<=L[r-2] ; i++ )
			{
				if( pPoly->B[r-2][i] != 0 )
					pPoly->elp[r][i+2] ^= NX_BCH_AlphaTo( NX_BCH_MODULAR( NX_BCH_IndexOf(Delta) + NX_BCH_IndexOf(pPoly->B[r-2][i]) ) );
			}

			// Form new scratch polynomial and register length
			if( 2 * L[r-2] >= r )
			{
				L[r] = L[r-2] ;
				for( i=0 ; i<=L[r-2] ; i++ )
				{
					pPoly->B[r][i+2] = pPoly->B[r-2][i];
				}
			}
			else
			{
				L[r] = r - L[r-2];
				for( i=0 ; i<=L[r-2] ; i++ )
				{
					if( pPoly->elp[r-2][i] != 0 )
						pPoly->B[r][i] = NX_BCH_AlphaTo( NX_BCH_MODULAR( NX_BCH_IndexOf(pPoly->elp[r-2][i]) + NX_BCH_VAR_N - NX_BCH_IndexOf(Delta) ) );
					else
						pPoly->B[r][i] = 0;
				}
			}
		}
	}

	if( L[(NX_BCH_VAR_T*2)-1] > NX_BCH_VAR_T )
	{
		if(ErrCnt)
			*ErrCnt = L[(NX_BCH_VAR_T*2)-1];
		return -1;
	}
	else
	{
		// Chien's search to find roots of the error location polynomial
		// Ref: L&C pp.216, Fig.6.1
		for( i=1 ; i<=L[(NX_BCH_VAR_T*2)-1] ; i++ )
			reg[i] = pPoly->elp[(NX_BCH_VAR_T*2)-1][i];

		count = 0;
		for( i=1 ; i<=NX_BCH_VAR_N ; i++ )
		{
			elp_sum = 1;
			for( j=1 ; j<=L[(NX_BCH_VAR_T*2)-1] ; j++ )
			{
				if( reg[j] != 0 )
				{
					reg[j] = NX_BCH_AlphaTo( NX_BCH_MODULAR( NX_BCH_IndexOf(reg[j]) + j ) );
					elp_sum ^= reg[j] ;
				}
			}

			if( !elp_sum )		// store root and error location number indices
			{
				// Convert error location from systematic form to storage form
				pLocation[count] = NX_BCH_VAR_N - i;

				if (pLocation[count] >= NX_BCH_VAR_R)
				{
					// Data Bit Error
					pLocation[count] = pLocation[count] - NX_BCH_VAR_R;
					pLocation[count] = (NX_BCH_VAR_K-1) - pLocation[count];
				}
				else
				{
					// ECC Error
					pLocation[count] = pLocation[count] + NX_BCH_VAR_K;
				}

				if( pLocation[count] < 0 ) {
					if(ErrCnt)
						*ErrCnt = L[(NX_BCH_VAR_T*2)-1];
					return -1;
				}
				//if( pLocation[count] >= 0 )
				count++;
			}
		}


		if( count == L[(NX_BCH_VAR_T*2)-1] )	// Number of roots = degree of pPoly->elp hence <= NX_BCH_VAR_T errors
		{
			return count;
		}
		else	// Number of roots != degree of ELP => >NX_BCH_VAR_T errors and cannot solve
		{
			if(ErrCnt)
				*ErrCnt = L[(NX_BCH_VAR_T*2)-1];
			return -1;
		}

		/*
		if( count != L[(NX_BCH_VAR_T*2)-1] )
		{
			NX_DEBUG_MSG( "\n\n\t\t ERROR -> count = " );
			NX_DEBUG_DEC( count );
			NX_DEBUG_MSG( ", L = " );
			NX_DEBUG_DEC( L[(NX_BCH_VAR_T*2)-1] );
			NX_DEBUG_MSG( "\n\n" );

			if( count < 4 )		return -1;
		}

		return count;
		*/
	}
}

//------------------------------------------------------------------------------
void NX_NAND_GetGenECC(unsigned int *pECC, int EccMode)
{
	int i, num;
	volatile U32 *pRegECC = (volatile U32 *)BASEADDR_NFECC;

	switch (EccMode) {
	case  4: num = 2;	break;
	case  8: num = 4;	break;
	case 16: num = 7;	break;
	case 24:
	default:
		ERROUT("not support ECC %d bit\n", EccMode);
		return;
	}

	for (i=0 ; i<num ; i++)
		*pECC++ = *pRegECC++;
}

void NX_NAND_SetOriECC(unsigned int *pECC, int EccMode)
{
	int i, num;
	volatile U32 *pRegOrgECC = (volatile U32 *)BASEADDR_NFORGECC;

	switch (EccMode) {
	case  4: num = 2;	break;
	case  8: num = 4;	break;
	case 16: num = 7;	break;
	case 24:
	default:
		ERROUT("not support ECC %d bit\n", EccMode);
		return;
	}

	for (i=0 ; num > i; i++)
		*pRegOrgECC++ = *pECC++;
}


//------------------------------------------------------------------------------
void NX_NAND_GetOddSyndrome(int *pSyndrome)
{
	const U32 BIT_SIZE	= 13;
	const U32 BIT_POS	= 13;
	const U32 BIT_MASK	= ((1UL<<BIT_SIZE)-1);

	register volatile U32 *pReg;
	register U32 regval;
	int i;

	NX_ASSERT( CNULL != pSyndrome );

	pReg = (volatile U32 *)BASEADDR_NFSYNDROME;

	for ( i=0 ; i<(NX_BCH_VAR_T/2) ; i++ ) {
		regval = *pReg++;
		*pSyndrome++ = (int)(regval & BIT_MASK);		// Syndrome <= NFSYNDROME[i][12: 0]
		*pSyndrome++ = (int)(regval >> BIT_POS);		// Syndrome <= NFSYNDROME[i][25:13]
	}
}

#endif /* CONFIG_MTD_NAND_NEXELL_HWECC */
