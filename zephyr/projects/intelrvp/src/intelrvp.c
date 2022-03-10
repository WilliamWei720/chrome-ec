/* Copyright 2022 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* TODO: b/218904113: Convert to using Zephyr GPIOs */
#include "gpio.h"
#include "hooks.h"

static void board_init(void)
{
	/* Enable SOC SPI */
	gpio_pin_set_dt(GPIO_DT_FROM_NODELABEL(ec_spi_oe_mecc), 1);
}
DECLARE_HOOK(HOOK_INIT, board_init, HOOK_PRIO_LAST);
