// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2010
 * Texas Instruments Incorporated, <www.ti.com>
 * Steve Sakoman  <steve@sakoman.com>
 */
#include <init.h>
#include <log.h>
#include <net.h>
#include <serial.h>
#include <linux/usb/musb.h>
#include <asm/omap_musb.h>
#include <asm/global_data.h>
#include <asm/mach-types.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/mmc_host_def.h>
#include <asm/arch/clock.h>
#include <asm/arch/gpio.h>
#include <asm/gpio.h>
#include <env.h>
#include <twl6030.h>
#include <cpu_func.h>

DECLARE_GLOBAL_DATA_PTR;

const struct omap_sysinfo sysinfo = {
	"Board: OMAP4 Cosmo\n"
};

/**
 * @brief board_init
 *
 * Return: 0
 */
int board_init(void)
{
	gpmc_init();

	gd->bd->bi_arch_number = 3000;
	gd->bd->bi_boot_params = (0x80000000 + 0x100); /* boot param addr */

	return 0;
}

#if defined(CONFIG_SPL_OS_BOOT)
int spl_start_uboot(void)
{
	return 0;
}
#endif /* CONFIG_SPL_OS_BOOT */

int board_eth_init(struct bd_info *bis)
{
	return 0;
}

int misc_init_r(void)
{
#ifdef CONFIG_TWL6030_POWER
	twl6030_init_battery_charging();
	/* Seems like vendor U-Boot doesn't power up the SD card? */
	twl6030_power_mmc_init(0);
#endif
	return 0;
}

void set_muxconf_regs(void)
{

}

#ifdef CONFIG_REVISION_TAG
/*
 * get_board_rev() - get board revision
 */
u32 get_board_rev(void)
{
	return 1;
}
#endif
