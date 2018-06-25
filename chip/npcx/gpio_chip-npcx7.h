/* Copyright 2017 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __CROS_EC_GPIO_CHIP_NPCX7_H
#define __CROS_EC_GPIO_CHIP_NPCX7_H

/*****************************************************************************/
/* Macro functions for MIWU mapping table */

/* MIWU0 */
/* Group A: NPCX_IRQ_MTC_WKINTAD_0 */
#define NPCX_WUI_GPIO_8_0 WUI(0, MIWU_GROUP_1, 0)
#define NPCX_WUI_GPIO_8_1 WUI(0, MIWU_GROUP_1, 1)
#define NPCX_WUI_GPIO_8_2 WUI(0, MIWU_GROUP_1, 2)
#define NPCX_WUI_GPIO_8_3 WUI(0, MIWU_GROUP_1, 3)
#ifndef NPCX_PSL_MODE_SUPPORT
#define NPCX_WUI_GPIO_8_4 WUI(0, MIWU_GROUP_1, 4) /* Used as VSBY in PSL */
#define NPCX_WUI_GPIO_8_5 WUI(0, MIWU_GROUP_1, 5) /* Used as PSL_OUT in PSL */
#endif
#define NPCX_WUI_GPIO_8_6 WUI(0, MIWU_GROUP_1, 6)
#define NPCX_WUI_GPIO_8_7 WUI(0, MIWU_GROUP_1, 7)

/* Group B: NPCX_IRQ_TWD_WKINTB_0 */
#define NPCX_WUI_GPIO_9_0 WUI(0, MIWU_GROUP_2, 0)
#define NPCX_WUI_GPIO_9_1 WUI(0, MIWU_GROUP_2, 1)
#define NPCX_WUI_GPIO_9_2 WUI(0, MIWU_GROUP_2, 2)
#define NPCX_WUI_GPIO_9_3 WUI(0, MIWU_GROUP_2, 3)
#define NPCX_WUI_GPIO_9_4 WUI(0, MIWU_GROUP_2, 4)
#define NPCX_WUI_GPIO_9_5 WUI(0, MIWU_GROUP_2, 5)

/* Group C: NPCX_IRQ_WKINTC_0 */
#define NPCX_WUI_GPIO_9_6 WUI(0, MIWU_GROUP_3, 0)
#define NPCX_WUI_GPIO_9_7 WUI(0, MIWU_GROUP_3, 1)
#define NPCX_WUI_GPIO_A_0 WUI(0, MIWU_GROUP_3, 2)
#define NPCX_WUI_GPIO_A_1 WUI(0, MIWU_GROUP_3, 3)
#define NPCX_WUI_GPIO_A_2 WUI(0, MIWU_GROUP_3, 4)
#define NPCX_WUI_GPIO_A_3 WUI(0, MIWU_GROUP_3, 5)
#define NPCX_WUI_GPIO_A_4 WUI(0, MIWU_GROUP_3, 6)
#define NPCX_WUI_GPIO_A_5 WUI(0, MIWU_GROUP_3, 7)

/* Group D: NPCX_IRQ_MTC_WKINTAD_0 */
#define NPCX_WUI_GPIO_A_6 WUI(0, MIWU_GROUP_4, 0)
#define NPCX_WUI_GPIO_A_7 WUI(0, MIWU_GROUP_4, 1)
#define NPCX_WUI_GPIO_B_0 WUI(0, MIWU_GROUP_4, 2)
#define NPCX_WUI_GPIO_B_1 WUI(0, MIWU_GROUP_4, 5)
#define NPCX_WUI_GPIO_B_2 WUI(0, MIWU_GROUP_4, 6)

/* Group E: NPCX_IRQ_WKINTEFGH_0 */
#define NPCX_WUI_GPIO_B_3 WUI(0, MIWU_GROUP_5, 0)
#define NPCX_WUI_GPIO_B_4 WUI(0, MIWU_GROUP_5, 1)
#define NPCX_WUI_GPIO_B_5 WUI(0, MIWU_GROUP_5, 2)
#define NPCX_WUI_GPIO_B_7 WUI(0, MIWU_GROUP_5, 4)

/* Group F: NPCX_IRQ_WKINTEFGH_0 */
#define NPCX_WUI_GPIO_C_0 WUI(0, MIWU_GROUP_6, 0)
#define NPCX_WUI_GPIO_C_1 WUI(0, MIWU_GROUP_6, 1)
#define NPCX_WUI_GPIO_C_2 WUI(0, MIWU_GROUP_6, 2)
#define NPCX_WUI_GPIO_C_3 WUI(0, MIWU_GROUP_6, 3)
#define NPCX_WUI_GPIO_C_4 WUI(0, MIWU_GROUP_6, 4)
#define NPCX_WUI_GPIO_C_5 WUI(0, MIWU_GROUP_6, 5)
#define NPCX_WUI_GPIO_C_6 WUI(0, MIWU_GROUP_6, 6)
#define NPCX_WUI_GPIO_C_7 WUI(0, MIWU_GROUP_6, 7)

