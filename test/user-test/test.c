// SPDX-License-Identifier: GPL-2.0

/*
 * Simple test case for FindStatic
 *
 */

#include <stdio.h>

#include "test.h"

static void print_msg_4(void);

/*
 * This function should be static.
 */
void print_msg(void)
{
	printf("Hello, World!\n");
}

/*
 * This function should not be static.
 */
void print_msg_2(void)
{
	printf("Hello, World!\n");
}

/*
 * This function is static.
 */
static void print_msg_3(void)
{
	printf("Hello, World!\n");
}

/*
 * This function is static.
 */
static void print_msg_4(void)
{
	printf("Hello, World!\n");
}

/*
 * Main should never be static.
 */
int main(void)
{
	print_msg();
	print_msg_2();
	print_msg_3();
	print_msg_4();
}
