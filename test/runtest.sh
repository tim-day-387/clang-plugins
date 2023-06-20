# SPDX-License-Identifier: GPL-2.0

#
# Run tests for FindStatic
#

root="$(pwd)"

clang -fplugin=./FindStatic.so ./test/user-test/test.c
rm -f ./a.out

cd ./test/kernel-test/
make CC="clang -fplugin=$root/FindStatic.so"
make clean
