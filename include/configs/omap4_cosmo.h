/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * LG Optimus Black codename sniper config
 *
 * Copyright (C) 2015 Paul Kocialkowski <contact@paulk.fr>
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/arch/cpu.h>
#include <asm/arch/omap.h>

/*
 * Clocks
 */

#define CFG_SYS_TIMERBASE	GPT2_BASE

/*
 * DRAM
 */
#define CFG_SYS_INIT_RAM_ADDR	0x81000000
#define CFG_SYS_INIT_RAM_SIZE	SZ_4M

/*
 * Memory
 */

#define CFG_SYS_SDRAM_BASE		0x80000000

/*
 * I2C
 */

#define CFG_I2C_MULTI_BUS

/*
 * Input
 */

/*
 * SPL
 */

/*
 * Serial
 */

/*
 * Environment
 */

/*
 * Boot
 */
#define CONFIG_BOOTCOMMAND \
	   "mmc dev 0;" \
	   "ums 0 mmc 0;" \
	   "sleep 3;" \
	   "reset -w;"
#endif
