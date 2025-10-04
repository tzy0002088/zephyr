/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

void debug_io();

int main(void)
{
	while (1)
	{
		debug_io();
	}	

	return 0;
}
