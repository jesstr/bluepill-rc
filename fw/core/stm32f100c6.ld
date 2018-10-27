
/*------------------------------------------------------------------------*/

             /* STM32F100VC */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 128K 
    CRC   (rx) : ORIGIN = 0x0801FFFC, LENGTH = 4
    RAM (rw)   : ORIGIN = 0x20000000, LENGTH = 8K
}

/* Place the stack at the end of memory, unless specified otherwise. */
PROVIDE (__stack_end__ = 0x20000000 + 8k);

SECTIONS
{
  .text :
  {
    KEEP(*(.vectors));        /* Startup code */
    *(.text .text.* .gnu.linkonce.t.*)
    *(.plt)
    *(.gnu.warning)
    *(.glue_7t) *(.glue_7)

    . = ALIGN(4);
    /* These are for running static constructors and destructors under ELF.  */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*crtend.o(.ctors))
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*crtend.o(.dtors))

    *(.rodata .rodata.* .gnu.linkonce.r.*)
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    *(.gcc_except_table)
    *(.eh_frame_hdr)
    *(.eh_frame)

    *(.init)
    *(.fini)

    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(.fini_array))
    KEEP (*(SORT(.fini_array.*)))
    PROVIDE_HIDDEN (__fini_array_end = .);

   . = ALIGN(4);
  } > FLASH

  .ARM.exidx : 
  {
	__exidx_start = .;
	*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	__exidx_end = .;
  } >FLASH

   _etext = . ;
   PROVIDE (etext = .);

   /* .data section which is used for initialized data */
   .data : AT (_etext)
   {
	. = ALIGN(4);
	_data = .;
	*(.data)
	*(.data*)
	. = ALIGN(4);
	_edata = .;
	PROVIDE (edata = .);
   } > RAM

 .fill LOADADDR(.data) + SIZEOF(.data) :
  {
    FILL(0xFFFFFFFF);
    . = ORIGIN(CRC) - 1;
    BYTE(0xFF)
  } > FLASH
  
  .crc :
  {
   . = ALIGN(4);
   KEEP(*(.crc))
   . = ALIGN(4);
  }>CRC

   /* .bss section which is used for uninitialized data */
   .bss (NOLOAD) :
   {
     __bss_start = . ;
     __bss_start__ = . ;
     *(.bss)
     . = ALIGN(4);
     *(.bss.*)
     . = ALIGN(4);
     *(COMMON)
     . = ALIGN(4);
   } > RAM

   . = ALIGN(4);
   __bss_end__ = . ;
   PROVIDE (__bss_end = .);
  _end = . ;
  PROVIDE (end = .);

  .stab 0 (NOLOAD) :
  {
    *(.stab)
  }

  .stabstr 0 (NOLOAD) :
  {
    *(.stabstr)
  }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
  .ARM.attributes 0 : { KEEP (*(.ARM.attributes)) }
  /DISCARD/ : { *(.note.GNU-stack)  }
}

