/* Copyright 2020 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * Specific register map for NPCX9 family of chips.
 *
 * Support chip variant:
 * - npcx993f
 * - npcx996f
 *
 * This header file should not be included directly.
 * Please include registers.h instead.
 */

#ifndef __CROS_EC_REGISTERS_H
#error "This header file should not be included directly."
#endif

/* NPCX-IRQ numbers */
#define NPCX_IRQ0_NOUSED		NPCX_IRQ_0
#define NPCX_IRQ1_NOUSED		NPCX_IRQ_1
#define NPCX_IRQ_KBSCAN			NPCX_IRQ_2
#define NPCX_IRQ_PM_CHAN_OBE		NPCX_IRQ_3
#define NPCX_IRQ_PECI			NPCX_IRQ_4
#define NPCX_IRQ_MTC_WKINTD_0		NPCX_IRQ_5
#define NPCX_IRQ_MTC			NPCX_IRQ_MTC_WKINTD_0
#define NPCX_IRQ_PORT80			NPCX_IRQ_6
#define NPCX_IRQ_CR_SIN2_WKINTA_0	NPCX_IRQ_7
#define NPCX_IRQ_SMB8			NPCX_IRQ_8
#define NPCX_IRQ_MFT_1			NPCX_IRQ_9
#define NPCX_IRQ_ADC			NPCX_IRQ_10
#define NPCX_IRQ_WKINTE_0		NPCX_IRQ_11
#define NPCX_IRQ_GDMA			NPCX_IRQ_12
#define NPCX_IRQ_SMB1			NPCX_IRQ_13
#define NPCX_IRQ_SMB2			NPCX_IRQ_14
#define NPCX_IRQ_WKINTC_0		NPCX_IRQ_15
#define NPCX_IRQ_SMB7			NPCX_IRQ_16
#define NPCX_IRQ_ITIM32_3		NPCX_IRQ_17
#define NPCX_IRQ_SHI			NPCX_IRQ_18
#define NPCX_IRQ_ESPI			NPCX_IRQ_18
#define NPCX_IRQ_SMB5			NPCX_IRQ_19
#define NPCX_IRQ_SMB6			NPCX_IRQ_20
#define NPCX_IRQ_PS2			NPCX_IRQ_21
#define NPCX_IRQ22_NOUSED		NPCX_IRQ_22
#define NPCX_IRQ_MFT_2			NPCX_IRQ_23
#define NPCX_IRQ_SHM			NPCX_IRQ_24
#define NPCX_IRQ_KBC_IBF		NPCX_IRQ_25
#define NPCX_IRQ_PM_CHAN_IBF		NPCX_IRQ_26
#define NPCX_IRQ_ITIM32_2		NPCX_IRQ_27
#define NPCX_IRQ_ITIM32_1		NPCX_IRQ_28
#define NPCX_I3C_MDMA5			NPCX_IRQ_29
#define NPCX_IRQ30_NOUSED		NPCX_IRQ_30
#define NPCX_IRQ_TWD_WKINTB_0		NPCX_IRQ_31
#define NPCX_IRQ_UART2			NPCX_IRQ_32
#define NPCX_IRQ_UART			NPCX_IRQ_33
#define NPCX_IRQ34_NOUSED		NPCX_IRQ_34
#define NPCX_IRQ_WKINTF_0		NPCX_IRQ_35
#define NPCX_IRQ_SMB3			NPCX_IRQ_36
#define NPCX_IRQ_SMB4			NPCX_IRQ_37
#define NPCX_IRQ_UART3			NPCX_IRQ_38
#define NPCX_IRQ_UART4			NPCX_IRQ_39
#define NPCX_IRQ40_NOUSED		NPCX_IRQ_40
#define NPCX_IRQ_MFT_3			NPCX_IRQ_41
#define NPCX_IRQ_WKINTG_0		NPCX_IRQ_42
#define NPCX_IRQ_ITIM32_4		NPCX_IRQ_43
#define NPCX_IRQ_ITIM32_5		NPCX_IRQ_44
#define NPCX_IRQ_ITIM32_6		NPCX_IRQ_45
#define NPCX_IRQ_WKINTH_0		NPCX_IRQ_46
#define NPCX_IRQ_WKINTA_1		NPCX_IRQ_47
#define NPCX_IRQ_WKINTB_1		NPCX_IRQ_48
#define NPCX_IRQ_KSI_WKINTC_1		NPCX_IRQ_49
#define NPCX_IRQ_WKINTD_1		NPCX_IRQ_50
#define NPCX_IRQ_WKINTE_1		NPCX_IRQ_51
#define NPCX_IRQ_WKINTF_1		NPCX_IRQ_52
#define NPCX_IRQ_WKINTG_1		NPCX_IRQ_53
#define NPCX_IRQ_WKINTH_1		NPCX_IRQ_54
#define NPCX_WKINTG_2			NPCX_IRQ_55
#define NPCX_IRQ_KBC_OBE		NPCX_IRQ_56
#define NPCX_IRQ_SPI			NPCX_IRQ_57
#define NPCX_IRQ_ITIM64			NPCX_IRQ_58
#define NPCX_IRQ_LCT_WKINTF_2		NPCX_IRQ_59
#define NPCX_IRQ_WKINTA_2		NPCX_IRQ_60
#define NPCX_IRQ_WKINTB_2		NPCX_IRQ_61
#define NPCX_IRQ_WKINTC_2		NPCX_IRQ_62
#define NPCX_IRQ_WKINTD_2		NPCX_IRQ_63
/* MIWU definition */
#define LCT_WUI_GROUP			MIWU_GROUP_6
#define LCT_WUI_MASK			MASK_PIN7

