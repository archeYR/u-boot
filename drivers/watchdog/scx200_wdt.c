// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2017 Intel Corporation
 */
#include <dm.h>
#include <log.h>
#include <wdt.h>
#include <div64.h>
#include <asm/post.h>

/* Verify that the configuration block really is there */
#define SCx200_CBA 0x3e		/* Configuration Base Address Register */
#define scx200_cb_probe(base) (inw((base) + SCx200_CBA) == (base))

/* General Configuration Block */
#define SCx200_CB_BASE_FIXED 0x9000	/* Base fixed at 0x9000 according to errata? */

/* Watchdog Timer */
#define SCx200_WDT_OFFSET 0x00	/* offset within configuration block */
#define SCx200_WDT_SIZE 0x05	/* size */

#define SCx200_WDT_WDTO 0x00	/* Time-Out Register */
#define SCx200_WDT_WDCNFG 0x02	/* Configuration Register */
#define SCx200_WDT_WDSTS 0x04	/* Status Register */
#define SCx200_WDT_WDSTS_WDOVF (1<<0) /* Overflow bit */
#define SCx200_WDT_WDSTS_WDINT (1<<1) /* Overflow bit */
#define SCx200_WDT_WDSTS_WDSMI (1<<2) /* Overflow bit */
#define SCx200_WDT_WDSTS_WDRST (1<<3) /* Overflow bit */

/* Bits of the WDCNFG register */
#define W_ENABLE 0x00fa		/* Enable watchdog */
#define W_DISABLE 0x0000	/* Disable watchdog */

/* The scaling factor for the timer, this depends on the value of W_ENABLE */
#define W_SCALE (32768/1024)

static int margin = 60;
static u16 wdto_restart;

static void scx200_wdt_ping(void)
{
	outw(wdto_restart, SCx200_CB_BASE_FIXED + SCx200_WDT_WDTO);
}

static void scx200_wdt_update_margin(void)
{
	printf("timer margin %d seconds\n", margin);
	wdto_restart = margin * W_SCALE;
}

static int scx200_wdt_reset(struct udevice *dev)
{
	scx200_wdt_ping();
	return 0;
}

static int scx200_wdt_stop(struct udevice *dev)
{
	puts("disabling watchdog timer\n");

	outw(0, SCx200_CB_BASE_FIXED + SCx200_WDT_WDTO);
	outb(SCx200_WDT_WDSTS_WDOVF, SCx200_CB_BASE_FIXED + SCx200_WDT_WDSTS);
	outw(W_DISABLE, SCx200_CB_BASE_FIXED + SCx200_WDT_WDCNFG);
	printf("Watchdog status: 0x%x\n", inw(SCx200_CB_BASE_FIXED + SCx200_WDT_WDSTS));
	return 0;
}

static int scx200_wdt_start(struct udevice *dev, u64 timeout_ms, ulong flags)
{
	margin = timeout_ms/1000;
	scx200_wdt_update_margin();
	printf("enabling watchdog timer, wdto_restart = %d\n", wdto_restart);

	outw(0, SCx200_CB_BASE_FIXED + SCx200_WDT_WDTO);
	outb(SCx200_WDT_WDSTS_WDOVF, SCx200_CB_BASE_FIXED + SCx200_WDT_WDSTS);
	outw(W_ENABLE, SCx200_CB_BASE_FIXED + SCx200_WDT_WDCNFG);

	scx200_wdt_ping();
	return 0;
}

static int scx200_wdt_expire_now(struct udevice *dev, ulong flags)
{
	margin = 0;
	scx200_wdt_update_margin();
	scx200_wdt_ping();
	return 0;
}

static const struct wdt_ops scx200_wdt_ops = {
	.reset = scx200_wdt_reset,
	.start = scx200_wdt_start,
	.stop = scx200_wdt_stop,
	.expire_now = scx200_wdt_expire_now,
};

static const struct udevice_id scx200_wdt_ids[] = {
	{ .compatible = "nsc,scx200-wdt" },
	{ /* sentinel */ }
};

static int scx200_wdt_probe(struct udevice *dev)
{
	if (!scx200_cb_probe(SCx200_CB_BASE_FIXED))
		return -1;
	printf("%s: Probing wdt%u\n", __func__, dev_seq(dev));
	return 0;
}

U_BOOT_DRIVER(wdt_scx200_) = {
	.name = "wdt_scx200",
	.id = UCLASS_WDT,
	.of_match = scx200_wdt_ids,
	.ops = &scx200_wdt_ops,
	.probe = scx200_wdt_probe,
};
