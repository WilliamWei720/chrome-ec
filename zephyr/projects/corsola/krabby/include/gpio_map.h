/* Copyright 2021 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __ZEPHYR_GPIO_MAP_H
#define __ZEPHYR_GPIO_MAP_H

#include <devicetree.h>
#include <gpio_signal.h>

/*
 * TODO(b:188674805) create a driver to pull this information from DeviceTree
 */
#include "power/mt8192.h"

#define GPIO_ENTERING_RW		GPIO_UNIMPLEMENTED
#define GPIO_WP_L			GPIO_UNIMPLEMENTED

#ifdef CONFIG_PLATFORM_EC_GMR_TABLET_MODE
#define GMR_TABLET_MODE_GPIO_L GPIO_TABLET_MODE_L
#endif

/* TODO: remove after icm426xx driver added */
static inline void motion_interrupt(enum gpio_signal signal)
{
}

/*
 * Set EC_CROS_GPIO_INTERRUPTS to a space-separated list of GPIO_INT items.
 *
 * Each GPIO_INT requires three parameters:
 *   gpio_signal - The enum gpio_signal for the interrupt gpio
 *   interrupt_flags - The interrupt-related flags (e.g. GPIO_INT_EDGE_BOTH)
 *   handler - The platform/ec interrupt handler.
 *
 * Ensure that this files includes all necessary headers to declare all
 * referenced handler functions.
 *
 * For example, one could use the follow definition:
 * #define EC_CROS_GPIO_INTERRUPTS \
 *   GPIO_INT(NAMED_GPIO(h1_ec_pwr_btn_odl), GPIO_INT_EDGE_BOTH, button_print)
 */
#define EC_CROS_GPIO_INTERRUPTS						\
	GPIO_INT(GPIO_POWER_BUTTON_L,					\
		 GPIO_INT_EDGE_BOTH, power_button_interrupt)		\
	GPIO_INT(GPIO_LID_OPEN,						\
		 GPIO_INT_EDGE_BOTH, lid_interrupt)			\
	GPIO_INT(GPIO_TABLET_MODE_L,					\
		 GPIO_INT_EDGE_BOTH, gmr_tablet_switch_isr)		\
	GPIO_INT(GPIO_AP_EC_WARM_RST_REQ,				\
		 GPIO_INT_EDGE_RISING, chipset_reset_request_interrupt)	\
	GPIO_INT(GPIO_AP_IN_SLEEP_L,					\
		 GPIO_INT_EDGE_BOTH, power_signal_interrupt)		\
	GPIO_INT(GPIO_BASE_IMU_INT_L,					\
		 GPIO_INT_EDGE_FALLING, motion_interrupt)		\
	GPIO_INT(GPIO_LID_ACCEL_INT_L,					\
		 GPIO_INT_EDGE_FALLING, lis2dw12_interrupt)		\
	GPIO_INT(GPIO_VOLUME_DOWN_L,					\
		 GPIO_INT_EDGE_BOTH, button_interrupt)			\
	GPIO_INT(GPIO_VOLUME_UP_L,					\
		 GPIO_INT_EDGE_BOTH, button_interrupt)			\
	GPIO_INT(GPIO_AP_XHCI_INIT_DONE,				\
		 GPIO_INT_EDGE_BOTH, usb_a0_interrupt)			\
	GPIO_INT(GPIO_AC_PRESENT,					\
		 GPIO_INT_EDGE_BOTH, extpower_interrupt)		\
	GPIO_INT(GPIO_WP,						\
		 GPIO_INT_EDGE_BOTH, switch_interrupt)			\
	GPIO_INT(GPIO_SPI0_CS,						\
		 GPIO_INT_EDGE_FALLING, spi_event)			\
	GPIO_INT(GPIO_X_EC_GPIO2,					\
		 GPIO_INT_EDGE_BOTH, x_ec_interrupt)

#endif /* __ZEPHYR_GPIO_MAP_H */
