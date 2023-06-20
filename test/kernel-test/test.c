// SPDX-License-Identifier: GPL-2.0

/*
 * Simple test case for FindStatic
 *
 */

#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

static int test_init(void)
{
	return 0;
}

static void test_exit(void)
{
}

module_init(test_init);
module_exit(test_exit);
