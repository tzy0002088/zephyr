/*
 * Copyright (C) 2025 Microchip Technology Inc. and its subsidiaries
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include <zephyr/init.h>
#include <zephyr/arch/arm/mmu/arm_mmu.h>
#include <zephyr/kernel.h>

static const struct arm_mmu_region mmu_regions[] = {
    MMU_REGION_FLAT_ENTRY("reg", 0, 0x80000000, MT_STRONGLY_ORDERED | MPERM_R | MPERM_W | MT_DEVICE),
    MMU_REGION_FLAT_ENTRY("normal", 0x80000000, 0x20000000, MT_STRONGLY_ORDERED | MPERM_R | MPERM_W | MPERM_X),
};

const struct arm_mmu_config mmu_config = {
    .num_regions = ARRAY_SIZE(mmu_regions),
    .mmu_regions = mmu_regions,
};

void relocate_vector_table(void)
{
    write_vbar(CONFIG_KERNEL_VM_BASE);
}

#define CCM_CCGR0 			*((volatile unsigned int *)0X020C4068)
#define CCM_CCGR1 			*((volatile unsigned int *)0X020C406C)

#define CCM_CCGR2 			*((volatile unsigned int *)0X020C4070)
#define CCM_CCGR3 			*((volatile unsigned int *)0X020C4074)
#define CCM_CCGR4 			*((volatile unsigned int *)0X020C4078)
#define CCM_CCGR5 			*((volatile unsigned int *)0X020C407C)
#define CCM_CCGR6 			*((volatile unsigned int *)0X020C4080)

#define SW_MUX_GPIO1_IO03 	*((volatile unsigned int *)0X020E0068)
#define SW_PAD_GPIO1_IO03 	*((volatile unsigned int *)0X020E02F4)

#define GPIO1_DR 			*((volatile unsigned int *)0X0209C000)
#define GPIO1_GDIR 			*((volatile unsigned int *)0X0209C004)
#define GPIO1_PSR 			*((volatile unsigned int *)0X0209C008)
#define GPIO1_ICR1 			*((volatile unsigned int *)0X0209C00C)
#define GPIO1_ICR2 			*((volatile unsigned int *)0X0209C010)
#define GPIO1_IMR 			*((volatile unsigned int *)0X0209C014)
#define GPIO1_ISR 			*((volatile unsigned int *)0X0209C018)
#define GPIO1_EDGE_SEL 		*((volatile unsigned int *)0X0209C01C)

static void clk_enable(void)
{
    CCM_CCGR0 = 0xffffffff;
    CCM_CCGR1 = 0xffffffff;
    CCM_CCGR2 = 0xffffffff;
    CCM_CCGR3 = 0xffffffff;
    CCM_CCGR4 = 0xffffffff;
    CCM_CCGR5 = 0xffffffff;
    CCM_CCGR6 = 0xffffffff;
}

static void led_init(void)
{
    SW_MUX_GPIO1_IO03 = 0x5;
    SW_PAD_GPIO1_IO03 = 0X10B0;
    GPIO1_GDIR = 0X0000008;
    GPIO1_DR |= (1<<3);
}

static void led_on(void)
{
    GPIO1_DR &= ~(1<<3); 
}

static void led_off(void)
{
    GPIO1_DR |= (1<<3); 
}

static void delay_short(volatile unsigned int n)
{
    while(n--){}
}

static void delay(volatile unsigned int n)
{
    while(n--)
    {
        delay_short(0x7ff);
    }
}

void debug_io()
{
    led_on();
    delay(500);
    led_off();
    delay(500);
}

void soc_prep_hook(void)
{
    clk_enable();
    led_init();
}

void soc_early_init_hook(void)
{
    led_on();
    delay(1000);
    led_off();
    delay(1000);
}
