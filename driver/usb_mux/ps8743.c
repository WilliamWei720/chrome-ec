/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Parade PS8743 USB Type-C Redriving Switch for USB Host / DisplayPort.
 */

#include "common.h"
#include "hooks.h"
#include "i2c.h"
#include "ps8743.h"
#include "usb_mux.h"
#include "usb_mux/ps8743_public.h"
#include "usb_pd.h"
#include "util.h"

int ps8743_read(const struct usb_mux *me, uint8_t reg, int *val)
{
	return i2c_read8(me->i2c_port, me->i2c_addr_flags,
			 reg, val);
}

int ps8743_write(const struct usb_mux *me, uint8_t reg, uint8_t val)
{
	return i2c_write8(me->i2c_port, me->i2c_addr_flags,
			  reg, val);
}

int ps8743_field_update(const struct usb_mux *me, uint8_t reg, uint8_t mask,
			uint8_t val)
{
	return i2c_field_update8(me->i2c_port, me->i2c_addr_flags,
			  reg, mask, val);
}


int ps8743_check_chip_id(const struct usb_mux *me, int *val)
{
	int id1;
	int id2;
	int res;

	/*
	 * Verify chip ID registers.
	 */
	res = ps8743_read(me, PS8743_REG_CHIP_ID1, &id1);
	if (res)
		return res;

	res  = ps8743_read(me, PS8743_REG_CHIP_ID2, &id2);
	if (res)
		return res;

	*val = (id2 << 8) + id1;

	return EC_SUCCESS;
}

static int ps8743_init(const struct usb_mux *me)
{
	int id1;
	int id2;
	int res;

	/* Reset chip back to power-on state */
	res = ps8743_write(me, PS8743_REG_MODE, PS8743_MODE_POWER_DOWN);
	if (res)
		return res;

	/*
	 * Verify chip ID registers.
	 */
	res = ps8743_read(me, PS8743_REG_CHIP_ID1, &id1);
	if (res)
		return res;

	res  = ps8743_read(me, PS8743_REG_CHIP_ID2, &id2);
	if (res)
		return res;

	if (id1 != PS8743_CHIP_ID1 || id2 != PS8743_CHIP_ID2)
		return EC_ERROR_UNKNOWN;

	/*
	 * Verify revision ID registers.
	 */
	res = ps8743_read(me, PS8743_REG_REVISION_ID1, &id1);
	if (res)
		return res;

	res = ps8743_read(me, PS8743_REG_REVISION_ID2, &id2);
	if (res)
		return res;

	/*
	 * From Parade: PS8743 may have REVISION_ID1 as 0 or 1
	 * Rev 1 is derived from Rev 0 and have same functionality.
	 */
	if (id1 != PS8743_REVISION_ID1_0 && id1 != PS8743_REVISION_ID1_1)
		return EC_ERROR_UNKNOWN;
	if (id2 != PS8743_REVISION_ID2)
		return EC_ERROR_UNKNOWN;

	return EC_SUCCESS;
}

/* Writes control register to set switch mode */
static int ps8743_set_mux(const struct usb_mux *me, mux_state_t mux_state,
			  bool *ack_required)
{
	/*
	 * For CE_DP, CE_USB, and FLIP, disable pin control and enable I2C
	 * control.
	 */
	uint8_t reg = (PS8743_MODE_IN_HPD_CONTROL |
		       PS8743_MODE_DP_REG_CONTROL |
		       PS8743_MODE_USB_REG_CONTROL |
		       PS8743_MODE_FLIP_REG_CONTROL);

	/* This driver does not use host command ACKs */
	*ack_required = false;

	if (mux_state & USB_PD_MUX_USB_ENABLED)
		reg |= PS8743_MODE_USB_ENABLE;
	if (mux_state & USB_PD_MUX_DP_ENABLED)
		reg |= PS8743_MODE_DP_ENABLE | PS8743_MODE_IN_HPD_ASSERT;
	if (mux_state & USB_PD_MUX_POLARITY_INVERTED)
		reg |= PS8743_MODE_FLIP_ENABLE;

	return ps8743_write(me, PS8743_REG_MODE, reg);
}

/* Reads control register and updates mux_state accordingly */
static int ps8743_get_mux(const struct usb_mux *me, mux_state_t *mux_state)
{
	int reg;
	int res;

	res = ps8743_read(me, PS8743_REG_STATUS, &reg);
	if (res)
		return res;

	*mux_state = 0;
	if (reg & PS8743_STATUS_USB_ENABLED)
		*mux_state |= USB_PD_MUX_USB_ENABLED;
	if (reg & PS8743_STATUS_DP_ENABLED)
		*mux_state |= USB_PD_MUX_DP_ENABLED;
	if (reg & PS8743_STATUS_POLARITY_INVERTED)
		*mux_state |= USB_PD_MUX_POLARITY_INVERTED;

	return EC_SUCCESS;
}

/* Tune USB Tx/Rx Equalization */
int ps8743_tune_usb_eq(const struct usb_mux *me, uint8_t tx, uint8_t rx)
{
	int ret;

	ret = ps8743_write(me, PS8743_REG_USB_EQ_TX, tx);
	ret |= ps8743_write(me, PS8743_REG_USB_EQ_RX, rx);

	return ret;
}

const struct usb_mux_driver ps8743_usb_mux_driver = {
	.init = ps8743_init,
	.set = ps8743_set_mux,
	.get = ps8743_get_mux,
};

static bool ps8743_port_is_usb2_only(const struct usb_mux *me)
{
	int val;

	if (ps8743_read(me, PS8743_MISC_DCI_SS_MODES, &val))
		return false;

	val &= (PS8743_SSTX_NORMAL_OPERATION_MODE |
		PS8743_SSTX_POWER_SAVING_MODE | PS8743_SSTX_SUSPEND_MODE);

	return (val != PS8743_SSTX_NORMAL_OPERATION_MODE &&
		val != PS8743_SSTX_POWER_SAVING_MODE);
}

static void ps8743_update_usb_mode_all(bool is_resume)
{
	for (int i = 0; i < board_get_usb_pd_port_count(); i++) {
		const struct usb_mux *mux = &usb_muxes[i];

		if (mux->driver != &ps8743_usb_mux_driver)
			continue;

		if (is_resume) {
			ps8743_field_update(mux, PS8743_REG_MODE,
					    PS8743_MODE_USB_ENABLE, 0xFF);
		} else if (ps8743_port_is_usb2_only(mux)) {
			ps8743_field_update(mux, PS8743_REG_MODE,
					    PS8743_MODE_USB_ENABLE, 0);
		}
	}
}

static void ps8743_suspend(void)
{
	ps8743_update_usb_mode_all(false);
}
DECLARE_HOOK(HOOK_CHIPSET_SUSPEND, ps8743_suspend, HOOK_PRIO_DEFAULT);

static void ps8743_resume(void)
{
	ps8743_update_usb_mode_all(true);
}
DECLARE_HOOK(HOOK_CHIPSET_RESUME, ps8743_resume, HOOK_PRIO_DEFAULT);