/* Modules Map */
#define NPCX_CR_UART_BASE_ADDR(mdl)	(0x400E0000 + ((mdl) * 0x2000L))
#define NPCX_LCT_BASE_ADDR		0x400D7000
#define NPCX_SMB_BASE_ADDR(mdl)		(((mdl) < 2) ? \
				(0x40009000 + ((mdl) * 0x2000L)) : \
				((mdl) < 4) ? \
				(0x400C0000 + (((mdl) - 2) * 0x2000L)) : \
				((mdl) == 4) ? \
				(0x40008000) : \
				(0x40017000 + (((mdl) - 5) * 0x1000L)))

#define NPCX_HFCBCD1			REG8(NPCX_HFCG_BASE_ADDR + 0x012)
#define NPCX_HFCBCD2			REG8(NPCX_HFCG_BASE_ADDR + 0x014)

enum {
	NPCX_UART_PORT0 = 0,	/* UART port 0 */
	NPCX_UART_PORT1 = 1,	/* UART port 1 */
	NPCX_UART_PORT2 = 2,	/* UART port 2 */
	NPCX_UART_PORT3 = 3,	/* UART port 3 */
	NPCX_UART_COUNT
};

 /* UART registers only used for FIFO mode */
#define NPCX_UFTSTS(n)			REG8(NPCX_CR_UART_BASE_ADDR(n) + 0x020)
#define NPCX_UFRSTS(n)			REG8(NPCX_CR_UART_BASE_ADDR(n) + 0x022)
#define NPCX_UFTCTL(n)			REG8(NPCX_CR_UART_BASE_ADDR(n) + 0x024)
#define NPCX_UFRCTL(n)			REG8(NPCX_CR_UART_BASE_ADDR(n) + 0x026)

/* UART FIFO register fields */
#define NPCX_UMDSL_FIFO_MD		0

#define NPCX_UFTSTS_TEMPTY_LVL		FIELD(0, 5)
#define NPCX_UFTSTS_TEMPTY_LVL_STS	5
#define NPCX_UFTSTS_TFIFO_EMPTY_STS	6
#define NPCX_UFTSTS_NXMIP		7

#define NPCX_UFRSTS_RFULL_LVL_STS	5
#define NPCX_UFRSTS_RFIFO_NEMPTY_STS	6
#define NPCX_UFRSTS_ERR			7

#define NPCX_UFTCTL_TEMPTY_LVL_SEL	FIELD(0, 5)
#define NPCX_UFTCTL_TEMPTY_LVL_EN	5
#define NPCX_UFTCTL_TEMPTY_EN		6
#define NPCX_UFTCTL_NXMIPEN		7

#define NPCX_UFRCTL_RFULL_LVL_SEL	FIELD(0, 5)
#define NPCX_UFRCTL_RFULL_LVL_EN	5
#define NPCX_UFRCTL_RNEMPTY_EN		6
#define NPCX_UFRCTL_ERR_EN		7

