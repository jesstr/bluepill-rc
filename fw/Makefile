TARGET := fw
BUILDDIR = ../build/$(TARGET)


ifeq ($(PRINTF),)
	PRINTF = custom
endif

ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -g
endif

ifeq ($(SWOTRACE), 1)
	CFLAGS += -DDEBUG
	CFLAGS += -DSWOTRACE
endif

INCLUDES = -Ilibs/stm32f10x_periph/inc 
INCLUDES += -Icore -Iinc -Isrc_fw
INCLUDES += -Imodules/led
INCLUDES += -I../shared
INCLUDES += -Ilibs/TinyStdio

CSRC = $(wildcard *.c core/*.c src/*.c)
CSRC += $(wildcard modules/led/*.c)
CSRC += $(wildcard ../shared/*.c)
CSRC += $(wildcard libs/stm32f10x_periph/src/*.c)
CSRC += $(wildcard libs/TinyStdio/*.c)

CFLAGS += -DPRINTF_$(shell echo $(PRINTF) | tr a-z A-Z)
CFLAGS += -DUSE_STDPERIPH_DRIVER 
CFLAGS += -DSTM32F10X_MD_VL

#no IAP
CFLAGS += -DSTANDALONE

#FreeRTOS
INCLUDES +=  -Ilibs/FreeRTOS/include -Ilibs/FreeRTOS/portable/GCC/ARM_CM3
CSRC += $(wildcard libs/FreeRTOS/*.c libs/FreeRTOS/portable/GCC/ARM_CM3/*.c)
CSRC += $(wildcard libs/FreeRTOS/portable/MemMang/heap_4.c)
CSRC += $(wildcard syscalls/syscalls.c)


LDSCRIPT := core/stm32f1xx.ld

FW_NAME = f103c8

-include ../Makefile.inc

prebuild:
ifeq ($(PRINTF),)
	$(error No printf library selected, please choose one of "stdlib" or "custom") 
endif
	@echo "PRINTF: $(PRINTF)"

postbuild: $(BUILDDIR)/$(TARGET).bin
	@echo "Copying to: $(OUTPUTDIR)/$(FW_NAME).bin"
	@cp $(BUILDDIR)/$(TARGET).bin $(OUTPUTDIR)/$(FW_NAME).bin
