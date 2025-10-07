/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "zephyr/irq.h"
#include <stdio.h>
#include <zephyr/sys/printk.h>
#include "zephyr/kernel.h"

int main(void)
{
	printk("hello zephyr %d.\n", sys_clock_cycle_get_32());
	while (1)
	{
		debug_io();
		printk("hello zephyr %d.\n", sys_clock_cycle_get_32());
	}

	return 0;
}