/* Group G: NPCX_IRQ_WKINTEFGH_0 */
#define NPCX_WUI_GPIO_D_0 WUI(0, MIWU_GROUP_7, 0)
#define NPCX_WUI_GPIO_D_1 WUI(0, MIWU_GROUP_7, 1)
#define NPCX_WUI_GPIO_D_2 WUI(0, MIWU_GROUP_7, 2)
#define NPCX_WUI_GPIO_D_3 WUI(0, MIWU_GROUP_7, 3)
#define NPCX_WUI_GPIO_D_4 WUI(0, MIWU_GROUP_7, 4)
#define NPCX_WUI_GPIO_D_5 WUI(0, MIWU_GROUP_7, 5)
#define NPCX_WUI_GPIO_D_7 WUI(0, MIWU_GROUP_7, 6)
#define NPCX_WUI_GPIO_E_0 WUI(0, MIWU_GROUP_7, 7)

/* Group H: NPCX_IRQ_WKINTEFGH_0 */
#define NPCX_WUI_GPIO_E_1 WUI(0, MIWU_GROUP_8, 0)
#define NPCX_WUI_GPIO_E_2 WUI(0, MIWU_GROUP_8, 1)
#define NPCX_WUI_GPIO_E_3 WUI(0, MIWU_GROUP_8, 2)
#define NPCX_WUI_GPIO_E_4 WUI(0, MIWU_GROUP_8, 3)
#define NPCX_WUI_GPIO_E_5 WUI(0, MIWU_GROUP_8, 4)
#define NPCX_WUI_GPIO_F_0 WUI(0, MIWU_GROUP_8, 5)
#define NPCX_WUI_GPIO_F_3 WUI(0, MIWU_GROUP_8, 6)
#ifndef NPCX_EXT32K_OSC_SUPPORT
#define NPCX_WUI_GPIO_E_7 WUI(0, MIWU_GROUP_8, 7) /* Used as CLKIN if support */
#endif

/* MIWU1 */
/* Group A: NPCX_IRQ_WKINTA_1 */
#define NPCX_WUI_GPIO_0_0 WUI(1, MIWU_GROUP_1, 0)
#define NPCX_WUI_GPIO_0_1 WUI(1, MIWU_GROUP_1, 1)
#define NPCX_WUI_GPIO_0_2 WUI(1, MIWU_GROUP_1, 2)
#define NPCX_WUI_GPIO_0_3 WUI(1, MIWU_GROUP_1, 3)
#define NPCX_WUI_GPIO_0_4 WUI(1, MIWU_GROUP_1, 4)
#define NPCX_WUI_GPIO_0_5 WUI(1, MIWU_GROUP_1, 5)
#define NPCX_WUI_GPIO_0_6 WUI(1, MIWU_GROUP_1, 6)
#define NPCX_WUI_GPIO_0_7 WUI(1, MIWU_GROUP_1, 7)

/* Group B: NPCX_IRQ_WKINTB_1 */
#define NPCX_WUI_GPIO_1_0 WUI(1, MIWU_GROUP_2, 0)
#define NPCX_WUI_GPIO_1_1 WUI(1, MIWU_GROUP_2, 1)
#define NPCX_WUI_GPIO_F_4 WUI(1, MIWU_GROUP_2, 2)
#define NPCX_WUI_GPIO_1_4 WUI(1, MIWU_GROUP_2, 4)
#define NPCX_WUI_GPIO_1_5 WUI(1, MIWU_GROUP_2, 5)
#define NPCX_WUI_GPIO_1_6 WUI(1, MIWU_GROUP_2, 6)
#define NPCX_WUI_GPIO_1_7 WUI(1, MIWU_GROUP_2, 7)

/* Group C: NPCX_IRQ_KSI_WKINTC_1 */
#define NPCX_WUI_GPIO_3_1 WUI(1, MIWU_GROUP_3, 0)
#define NPCX_WUI_GPIO_3_0 WUI(1, MIWU_GROUP_3, 1)
#define NPCX_WUI_GPIO_2_7 WUI(1, MIWU_GROUP_3, 2)
#define NPCX_WUI_GPIO_2_6 WUI(1, MIWU_GROUP_3, 3)
#define NPCX_WUI_GPIO_2_5 WUI(1, MIWU_GROUP_3, 4)
#define NPCX_WUI_GPIO_2_4 WUI(1, MIWU_GROUP_3, 5)
#define NPCX_WUI_GPIO_2_3 WUI(1, MIWU_GROUP_3, 6)
#define NPCX_WUI_GPIO_2_2 WUI(1, MIWU_GROUP_3, 7)

/* Group D: NPCX_IRQ_WKINTD_1 */
#define NPCX_WUI_GPIO_2_0 WUI(1, MIWU_GROUP_4, 0)
#define NPCX_WUI_GPIO_2_1 WUI(1, MIWU_GROUP_4, 1)
#define NPCX_WUI_GPIO_F_5 WUI(1, MIWU_GROUP_4, 2)
#define NPCX_WUI_GPIO_3_3 WUI(1, MIWU_GROUP_4, 3)
#define NPCX_WUI_GPIO_3_4 WUI(1, MIWU_GROUP_4, 4)
#define NPCX_WUI_GPIO_3_6 WUI(1, MIWU_GROUP_4, 6)
#define NPCX_WUI_GPIO_3_7 WUI(1, MIWU_GROUP_4, 7)

