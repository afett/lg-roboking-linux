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

#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <asm/io.h>
#include <asm/sizes.h>
#include <asm/mach-types.h>

/* nexell soc headers */
#include <mach/platform.h>
#include <mach/devices.h>

#if	(0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "nand: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#define ERROUT(msg...)		{ 					\
		printk(KERN_ERR "ERROR: %s, %s line %d: \n",		\
			__FILE__, __FUNCTION__, __LINE__),	\
		printk(KERN_ERR msg); }

#define CLEAR_RnB(r)							\
	r = NX_MCUS_GetInterruptPending(0);			\
	if (r) {									\
		NX_MCUS_ClearInterruptPending(0); 		\
		NX_MCUS_GetInterruptPending  (0); 		\
	}
#define CHECK_RnB()	NX_MCUS_GetInterruptPending(0);

#include <linux/sched.h>
#include <asm/stacktrace.h>
#include <asm/traps.h>
#include <asm/unwind.h>

/*------------------------------------------------------------------------------
 * nand partition
 */
#if defined (CONFIG_MTD_PARTITIONS)
static const char *part_probes[] = { "cmdlinepart", NULL };

#if defined (CONFIG_MTD_NAND_NEXELL_PARTS_MAP)
#include <cfg_nand_map.h>
static inline void def_mtd_parts(struct mtd_info *mtd, struct mtd_partition *parts)
{
}
#else
#define BOOTLD_OFFSET	(8*1024*1024)
static struct mtd_partition partition_map[] = {
	{
		.name   = "Data parts",
		.offset = BOOTLD_OFFSET,
	},
};

static inline void def_mtd_parts(struct mtd_info *mtd, struct mtd_partition *parts)
{
	parts->size = mtd->size - BOOTLD_OFFSET;
}

#endif /* CONFIG_MTD_NAND_NEXELL_PARTS_MAP */
#endif /* CONFIG_MTD_PARTITIONS */

/*------------------------------------------------------------------------------
 * nand interface
 */
static void nand_dev_select(struct mtd_info *mtd, int chipnr)
{
	DBGOUT("%s, chipnr=%d\n", __func__, chipnr);
#if defined(CFG_NAND_OPTIONS)
	struct nand_chip *chip = mtd->priv;
	chip->options |= CFG_NAND_OPTIONS;
#endif

	if (chipnr > 4) {
		ERROUT("not support nand chip index %d\n", chipnr);
		return;
	}

	if (-1 == chipnr) {
		NX_MCUS_SetNFCSEnable(CFALSE);		// nand chip select control disable
	} else {
		NX_MCUS_SetNFBank(chipnr);
		NX_MCUS_SetNFCSEnable(CTRUE);
	}
}

#define MASK_CLE	0x10	/* NFCM   + 2C00_0000 */
#define MASK_ALE	0x18	/* NFADDR + 2C00_0000 */

static void nand_dev_ctrl(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *chip = mtd->priv;
	void __iomem* addr = chip->IO_ADDR_W;
	int ret = 0;

	if (cmd == NAND_CMD_NONE)
		return;

	if (ctrl & NAND_CLE)
		writeb(cmd, addr + MASK_CLE);
	else if (ctrl & NAND_ALE)
		writeb(cmd, addr + MASK_ALE);

	if (cmd != NAND_CMD_RESET &&
		cmd != NAND_CMD_READSTART)
		CLEAR_RnB(ret);
}

static int nand_dev_ready(struct mtd_info *mtd)
{
	int ret;
	CLEAR_RnB(ret);
	DBGOUT("[%s, RnB=%d]\n", ret?"READY":"BUSY", NX_MCUS_IsNFReady());
	return ret;
}

static void nand_dev_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	readsl(chip->IO_ADDR_R, buf, (len >> 2));
	if (len & 3)
		readsb(chip->IO_ADDR_R, buf + (len & ~0x3), (len & 3));
}

static void nand_dev_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	writesl(chip->IO_ADDR_W, buf, (len >> 2));
	if (len & 3)
		writesb(chip->IO_ADDR_W, buf + (len & ~0x3), (len & 3));
}
/*------------------------------------------------------------------------------
 * u-boot nand module
 */
