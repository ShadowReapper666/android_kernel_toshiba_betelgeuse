/*
 * arch/arm/mach-tegra/board-betelgeuse.c
 *
 * Copyright (C) 2011 Eduardo José Tagle <ejtagle@tutopia.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/console.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/platform_device.h>
#include <linux/serial_8250.h>
#include <linux/clk.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/dma-mapping.h>
#include <linux/fsl_devices.h>
#include <linux/pda_power.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/reboot.h>
#include <linux/i2c-tegra.h>
#include <linux/memblock.h>
#include <linux/antares_dock.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>
#include <asm/setup.h>

#include <mach/io.h>
#include <mach/w1.h>
#include <mach/iomap.h>
#include <mach/irqs.h>
#include <mach/iomap.h>
#include <mach/sdhci.h>
#include <mach/gpio.h>
#include <mach/clk.h>
//#include <mach/tegra2_i2s.h>
#include <mach/system.h>

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,38)	
#include <mach/suspend.h>
#else
#include "pm.h"
#endif

#include "board.h"
#include "fuse.h"
#include "board-betelgeuse.h"
#include "clock.h"
#include "gpio-names.h"
#include "devices.h"
#include "wakeups-t2.h"
#include "wdt-recovery.h"

/* NVidia bootloader tags */
#define ATAG_NVIDIA			0x41000801
#define MAX_MEMHDL			8

struct tag_tegra {
        __u32 bootarg_len;
        __u32 bootarg_key;
        __u32 bootarg_nvkey;
        __u32 bootarg[];
};

struct memhdl {
        __u32 id;
        __u32 start;
        __u32 size;
};

enum {
        RM = 1,
        DISPLAY,
        FRAMEBUFFER,
        CHIPSHMOO,
        CHIPSHMOO_PHYS,
        CARVEOUT,
        WARMBOOT,
};

static int num_memhdl = 0;

static struct memhdl nv_memhdl[MAX_MEMHDL];

static const char atag_ids[][16] = {
        "RM             ",
        "DISPLAY        ",
        "FRAMEBUFFER    ",
        "CHIPSHMOO      ",
        "CHIPSHMOO_PHYS ",
        "CARVEOUT       ",
        "WARMBOOT       ",
};

static int __init parse_tag_nvidia(const struct tag *tag)
{
        int i;
        struct tag_tegra *nvtag = (struct tag_tegra *)tag;
        __u32 id;

        switch (nvtag->bootarg_nvkey) {
        case FRAMEBUFFER:
                id = nvtag->bootarg[1];
                for (i=0; i<num_memhdl; i++)
                        if (nv_memhdl[i].id == id) {
				tegra_bootloader_fb_start = nv_memhdl[i].start;
				tegra_bootloader_fb_size = nv_memhdl[i].size;
			}
                break;
        case WARMBOOT:
                id = nvtag->bootarg[1];
                for (i=0; i<num_memhdl; i++) {
                        if (nv_memhdl[i].id == id) {
                                tegra_lp0_vec_start = nv_memhdl[i].start;
                                tegra_lp0_vec_size = nv_memhdl[i].size;
                        }
                }
                break;
        }

        if (nvtag->bootarg_nvkey & 0x10000) {
                char pmh[] = " PreMemHdl     ";
                id = nvtag->bootarg_nvkey;
                if (num_memhdl < MAX_MEMHDL) {
                        nv_memhdl[num_memhdl].id = id;
                        nv_memhdl[num_memhdl].start = nvtag->bootarg[1];
                        nv_memhdl[num_memhdl].size = nvtag->bootarg[2];
                        num_memhdl++;
                }
                pmh[11] = '0' + id;
                print_hex_dump(KERN_INFO, pmh, DUMP_PREFIX_NONE,
                                32, 4, &nvtag->bootarg[0], 4*(tag->hdr.size-2), false);
        }
        else if (nvtag->bootarg_nvkey <= ARRAY_SIZE(atag_ids))
                print_hex_dump(KERN_INFO, atag_ids[nvtag->bootarg_nvkey-1], DUMP_PREFIX_NONE,
                                32, 4, &nvtag->bootarg[0], 4*(tag->hdr.size-2), false);
        else
                pr_warning("unknown ATAG key %d\n", nvtag->bootarg_nvkey);

        return 0;
}
__tagtable(ATAG_NVIDIA, parse_tag_nvidia);

extern unsigned int system_serial_low;
extern unsigned int system_serial_high;

static int __init betelgeuse_export_chipid(void)
{
	unsigned long long chip_uid = tegra_chip_uid() ;

	system_serial_low = ((unsigned int *)&chip_uid)[0] ;
	system_serial_high = ((unsigned int *)&chip_uid)[1] ;

	pr_info("%s : Chip UID 0x%llx\n", __func__, chip_uid ) ;
	
	return 0;
}

static struct tegra_suspend_platform_data betelgeuse_suspend = {
	.cpu_timer = 5000,
	.cpu_off_timer = 5000,
	.core_timer = 0x7e7e,
	.core_off_timer = 0x7f,
	.corereq_high = false,
	.sysclkreq_high = true,
	.suspend_mode = TEGRA_SUSPEND_LP0,
};