/* Group E: NPCX_IRQ_WKINTE_1 */
#define NPCX_WUI_GPIO_4_0 WUI(1, MIWU_GROUP_5, 0)
#define NPCX_WUI_GPIO_4_1 WUI(1, MIWU_GROUP_5, 1)
#define NPCX_WUI_GPIO_4_2 WUI(1, MIWU_GROUP_5, 2)
#define NPCX_WUI_GPIO_4_3 WUI(1, MIWU_GROUP_5, 3)
#define NPCX_WUI_GPIO_4_4 WUI(1, MIWU_GROUP_5, 4)
#define NPCX_WUI_GPIO_4_5 WUI(1, MIWU_GROUP_5, 5)
#define NPCX_WUI_GPIO_4_6 WUI(1, MIWU_GROUP_5, 6)
#define NPCX_WUI_GPIO_4_7 WUI(1, MIWU_GROUP_5, 7)

/* Group F: NPCX_IRQ_WKINTF_1 */
#define NPCX_WUI_GPIO_5_0 WUI(1, MIWU_GROUP_6, 0)
#define NPCX_WUI_GPIO_5_1 WUI(1, MIWU_GROUP_6, 1)
#define NPCX_WUI_GPIO_5_2 WUI(1, MIWU_GROUP_6, 2)
#define NPCX_WUI_GPIO_5_3 WUI(1, MIWU_GROUP_6, 3)
#define NPCX_WUI_GPIO_5_4 WUI(1, MIWU_GROUP_6, 4)
#define NPCX_WUI_GPIO_5_5 WUI(1, MIWU_GROUP_6, 5)
#define NPCX_WUI_GPIO_5_6 WUI(1, MIWU_GROUP_6, 6)
#define NPCX_WUI_GPIO_5_7 WUI(1, MIWU_GROUP_6, 7)

/* Group G: NPCX_IRQ_WKINTG_1 */
#define NPCX_WUI_GPIO_6_0 WUI(1, MIWU_GROUP_7, 0)
#define NPCX_WUI_GPIO_6_1 WUI(1, MIWU_GROUP_7, 1)
#define NPCX_WUI_GPIO_6_2 WUI(1, MIWU_GROUP_7, 2)
#define NPCX_WUI_GPIO_6_3 WUI(1, MIWU_GROUP_7, 3)
#define NPCX_WUI_GPIO_6_4 WUI(1, MIWU_GROUP_7, 4)
#ifndef NPCX_EXT32K_OSC_SUPPORT
#define NPCX_WUI_GPIO_7_1 WUI(1, MIWU_GROUP_7, 7) /* Used as CLKOUT if support*/
#endif

/* Group H: NPCX_IRQ_WKINTH_1 */
#define NPCX_WUI_GPIO_7_0 WUI(1, MIWU_GROUP_8, 0)
#define NPCX_WUI_GPIO_6_7 WUI(1, MIWU_GROUP_8, 1)
#define NPCX_WUI_GPIO_7_2 WUI(1, MIWU_GROUP_8, 2)
#define NPCX_WUI_GPIO_7_3 WUI(1, MIWU_GROUP_8, 3)
#define NPCX_WUI_GPIO_7_4 WUI(1, MIWU_GROUP_8, 4)
#define NPCX_WUI_GPIO_7_5 WUI(1, MIWU_GROUP_8, 5)
#define NPCX_WUI_GPIO_7_6 WUI(1, MIWU_GROUP_8, 6)

/* MIWU2 */
/* Group F: NPCX_IRQ_WKINTFG_2 */
#define NPCX_WUI_GPIO_F_1 WUI(2, MIWU_GROUP_6, 1)
#define NPCX_WUI_GPIO_F_2 WUI(2, MIWU_GROUP_6, 2)

/* Others GPO without MIWU functionality */
#define NPCX_WUI_GPIO_1_2 WUI_NONE
#define NPCX_WUI_GPIO_1_3 WUI_NONE /* Software strap pin GP_SEL1_L */
#define NPCX_WUI_GPIO_3_2 WUI_NONE
#define NPCX_WUI_GPIO_3_5 WUI_NONE
#define NPCX_WUI_GPIO_6_5 WUI_NONE /* Software strap pin FLPRG_L */
#define NPCX_WUI_GPIO_6_6 WUI_NONE
#define NPCX_WUI_GPIO_7_7 WUI_NONE
#define NPCX_WUI_GPIO_B_6 WUI_NONE /* Software strap pin GP_SEL0_L */
#define NPCX_WUI_GPIO_D_6 WUI_NONE

/*****************************************************************************/
/* Macro functions for Alternative mapping table */

