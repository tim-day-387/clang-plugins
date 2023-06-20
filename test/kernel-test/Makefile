# SPDX-License-Identifier: GPL-2.0

#
# Makefile for test kernel module.
#

ifneq ($(KERNELRELEASE),)
	obj-m := test.o

else

	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

endif
