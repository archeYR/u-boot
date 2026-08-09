// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2015, Bin Meng <bmeng.cn@gmail.com>
 */

#include <cpu_func.h>
#include <init.h>
#include <pci.h>
#include <dm/platdata.h>
#include <asm/irq.h>
#include <asm/post.h>
#include <asm/processor.h>
#include <asm/u-boot-x86.h>
#include <asm/i8254.h>

/* GX1 configuration I/O registers */

#define CONFIG_CCR3 0xc3
#  define CONFIG_CCR3_MAPEN 0x10
#define CONFIG_GCR  0xb8

static u8 gx1_read_conf_reg(u8 reg)
{
	u8 val, ccr3;

	outb(CONFIG_CCR3, 0x22);
	ccr3 = inb(0x23);
	outb(CONFIG_CCR3, 0x22);
	outb(ccr3 | CONFIG_CCR3_MAPEN, 0x23);
	outb(reg, 0x22);
	val = inb(0x23);
	outb(CONFIG_CCR3, 0x22);
	outb(ccr3, 0x23);

	return val;
}

unsigned gx1_gx_base(void)
{
	return (gx1_read_conf_reg(CONFIG_GCR) & 0x03) << 30;
}

void beep(void)
{
	i8254_enable_beep(10000);
}

int board_early_init_r(void)
{
	return 0;
}
