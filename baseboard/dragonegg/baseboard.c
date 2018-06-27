/* Copyright 2018 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* DragonEgg family-specific configuration */

#include "gpio.h"
#include "i2c.h"
#include "util.h"

/******************************************************************************/
/* I2C port map configuration */
/* TODO(b/111125177): Increase these speeds to 400 kHz and verify operation */
const struct i2c_port_t i2c_ports[] = {
	{"eeprom", IT83XX_I2C_CH_A, 100, GPIO_I2C0_SCL, GPIO_I2C0_SDA},
	{"sensor", IT83XX_I2C_CH_B, 100, GPIO_I2C1_SCL, GPIO_I2C1_SDA},
	{"usbc12",  IT83XX_I2C_CH_C, 100, GPIO_I2C2_SCL, GPIO_I2C2_SDA},
	{"usbc0",  IT83XX_I2C_CH_E, 100, GPIO_I2C4_SCL, GPIO_I2C4_SDA},
	{"power",  IT83XX_I2C_CH_F, 100, GPIO_I2C5_SCL, GPIO_I2C5_SDA}
};
const unsigned int i2c_ports_used = ARRAY_SIZE(i2c_ports);
