/*
 * Copyright (C) 2011 Artem Makhutov <artem@makhutov.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA
 */

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <mach/io.h>
#include <mach/iomap.h>
#include <linux/mfd/core.h>
#include <linux/gpio.h>

#include "board.h"
#include "board-betelgeuse.h"
#include "devices.h"
#include "gpio-names.h"

#include "../../../drivers/staging/nvec2/nvec.h"
#include "../../../drivers/staging/nvec2/nvec_power.h"

/* Power controller of Nvidia embedded controller platform data */
static struct nvec_power_platform_data nvec_power_pdata = {
	.low_batt_irq = TEGRA_GPIO_TO_IRQ(BETELGEUSE_LOW_BATT),	/* If there is a low battery IRQ */
	.in_s3_state_gpio = BETELGEUSE_IN_S3,			/* Gpio pin used to flag that system is suspended */
	.low_batt_alarm_percent = 5,				/* Percent of batt below which system is forcibly turned off */
};

/* Power controller of Nvidia embedded controller */
static struct nvec_subdev_info nvec_subdevs[] = {
        {
                .name = "nvec-power",
                .id   = 1,
                .platform_data = &nvec_power_pdata,
        },
        {
                .name = "nvec-kbd",
                .id   = 1,
        },
        {
                .name = "nvec-mouse",
                .id   = 1,
        },
};

/* The NVidia Embedded controller */
static struct nvec_platform_data nvec_mfd_platform_data = {
	.i2c_addr       = BETELGEUSE_NVEC_I2C_ADDR,
	.gpio           = BETELGEUSE_NVEC_REQ,
	.irq            = INT_I2C3,
	.base           = TEGRA_I2C3_BASE,
	.size           = TEGRA_I2C3_SIZE,
	.clock          = "tegra-i2c.2",
	.subdevs        = nvec_subdevs,
	.num_subdevs = ARRAY_SIZE(nvec_subdevs),
};

static struct platform_device betelgeuse_nvec_mfd = {
	.name = "nvec",
	.dev = {
		.platform_data = &nvec_mfd_platform_data,
	},
};

static struct platform_device *betelgeuse_nv_devices[] __initdata = {
	&betelgeuse_nvec_mfd,
};

int __init betelgeuse_nvec_register_devices(void)
{
	platform_add_devices(betelgeuse_nv_devices, ARRAY_SIZE(betelgeuse_nv_devices));
	return 0;
}
