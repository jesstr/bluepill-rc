OUTPUTDIR = $(BUILDDIR)/../../output

DEPDIR = $(BUILDDIR)/.dep
PWD = $(shell pwd)

COMMONFLAGS = -mcpu=cortex-m3 -mthumb -ggdb3
CFLAGS += $(COMMONFLAGS) -Os $(INCLUDES) -I.
CFLAGS += -std=c99 -Wall -Wextra -static -fdata-sections -ffunction-sections -fno-hosted -fno-builtin
CFLAGS += -nostdlib -nodefaultlibs
CFLAGS += -mapcs-frame -msoft-float
CFLAGS += -Werror-implicit-function-declaration -Werror
CFLAGS += -MD -MP -MF $(DEPDIR)/$(@F).d
LDFLAGS = $(COMMONFLAGS) -static
LDFLAGS += -fno-exceptions -ffunction-sections -fdata-sections
LDFLAGS += -static -Xlinker --gc-sections
LDFLAGS += -Wl,-Map=$(BUILDDIR)/$(TARGET).map,--cref
ASFLAGS = $(COMMONFLAGS)

LIBFLAGS = -Wno-unused-but-set-variable 
LIBFLAGS += -Wno-char-subscripts
LIBFLAGS += -Wno-sign-compare
LIBFLAGS += -Wno-unused-parameter
LIBFLAGS += -Wno-strict-aliasing
LIBFLAGS += -Wno-pointer-sign
LIBFLAGS += -Wno-address
LIBFLAGS += -Wno-format

CROSS = arm-none-eabi
GCC = $(CROSS)-gcc
AS = $(CROSS)-as
SIZE = $(CROSS)-size
OBJCOPY = $(CROSS)-objcopy
OBJDUMP = $(CROSS)-objdump
NM = $(CROSS)-nm

COBJ = $(addprefix $(BUILDDIR)/, $(CSRC:.c=.c.o))
LIBOBJ = $(addprefix $(BUILDDIR)/, $(LIBSRC:.c=.c.o))
ASMOBJ = $(addprefix $(BUILDDIR)/, $(ASMSRC:.s=.s.o))
OBJ = $(COBJ) $(ASMOBJ)

V = $(if $(VERBOSE), , @)

RAM_SIZE_KB = 20
FLASH_SIZE_KB = (64 - 1)

all: prebuild $(BUILDDIR)/$(TARGET).bin postbuild
	@$(SIZE) $(BUILDDIR)/$(TARGET).elf
	@$(SIZE) $(BUILDDIR)/$(TARGET).elf -A | awk '/.data/ || /.bss/ {sum+=$$2} \
		END{per=sum/$(RAM_SIZE_KB)/1024*100; \
		print "RAM: "  per"% ", sum " bytes (" $(RAM_SIZE_KB)*1024-sum " bytes free)"}'
	@$(SIZE) $(BUILDDIR)/$(TARGET).elf -A | awk '/.fill/ {fill=$$2} \
		END{per=($(FLASH_SIZE_KB)*1024-fill)/$(FLASH_SIZE_KB)/1024*100; \
		print "FLASH: "  per"% ", $(FLASH_SIZE_KB)*1024-fill " bytes (" fill " bytes free)"}'
	@echo =======================================================================

$(BUILDDIR)/$(TARGET).bin: $(BUILDDIR)/$(TARGET).elf
	@$(OBJCOPY) -O binary $(BUILDDIR)/$(TARGET).elf $(BUILDDIR)/$(TARGET).bin
	@$(OBJCOPY) -O ihex $(BUILDDIR)/$(TARGET).elf $(BUILDDIR)/$(TARGET).hex
	@$(OBJDUMP) -h -S -z $(BUILDDIR)/$(TARGET).elf > $(BUILDDIR)/$(TARGET).lss
	@$(NM) -n $(BUILDDIR)/$(TARGET).elf > $(BUILDDIR)/$(TARGET).sym
	@mkdir -p $(OUTPUTDIR)
	@cp $(BUILDDIR)/$(TARGET).bin $(OUTPUTDIR)
	
$(BUILDDIR)/$(TARGET).elf: $(OBJ) $(LIBOBJ)
	@echo Linking $@
	$(V)$(GCC) $(LDFLAGS) -T $(PWD)/$(LDSCRIPT) -o $@ $(OBJ) $(LIBOBJ)

$(COBJ): $(BUILDDIR)/%.c.o : %.c
	@echo Compiling $<
	@-mkdir -p $(@D)
	$(V)$(GCC) $(CFLAGS) -c $< -o $@
	
$(LIBOBJ): $(BUILDDIR)/%.c.o : %.c
	@echo Compiling $<
	@-mkdir -p $(@D)
	$(V)$(GCC) $(CFLAGS) $(LIBFLAGS) -c $< -o $@

$(ASMOBJ): $(BUILDDIR)/%.s.o : %.s
	@echo Assembling $<
	@-mkdir -p $(@D)
	$(V)$(AS) $(ASFLAGS) -c ./$< -o $@


-include $(shell mkdir -p $(DEPDIR) 2>/dev/null) $(wildcard $(DEPDIR)/*)

.PHONY: clean output

clean:
	rm -rf $(BUILDDIR)