#if defined (CONFIG_MTD_NAND_NEXELL_HWECC) || defined (CONFIG_MTD_NAND_NEXELL_HWECC_MODULE)
extern int nand_hw_ecc_init (struct nand_chip *chip);
extern int nand_hw_ecc_check(struct mtd_info  *mtd);
#endif

static int nand_dev_init(struct nand_chip *chip)
{
	int ret = 0;
	DBGOUT("%s\n", __func__);

	NX_MCUS_SetBaseAddress((U32)IO_ADDRESS(NX_MCUS_GetPhysicalAddress()));
	NX_MCUS_SetECCMode(CFG_NAND_ECC_MODE);
	NX_MCUS_SetAutoResetEnable(CFG_NAND_AUTORESET);

	NX_MCUS_ClearInterruptPending(0);
	NX_MCUS_SetInterruptEnableAll(CFALSE);

	NX_MCUS_SetNFBank(0);
	NX_MCUS_SetNFCSEnable(CFALSE);

	/* insert callbacks */
	chip->IO_ADDR_R 	= (void __iomem *)__PB_IO_MAP_NAND_VIRT;
	chip->IO_ADDR_W 	= (void __iomem *)__PB_IO_MAP_NAND_VIRT;
	chip->cmd_ctrl 		= nand_dev_ctrl;
	chip->dev_ready 	= nand_dev_ready;
	chip->select_chip 	= nand_dev_select;
	chip->chip_delay 	= 15;
	chip->read_buf 		= nand_dev_read_buf;
	chip->write_buf 	= nand_dev_write_buf;

#if !defined (CONFIG_MTD_NAND_NEXELL_HWECC)
	chip->ecc.mode = NAND_ECC_SOFT;
	printk(KERN_INFO "NAND ecc: Software \n");
#else
	ret = nand_hw_ecc_init(chip);
	printk(KERN_INFO "NAND ecc: Hardware %d bit \n", CFG_NAND_ECC_MODE);
#endif
	return ret;
}

static int nand_dev_check(struct mtd_info *mtd)
{
	int ret = 0;
#if defined (CONFIG_MTD_NAND_NEXELL_HWECC)
	ret = nand_hw_ecc_check(mtd);
#endif
	return ret;
}

#if !defined (CONFIG_SYS_NAND_MAX_CHIPS)
#define CONFIG_SYS_NAND_MAX_CHIPS   1
#endif

static struct mtd_info 	nand_info = { 0, };
static struct nand_chip nand_chip = { 0, };

static int __init nand_drv_init(void)
{
	struct mtd_info 	* mtd  = &nand_info;
	struct nand_chip 	* chip = &nand_chip;
	struct mtd_partition* mtd_parts = NULL;
	int parts_nr = 0;
	int maxchips = CONFIG_SYS_NAND_MAX_CHIPS;
	int cmd_part = 1;

	mtd->priv  = chip;
	mtd->owner = THIS_MODULE;

	printk(KERN_NOTICE "Searching NAND device ...\n");

	if (nand_dev_init(chip))
		return -ENXIO;

	if (nand_scan(mtd, maxchips))
		return -ENXIO;

	if (nand_dev_check(mtd))
		return -ENXIO;

#if defined (CONFIG_MTD_PARTITIONS)
	mtd->name = NAND_DEV_NAME;
	parts_nr  = parse_mtd_partitions(mtd, part_probes, &mtd_parts, 0);
	if (0 >= parts_nr) {
		cmd_part  = 0;
		mtd_parts = partition_map;
		parts_nr  = ARRAY_SIZE(partition_map);
		def_mtd_parts(mtd, partition_map);
	}

	/* Register the partitions */
	add_mtd_partitions(mtd, mtd_parts, parts_nr);

	printk(KERN_NOTICE "Using %s partition definition\n",
		cmd_part?"command line":"static");
#endif
	return 0;
}

static void __exit nand_drv_exit(void)
{
#if defined (CONFIG_MTD_PARTITIONS)
	struct mtd_info *mtd = &nand_info;

	/* Unregister the partitions */
	del_mtd_device(mtd);
#endif
}

module_init(nand_drv_init);
module_exit(nand_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jhkim <jhkim@nexell.co.kr>");
MODULE_DESCRIPTION("MTD nand driver for the Nexell");
