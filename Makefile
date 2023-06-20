# SPDX-License-Identifier: GPL-2.0

#
# Makefile for Clang plugins.
#

CPPFLAGS += -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
CXXFLAGS += -fPIC -O -Wall -Werror -g3 -v -I/usr/lib/llvm-11/include/
CXX = clang

.PHONY: all clean
all: FindStatic.so

FindStatic.so: FindStatic.o
	$(CXX) -shared $^ -I/usr/lib/llvm -lclang -o $@

clean:
	rm *.o *.so