static struct dock_platform_data dock_on_platform_data = {
	.irq		= TEGRA_GPIO_TO_IRQ(BETELGEUSE_DOCK_GPIO),
	.gpio_num	= BETELGEUSE_DOCK_GPIO,
};
static struct platform_device tegra_dock_device =
{
	.name = "tegra_dock",
	.id   = -1,
	.dev = {
		.platform_data = &dock_on_platform_data,
	},
};
static struct platform_device tegra_wlan_ar6000_pm_device = 
{
        .name           = "wlan_ar6000_pm",
        .id             = 1,
        .num_resources  = 0,
        .resource       = NULL,
};

static struct platform_device *betelgeuse_devices[] __initdata = {
	&tegra_gart_device,
	&tegra_avp_device,
	&tegra_dock_device,
};

static void __init tegra_betelgeuse_init(void)
{
	struct clk *clk;

	/* force consoles to stay enabled across suspend/resume */
	// console_suspend_enabled = 0;	

	/* Init the suspend information */
	tegra_init_suspend(&betelgeuse_suspend);

	/* Set the SDMMC1 (wifi) tap delay to 6.  This value is determined
	 * based on propagation delay on the PCB traces. */
	clk = clk_get_sys("sdhci-tegra.0", NULL);
	if (!IS_ERR(clk)) {
		tegra_sdmmc_tap_delay(clk, 6);
		clk_put(clk);
	} else {
		pr_err("Failed to set wifi sdmmc tap delay\n");
	}

	/* Initialize the pinmux */
	betelgeuse_pinmux_init();

	/* Initialize the clocks - clocks require the pinmux to be initialized first */
	betelgeuse_clks_init();

	/* Register i2c devices - required for Power management and MUST be done before the power register */
	betelgeuse_i2c_register_devices();

	/* Register the power subsystem - Including the poweroff handler - Required by all the others */
	betelgeuse_power_register_devices();

	/* Add ar6000 as it must be one of the first registered devices */
	platform_device_register(&tegra_wlan_ar6000_pm_device);
	
	/* Register GPU devices */
	betelgeuse_panel_init();

	/* Register NVEC devices */
	betelgeuse_nvec_register_devices();
	
	/* Register the USB device */
	betelgeuse_usb_register_devices();

	/* Register UART devices */
	betelgeuse_uart_register_devices();
	
	/* Register SPI devices */
	betelgeuse_spi_register_devices();

	/* Register Audio devices */
	betelgeuse_audio_register_devices();

	/* Register Jack devices */
//	betelgeuse_jack_register_devices();

	/* Register AES encryption devices */
	betelgeuse_aes_register_devices();

	/* Register Watchdog devices */
	betelgeuse_wdt_register_devices();

	/* Register all the keyboard devices */
	betelgeuse_keyboard_register_devices();
	
	/* Register touchscreen devices */
	betelgeuse_touch_register_devices();
	
	/* Register SDHCI devices */
	betelgeuse_sdhci_register_devices();

	/* Register accelerometer device */
	betelgeuse_sensors_register_devices();
	
	/* Register wlan devices */
	betelgeuse_wlan_register_devices();
	
	/* Register Bluetooth powermanagement devices */
	//betelgeuse_bt_pm_register_devices();
//	betelgeuse_bt_rfkill();
//	betelgeuse_setup_bluesleep();

	/* Register Camera powermanagement devices */
	betelgeuse_camera_register_devices();

	/* Enable the ram console */
	tegra_ram_console_debug_init();

	platform_add_devices(betelgeuse_devices, ARRAY_SIZE(betelgeuse_devices));
#if 0
	/* Finally, init the external memory controller and memory frequency scaling
   	   NB: This is not working on BETELGEUSE. And seems there is no point in fixing it,
	   as the EMC clock is forced to the maximum speed as soon as the 2D/3D engine
	   starts.*/
	betelgeuse_init_emc();
#endif
	tegra_release_bootloader_fb();
#ifdef CONFIG_TEGRA_WDT_RECOVERY
	tegra_wdt_recovery_init();
#endif
	betelgeuse_export_chipid();
}

static void __init tegra_betelgeuse_reserve(void)
{
	if (memblock_reserve(0x0, 4096) < 0)
		pr_warn("Cannot reserve first 4K of memory for safety\n");

	/* Reserve the graphics memory */
	tegra_reserve(BETELGEUSE_GPU_MEM_SIZE, BETELGEUSE_FB1_MEM_SIZE, BETELGEUSE_FB2_MEM_SIZE);

	/* Reserve ram console memory */
	tegra_ram_console_debug_reserve(SZ_1M);
}

static void __init tegra_betelgeuse_fixup(struct machine_desc *desc,
	struct tag *tags, char **cmdline, struct meminfo *mi)
{
	mi->nr_banks = BETELGEUSE_MEM_BANKS;
	mi->bank[0].start = PHYS_OFFSET;
	mi->bank[0].size  = BETELGEUSE_MEM_SIZE;
}

MACHINE_START(BETELGEUSE, "betelgeuse")
	.boot_params	= 0x00000100,
	.map_io         = tegra_map_common_io,
	.init_early     = tegra_init_early,
	.init_irq       = tegra_init_irq,
	.timer          = &tegra_timer,
	.init_machine	= tegra_betelgeuse_init,
	.reserve	= tegra_betelgeuse_reserve,
	.fixup		= tegra_betelgeuse_fixup,
MACHINE_END
