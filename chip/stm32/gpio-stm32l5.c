/* Copyright 2021 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "clock.h"
#include "common.h"
#include "gpio.h"
#include "gpio_chip.h"
#include "hooks.h"
#include "registers.h"
#include "task.h"
#include "util.h"

void gpio_enable_clocks(void)
{
	/*
	 * Enable all GPIOs clocks
	 *
	 * TODO(crosbug.com/p/23770): only enable the banks we need to,
	 * and support disabling some of them in low-power idle.
	 */
	STM32_RCC_AHB2ENR |= STM32_RCC_AHB2ENR_GPIOMASK;

	/* Delay 1 AHB clock cycle after the clock is enabled */
	clock_wait_bus_cycles(BUS_AHB, 1);
}

static void gpio_init(void)
{
	/* Enable IRQs now that pins are set up */
	task_enable_irq(STM32_IRQ_EXTI0);
	task_enable_irq(STM32_IRQ_EXTI1);
	task_enable_irq(STM32_IRQ_EXTI2);
	task_enable_irq(STM32_IRQ_EXTI3);
	task_enable_irq(STM32_IRQ_EXTI4);
	task_enable_irq(STM32_IRQ_EXTI5);
	task_enable_irq(STM32_IRQ_EXTI6);
	task_enable_irq(STM32_IRQ_EXTI7);
	task_enable_irq(STM32_IRQ_EXTI8);
	task_enable_irq(STM32_IRQ_EXTI9);
	task_enable_irq(STM32_IRQ_EXTI10);
	task_enable_irq(STM32_IRQ_EXTI11);
	task_enable_irq(STM32_IRQ_EXTI12);
	task_enable_irq(STM32_IRQ_EXTI13);
	task_enable_irq(STM32_IRQ_EXTI14);
	task_enable_irq(STM32_IRQ_EXTI15);

}
DECLARE_HOOK(HOOK_INIT, gpio_init, HOOK_PRIO_DEFAULT);

DECLARE_IRQ(STM32_IRQ_EXTI0, gpio_interrupt, 1);
DECLARE_IRQ(STM32_IRQ_EXTI1, gpio_interrupt, 1);
DECLARE_IRQ(STM32_IRQ_EXTI2, gpio_interrupt, 1);
DECLARE_IRQ(STM32_IRQ_EXTI3, gpio_interrupt, 1);
DECLARE_IRQ(STM32_IRQ_EXTI4, gpio_interrupt, 1);
DECLARE_IRQ(STM32_IRQ_EXTI5, gpio_interrupt, 1);
DECLARE_IRQ(STM32_IRQ_EXTI6, gpio_interrupt, 1);
DECLARE_IRQ(STM32_IRQ_EXTI7, gpio_interrupt, 1);
DECLARE_IRQ(STM32_IRQ_EXTI8, gpio_interrupt, 1);
DECLARE_IRQ(STM32_IRQ_EXTI9, gpio_interrupt, 1);
DECLARE_IRQ(STM32_IRQ_EXTI10, gpio_interrupt, 1);
DECLARE_IRQ(STM32_IRQ_EXTI11, gpio_interrupt, 1);
DECLARE_IRQ(STM32_IRQ_EXTI12, gpio_interrupt, 1);
DECLARE_IRQ(STM32_IRQ_EXTI13, gpio_interrupt, 1);
DECLARE_IRQ(STM32_IRQ_EXTI14, gpio_interrupt, 1);
DECLARE_IRQ(STM32_IRQ_EXTI15, gpio_interrupt, 1);

#include "gpio-f0-l.c"
