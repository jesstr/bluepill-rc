TARGETS = fw
BUILDDIR = build

.PHONY: all $(TARGETS) clean distclean help

all: $(TARGETS)

help:
	@echo "Targets:"
	@echo "all:			a firmware for battery symmetry control block"
	@echo "distclean:			a firmware for battery symmetry control block"
	@echo
	@echo "Variables:"
	@echo
	@echo "	DEBUG			enable debug output to USART1 in all firmware parts (needs 'make distclean')"
	@echo "	VERBOSE			switch on verbose output during compiling"
	@echo "	SWOTRACE		enable SWO trace output on PB3"

$(TARGETS):
	@$(MAKE) -C $@

clean: $(TARGETS)

distclean:
	rm -rf build output

openocd:
	$(shell openocd -f configs/stm321xx.cfg)

trace:
	$(shell swo-tracer -t tracefile.bin >&2)

logtrace:
	$(shell swo-tracer -t tracefile.bin | tee trace.txt >&2)

reset:
	st-flash --reset read /dev/null 0x8000000 1

erase:
	st-flash --reset erase

fwflash:	
	st-flash --reset write output/fw.bin 0x8000000
