OUTPUT_ARCH( "riscv" )
ENTRY(_entry)


MEMORY
{
    ROM (rx) : ORIGIN = 0x8000, LENGTH = 32k
    RAM (rwx) : ORIGIN = 0x4000, LENGTH = 4k
}

RAM_START_ADDRESS   = ORIGIN(RAM);       /* Must be the same value MEMORY region RAM ORIGIN above. */
RAM_SIZE            = LENGTH(RAM);              /* Must be the same value MEMORY region RAM LENGTH above. */

__stack = ORIGIN(RAM) + LENGTH(RAM);
__stack_size = DEFINED(__stack_size) ? __stack_size : 2K;

SECTIONS
{
  .text : ALIGN(0x10)
  {
  	__envm_start = .;
    KEEP(*(.vectors))
    KEEP (*(SORT_NONE(.text.entry)))   
    . = ALIGN(0x10);
    *(.text .text.* .gnu.linkonce.t.*)
    *(.plt)
    . = ALIGN(0x10);
    
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*crtend.o(.ctors))
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*crtend.o(.dtors))
    
    *(.rodata .rodata.* .gnu.linkonce.r.*)
    *(.gcc_except_table) 
    *(.eh_frame_hdr)
    *(.eh_frame)
    
    KEEP (*(.init))
    KEEP (*(.fini))

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
    . = ALIGN(0x10);
    
  } >ROM

    .data : ALIGN(4)
    {
        FILL(0xFF)
        
        __data_start__ = . ;            /* Standard newlib definition. */
        __data_begin__ = . ;            /* µOS++ specific */
        *(.data_begin .data_begin.*)    /* µOS++ __data_begin_guard */

        *(.data .data.*)
        *(.gnu.linkonce.d.*)
        
        . = ALIGN(8);
        /* 
         * RISC-V specific; the compiler optimises memory accesses
         * in the +/- 2K range around __global_pointer$ to GP relative.
         * For this to work, GP must be loaded during startup with the
         * address of __global_pointer$.
         * This optimisation favours a 4K range. Newlib places
         * several impure and malloc pointers in the .sdata section.
         */
        PROVIDE( __global_pointer$ = . + (4K / 2) );
        
        *(.sdata .sdata.*)
        *(.gnu.linkonce.s.*)

        /* RISC-V specific; not sure if needed. */
        . = ALIGN(8);
        *(.srodata.cst16)
        *(.srodata.cst8)
        *(.srodata.cst4)
        *(.srodata.cst2)
        *(.srodata .srodata.*)
        
        *(.data_end .data_end.*)        /* µOS++ __data_end_guard; must be last */        
        . = ALIGN(4);
        __data_end__ = . ;                /* Standard newlib definition. */
    } >RAM AT>ROM

    /* 
     * This address is used by the µOS++ startup code to 
     * initialise the .data section.
     */
    __data_load_addr__ = LOADADDR(.data);

      . = ALIGN(4);
      PROVIDE( __edata = . );
      PROVIDE( _edata = . );
      PROVIDE( edata = . );



    /* The primary uninitialised data section. */
    .bss (NOLOAD) : ALIGN(4)
    {
        __bss_start = .;                 /* Standard newlib definition. */
        __bss_start__ = .;                 /* Standard newlib definition. */
           __bss_begin__ = .;                 /* µOS++ specific */
         *(.bss_begin .bss_begin.*)     /* µOS++ __bss_begin_guard */ 

        *(.sbss .sbss.*)
        *(.gnu.linkonce.sb.*)
        
        *(.bss .bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)
        
        *(.bss_end .bss_end.*)             /* µOS++ __bss_end_guard; must be last */
        . = ALIGN(4);
        __bss_end__ = .;                /* Standard newlib definition. */
        __bss_end = .;                    /* Standard newlib definition. */
     } >RAM AT>RAM


   /* 
     * Similar to .bss, but not initialised to zero. µOS++ extension.
     */
    .noinit (NOLOAD) : ALIGN(4)
    {
        __noinit_begin__ = .;            /* µOS++ extension. */
        
        *(.noinit .noinit.*) 
        
         . = ALIGN(4) ;
        __noinit_end__ = .;               /* µOS++ extension. */
    } >RAM AT>RAM

    
    /* _sbrk() expects at least word alignment. */
      . = ALIGN(8);
    PROVIDE( __end = . );
    PROVIDE( _end = . );
    PROVIDE( end = . ); 
    
    PROVIDE( __heap_begin__ = . );        /* µOS++ extension. */
    
      .stack __stack - __stack_size :
      {
        PROVIDE( _heap_end = . );        /* Standard newlib definition. */
        PROVIDE( __heap_end__ = . );    /* µOS++ extension. */
        . += __stack_size;
      } >RAM AT>RAM

  /* Remove information from the compiler libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

}