/* KBSCAN register fields */
#define NPCX_KBHDRV_FIELD		FIELD(6, 2)

/* GLUE registers */
#define NPCX_GLUE_PSL_CTS		REG8(NPCX_GLUE_REGS_BASE + 0x027)
#define NPCX_GLUE_PSL_MCTL1		REG8(NPCX_GLUE_REGS_BASE + 0x034)
#define NPCX_GLUE_PSL_MCTL2		REG8(NPCX_GLUE_REGS_BASE + 0x038)

/* PSL register fields */
#define NPCX_GLUE_PSL_MCTL1_VCC1_RST_PSL	7
#define NPCX_GLUE_PSL_MCTL1_PSL_GPO_CTL		6
#define NPCX_GLUE_PSL_MCTL1_LCT_EV		4
#define NPCX_GLUE_PSL_MCTL1_VCC1_RST_EV		3
#define NPCX_GLUE_PSL_MCTL1_PLS_EN		1
#define NPCX_GLUE_PSL_MCTL1_OD_EN		0

#define NPCX_GLUE_PSL_MCTL2_VCC1_RST_PSL_LK	7
#define NPCX_GLUE_PSL_MCTL2_PSL_GP_EN		6
#define NPCX_GLUE_PSL_MCTL2_AC_IN_BLOCK_EN	3
#define NPCX_GLUE_PSL_MCTL2_AC_IN_SEL		FIELD(0, 1)

/* GPIO registers */
#define NPCX_PLOCK_CTL(n)		REG8(NPCX_GPIO_BASE_ADDR(n) + 0x007)

/* System Configuration (SCFG) Registers */

/* SCFG enumeration */
enum {
	ALT_GROUP_0,
	ALT_GROUP_1,
	ALT_GROUP_2,
	ALT_GROUP_3,
	ALT_GROUP_4,
	ALT_GROUP_5,
	ALT_GROUP_6,
	ALT_GROUP_7,
	ALT_GROUP_8,
	ALT_GROUP_9,
	ALT_GROUP_A,
	ALT_GROUP_B,
	ALT_GROUP_C,
	ALT_GROUP_D,
	ALT_GROUP_E,
	ALT_GROUP_F,
	ALT_GROUP_G,
	ALT_GROUP_H,
	ALT_GROUP_J,
	ALT_GROUP_COUNT
};

#define NPCX_DEVALT(n)			REG8(NPCX_SCFG_BASE_ADDR + 0x010 + (n))

#define NPCX_LV_GPIO_CTL_ADDR(n)	(((n) < 5) ? \
					(NPCX_SCFG_BASE_ADDR + 0x02A + (n)) :\
					(NPCX_SCFG_BASE_ADDR + 0x026))
#define NPCX_LV_GPIO_CTL(n)		REG8(NPCX_LV_GPIO_CTL_ADDR(n))
/* Device Alternate Function Lock */
#define NPCX_DEVALT_LK(n)		REG8(NPCX_SCFG_BASE_ADDR + 0x210 + (n))

/* pin-mux for I2C */
#define NPCX_DEVALT2_I2C0_0_SL		0
#define NPCX_DEVALT2_I2C7_0_SL		1
#define NPCX_DEVALT2_I2C1_0_SL		2
#define NPCX_DEVALT2_I2C6_0_SL		3
#define NPCX_DEVALT2_I2C2_0_SL		4
#define NPCX_DEVALT2_I2C5_0_SL		5
#define NPCX_DEVALT2_I2C3_0_SL		6
#define NPCX_DEVALT6_I2C6_1_SL		5
#define NPCX_DEVALT6_I2C5_1_SL		6
#define NPCX_DEVALT6_I2C4_1_SL		7

/* pin-mux for ADC */
#define NPCX_DEVALTF_ADC5_SL		0
#define NPCX_DEVALTF_ADC6_SL		1
#define NPCX_DEVALTF_ADC7_SL		2
#define NPCX_DEVALTF_ADC8_SL		3
#define NPCX_DEVALTF_ADC9_SL		4
#define NPCX_DEVALTF_ADC10_SL		5
#define NPCX_DEVALTF_ADC11_SL		6

