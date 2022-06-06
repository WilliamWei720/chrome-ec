/* Copyright 2022 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "common.h"
#include "console.h"
#include "hooks.h"
#include "keyboard_8042_sharedlib.h"
#include "keyboard_scan.h"
#include "timer.h"


#define CPRINTF(format, args...) cprintf(CC_KEYBOARD, format, ## args)
#define CPRINTS(format, args...) cprints(CC_KEYBOARD, format, ## args)

/* Keyboard scan setting */
__override struct keyboard_scan_config keyscan_config = {
	/* Increase from 50 us, because KSO_02 passes through the H1. */
	.output_settle_us = 80,
	/* Other values should be the same as the default configuration. */
	.debounce_down_us = 9 * MSEC,
	.debounce_up_us = 30 * MSEC,
	.scan_period_us = 3 * MSEC,
	.min_post_scan_delay_us = 1000,
	.poll_timeout_us = 100 * MSEC,
	/* TODO(b/220800586): need to verify on rgb keyboard */
	.actual_key_mask = {
		0x1c, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0x86, 0xff, 0xff, 0x55, 0xff, 0xff, 0xff,
		0xff  /* full set */
	},
};

/* TODO(b/220800586): need to verify on rgb keyboard */
static uint16_t scancode_set2_rgb[KEYBOARD_COLS_MAX][KEYBOARD_ROWS] = {
	{0x0000, 0x0000, 0x0014, 0xe01f, 0xe014, 0xe007, 0x0000, 0x0000},
	{0x001f, 0x0076, 0x0017, 0x000e, 0x001c, 0x003a, 0x000d, 0x0016},
	{0x006c, 0x000c, 0x0004, 0x0006, 0x002a, 0xe071, 0x0026, 0x002a},
	{0x0032, 0x0034, 0x002c, 0x002e, 0x002b, 0x0029, 0x0025, 0x002d},
	{0x0078, 0x0009, 0x0083, 0x000b, 0x0003, 0x0041, 0x001e, 0x001d},
	{0x0051, 0x0000, 0x005b, 0x0000, 0x0042, 0x0022, 0x003e, 0x0043},
	{0x0031, 0x0033, 0x0035, 0x0036, 0x003b, 0x001b, 0x003d, 0x003c},
	{0x0000, 0x0052, 0x0061, 0x0000, 0x0000, 0x0000, 0x0000, 0x0059},
	{0x0055, 0x0052, 0x0054, 0x004e, 0x004c, 0x0024, 0x0044, 0x004d},
	{0x0045, 0x0001, 0x000a, 0x002f, 0x004b, 0x0049, 0x0046, 0x001A},
	{0xe011, 0x0000, 0x006a, 0x0000, 0x005d, 0x0000, 0x0011, 0x0000},
	{0xe07a, 0x005d, 0xe075, 0x006b, 0x005a, 0xe072, 0x004a, 0x0066},
	{0xe06b, 0xe074, 0xe069, 0x0067, 0xe0c6, 0x0064, 0x0015, 0xe07d},
	{0x0073, 0x0066, 0xe071, 0x005d, 0x005a, 0xe04a, 0x0070, 0x0021},
	{0x0023, 0xe05a, 0x0075, 0x0067, 0xe069, 0xe07a, 0x007d, 0x0069},
};

/* TODO(b/220800586): need to verify on rgb keyboard */
static void keyboard_matrix_init(void)
{
	CPRINTS("%s", __func__);

	register_scancode_set2((uint16_t *) &scancode_set2_rgb,
					sizeof(scancode_set2_rgb));
}
DECLARE_HOOK(HOOK_INIT, keyboard_matrix_init, HOOK_PRIO_DEFAULT);
