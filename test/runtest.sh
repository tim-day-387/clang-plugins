# SPDX-License-Identifier: GPL-2.0

#
# Run tests for FindStatic
#

clang-11 -fplugin=./FindStatic.so ./test/user-test/test.c
rm -f ./a.out