/* pin-mux for PSL */
#define NPCX_DEVALTD_PSL_IN1_AHI	0
#define NPCX_DEVALTD_NPSL_IN1_SL	1
#define NPCX_DEVALTD_PSL_IN2_AHI	2
#define NPCX_DEVALTD_NPSL_IN2_SL	3
#define NPCX_DEVALTD_PSL_IN3_AHI	4
#define NPCX_DEVALTD_PSL_IN3_SL		5
#define NPCX_DEVALTD_PSL_IN4_AHI	6
#define NPCX_DEVALTD_PSL_IN4_SL		7

/* pin-mux for Misc. */

/* pin-mux for UART */
#define NPCX_DEVALTJ_CR_SIN1_SL1	0
#define NPCX_DEVALTJ_CR_SOUT1_SL1	1
#define NPCX_DEVALTJ_CR_SIN1_SL2	2
#define NPCX_DEVALTJ_CR_SOUT1_SL2	3
#define NPCX_DEVALTJ_CR_SIN2_SL		4
#define NPCX_DEVALTJ_CR_SOUT2_SL	5
#define NPCX_DEVALTJ_CR_SIN3_SL		6
#define NPCX_DEVALTJ_CR_SOUT3_SL	7
#define NPCX_DEVALTE_CR_SIN4_SL		6
#define NPCX_DEVALTE_CR_SOUT4_SL	7

/* SHI module version 2 enable bit */
#define NPCX_DEVALTF_SHI_NEW		7

/* VCC_RST Pull-Up Disable */
#define NPCX_DEVALTG_VCC1_RST_PUD	5
#define NPCX_DEVALTG_PSL_OUT_SL		6
#define NPCX_DEVALTG_PSL_GPO_SL		7

/* SMBus register fields */
#define NPCX_SMBSEL_SMB4SEL		4
#define NPCX_SMBSEL_SMB5SEL		5
#define NPCX_SMBSEL_SMB6SEL		6

/* SMB enumeration: I2C port definitions. */
enum {
	NPCX_I2C_PORT0_0 = 0,	/* I2C port 0, bus 0 */
	NPCX_I2C_PORT1_0,	/* I2C port 1, bus 0 */
	NPCX_I2C_PORT2_0,	/* I2C port 2, bus 0 */
	NPCX_I2C_PORT3_0,	/* I2C port 3, bus 0 */
	NPCX_I2C_PORT4_1,	/* I2C port 4, bus 1 */
	NPCX_I2C_PORT5_0,	/* I2C port 5, bus 0 */
	NPCX_I2C_PORT5_1,	/* I2C port 5, bus 1 */
	NPCX_I2C_PORT6_0,	/* I2C port 6, bus 0 */
	NPCX_I2C_PORT6_1,	/* I2C port 6, bus 1 */
	NPCX_I2C_PORT7_0,	/* I2C port 7, bus 0 */
	NPCX_I2C_COUNT,
};

/* Power Management Controller (PMC) Registers */
#define NPCX_FMUL_WIN_DLY	REG8(NPCX_PMC_BASE_ADDR + 0x010)
#define NPCX_RAM_PD(offset)	REG8(NPCX_PMC_BASE_ADDR + 0x020 + (offset))

/* PMC register fields */
#define NPCX_PWDWN_CTL3_SMB4_PD		4
#define NPCX_PWDWN_CTL7_SMB5_PD		0
#define NPCX_PWDWN_CTL7_SMB6_PD		1
#define NPCX_PWDWN_CTL7_SMB7_PD		2
#define NPCX_PWDWN_CTL7_ITIM64_PD	5
#define NPCX_PWDWN_CTL7_UART2_PD	6

/*
 * PMC enumeration:
 * Offsets from CGC_BASE registers for each peripheral.
 */
enum {
	CGC_OFFSET_KBS = 0,
	CGC_OFFSET_UART = 0,
	CGC_OFFSET_FAN = 0,
	CGC_OFFSET_FIU = 0,
	CGC_OFFSET_PS2 = 0,
	CGC_OFFSET_PWM = 1,
	CGC_OFFSET_I2C = 2,
	CGC_OFFSET_ADC = 3,
	CGC_OFFSET_PECI = 3,
	CGC_OFFSET_SPI = 3,
	CGC_OFFSET_TIMER = 3,
	CGC_OFFSET_LPC = 4,
	CGC_OFFSET_ESPI = 5,
	CGC_OFFSET_I2C2 = 6,
	CGC_OFFSET_UART2 = 6,
};

