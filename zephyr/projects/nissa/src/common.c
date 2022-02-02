/* Copyright 2022 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <device.h>
#include <drivers/cros_cbi.h>

#include "battery.h"
#include "charger.h"
#include "charge_state_v2.h"
#include "chipset.h"
#include "hooks.h"
#include "usb_mux.h"
#include "system.h"

#include "sub_board.h"

#define CPRINTS(format, args...) cprints(CC_SYSTEM, format, ## args)
#define CPRINTF(format, args...) cprintf(CC_SYSTEM, format, ## args)

struct usb_mux usb_muxes[CONFIG_USB_PD_PORT_MAX_COUNT] = {
	{
		.usb_port = 0,
		.driver = &virtual_usb_mux_driver,
		.hpd_update = &virtual_hpd_update,
	},
	{ /* sub-board */
		.usb_port = 1,
		.driver = &virtual_usb_mux_driver,
		.hpd_update = &virtual_hpd_update,
	},
};

static uint8_t cached_usb_pd_port_count;

__override uint8_t board_get_usb_pd_port_count(void)
{
	if (cached_usb_pd_port_count == 0)
		CPRINTS("USB PD Port count not initialized!");
	return cached_usb_pd_port_count;
}

/*
 * Initialise the USB PD port count, which
 * depends on which sub-board is attached.
 */
static void init_usb_pd_port_count(void)
{
	switch (nissa_get_sb_type()) {
	default:
		cached_usb_pd_port_count = 1;
		break;

	case NISSA_SB_C_A:
	case NISSA_SB_C_LTE:
		cached_usb_pd_port_count = 2;
		break;
	}
}
/*
 * Make sure setup is done after EEPROM is readable.
 */
DECLARE_HOOK(HOOK_INIT, init_usb_pd_port_count, HOOK_PRIO_INIT_I2C + 1);

void board_set_charge_limit(int port, int supplier, int charge_ma,
			    int max_ma, int charge_mv)
{
	int icl = MAX(charge_ma, CONFIG_CHARGER_INPUT_CURRENT);

	/*
	 * Assume charger overdraws by about 4%, keeping the actual draw
	 * within spec. This adjustment can be changed with characterization
	 * of actual hardware.
	 */
	icl = icl * 96 / 100;
	charge_set_input_current_limit(icl, charge_mv);
}

int pd_check_vconn_swap(int port)
{
	/* Allow VCONN swaps if the AP is on. */
	return chipset_in_state(CHIPSET_STATE_ANY_SUSPEND | CHIPSET_STATE_ON);
}

/*
 * Count of chargers depends on sub board presence.
 */
__override uint8_t board_get_charger_chip_count(void)
{
	return board_get_usb_pd_port_count();
}

/*
 * Retrieve sub-board type from FW_CONFIG.
 */
enum nissa_sub_board_type nissa_get_sb_type(void)
{
	static enum nissa_sub_board_type sb = NISSA_SB_UNKNOWN;
	int ret;
	uint32_t val;
	const struct device *dev;

	/*
	 * Return cached value.
	 */
	if (sb != NISSA_SB_UNKNOWN)
		return sb;

	sb = NISSA_SB_NONE;	/* Defaults to none */
	dev = device_get_binding(CROS_CBI_LABEL);
	if (dev == NULL) {
		CPRINTS("No %s device", CROS_CBI_LABEL);
	} else {
		ret = cros_cbi_get_fw_config(dev, FW_SUB_BOARD, &val);
		if (ret != 0) {
			CPRINTS("Error retrieving CBI FW_CONFIG field %d",
				FW_SUB_BOARD);
			return sb;
		}
		switch (val) {
		default:
			CPRINTS("No sub-board defined");
			break;
		case FW_SUB_BOARD_1:
			sb = NISSA_SB_C_A;
			CPRINTS("SB: USB type C, USB type A");
			break;

		case FW_SUB_BOARD_2:
			sb = NISSA_SB_C_LTE;
			CPRINTS("SB: USB type C, WWAN LTE");
			break;

		case FW_SUB_BOARD_3:
			sb = NISSA_SB_HDMI_A;
			CPRINTS("SB: HDMI, USB type A");
			break;
		}
	}
	return sb;
}