/* I2C Module */
#define NPCX_ALT_I2C0SDA0 ALT(B, 4, NPCX_ALT(2, I2C0_0_SL)) /* SMB0SDA0 */
#define NPCX_ALT_I2C0SCL0 ALT(B, 5, NPCX_ALT(2, I2C0_0_SL)) /* SMB0SCL0 */
#define NPCX_ALT_I2C1SDA0 ALT(8, 7, NPCX_ALT(2, I2C1_0_SL)) /* SMB1SDA0 */
#define NPCX_ALT_I2C1SCL0 ALT(9, 0, NPCX_ALT(2, I2C1_0_SL)) /* SMB1SCL0 */
#define NPCX_ALT_I2C2SDA0 ALT(9, 1, NPCX_ALT(2, I2C2_0_SL)) /* SMB2SDA0 */
#define NPCX_ALT_I2C2SCL0 ALT(9, 2, NPCX_ALT(2, I2C2_0_SL)) /* SMB2SCL0 */
#define NPCX_ALT_I2C3SDA0 ALT(D, 0, NPCX_ALT(2, I2C3_0_SL)) /* SMB3SDA0 */
#define NPCX_ALT_I2C3SCL0 ALT(D, 1, NPCX_ALT(2, I2C3_0_SL)) /* SMB3SCL0 */
#ifdef NPCX_PSL_MODE_SUPPORT
#define NPCX_ALT_I2C4SDA0 /* No I2CSDA since GPIO85 used as PSL_OUT */
#define NPCX_ALT_I2C4SCL0 /* Used as PSL_OUT */
#else
#define NPCX_ALT_I2C4SDA0 ALT(8, 6, NPCX_ALT(2, I2C4_0_SL)) /* SMB4SDA0 */
#define NPCX_ALT_I2C4SCL0 ALT(8, 5, NPCX_ALT(2, I2C4_0_SL)) /* SMB4SCL0 */
#endif
#define NPCX_ALT_I2C4SDA1 ALT(F, 2, NPCX_ALT(6, I2C4_1_SL)) /* SMB4SDA1 */
#define NPCX_ALT_I2C4SCL1 ALT(F, 3, NPCX_ALT(6, I2C4_1_SL)) /* SMB4SCL1 */
#define NPCX_ALT_I2C5SDA0 ALT(3, 6, NPCX_ALT(2, I2C5_0_SL)) /* SMB5SDA0 */
#define NPCX_ALT_I2C5SCL0 ALT(3, 3, NPCX_ALT(2, I2C5_0_SL)) /* SMB5SCL0 */
#define NPCX_ALT_I2C5SDA1 ALT(F, 4, NPCX_ALT(6, I2C5_1_SL)) /* SMB5SDA1 */
#define NPCX_ALT_I2C5SCL1 ALT(F, 5, NPCX_ALT(6, I2C5_1_SL)) /* SMB5SCL1 */
#define NPCX_ALT_I2C6SDA0 ALT(C, 1, NPCX_ALT(2, I2C6_0_SL)) /* SMB6SDA0 */
#if !(NPCX7_PWM1_SEL)
#define NPCX_ALT_I2C6SCL0 ALT(C, 2, NPCX_ALT(2, I2C6_0_SL)) /* SMB6SCL0 */
#else
#define NPCX_ALT_I2C6SCL0 /* Used as PWM1 */
#endif
#define NPCX_ALT_I2C6SDA1 ALT(E, 3, NPCX_ALT(6, I2C6_1_SL)) /* SMB6SDA1 */
#define NPCX_ALT_I2C6SCL1 ALT(E, 4, NPCX_ALT(6, I2C6_1_SL)) /* SMB6SCL1 */
#define NPCX_ALT_I2C7SDA0 ALT(B, 2, NPCX_ALT(2, I2C7_0_SL)) /* SMB7SDA0 */
#define NPCX_ALT_I2C7SCL0 ALT(B, 3, NPCX_ALT(2, I2C7_0_SL)) /* SMB7SCL0 */

/* ADC Module */
#define NPCX_ALT_ADC0 ALT(4, 5, NPCX_ALT(6, ADC0_SL)) /* ADC0  */
#define NPCX_ALT_ADC1 ALT(4, 4, NPCX_ALT(6, ADC1_SL)) /* ADC1  */
#define NPCX_ALT_ADC2 ALT(4, 3, NPCX_ALT(6, ADC2_SL)) /* ADC2  */
#define NPCX_ALT_ADC3 ALT(4, 2, NPCX_ALT(6, ADC3_SL)) /* ADC3  */
#define NPCX_ALT_ADC4 ALT(4, 1, NPCX_ALT(6, ADC4_SL)) /* ADC4  */
#define NPCX_ALT_ADC5 ALT(3, 7, NPCX_ALT(F, ADC5_SL)) /* ADC5  */
#define NPCX_ALT_ADC6 ALT(3, 4, NPCX_ALT(F, ADC6_SL)) /* ADC6  */
#define NPCX_ALT_ADC7 ALT(E, 1, NPCX_ALT(F, ADC7_SL)) /* ADC7  */
#define NPCX_ALT_ADC8 ALT(F, 1, NPCX_ALT(F, ADC8_SL)) /* ADC8  */
#define NPCX_ALT_ADC9 ALT(F, 0, NPCX_ALT(F, ADC9_SL)) /* ADC9  */

/* UART Module 1/2 */
#if NPCX_UART_MODULE2
/* CR_SIN Select 2 */
#define NPCX_ALT_CR_SIN  ALT(6, 4, NPCX_ALT(C, UART_SL2))
/* CR_SOUT Select 2 */
#define NPCX_ALT_CR_SOUT ALT(6, 5, NPCX_ALT(C, UART_SL2))
#else
/* CR_SIN Select 1 */
#define NPCX_ALT_CR_SIN  ALT(1, 0, NPCX_ALT_INV(9, NO_KSO08_SL))
/* CR_SOUT Select 1 */
#define NPCX_ALT_CR_SOUT ALT(1, 1, NPCX_ALT_INV(9, NO_KSO09_SL))
#endif

