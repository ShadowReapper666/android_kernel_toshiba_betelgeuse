/*
 * arch/arm/mach-tegra/board-betelgeuse.h
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

#ifndef _MACH_TEGRA_BOARD_BETELGEUSE_H
#define _MACH_TEGRA_BOARD_BETELGEUSE_H

#define TPS6586X_INT_BASE	TEGRA_NR_IRQS

#define BETELGEUSE_CAMERA_POWER 	TEGRA_GPIO_PV4 	/* 1=powered on */

#define BETELGEUSE_BL_ENB		TEGRA_GPIO_PB5
#define BETELGEUSE_LVDS_SHUTDOWN	TEGRA_GPIO_PB2		// Is this right?
#define BETELGEUSE_EN_VDD_PANEL		TEGRA_GPIO_PC6 
#define BETELGEUSE_BL_VDD		TEGRA_GPIO_PW0
#define BETELGEUSE_BL_PWM		TEGRA_GPIO_PB4 		/* PWM */
#define BETELGEUSE_HDMI_ENB		TEGRA_GPIO_PV5 		/* unconfirmed */ // Does betelgeuse have HDMI enbl?
#define BETELGEUSE_HDMI_HPD		TEGRA_GPIO_PN7		/* 1=HDMI plug detected */

#define BETELGEUSE_BL_PWM_ID		0			/* PWM0 controls backlight */

#define BETELGEUSE_MEM_SIZE 		SZ_512M			/* Total memory */
#define BETELGEUSE_MEM_BANKS		1

#define BETELGEUSE_GPU_MEM_SIZE 	SZ_128M			/* Memory reserved for GPU */

#define BETELGEUSE_FB1_MEM_SIZE 	SZ_8M			/* Memory reserved for Framebuffer 1: LCD */
#define BETELGEUSE_FB2_MEM_SIZE 	SZ_1M			/* Memory reserved for Framebuffer 2: HDMI out */

#define BETELGEUSE_DOCK_GPIO		TEGRA_GPIO_PV2

/*#define BETELGEUSE_48KHZ_AUDIO*/ /* <- define this if you want 48khz audio sampling rate instead of 44100Hz */


// TPS6586x GPIOs as registered 
#define PMU_GPIO_BASE		(TEGRA_NR_GPIOS) 
#define PMU_GPIO0 		(PMU_GPIO_BASE)
#define PMU_GPIO1 		(PMU_GPIO_BASE + 1) 
#define PMU_GPIO2 		(PMU_GPIO_BASE + 2)
#define PMU_GPIO3 		(PMU_GPIO_BASE + 3)

#define PMU_IRQ_BASE		(TEGRA_NR_IRQS)
#define PMU_IRQ_RTC_ALM1 	(TPS6586X_INT_BASE + TPS6586X_INT_RTC_ALM1)

#define	BETELGEUSE_ENABLE_VDD_VID	TEGRA_GPIO_PD1	/* 1=enabled.  Powers HDMI. Wait 500uS to let it stabilize before returning */


// SDIO stuff
#define BETELGEUSE_SDHC_INT_CD		TEGRA_GPIO_PH2
#define BETELGEUSE_SDHC_INT_WP		TEGRA_GPIO_PH3
#define BETELGEUSE_SDHC_INT_POWER	TEGRA_GPIO_PI6

#define BETELGEUSE_SDHC_EXT_CD		TEGRA_GPIO_PI5
#define BETELGEUSE_SDHC_EXT_WP		TEGRA_GPIO_PH1
#define BETELGEUSE_SDHC_EXT_POWER	TEGRA_GPIO_PT3

// Touch screen
#define BETELGEUSE_TS_IRQ		TEGRA_GPIO_PU4

// BT
#define BETELGEUSE_BT_RESET		TEGRA_GPIO_PU0

/* WLAN pwr and reset gpio */
#define TEGRA_GPIO_WLAN_PWR_LOW		TEGRA_GPIO_PK5
#define TEGRA_GPIO_WLAN_RST_LOW		TEGRA_GPIO_PK6

