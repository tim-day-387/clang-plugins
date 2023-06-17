// SPDX-License-Identifier: GPL-2.0

/*
 * Simple test case for FindStatic
 *
 */

#include <stdio.h>

/*
 * This function should be static.
 */
void print_msg(void)
{
	printf("Hello, World!\n");
}

/*
 * Main should never be static.
 */
int main(void)
{
	print_msg();
}
