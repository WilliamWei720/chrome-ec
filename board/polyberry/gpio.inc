/* -*- mode:c -*-
 *
 * Copyright 2016 The ChromiumOS Authors
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* Declare symbolic names for all the GPIOs that we care about.
 * Note: Those with interrupt handlers must be declared first. */

/* Outputs */
GPIO(PM_RESET_L,		PIN(D, 0),  GPIO_OUT_HIGH)
GPIO(PM_KPD_PWR_L,		PIN(D, 1),  GPIO_OUT_HIGH)
GPIO(AP_RESET_L,		PIN(D, 2),  GPIO_OUT_HIGH)
GPIO(AP_FORCED_USB_BOOT,	PIN(D, 3),  GPIO_OUT_LOW)
GPIO(AP_PS_HOLD,		PIN(D, 4),  GPIO_OUT_HIGH)

GPIO(MUX_EN_L,			PIN(A, 7),  GPIO_INPUT)
GPIO(USB_MUX_SEL,		PIN(A, 6),  GPIO_OUT_HIGH)
GPIO(PHY_RESET,			PIN(C, 4),  GPIO_INPUT)
GPIO(LED_BLUE,			PIN(A, 2),  GPIO_ODR_LOW)
GPIO(LED_GRN,			PIN(B, 8),  GPIO_ODR_LOW)
GPIO(LED_RED,			PIN(B, 15), GPIO_ODR_LOW)

/* Clock function */
GPIO(MCU_TO_PHY_MCO,		PIN(C, 9), GPIO_INPUT)


/* These pin assignments aren't used as GPIO. Let's note them here
 * for readability but not initialize them.
 * USART1 TX/RX - AP
 * GPIO(MCU_UART1_TX,		PIN(A, 9), GPIO_INPUT)
 * GPIO(MCU_UART1_RX,		PIN(A, 10), GPIO_INPUT)
 * USART2 TX/RX - Sensor Hub
 * GPIO(MCU_UART2_TX,		PIN(D, 5), GPIO_INPUT)
 * GPIO(MCU_UART2_RX,		PIN(D, 6), GPIO_INPUT)

 * USART3 TX/RX - Console
 * GPIO(MCU_UART3_TX,		PIN(C, 10), GPIO_INPUT)
 * GPIO(MCU_UART3_RX,		PIN(C, 11), GPIO_INPUT)
 * USART5 TX/RX - SSC (?)
 * GPIO(MCU_UART5_TX,		PIN(E, 7), GPIO_INPUT)
 * GPIO(MCU_UART5_RX,		PIN(E, 8), GPIO_INPUT)
 */

/* USB pins */
GPIO(USB_FS_DM,			PIN(A, 11), GPIO_INPUT)
GPIO(USB_FS_DP,			PIN(A, 12), GPIO_INPUT)

GPIO(USB_HS_ULPI_NXT,		PIN(C, 3), GPIO_INPUT)
GPIO(USB_HS_ULPI_DIR,		PIN(C, 2), GPIO_INPUT)
GPIO(USB_HS_ULPI_STP,		PIN(C, 0), GPIO_INPUT)
GPIO(USB_HS_ULPI_CK,		PIN(A, 5), GPIO_INPUT)

GPIO(USB_HS_ULPI_D7,		PIN(B, 5), GPIO_INPUT)
GPIO(USB_HS_ULPI_D6,		PIN(B,13), GPIO_INPUT)
GPIO(USB_HS_ULPI_D5,		PIN(B,12), GPIO_INPUT)
GPIO(USB_HS_ULPI_D4,		PIN(B, 2), GPIO_INPUT)
GPIO(USB_HS_ULPI_D3,		PIN(B,10), GPIO_INPUT)
GPIO(USB_HS_ULPI_D2,		PIN(B, 1), GPIO_INPUT)
GPIO(USB_HS_ULPI_D1,		PIN(B, 0), GPIO_INPUT)
GPIO(USB_HS_ULPI_D0,		PIN(A, 3), GPIO_INPUT)


/* Unimplemented signals since we are not an EC */
UNIMPLEMENTED(ENTERING_RW)
UNIMPLEMENTED(WP_L)


ALTERNATE(PIN_MASK(A, 0x0600), 7, MODULE_UART, 0) /* USART1: PA9/PA10 - AP */
ALTERNATE(PIN_MASK(D, 0x0060), 7, MODULE_UART, 0) /* USART2: PD5/PD6 - SH */
ALTERNATE(PIN_MASK(C, 0x0c00), 7, MODULE_UART, 0) /* USART3: PC10/PC11 - Console */
ALTERNATE(PIN_MASK(D, 0x00c0), 8, MODULE_UART, 0) /* USART5: PE7/PE8 - SSC */

/* OTG FS */
ALTERNATE(PIN_MASK(A, 0x1800), 10, MODULE_USB, 0) /* DWC USB OTG: PA11/12 */

/* OTG HS */
ALTERNATE(PIN_MASK(A, 0x0028), 10, MODULE_USB, 0) /* DWC USB OTG HS */
ALTERNATE(PIN_MASK(B, 0x3427), 10, MODULE_USB, 0) /* DWC USB OTG HS */
ALTERNATE(PIN_MASK(C, 0x000d), 10, MODULE_USB, 0) /* DWC USB OTG HS */
ALTERNATE(PIN_MASK(C, 0x0200), 0, MODULE_MCO, 0) /* MCO2: PC9 */