#
# Copyright 2011 MOSTiTECH co., ltd.
#
# Makefile for the Nexell platform 

obj-y := device.o irq.o timer.o board.o

#obj-y += adc.o
obj-y += dma.o
#obj-y += dpc.o
obj-y += gpio.o
obj-y += init.o
#obj-y += pwm.o
obj-y += wdt.o

obj-y += nx_module/

KBUILD_CFLAGS += -D__LINUX__ -D__PRINTK__ -DNX_RELEASE