#ifdef NPCX_SECOND_UART
#define NPCX_ALT_CR_SIN2  ALT(7, 5, NPCX_ALT(A, UART2_SL)) /* CR_SIN2 */
#define NPCX_ALT_CR_SOUT2 ALT(8, 6, NPCX_ALT(A, UART2_SL)) /* CR_SOUT2 */
#else
#define NPCX_ALT_CR_SIN2
#define NPCX_ALT_CR_SOUT2
#endif

/* SPI Module */
#define NPCX_ALT_SPIP_MISO ALT(9, 5, NPCX_ALT(0, SPIP_SL)) /* SPIP_MISO */
#define NPCX_ALT_SPIP_CS1  ALT(A, 5, NPCX_ALT(0, SPIP_SL)) /* SPIP_CS1  */
#define NPCX_ALT_SPIP_MOSI ALT(A, 3, NPCX_ALT(0, SPIP_SL)) /* SPIP_MOSI */
#define NPCX_ALT_SPIP_SCLK ALT(A, 1, NPCX_ALT(0, SPIP_SL)) /* SPIP_SCLK */

/* PWM Module */
#define NPCX_ALT_PWM0 ALT(C, 3, NPCX_ALT(4, PWM0_SL)) /* PWM0 */
#if NPCX7_PWM1_SEL
#define NPCX_ALT_PWM1 ALT(C, 2, NPCX_ALT(4, PWM1_SL)) /* PWM1 */
#else
#define NPCX_ALT_PWM1 /* Used as SMB6SCL0 */
#endif
#define NPCX_ALT_PWM2 ALT(C, 4, NPCX_ALT(4, PWM2_SL)) /* PWM2 */
#define NPCX_ALT_PWM3 ALT(8, 0, NPCX_ALT(4, PWM3_SL)) /* PWM3 */
#define NPCX_ALT_PWM4 ALT(B, 6, NPCX_ALT(4, PWM4_SL)) /* PWM4 */
#define NPCX_ALT_PWM5 ALT(B, 7, NPCX_ALT(4, PWM5_SL)) /* PWM5 */
#define NPCX_ALT_PWM6 ALT(C, 0, NPCX_ALT(4, PWM6_SL)) /* PWM6 */
#define NPCX_ALT_PWM7 ALT(6, 0, NPCX_ALT(4, PWM7_SL)) /* PWM7 */

/* MFT Module */
#if NPCX_TACH_SEL2
#define NPCX_ALT_TA1_TACH1 ALT(9, 3, NPCX_ALT(C, TA1_SL2)) /* TA1_SEL2 */
#define NPCX_ALT_TA2_TACH2 ALT(A, 6, NPCX_ALT(C, TA2_SL2)) /* TA2_SEL2 */
#else
#define NPCX_ALT_TA1_TACH1 ALT(4, 0, NPCX_ALT(3, TA1_SL1)) /* TA1_SEL1 */
#define NPCX_ALT_TA2_TACH2 ALT(7, 3, NPCX_ALT(3, TA2_SL1)) /* TA2_SEL1 */
#endif

/* Keyboard Scan Module (Inputs) */
#define NPCX_ALT_KSI0 ALT(3, 1, NPCX_ALT_INV(7, NO_KSI0_SL)) /* KSI0 */
#define NPCX_ALT_KSI1 ALT(3, 0, NPCX_ALT_INV(7, NO_KSI1_SL)) /* KSI1 */
#define NPCX_ALT_KSI2 ALT(2, 7, NPCX_ALT_INV(7, NO_KSI2_SL)) /* KSI2 */
#define NPCX_ALT_KSI3 ALT(2, 6, NPCX_ALT_INV(7, NO_KSI3_SL)) /* KSI3 */
#define NPCX_ALT_KSI4 ALT(2, 5, NPCX_ALT_INV(7, NO_KSI4_SL)) /* KSI4 */
#define NPCX_ALT_KSI5 ALT(2, 4, NPCX_ALT_INV(7, NO_KSI5_SL)) /* KSI5 */
#define NPCX_ALT_KSI6 ALT(2, 3, NPCX_ALT_INV(7, NO_KSI6_SL)) /* KSI6 */
#define NPCX_ALT_KSI7 ALT(2, 2, NPCX_ALT_INV(7, NO_KSI7_SL)) /* KSI7 */