enum NPCX_PMC_PWDWN_CTL_T {
	NPCX_PMC_PWDWN_1 = 0,
	NPCX_PMC_PWDWN_2 = 1,
	NPCX_PMC_PWDWN_3 = 2,
	NPCX_PMC_PWDWN_4 = 3,
	NPCX_PMC_PWDWN_5 = 4,
	NPCX_PMC_PWDWN_6 = 5,
	NPCX_PMC_PWDWN_7 = 6,
	NPCX_PMC_PWDWN_CNT,
};

#define CGC_I2C_MASK			(BIT(NPCX_PWDWN_CTL3_SMB0_PD) | \
					BIT(NPCX_PWDWN_CTL3_SMB1_PD) | \
					BIT(NPCX_PWDWN_CTL3_SMB2_PD) | \
					BIT(NPCX_PWDWN_CTL3_SMB3_PD) | \
					BIT(NPCX_PWDWN_CTL3_SMB4_PD))
#define CGC_I2C_MASK2			(BIT(NPCX_PWDWN_CTL7_SMB5_PD) | \
					BIT(NPCX_PWDWN_CTL7_SMB6_PD) | \
					BIT(NPCX_PWDWN_CTL7_SMB7_PD))
#define CGC_UART2_MASK			BIT(NPCX_PWDWN_CTL7_UART2_PD)

/* BBRAM register fields */
#define NPCX_BKUP_STS_VSBY_STS		1
#define NPCX_BKUP_STS_VCC1_STS		0
#define NPCX_BKUP_STS_ALL_MASK \
	(BIT(NPCX_BKUP_STS_IBBR) | BIT(NPCX_BKUP_STS_VSBY_STS) | \
	BIT(NPCX_BKUP_STS_VCC1_STS))
#define NPCX_BBRAM_SIZE			128 /* Size of BBRAM */

/* ITIM registers */
#define NPCX_ITCNT32(n)			REG32(NPCX_ITIM_BASE_ADDR(n) + 0x008)

/* Timer counter register used for 1 micro-second system tick */
#define NPCX_ITCNT_SYSTEM		NPCX_ITCNT32(ITIM32_6)
/* Timer counter register used for others */
#define NPCX_ITCNT			NPCX_ITCNT32

/* ITIM module No. used for event */
#define ITIM_EVENT_NO			ITIM32_1
/* ITIM module No. used for watchdog */
#define ITIM_WDG_NO				ITIM32_5
/* ITIM module No. used for 1 micro-second system tick */
#define ITIM_SYSTEM_NO			ITIM32_6

/* ITIM enumeration */
enum ITIM_MODULE_T {
	ITIM32_1,
	ITIM32_2,
	ITIM32_3,
	ITIM32_4,
	ITIM32_5,
	ITIM32_6,
	ITIM_MODULE_COUNT,
};

/* Serial Host Interface (SHI) Registers - only available on SHI Version 2 */
#define NPCX_SHICFG3			REG8(NPCX_SHI_BASE_ADDR + 0x00C)
#define NPCX_SHICFG4			REG8(NPCX_SHI_BASE_ADDR + 0x00D)
#define NPCX_SHICFG5			REG8(NPCX_SHI_BASE_ADDR + 0x00E)
#define NPCX_EVSTAT2			REG8(NPCX_SHI_BASE_ADDR + 0x00F)
#define NPCX_EVENABLE2			REG8(NPCX_SHI_BASE_ADDR + 0x010)
#define NPCX_OBUF(n)			REG8(NPCX_SHI_BASE_ADDR + 0x020 + (n))
#define NPCX_IBUF(n)			REG8(NPCX_SHI_BASE_ADDR + 0x0A0 + (n))

