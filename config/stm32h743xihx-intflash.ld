ENTRY(Reset_Handler)
HEAP_SIZE = 0x1000000; /* 8MB */
GRAM_SIZE = 0x600000; /* 6MB */
TTY_SIZE = 0x1000;  /* 4KB must be powers of 2*/
_estack = ORIGIN(int_dtcm) + LENGTH(int_dtcm);
_sext_sdram = ORIGIN(ext_sdram);
_eext_sdram = _sext_sdram + LENGTH(ext_sdram);
MEMORY
{
    int_itcm (rwx)          : ORIGIN = 0x00000000, LENGTH = 64K
    int_flash (rx)          : ORIGIN = 0x08000000, LENGTH = 2048K
    int_dtcm (rwx)          : ORIGIN = 0x20000000, LENGTH = 128K
    int_sram_d1 (rwx)       : ORIGIN = 0x24000000, LENGTH = 512K
    int_sram_d2 (rwx)       : ORIGIN = 0x30000000, LENGTH = 288K
    int_sram_d3 (rwx)       : ORIGIN = 0x38000000, LENGTH = 64K
    ext_sdram (rwx)         : ORIGIN = 0xc0000000, LENGTH = 32M
}

/* Define output sections */
SECTIONS
{
    .text :
    {
        /* vector */
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);

        /* text */
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.glue_7)
        *(.glue_7t)
        *(.eh_frame)
        KEEP (*(.init))
        KEEP (*(.fini))
        . = ALIGN(4);
        _etext = .;

        /* rodata */
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);

        /* arm */
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;

        /* newlib */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);

        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    } > int_flash

    _sidata_int_dtcm = LOADADDR(.data.int_dtcm);

    .data.int_dtcm :
    {
        /* got */
        . = ALIGN(4);
        PROVIDE_HIDDEN (_sdata_int_dtcm = .);
        *(.got)
        *(.got.plt)

        /* data & tls data before ext_sdram initialization */
        . = ALIGN(4);
        *libcmsis.a:(.data .data* .tdata .tdata*)
        *libhal.a:stm32h7xx_hal.o(.data .data* .tdata .tdata*)
        *libbsp.a:bsp.o(.data .data* .tdata .tdata*)
        *libbsp.a:bsp_utils.o(.data .data* .tdata .tdata*)
        *libbsp.a:bsp_debug.o(.data .data* .tdata .tdata*)
        *libbsp.a:bsp_sdram.o(.data .data* .tdata .tdata*)
        *libbsp.a:bsp_module.o(.data .data* .tdata .tdata*)
        *libc.a:(.data .data* .tdata .tdata*)
        *libm.a:(.data .data* .tdata .tdata*)
        *libgcc.a:(.data .data* .tdata .tdata*)
        *syscall.o(.data .data* .tdata .tdata*)

        PROVIDE_HIDDEN ( _edata_int_dtcm = .);
    } > int_dtcm AT > int_flash

    _sidata_ext_sdram = LOADADDR(.data.ext_sdram);
    .data.ext_sdram :
    {
        /* data */
        . = ALIGN(4);
        PROVIDE_HIDDEN (_sdata_ext_sdram = .);
        *(.data)
        *(.data*)

        /* tls data */
        . = ALIGN(4);
        *(.tdata)
        *(.tdata*)
        PROVIDE_HIDDEN (_edata_ext_sdram = .);
    } > ext_sdram AT > int_flash

    .bss.int_dtcm :
    {
        /* bss & tls bss before ext_sdram initialization */
        . = ALIGN(4);
        PROVIDE_HIDDEN (_sbss_int_dtcm = .);

        *libcmsis.a:(.bss .bss* COMMON .tbss)
        *libhal.a:stm32h7xx_hal.o(.bss .bss* COMMON .tbss)
        *libbsp.a:bsp.o(.bss .bss* COMMON .tbss)
        *libbsp.a:bsp_utils.o(.bss .bss* COMMON .tbss)
        *libbsp.a:bsp_debug.o(.bss .bss* COMMON .tbss)
        *libbsp.a:bsp_sdram.o(.bss .bss* COMMON .tbss)
        *libbsp.a:bsp_module.o(.bss .bss* COMMON .tbss)
        *libc.a:(.bss .bss* COMMON .tbss)
        *libgcc.a:(.bss .bss* COMMON .tbss)
        *syscall.o(.bss .bss* COMMON .tbss)

        PROVIDE_HIDDEN (_ebss_int_dtcm = .);
    } > int_dtcm

    .tty.int_dtcm :
    {
        /* used for uart dma cache */
        . = ALIGN(16);
        PROVIDE_HIDDEN (_stty_int_dtcm = .);
        . += TTY_SIZE;
        PROVIDE_HIDDEN (_etty_int_dtcm = .);
    } > int_dtcm

    .bss.ext_sdram :
    {
        /* bss */
        . = ALIGN(4);
        PROVIDE_HIDDEN (_sbss_ext_sdram = .);
        *(.bss)
        *(.bss*) 
        *(COMMON)

        /* tls bss */
        . = ALIGN(4);
        *(.tbss)
        *(.tbss*)
        PROVIDE_HIDDEN (_ebss_ext_sdram = .);
    } > ext_sdram

    .heap.ext_sdram :
    {
        /* heap used by the newlib memory management */
        . = ALIGN(16);
        PROVIDE_HIDDEN (_sheap_ext_sdram = .);
        . += HEAP_SIZE;
        PROVIDE_HIDDEN (_eheap_ext_sdram = .);
    } > ext_sdram

    .gram.ext_sdram :
    {
        . = ALIGN(128);
        PROVIDE_HIDDEN (_sgram0_ext_sdram = .);
        . += GRAM_SIZE / 2;
        PROVIDE_HIDDEN (_egram0_ext_sdram = .);
        . = ALIGN(128);
        PROVIDE_HIDDEN (_sgram1_ext_sdram = .);
        . += GRAM_SIZE / 2;
        PROVIDE_HIDDEN (_egram1_ext_sdram = .);
    } > ext_sdram

    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    .ARM.attributes 0 : { *(.ARM.attributes) }
}


