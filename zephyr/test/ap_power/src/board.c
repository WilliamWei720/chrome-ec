/* Copyright 2022 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <zephyr.h>
#include <ztest.h>

#include <power_signals.h>

static bool signal_PWR_ALL_SYS_PWRGD;
static bool signal_PWR_DSW_PWROK;
static bool signal_PWR_PG_PP1P05;

int board_power_signal_set(enum power_signal signal, int value)
{
	switch (signal) {
	default:
		zassert_unreachable("Unknown signal");
		return -1;

	case PWR_ALL_SYS_PWRGD:
		signal_PWR_ALL_SYS_PWRGD = value;
		return 0;

	case PWR_DSW_PWROK:
		signal_PWR_DSW_PWROK = value;
		return 0;

	case PWR_PG_PP1P05:
		signal_PWR_PG_PP1P05 = value;
		return 0;
	}
}

int board_power_signal_get(enum power_signal signal)
{
	switch (signal) {
	default:
		zassert_unreachable("Unknown signal");
		return -1;

	case PWR_ALL_SYS_PWRGD:
		return signal_PWR_ALL_SYS_PWRGD;

	case PWR_DSW_PWROK:
		return signal_PWR_DSW_PWROK;

	case PWR_PG_PP1P05:
		return signal_PWR_PG_PP1P05;
	}
}
