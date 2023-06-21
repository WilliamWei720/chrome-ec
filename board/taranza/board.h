/* Copyright 2023 The ChromiumOS Authors
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Taranza board configuration */

#ifndef __CROS_EC_BOARD_H
#define __CROS_EC_BOARD_H

/* Select Baseboard features */
#define VARIANT_DEDEDE_EC_IT8320
#include "baseboard.h"

/* System unlocked in early development */
#define CONFIG_SYSTEM_UNLOCKED

#define CONFIG_CMD_CHARGER_DUMP

/* I2C Bus Configuration */
#define I2C_PORT_HDMI2_EDID IT83XX_I2C_CH_B
#undef I2C_PORT_USB_C0
#define I2C_PORT_USB_C0 IT83XX_I2C_CH_C
#define I2C_PORT_HDMI2_SRC_DDC IT83XX_I2C_CH_D
#define I2C_PORT_HDMI1_EDID IT83XX_I2C_CH_E
#define I2C_PORT_HDMI1_SRC_DDC IT83XX_I2C_CH_F

/* Power */
#undef CONFIG_CHARGER
#undef CONFIG_CHARGER_DISCHARGE_ON_AC
#undef CONFIG_USB_PD_VBUS_MEASURE_CHARGER
#define CONFIG_CHARGER_MIN_POWER_MW_FOR_POWER_ON 16000
#define PD_MAX_VOLTAGE_MV 20000
#undef PD_MAX_CURRENT_MA
#define PD_MAX_CURRENT_MA 3250
#undef PD_MAX_POWER_MW
#define PD_MAX_POWER_MW 65000
#define CONFIG_USB_PD_VBUS_DETECT_GPIO

/* Override macro for C0 only */
#define PORT_TO_HPD(port) (GPIO_USB_C0_DP_HPD)

/* Power: Dedicated barreljack charger port */
#undef CONFIG_DEDICATED_CHARGE_PORT_COUNT
#define CONFIG_DEDICATED_CHARGE_PORT_COUNT 1
#define DEDICATED_CHARGE_PORT 1

/* USB Type-C */
#undef CONFIG_USB_CHARGER
#undef CONFIG_USB_MUX_PI3USB31532

/* TCPC */
#define CONFIG_USB_PD_PORT_MAX_COUNT 1
#define CONFIG_USB_PD_TCPM_ITE_ON_CHIP /* C0: ITE EC TCPC */
#define CONFIG_USB_PD_ITE_ACTIVE_PORT_COUNT 1

/* PPC */
#define CONFIG_USB_PD_DISCHARGE_PPC
#define CONFIG_USB_PD_VBUS_DETECT_PPC
#define CONFIG_USBC_PPC
#define CONFIG_USBC_PPC_SYV682X

/* USB Mux and Retimer */
#define CONFIG_USB_MUX_IT5205 /* C0: ITE Mux */
#define I2C_PORT_USB_MUX I2C_PORT_USB_C0 /* Required for ITE Mux */

/* USB Type A Features */
#define CONFIG_USB_PORT_POWER_DUMB
#define USB_PORT_COUNT 5 /* Type A ports */

/* No battery */
#undef CONFIG_BATTERY_CUT_OFF
#undef CONFIG_BATTERY_PRESENT_GPIO
#undef CONFIG_BATTERY_REQUESTS_NIL_WHEN_DEAD
#undef CONFIG_BATTERY_REVIVE_DISCONNECT
#undef CONFIG_BATTERY_SMART

/* Thermistors */
#define CONFIG_TEMP_SENSOR
#define CONFIG_THERMISTOR
#define CONFIG_STEINHART_HART_3V3_51K1_47K_4050B

/* Buttons */
#define CONFIG_DEDICATED_RECOVERY_BUTTON
#define CONFIG_DEDICATED_RECOVERY_BUTTON_2
#define CONFIG_DEDICATED_RECOVERY_BUTTON_FLAGS BUTTON_FLAG_ACTIVE_HIGH
#define CONFIG_POWER_BUTTON
#define CONFIG_POWER_BUTTON_IGNORE_LID
#define CONFIG_POWER_BUTTON_X86
#define CONFIG_EMULATED_SYSRQ

/* CEC */
#define CONFIG_CEC
#define CONFIG_CEC_BITBANG
#define CEC_GPIO_OUT GPIO_HDMI2_CEC
#define CEC_GPIO_IN GPIO_HDMI2_CEC_IN
#undef CEC_GPIO_PULL_UP /* Pull-up to PP3300_Z1 */

/* No Keyboard */
#undef CONFIG_KEYBOARD_COL2_INVERTED
#undef CONFIG_KEYBOARD_PROTOCOL_8042
#undef CONFIG_CMD_KEYBOARD
#undef CONFIG_KEYBOARD_BOOT_KEYS
#undef CONFIG_KEYBOARD_RUNTIME_KEYS

/* No backlight */
#undef CONFIG_BACKLIGHT_LID
#undef GPIO_ENABLE_BACKLIGHT

/* Unused features - Misc */
#undef CONFIG_HIBERNATE
#undef CONFIG_VOLUME_BUTTONS
#undef CONFIG_LID_SWITCH
#undef CONFIG_TABLET_MODE
#undef CONFIG_TABLET_MODE_SWITCH
#undef CONFIG_GMR_TABLET_MODE
#undef GPIO_TABLET_MODE_L

/* Unused GPIOs */
#undef GPIO_USB_C1_DP_HPD

/* Pin renaming */
#define GPIO_RECOVERY_L GPIO_EC_RECOVERY_BTN_OD
#define GPIO_RECOVERY_L_2 GPIO_H1_EC_RECOVERY_BTN_ODL
#define GPIO_POWER_BUTTON_L GPIO_H1_EC_PWR_BTN_ODL

#ifndef __ASSEMBLER__

#include "gpio_signal.h"
#include "registers.h"

enum charge_port {
	CHARGE_PORT_TYPEC0,
	CHARGE_PORT_BARRELJACK,
};

enum usbc_port { USBC_PORT_C0 = 0, USBC_PORT_COUNT };

/* ADC channels */
enum adc_channel {
	ADC_VSNS_PP3300_A, /* ADC0 */
	ADC_TEMP_SENSOR_1, /* ADC2 */
	ADC_TEMP_SENSOR_2, /* ADC3 */
	ADC_VBUS, /* ADC4 */
	ADC_TEMP_SENSOR_3, /* ADC13 */
	ADC_PPVAR_PWR_IN_IMON, /* ADC15 */
	ADC_SNS_PPVAR_PWR_IN, /* ADC16 */
	ADC_CH_COUNT
};

enum temp_sensor_id {
	TEMP_SENSOR_1,
	TEMP_SENSOR_2,
	TEMP_SENSOR_3,
	TEMP_SENSOR_COUNT
};

enum cec_port { CEC_PORT_0, CEC_PORT_COUNT };

#endif /* !__ASSEMBLER__ */

#endif /* __CROS_EC_BOARD_H */