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

#STM32 Standard Peripheral Library
SPL_PATH = libs/STM32F10x_StdPeriph_Lib_V3.5.0/Library/

INCLUDES = -I$(SPL_PATH)/STM32F10x_StdPeriph_Driver/inc
INCLUDES += -I$(SPL_PATH)/CMSIS/CM3/CoreSupport
INCLUDES += -I$(SPL_PATH)/CMSIS/CM3/DeviceSupport/ST/STM32F10x

ASMSRC = $(SPL_PATH)/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/TrueSTUDIO/startup_stm32f10x_md_vl.s
CSRC = $(wildcard $(SPL_PATH)/STM32F10x_StdPeriph_Driver/src/*.c)
CSRC += $(wildcard $(SPL_PATH)/CMSIS/CM3/DeviceSupport/ST/STM32F10x/*.c)

#Application
INCLUDES += -Iconf
INCLUDES += -Iinc
INCLUDES += -Isrc_fw
INCLUDES += -Imodules/led
INCLUDES += -I../shared
INCLUDES += -Ilibs/TinyStdio

CSRC += $(wildcard src/*.c)
CSRC += $(wildcard modules/led/*.c)
CSRC += $(wildcard ../shared/*.c)
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


LDSCRIPT := conf/stm32f1xx.ld

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
