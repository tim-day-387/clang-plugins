# SPDX-License-Identifier: GPL-2.0

#
# Makefile for Clang plugins.
#

CPPFLAGS += -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
CXXFLAGS += -fPIC -O -Wall -Werror -g3 -v -I/usr/lib/llvm-11/include/

.PHONY: all clean
all: PrintFunctionNames.so

PrintFunctionNames.so: PrintFunctionNames.o
	$(CXX) -shared $^ -I/usr/lib/llvm-11 -lclang -o $@
