#
# Makefile for the linux kernel.
#

BUILDDIR:=$(PWD)
KERNELSRCDIR=$(BUILDDIR)/../../../..
VERBOSE = 0
ARCH=arm
CROSS_COMPILE=/android/android-cm-g/prebuilt/linux-x86/toolchain/arm-eabi-4.4.0/bin/arm-eabi-
EXTRA_CFLAGS=-I $(PWD)/include

obj-m		:= splboot.o

splboot-objs := splload.o proc-v7.o tlb-v7.o cache-v7.o abort-ev7.o mmu.o

all:
	make -C $(KERNELSRCDIR) ARCH=arm CROSS_COMPILE=/android/android-cm-g/prebuilt/linux-x86/toolchain/arm-eabi-4.4.0/bin/arm-eabi- SUBDIRS=$(BUILDDIR) KBUILD_VERBOSE=$(VERBOSE) modules

clean:

