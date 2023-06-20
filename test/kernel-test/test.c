// SPDX-License-Identifier: GPL-2.0

/*
 * Simple test case for FindStatic
 *
 */

#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

/*
 * This function should not be static.
 */
void foobar(void)
{
	printk("test");
}
EXPORT_SYMBOL(foobar);

/*
 * This function should be static.
 */
int test_init(void)
{
	foobar();
	return 0;
}

/*
 * This function is static.
 */
static void test_exit(void)
{
}

module_init(test_init);
module_exit(test_exit);
