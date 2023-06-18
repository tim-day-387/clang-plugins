// SPDX-License-Identifier: GPL-2.0

/*
 * Simple test case for FindStatic
 *
 */

void print_msg_2(void);

/*
 * This function should not be static.
 */
extern void print_msg_5(void) {
	printf("Hello, World!\n");
}
