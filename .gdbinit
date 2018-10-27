target remote localhost:3333
source .macros.gdb
file build/fw/fw.elf
load

monitor reset halt