/* SHI register fields */
#define NPCX_SHICFG3_OBUFLVLDIS		7
#define NPCX_SHICFG4_IBUFLVLDIS		7
#define NPCX_SHICFG5_IBUFLVL2		FIELD(0, 6)
#define NPCX_SHICFG5_IBUFLVL2DIS	7
#define NPCX_EVSTAT2_IBHF2		0
#define NPCX_EVSTAT2_CSNRE		1
#define NPCX_EVSTAT2_CSNFE		2
#define NPCX_EVENABLE2_IBHF2EN		0
#define NPCX_EVENABLE2_CSNREEN		1
#define NPCX_EVENABLE2_CSNFEEN		2

/* eSPI register fields */
#define NPCX_ESPIIE_BMTXDONEIE		19
#define NPCX_ESPIIE_PBMRXIE		20
#define NPCX_ESPIIE_PMSGRXIE		21
#define NPCX_ESPIIE_BMBURSTERRIE	22
#define NPCX_ESPIIE_BMBURSTDONEIE	23

#define NPCX_ESPIWE_PBMRXWE		20
#define NPCX_ESPIWE_PMSGRXWE		21

#define NPCX_ESPISTS_VWUPDW		17
#define NPCX_ESPISTS_BMTXDONE		19
#define NPCX_ESPISTS_PBMRX		20
#define NPCX_ESPISTS_PMSGRX		21
#define NPCX_ESPISTS_BMBURSTERR		22
#define NPCX_ESPISTS_BMBURSTDONE	23
#define NPCX_ESPISTS_ESPIRST_LVL	24

#define ESPIIE_BMTXDONE			BIT(NPCX_ESPIIE_BMTXDONEIE)
#define ESPIIE_PBMRX			BIT(NPCX_ESPIIE_PBMRXIE)
#define ESPIIE_PMSGRX			BIT(NPCX_ESPIIE_PMSGRXIE)
#define ESPIIE_BMBURSTERR		BIT(NPCX_ESPIIE_BMBURSTERRIE)
#define ESPIIE_BMBURSTDONE		BIT(NPCX_ESPIIE_BMBURSTDONEIE)

#define ESPIWE_PBMRX			BIT(NPCX_ESPIWE_PBMRXWE)
#define ESPIWE_PMSGRX			BIT(NPCX_ESPIWE_PMSGRXWE)

/* Bit field manipulation for VWEVMS Value */
#define VWEVMS_WK_EN(e)			(((e)<<20) & 0x00100000)
#define VWEVMS_INTWK_EN(e)		(VWEVMS_INT_EN(e) | VWEVMS_WK_EN(e))

/* eSPI max supported frequency */
enum {
	NPCX_ESPI_MAXFREQ_20 = 0,
	NPCX_ESPI_MAXFREQ_25 = 1,
	NPCX_ESPI_MAXFREQ_33 = 2,
	NPCX_ESPI_MAXFREQ_50 = 3,
	NPCX_ESPI_MAXFREQ_66 = 4,
	NPCX_ESPI_MAXFREQ_NONE = 0xFF
};

/* eSPI Operating Frequency */
enum {
	NPCX_ESPI_OPFREQ_20 = 0,
	NPCX_ESPI_OPFREQ_25 = 1,
	NPCX_ESPI_OPFREQ_33 = 2,
	NPCX_ESPI_OPFREQ_50 = 3,
	NPCX_ESPI_OPFREQ_66 = 4,
	NPCX_ESPI_OPFREQ_NONE = 0xFF
};

/* eSPI max frequency support per FMCLK */
#if (FMCLK <= 33000000)
#define NPCX_ESPI_MAXFREQ_MAX		NPCX_ESPI_MAXFREQ_33
#else
#define NPCX_ESPI_MAXFREQ_MAX		NPCX_ESPI_MAXFREQ_50
#endif

/* UART registers */
#define NPCX_UART_WK_GROUP		MIWU_GROUP_8
#define NPCX_UART_WK_BIT		7
#define NPCX_UART2_WK_GROUP		MIWU_GROUP_1
#define NPCX_UART2_WK_BIT		6

/* MIWU registers */
#define NPCX_WKEDG_ADDR(port, n)	(NPCX_MIWU_BASE_ADDR(port) + 0x00 + \
					((n) * 0x10))
#define NPCX_WKAEDG_ADDR(port, n)	(NPCX_MIWU_BASE_ADDR(port) + 0x01 + \
					((n) * 0x10))
#define NPCX_WKMOD_ADDR(port, n)	(NPCX_MIWU_BASE_ADDR(port) + 0x02 + \
					((n) * 0x10))
