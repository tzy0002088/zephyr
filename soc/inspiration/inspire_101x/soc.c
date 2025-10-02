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

};

const struct arm_mmu_config mmu_config = {
};

void relocate_vector_table(void)
{
}

void soc_early_init_hook(void)
{
}