/* Keyboard Scan Module (Outputs) */
#define NPCX_ALT_KSO00 ALT(2, 1, NPCX_ALT_INV(8, NO_KSO00_SL)) /* KSO00 */
#define NPCX_ALT_KSO01 ALT(2, 0, NPCX_ALT_INV(8, NO_KSO01_SL)) /* KSO01 */
#define NPCX_ALT_KSO02 ALT(1, 7, NPCX_ALT_INV(8, NO_KSO02_SL)) /* KSO02 */
#define NPCX_ALT_KSO03 ALT(1, 6, NPCX_ALT_INV(8, NO_KSO03_SL)) /* KSO03 */
#define NPCX_ALT_KSO04 ALT(1, 5, NPCX_ALT_INV(8, NO_KSO04_SL)) /* KSO04 */
#define NPCX_ALT_KSO05 ALT(1, 4, NPCX_ALT_INV(8, NO_KSO05_SL)) /* KSO05 */
#define NPCX_ALT_KSO06 ALT(1, 3, NPCX_ALT_INV(8, NO_KSO06_SL)) /* KSO06 */
#define NPCX_ALT_KSO07 ALT(1, 2, NPCX_ALT_INV(8, NO_KSO07_SL)) /* KSO07 */
#if NPCX_UART_MODULE2
#define NPCX_ALT_KSO08 ALT(1, 1, NPCX_ALT_INV(9, NO_KSO08_SL)) /* KSO08 */
#define NPCX_ALT_KSO09 ALT(1, 0, NPCX_ALT_INV(9, NO_KSO09_SL)) /* KSO09 */
#else
#define NPCX_ALT_KSO08 /* Used as CR_SOUT */
#define NPCX_ALT_KSO09 /* Used as CR_SIN */
#endif
#define NPCX_ALT_KSO10 ALT(0, 7, NPCX_ALT_INV(9, NO_KSO10_SL)) /* KSO10 */
#define NPCX_ALT_KSO11 ALT(0, 6, NPCX_ALT_INV(9, NO_KSO11_SL)) /* KSO11 */
#define NPCX_ALT_KSO12 ALT(0, 5, NPCX_ALT_INV(9, NO_KSO12_SL)) /* KSO12 */
#define NPCX_ALT_KSO13 ALT(0, 4, NPCX_ALT_INV(9, NO_KSO13_SL)) /* KSO13 */
#define NPCX_ALT_KSO14 ALT(8, 2, NPCX_ALT_INV(9, NO_KSO14_SL)) /* KSO14 */
#define NPCX_ALT_KSO15 ALT(8, 3, NPCX_ALT_INV(9, NO_KSO15_SL)) /* KSO15 */
#define NPCX_ALT_KSO16 ALT(0, 3, NPCX_ALT_INV(A, NO_KSO16_SL)) /* KSO16 */
#define NPCX_ALT_KSO17 ALT(B, 1, NPCX_ALT_INV(A, NO_KSO17_SL)) /* KSO17 */

/* Clock module */
#define NPCX_ALT_32KHZ_OUT ALT(7, 5, NPCX_ALT(A, 32K_OUT_SL))  /* 32KHZ_OUT */
#ifdef NPCX_EXT32K_OSC_SUPPORT
#define NPCX_ALT_32KCLKIN  /* Dedicated 32K clock input in NPCX7mnF/G series */
#else
#define NPCX_ALT_32KCLKIN  ALT(E, 7, NPCX_ALT(A, 32KCLKIN_SL)) /* 32KCLKIN */
#endif

/* PSL module */
#ifdef NPCX_PSL_MODE_SUPPORT
#define NPCX_ALT_PSL_IN1 ALT(D, 2, NPCX_ALT_INV(D, NPSL_IN1_SL)) /* PSL_IN1 */
#define NPCX_ALT_PSL_IN2 ALT(0, 0, NPCX_ALT_INV(D, NPSL_IN2_SL)) /* PSL_IN2 */
#define NPCX_ALT_PSL_IN3 ALT(0, 1, NPCX_ALT(D, PSL_IN3_SL))      /* PSL_IN3 */
#define NPCX_ALT_PSL_IN4 ALT(0, 2, NPCX_ALT(D, PSL_IN4_SL))      /* PSL_IN4 */
#else
#define NPCX_ALT_PSL_IN1 /* NO PSL in NPCX7mnG series */
#define NPCX_ALT_PSL_IN2 /* NO PSL in NPCX7mnG series */
#define NPCX_ALT_PSL_IN3 /* NO PSL in NPCX7mnG series */
#define NPCX_ALT_PSL_IN4 /* NO PSL in NPCX7mnG series */
#endif

