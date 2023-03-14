/* Copyright 2016 The ChromiumOS Authors
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef EC_PANICINFO_H
#define EC_PANICINFO_H

#include "panic_defs.h"

#include <stdio.h>

/**
 * Prints panic information to stdout.
 *
 * @param pdata  Panic information to print
 * @return 0 if success or non-zero error code if error.
 */
int parse_panic_info(const char *data, size_t size);

/**
 * Read stdin to data.
 *
 * @param data  Raw information to store.
 * @param max_size  Maximum size can be stored to data.
 * @return data length if success or -1 on error.
 */
int get_panic_input(char *data, size_t max_size);

#endif /* EC_PANICINFO_H */