#define NPCX_WKPND_ADDR(port, n)	(NPCX_MIWU_BASE_ADDR(port) + 0x03 + \
					((n) * 0x10))
#define NPCX_WKPCL_ADDR(port, n)	(NPCX_MIWU_BASE_ADDR(port) + 0x04 + \
					((n) * 0x10))
#define NPCX_WKEN_ADDR(port, n)		(NPCX_MIWU_BASE_ADDR(port) + 0x05 + \
					((n) * 0x10))
#define NPCX_WKST_ADDR(port, n)		(NPCX_MIWU_BASE_ADDR(port) + 0x06 + \
					((n) * 0x10))
#define NPCX_WKINEN_ADDR(port, n)	(NPCX_MIWU_BASE_ADDR(port) + 0x07 + \
					((n) * 0x10))

#define NPCX_WKEDG(port, n)		REG8(NPCX_WKEDG_ADDR(port, n))
#define NPCX_WKAEDG(port, n)		REG8(NPCX_WKAEDG_ADDR(port, n))
#define NPCX_WKPND(port, n)		REG8(NPCX_WKPND_ADDR(port, n))
#define NPCX_WKPCL(port, n)		REG8(NPCX_WKPCL_ADDR(port, n))
#define NPCX_WKEN(port, n)		REG8(NPCX_WKEN_ADDR(port, n))
#define NPCX_WKST(port, n)		REG8(NPCX_WKST_ADDR(port, n))
#define NPCX_WKINEN(port, n)		REG8(NPCX_WKINEN_ADDR(port, n))
#define NPCX_WKMOD(port, n)		REG8(NPCX_WKMOD_ADDR(port, n))

/* LCT register */
#define NPCX_LCTCONT			REG8(NPCX_LCT_BASE_ADDR + 0x002)
#define NPCX_LCTSTAT			REG8(NPCX_LCT_BASE_ADDR + 0x004)
#define NPCX_LCTSECOND			REG8(NPCX_LCT_BASE_ADDR + 0x005)
#define NPCX_LCTMINUTE			REG8(NPCX_LCT_BASE_ADDR + 0x006)
#define NPCX_LCTHOUR			REG8(NPCX_LCT_BASE_ADDR + 0x008)
#define NPCX_LCTDAY			REG8(NPCX_LCT_BASE_ADDR + 0x00A)
#define NPCX_LCTWEEK			REG8(NPCX_LCT_BASE_ADDR + 0x00C)
/* LCTCONT fields */
#define NPCX_LCTCONT_EN			0
#define NPCX_LCTCONT_EN_FIELD		FIELD(0, 1)
#define NPCX_LCTCONT_EVEN		1
#define NPCX_LCTCONT_PSL_EN		2
#define NPCX_LCTCONT_CLK_EN		6
#define NPCX_LCTCONT_VSBY_PWR		7
/* LCTSTAT fields */
#define NPCX_LCTSTAT_EVST		0

/* UART registers and functions */
#if NPCX_UART_MODULE2
/*
 * To be used as 2nd parameter to NPCX_WK*() macro, table (1st parameter) is
 * always 1 == MIWU_TABLE_1.
 */
#define NPCX_UART_MIWU_IRQ NPCX_IRQ_WKINTG_1
#define NPCX_UART_DEVALT		NPCX_DEVALT(ALT_GROUP_J)
#define NPCX_UART_DEVALT_SIN_SL		NPCX_DEVALTJ_CR_SIN1_SL2
#define NPCX_UART_DEVALT_SOUT_SL	NPCX_DEVALTJ_CR_SOUT1_SL2
#define NPCX_UART_ALT_DEVALT		NPCX_DEVALT(ALT_GROUP_J)
#define NPCX_UART_ALT_DEVALT_SIN_SL	NPCX_DEVALTJ_CR_SIN1_SL1
#define NPCX_UART_ALT_DEVALT_SOUT_SL	NPCX_DEVALTJ_CR_SOUT1_SL1
#else /* !NPCX_UART_MODULE2 */
#define NPCX_UART_MIWU_IRQ		NPCX_IRQ_WKINTB_1
#define NPCX_UART_DEVALT		NPCX_DEVALT(ALT_GROUP_J)
#define NPCX_UART_DEVALT_SIN_SL		NPCX_DEVALTJ_CR_SIN1_SL1
#define NPCX_UART_DEVALT_SOUT_SL	NPCX_DEVALTJ_CR_SOUT1_SL1
#define NPCX_UART_ALT_DEVALT		NPCX_DEVALT(ALT_GROUP_J)
#define NPCX_UART_ALT_DEVALT_SIN_SL	NPCX_DEVALTJ_CR_SIN1_SL2
#define NPCX_UART_ALT_DEVALT_SOUT_SL	NPCX_DEVALTJ_CR_SOUT1_SL2
#endif /* NPCX_UART_MODULE2 */

