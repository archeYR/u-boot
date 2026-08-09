// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2015, Bin Meng <bmeng.cn@gmail.com>
 */

#include <init.h>
#include <spl.h>
#include <asm/global_data.h>
#include <asm/mtrr.h>
#include <asm/post.h>
#include <linux/sizes.h>

/* Memory controller registers */

#define MC_BANK_CFG		0x08
#  define MC_BCFG_DIMM0_SZ_MASK		0x00000700
#  define MC_BCFG_DIMM0_PG_SZ_MASK	0x00000070
#  define MC_BCFG_DIMM0_PG_SZ_NO_DIMM	0x00000070

#define MC_REGS 0x8400

unsigned gx1_gx_base(void);

DECLARE_GLOBAL_DATA_PTR;

int gx1_dram_bank_size(char bank)
{
	void *mc_regs;
	u32 bank_cfg;
	unsigned bank_size;

	mc_regs = (void *)(gx1_gx_base() + MC_REGS);
	bank_cfg = readl(gx1_gx_base() + MC_REGS + MC_BANK_CFG);

	switch (bank)
	{
		case 0:
			if ((bank_cfg & MC_BCFG_DIMM0_PG_SZ_MASK) != MC_BCFG_DIMM0_PG_SZ_NO_DIMM)
				bank_size = (0x400000 << ((bank_cfg & MC_BCFG_DIMM0_SZ_MASK) >> 8));
			break;
		case 1:
			bank_cfg >>= 16; /* look at DIMM1 */
			if ((bank_cfg & MC_BCFG_DIMM0_PG_SZ_MASK) != MC_BCFG_DIMM0_PG_SZ_NO_DIMM)
				bank_size = (0x400000 << ((bank_cfg & MC_BCFG_DIMM0_SZ_MASK) >> 8));
			break;
		default:
			bank_size = 0;
	}

	return bank_size;
}

int dram_init(void)
{
	gd->ram_size = gx1_dram_bank_size(0) + gx1_dram_bank_size(1) - SZ_2M - SZ_512K;

	return 0;
}

int dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = 0;
	gd->bd->bi_dram[0].size = gx1_dram_bank_size(0);
	gd->bd->bi_dram[1].start = gx1_dram_bank_size(0);
	gd->bd->bi_dram[1].size = gx1_dram_bank_size(1) - SZ_2M - SZ_512K;

	return 0;
}