/* WOV module */
#ifdef NPCX_WOV_SUPPORT
#define NPCX_ALT_I2S_SYNC ALT(A, 5, NPCX_ALT(E, I2S_SL)) /* I2S_SYNC */
#define NPCX_ALT_I2S_SCLK ALT(A, 7, NPCX_ALT(E, I2S_SL)) /* I2S_SCLK */
#define NPCX_ALT_I2S_SDAT ALT(B, 0, NPCX_ALT(E, I2S_SL)) /* I2S_DATA */
#define NPCX_ALT_DMIC_CLK ALT(9, 4, NPCX_ALT(E, WOV_SL)) /* DMIC_CLK */
#define NPCX_ALT_DMIC_IN  ALT(9, 7, NPCX_ALT(E, WOV_SL)) /* DMIC_IN  */
#else
#define NPCX_ALT_I2S_SYNC
#define NPCX_ALT_I2S_SCLK
#define NPCX_ALT_I2S_SDAT
#define NPCX_ALT_DMIC_CLK
#define NPCX_ALT_DMIC_IN
#endif
#define NPCX_ALT_TABLE { \
	NPCX_ALT_I2C0SDA0 \
	NPCX_ALT_I2C0SCL0 \
	NPCX_ALT_I2C1SDA0 \
	NPCX_ALT_I2C1SCL0 \
	NPCX_ALT_I2C2SDA0 \
	NPCX_ALT_I2C2SCL0 \
	NPCX_ALT_I2C3SDA0 \
	NPCX_ALT_I2C3SCL0 \
	NPCX_ALT_I2C4SDA0 \
	NPCX_ALT_I2C4SCL0 \
	NPCX_ALT_I2C4SDA1 \
	NPCX_ALT_I2C4SCL1 \
	NPCX_ALT_I2C5SDA0 \
	NPCX_ALT_I2C5SCL0 \
	NPCX_ALT_I2C5SDA1 \
	NPCX_ALT_I2C5SCL1 \
	NPCX_ALT_I2C6SDA0 \
	NPCX_ALT_I2C6SCL0 \
	NPCX_ALT_I2C6SDA1 \
	NPCX_ALT_I2C6SCL1 \
	NPCX_ALT_I2C7SDA0 \
	NPCX_ALT_I2C7SCL0 \
	NPCX_ALT_ADC0 \
	NPCX_ALT_ADC1 \
	NPCX_ALT_ADC2 \
	NPCX_ALT_ADC3 \
	NPCX_ALT_ADC4 \
	NPCX_ALT_ADC5 \
	NPCX_ALT_ADC6 \
	NPCX_ALT_ADC7 \
	NPCX_ALT_ADC8 \
	NPCX_ALT_ADC9 \
	NPCX_ALT_CR_SIN \
	NPCX_ALT_CR_SOUT \
	NPCX_ALT_CR_SIN2 \
	NPCX_ALT_CR_SOUT2 \
	NPCX_ALT_SPIP_MISO \
	NPCX_ALT_SPIP_CS1 \
	NPCX_ALT_SPIP_MOSI \
	NPCX_ALT_SPIP_SCLK \
	NPCX_ALT_PWM0 \
	NPCX_ALT_PWM1 \
	NPCX_ALT_PWM2 \
	NPCX_ALT_PWM3 \
	NPCX_ALT_PWM4 \
	NPCX_ALT_PWM5 \
	NPCX_ALT_PWM6 \
	NPCX_ALT_PWM7 \
	NPCX_ALT_TA1_TACH1 \
	NPCX_ALT_TA2_TACH2 \
	NPCX_ALT_KSI0 \
	NPCX_ALT_KSI1 \
	NPCX_ALT_KSI2 \
	NPCX_ALT_KSI3 \
	NPCX_ALT_KSI4 \
	NPCX_ALT_KSI5 \
	NPCX_ALT_KSI6 \
	NPCX_ALT_KSI7 \
	NPCX_ALT_KSO00 \
	NPCX_ALT_KSO01 \
	NPCX_ALT_KSO02 \
	NPCX_ALT_KSO03 \
	NPCX_ALT_KSO04 \
	NPCX_ALT_KSO05 \
	NPCX_ALT_KSO06 \
	NPCX_ALT_KSO07 \
	NPCX_ALT_KSO08 \
	NPCX_ALT_KSO09 \
	NPCX_ALT_KSO10 \
	NPCX_ALT_KSO11 \
	NPCX_ALT_KSO12 \
	NPCX_ALT_KSO13 \
	NPCX_ALT_KSO14 \
	NPCX_ALT_KSO15 \
	NPCX_ALT_KSO16 \
	NPCX_ALT_KSO17 \
	NPCX_ALT_32KHZ_OUT \
	NPCX_ALT_32KCLKIN \
	NPCX_ALT_PSL_IN1 \
	NPCX_ALT_PSL_IN2 \
	NPCX_ALT_PSL_IN3 \
	NPCX_ALT_PSL_IN4 \
	NPCX_ALT_I2S_SYNC \
	NPCX_ALT_I2S_SCLK \
	NPCX_ALT_I2S_SDAT \
	NPCX_ALT_DMIC_CLK \
	NPCX_ALT_DMIC_IN \
}

/*****************************************************************************/
/* Macro functions for Low-Voltage mapping table */

/* Low-Voltage GPIO Control 0 */
#define NPCX_LVOL_CTRL_0_0  NPCX_GPIO(B, 5)
#define NPCX_LVOL_CTRL_0_1  NPCX_GPIO(B, 4)
#define NPCX_LVOL_CTRL_0_2  NPCX_GPIO(B, 3)
#define NPCX_LVOL_CTRL_0_3  NPCX_GPIO(B, 2)
#define NPCX_LVOL_CTRL_0_4  NPCX_GPIO(9, 0)
#define NPCX_LVOL_CTRL_0_5  NPCX_GPIO(8, 7)
#define NPCX_LVOL_CTRL_0_6  NPCX_GPIO(0, 0)
#define NPCX_LVOL_CTRL_0_7  NPCX_GPIO(3, 3)