#define BETELGEUSE_LOW_BATT		TEGRA_GPIO_PW3 /*(0=low battery)*/
#define BETELGEUSE_IN_S3		TEGRA_GPIO_PAA7 /*1 = in S3 */

#define BETELGEUSE_NVEC_REQ	TEGRA_GPIO_PBB1	/* Set to 0 to send a command to the NVidia Embedded controller */
#define BETELGEUSE_NVEC_I2C_ADDR 0x8a 			/* I2C address of Tegra, when acting as I2C slave */

//#define TEGRA_GPIO_POWERKEY	 	TEGRA_GPIO_PJ7
#define TEGRA_GPIO_POWERKEY	 	TEGRA_GPIO_PA0

// USB
#define BETELGEUSE_USB1_RESET	TEGRA_GPIO_PV0
#define BETELGEUSE_USB0_VBUS	TEGRA_GPIO_PD0
#define BETELGEUSE_USB2_VBUS	TEGRA_GPIO_PD3

// Sensors
#define ADT7461_IRQ_GPIO	TEGRA_GPIO_PN6
#define AKM8975_IRQ_GPIO	TEGRA_GPIO_PV1
#define LSM303DHL_IRQ_GPIO	TEGRA_GPIO_PV7
#define FOLIOCAP_IRQ_GPIO	TEGRA_GPIO_PU1


// WM8903 gpios
#define WM8903_GPIO_BASE	(TEGRA_NR_GPIOS + 32)
#define WM8903_GP1		(WM8903_GPIO_BASE + 0)
#define WM8903_GP2		(WM8903_GPIO_BASE + 1)
#define WM8903_GP3		(WM8903_GPIO_BASE + 2)
#define WM8903_GP4		(WM8903_GPIO_BASE + 3)
#define WM8903_GP5		(WM8903_GPIO_BASE + 4)

// Audio
#define TEGRA_GPIO_CDC_IRQ	TEGRA_GPIO_PX3
#define TEGRA_GPIO_SPKR_EN	WM8903_GP3
#define BETELGEUSE_HP_DETECT	TEGRA_GPIO_PW2 	/* HeadPhone detect for audio codec: 1=Hedphone plugged */
#define TEGRA_GPIO_HP_DET	TEGRA_GPIO_PW2

#define TEGRA_GPIO_INT_MIC_EN	TEGRA_GPIO_PX0
#define TEGRA_GPIO_EXT_MIC_EN	TEGRA_GPIO_PX1


/* The switch used to indicate rotation lock */
//#define SW_ROTATION_LOCK 	(SW_MAX-1)

extern void betelgeuse_wifi_set_cd(int val);

extern void betelgeuse_init_emc(void);
extern void betelgeuse_pinmux_init(void);
extern void betelgeuse_clks_init(void);

extern int betelgeuse_usb_register_devices(void);
extern int betelgeuse_audio_register_devices(void);
extern int betelgeuse_jack_register_devices(void);
int __init betelgeuse_panel_init(void);
extern int betelgeuse_uart_register_devices(void);
extern int betelgeuse_spi_register_devices(void);
extern int betelgeuse_aes_register_devices(void);
extern int betelgeuse_wdt_register_devices(void);
extern int betelgeuse_i2c_register_devices(void);
extern int betelgeuse_power_register_devices(void);
extern int betelgeuse_keyboard_register_devices(void);
extern int betelgeuse_touch_register_devices(void);
extern int betelgeuse_sdhci_register_devices(void);
extern int betelgeuse_sensors_register_devices(void);
extern int betelgeuse_wlan_register_devices(void);
extern void betelgeuse_setup_bluesleep(void);
extern void betelgeuse_bt_rfkill(void);
extern int betelgeuse_camera_register_devices(void);
extern int betelgeuse_nvec_register_devices(void);

#endif
