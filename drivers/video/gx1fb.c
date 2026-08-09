// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2012 Stephen Warren
 */

#include <dm.h>
#include <log.h>
#include <video.h>
#include <asm/io.h>
#include <asm/global_data.h>

#define DC_REGS 0x8300
#define DC_UNLOCK		0x00
#  define DC_UNLOCK_CODE		0x00004758

#define DC_OUTPUT_CFG		0x0C
#  define DC_OCFG_PCKE			0x00000004
#  define DC_OCFG_PDEL			0x00001000
#  define DC_OCFG_PDEH			0x00002000

#define DC_FB_ST_OFFSET		0x10
#define DC_LINE_DELTA		0x24
#define DC_BUF_SIZE		0x28

#define MC_GBASE_ADD		0x14
#  define MC_GADD_GBADD_MASK		0x000003ff


#define MC_REGS 0x8400

unsigned gx1_gx_base(void);
extern int gx1_dram_bank_size(char bank);

int gx1_frame_buffer_size(void)
{
	void *mc_regs;
	unsigned fb_base;

	mc_regs = (void *)(gx1_gx_base() + MC_REGS);

	fb_base = (readl(mc_regs + MC_GBASE_ADD) & MC_GADD_GBADD_MASK) << 19;

	return (gx1_dram_bank_size(0) + gx1_dram_bank_size(1)) - fb_base;
}

static int gx1_line_delta(int xres, int bpp)
{
	int line_delta = xres * (bpp >> 3);

	if (line_delta > 2048)
		line_delta = 4096;
	else if (line_delta > 1024)
		line_delta = 2048;
	else
		line_delta = 1024;
	return line_delta;
}


static int gx1fb_video_probe(struct udevice *dev)
{
	struct video_uc_plat *plat = dev_get_uclass_plat(dev);
	struct video_priv *uc_priv = dev_get_uclass_priv(dev);
	unsigned int gxbase;
	u32 ocfg;

	gxbase = gx1_gx_base();

	/* Unlock the display controller registers. */
	readl(gxbase + DC_REGS + DC_UNLOCK);
	writel(DC_UNLOCK_CODE, gxbase + DC_REGS + DC_UNLOCK);

	/* Framebuffer start offset. */
	//writel(0, gxbase + DC_REGS + DC_FB_ST_OFFSET);

	/* Line delta and line buffer length for 16BPP. */
	writel(gx1_line_delta(640, 16) >> 2, gxbase + DC_REGS + DC_LINE_DELTA);
	writel(((640 * 16/8) >> 3) + 2,
	       gxbase + DC_REGS + DC_BUF_SIZE);

	/* Set 16BPP mode */
	ocfg = DC_OCFG_PCKE | DC_OCFG_PDEL | DC_OCFG_PDEH;
	writel(ocfg, gxbase + DC_REGS + DC_OUTPUT_CFG);

	/* Relock display controller registers */
	writel(0, gxbase + DC_REGS + DC_UNLOCK);

	/* Enable dcache for the frame buffer */
	video_set_flush_dcache(dev, true);

	uc_priv->bpix = VIDEO_BPP16;
	uc_priv->line_length = gx1_line_delta(640, 16);
	uc_priv->xsize = 640;
	uc_priv->ysize = 480;
	uc_priv->rot = 0;
	plat->base = gxbase + 0x800000;
	plat->size = gx1_frame_buffer_size();

	return 0;
}

static const struct udevice_id gx1fb_video_ids[] = {
	{ .compatible = "nsc,gx1-fb" },
	{ }
};

U_BOOT_DRIVER(gx1fb_video) = {
	.name	= "gx1fb_video",
	.id	= UCLASS_VIDEO,
	.of_match = gx1fb_video_ids,
	.probe	= gx1fb_video_probe,
};