/* Low-Voltage GPIO Control 1 */
#define NPCX_LVOL_CTRL_1_0  NPCX_GPIO(9, 2)
#define NPCX_LVOL_CTRL_1_1  NPCX_GPIO(9, 1)
#define NPCX_LVOL_CTRL_1_2  NPCX_GPIO(D, 1)
#define NPCX_LVOL_CTRL_1_3  NPCX_GPIO(D, 0)
#define NPCX_LVOL_CTRL_1_4  NPCX_GPIO(3, 6)
#define NPCX_LVOL_CTRL_1_5  NPCX_GPIO(6, 4)
#define NPCX_LVOL_CTRL_1_6  NPCX_GPIO(6, 5)
#define NPCX_LVOL_CTRL_1_7  NPCX_GPIO_NONE

/* Low-Voltage GPIO Control 2 */
#define NPCX_LVOL_CTRL_2_0  NPCX_GPIO(7, 4)
#ifdef NPCX_PSL_MODE_SUPPORT
#define NPCX_LVOL_CTRL_2_1  NPCX_GPIO_NONE /* Remove 1.8V support since PSL */
#define NPCX_LVOL_CTRL_2_2  NPCX_GPIO_NONE /* Remove 1.8V support since PSL */
#else
#define NPCX_LVOL_CTRL_2_1  NPCX_GPIO(8, 4)
#define NPCX_LVOL_CTRL_2_2  NPCX_GPIO(8, 5)
#endif

#define NPCX_LVOL_CTRL_2_3  NPCX_GPIO(7, 3)
#define NPCX_LVOL_CTRL_2_4  NPCX_GPIO(C, 1)
#define NPCX_LVOL_CTRL_2_5  NPCX_GPIO(C, 7)
#ifdef NPCX_EXT32K_OSC_SUPPORT
#define NPCX_LVOL_CTRL_2_6  NPCX_GPIO_NONE /* Remove 1.8V support since CLKIN */
#else
#define NPCX_LVOL_CTRL_2_6  NPCX_GPIO(E, 7)
#endif
#define NPCX_LVOL_CTRL_2_7  NPCX_GPIO(3, 4)

/* Low-Voltage GPIO Control 3 */
#define NPCX_LVOL_CTRL_3_0  NPCX_GPIO(C, 6)
#define NPCX_LVOL_CTRL_3_1  NPCX_GPIO(3, 7)
#define NPCX_LVOL_CTRL_3_2  NPCX_GPIO(4, 0)
#ifdef NPCX_EXT32K_OSC_SUPPORT
#define NPCX_LVOL_CTRL_3_3  NPCX_GPIO_NONE /* Remove 1.8V support since CLKOUT*/
#else
#define NPCX_LVOL_CTRL_3_3  NPCX_GPIO(7, 1)
#endif
#define NPCX_LVOL_CTRL_3_4  NPCX_GPIO(8, 2)
#define NPCX_LVOL_CTRL_3_5  NPCX_GPIO(7, 5)
#define NPCX_LVOL_CTRL_3_6  NPCX_GPIO(8, 0)
#define NPCX_LVOL_CTRL_3_7  NPCX_GPIO(C, 5)

/* Low-Voltage GPIO Control 4 */
#ifdef NPCX_PSL_MODE_SUPPORT
#define NPCX_LVOL_CTRL_4_0  NPCX_GPIO_NONE /* Remove 1.8V support since PSL */
#else
#define NPCX_LVOL_CTRL_4_0  NPCX_GPIO(8, 6)
#endif
#define NPCX_LVOL_CTRL_4_1  NPCX_GPIO(C, 2)
#define NPCX_LVOL_CTRL_4_2  NPCX_GPIO(F, 3)
#define NPCX_LVOL_CTRL_4_3  NPCX_GPIO(F, 2)
#define NPCX_LVOL_CTRL_4_4  NPCX_GPIO(F, 5)
#define NPCX_LVOL_CTRL_4_5  NPCX_GPIO(F, 4)
#define NPCX_LVOL_CTRL_4_6  NPCX_GPIO(E, 4)
#define NPCX_LVOL_CTRL_4_7  NPCX_GPIO(E, 3)

/* Low-Voltage GPIO Control 5 */
#define NPCX_LVOL_CTRL_5_0  NPCX_GPIO(7, 2)
#define NPCX_LVOL_CTRL_5_1  NPCX_GPIO_NONE
#define NPCX_LVOL_CTRL_5_2  NPCX_GPIO_NONE
#define NPCX_LVOL_CTRL_5_3  NPCX_GPIO(5, 0)
#define NPCX_LVOL_CTRL_5_4  NPCX_GPIO_NONE
#define NPCX_LVOL_CTRL_5_5  NPCX_GPIO_NONE
#define NPCX_LVOL_CTRL_5_6  NPCX_GPIO_NONE
#define NPCX_LVOL_CTRL_5_7  NPCX_GPIO_NONE

/* 6 Low-Voltage Control Groups on npcx7 */
#define NPCX_LVOL_TABLE { { NPCX_LVOL_CTRL_ITEMS(0), }, \
			  { NPCX_LVOL_CTRL_ITEMS(1), }, \
			  { NPCX_LVOL_CTRL_ITEMS(2), }, \
			  { NPCX_LVOL_CTRL_ITEMS(3), }, \
			  { NPCX_LVOL_CTRL_ITEMS(4), }, \
			  { NPCX_LVOL_CTRL_ITEMS(5), }, }

#endif /* __CROS_EC_GPIO_CHIP_NPCX7_H */