/* ADC register */
#define NPCX_ADCSTS		REG16(NPCX_ADC_BASE_ADDR + 0x000)
#define NPCX_ADCCNF		REG16(NPCX_ADC_BASE_ADDR + 0x002)
#define NPCX_ATCTL		REG16(NPCX_ADC_BASE_ADDR + 0x004)
#define NPCX_ASCADD		REG16(NPCX_ADC_BASE_ADDR + 0x006)
#define NPCX_ADCCS		REG16(NPCX_ADC_BASE_ADDR + 0x008)
#define NPCX_THRCTS		REG16(NPCX_ADC_BASE_ADDR + 0x01A)
#define NPCX_ADCCNF2		REG16(NPCX_ADC_BASE_ADDR + 0x020)
#define NPCX_GENDLY		REG16(NPCX_ADC_BASE_ADDR + 0x022)
#define NPCX_MEAST		REG16(NPCX_ADC_BASE_ADDR + 0x026)
/* NOTE: This is 0-based for the ADC channels. */
#define NPCX_CHNDAT(n)		REG16(NPCX_ADC_BASE_ADDR + 0x040 + (2L*(n)))
/* NOTE: These are 1-based for the threshold detectors. */
#define NPCX_THRCTL(n)		REG16(NPCX_ADC_BASE_ADDR + 0x05E + (2L*(n)))

/* ADC register fields */
#define NPCX_ADCSTS_EOCEV		0
#define NPCX_ADCCNF_ADCMD_FIELD		FIELD(1, 2)
#define NPCX_ADCCNF_ADCRPTC		3
#define NPCX_ADCCNF_INTECEN		6
#define NPCX_ADCCNF_START		4
#define NPCX_ADCCNF_ADCEN		0
#define NPCX_ADCCNF_STOP		11
#define NPCX_ATCTL_SCLKDIV_FIELD	FIELD(0, 6)
#define NPCX_ATCTL_DLY_FIELD		FIELD(8, 3)
#define NPCX_ASCADD_SADDR_FIELD		FIELD(0, 5)
#define NPCX_THRCTS_ADC_WKEN		15
#define NPCX_THRCTS_THR6_IEN		13
#define NPCX_THRCTS_THR5_IEN		12
#define NPCX_THRCTS_THR4_IEN		11
#define NPCX_THRCTS_THR3_IEN		10
#define NPCX_THRCTS_THR2_IEN		9
#define NPCX_THRCTS_THR1_IEN		8
#define NPCX_THRCTS_ADC_EVENT		7
#define NPCX_THRCTS_THR6_STS		5
#define NPCX_THRCTS_THR5_STS		4
#define NPCX_THRCTS_THR4_STS		3
#define NPCX_THRCTS_THR3_STS		2
#define NPCX_THRCTS_THR2_STS		1
#define NPCX_THRCTS_THR1_STS		0
#define NPCX_CHNDAT_CHDAT_FIELD		FIELD(0, 10)
#define NPCX_CHNDAT_NEW			15
#define NPCX_THRCTL_THEN		15
#define NPCX_THRCTL_L_H			14
#define NPCX_THRCTL_CHNSEL		FIELD(10, 4)
#define NPCX_THRCTL_THRVAL		FIELD(0, 10)

#define NPCX_ADC_THRESH1		1
#define NPCX_ADC_THRESH2		2
#define NPCX_ADC_THRESH3		3
#define NPCX_ADC_THRESH4		4
#define NPCX_ADC_THRESH5		5
#define NPCX_ADC_THRESH6		6
#define NPCX_ADC_THRESH_CNT		6
