/*
 * GENERATED FILE - DO NOT EDIT
 * Copyright (c) 2008-2013 Code Red Technologies Ltd,
 * Copyright 2015, 2018-2019 NXP
 * (c) NXP Semiconductors 2013-2019
 * Generated linker script file for MIMXRT1052xxxxB
 * Created from linkscript.ldt by FMCreateLinkLibraries
 * Using Freemarker v2.3.23
 * MCUXpresso IDE v11.0.1 [Build 2563] [2019-09-01] on Dec 15, 2019 4:47:31 PM
 */

INCLUDE "evkbimxrt1050_freertos_test_Debug-SDRAM-Flash_library.ld"
INCLUDE "evkbimxrt1050_freertos_test_Debug-SDRAM-Flash_memory.ld"

ENTRY(ResetISR)

SECTIONS
{
     /* Image Vector Table and Boot Data for booting from external flash */
    .boot_hdr : ALIGN(4)
    {
        FILL(0xff)
        __boot_hdr_start__ = ABSOLUTE(.) ;
        KEEP(*(.boot_hdr.conf))
        . = 0x1000 ;
        KEEP(*(.boot_hdr.ivt))
        . = 0x1020 ;
        KEEP(*(.boot_hdr.boot_data))
        . = 0x1030 ;
        KEEP(*(.boot_hdr.dcd_data))
        __boot_hdr_end__ = ABSOLUTE(.) ;
        . = 0x2000 ;
    } > BOARD_SDRAM AT> BOARD_FLASH

    /* MAIN TEXT SECTION */
    .text : ALIGN(4)
    {
        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
        /* Global Section Table */
        . = ALIGN(4) ;
        __section_table_start = .;
        __data_section_table = .;
        LONG((LOADADDR(.data) - __base_BOARD_FLASH) + __base_BOARD_SDRAM);
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        LONG((LOADADDR(.data_RAM2) - __base_BOARD_FLASH) + __base_BOARD_SDRAM);
        LONG(    ADDR(.data_RAM2));
        LONG(  SIZEOF(.data_RAM2));
        LONG((LOADADDR(.data_RAM3) - __base_BOARD_FLASH) + __base_BOARD_SDRAM);
        LONG(    ADDR(.data_RAM3));
        LONG(  SIZEOF(.data_RAM3));
        LONG((LOADADDR(.data_RAM4) - __base_BOARD_FLASH) + __base_BOARD_SDRAM);
        LONG(    ADDR(.data_RAM4));
        LONG(  SIZEOF(.data_RAM4));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        LONG(    ADDR(.bss_RAM2));
        LONG(  SIZEOF(.bss_RAM2));
        LONG(    ADDR(.bss_RAM3));
        LONG(  SIZEOF(.bss_RAM3));
        LONG(    ADDR(.bss_RAM4));
        LONG(  SIZEOF(.bss_RAM4));
        __bss_section_table_end = .;
        __section_table_end = . ;
        /* End of Global Section Table */

        *(.after_vectors*)

    } > BOARD_SDRAM AT> BOARD_FLASH

    .text : ALIGN(4)
    {
       *(.text*)
       *(.rodata .rodata.* .constdata .constdata.*)
       . = ALIGN(4);
    } > BOARD_SDRAM AT> BOARD_FLASH
    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this.
     */
    .ARM.extab : ALIGN(4)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > BOARD_SDRAM AT> BOARD_FLASH

    __exidx_start = .;

    .ARM.exidx : ALIGN(4)
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > BOARD_SDRAM AT> BOARD_FLASH
    __exidx_end = .;

    _etext = .;

    /* DATA section for SRAM_DTC */

    .data_RAM2 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM2 = .) ;
        *(.ramfunc.$RAM2)
        *(.ramfunc.$SRAM_DTC)
        *(.data.$RAM2)
        *(.data.$SRAM_DTC)
        *(.data.$RAM2.*)
        *(.data.$SRAM_DTC.*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM2 = .) ;
     } > SRAM_DTC AT>BOARD_FLASH
    /* DATA section for SRAM_ITC */

    .data_RAM3 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM3 = .) ;
        *(.ramfunc.$RAM3)
        *(.ramfunc.$SRAM_ITC)
        *(.data.$RAM3)
        *(.data.$SRAM_ITC)
        *(.data.$RAM3.*)
        *(.data.$SRAM_ITC.*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM3 = .) ;
     } > SRAM_ITC AT>BOARD_FLASH
    /* DATA section for SRAM_OC */

    .data_RAM4 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM4 = .) ;
        *(.ramfunc.$RAM4)
        *(.ramfunc.$SRAM_OC)
        *(.data.$RAM4)
        *(.data.$SRAM_OC)
        *(.data.$RAM4.*)
        *(.data.$SRAM_OC.*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM4 = .) ;
     } > SRAM_OC AT>BOARD_FLASH
    /* MAIN DATA SECTION */
    .uninit_RESERVED (NOLOAD) :
    {
        . = ALIGN(4) ;
        KEEP(*(.bss.$RESERVED*))
       . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
    } > BOARD_SDRAM

    /* Main DATA section (BOARD_SDRAM) */
    .data : ALIGN_WITH_INPUT
    {
       FILL(0xff)
       _data = . ;
       *(vtable)
       *(.ramfunc*)
       KEEP(*(CodeQuickAccess))
       KEEP(*(DataQuickAccess))
       *(RamFunction)
       *(.data*)
       . = ALIGN(4) ;
       _edata = . ;
    } > BOARD_SDRAM AT>BOARD_FLASH

    /* BSS section for SRAM_DTC */
    .bss_RAM2 :
    {
       . = ALIGN(4) ;
       PROVIDE(__start_bss_RAM2 = .) ;
       *(.bss.$RAM2)
       *(.bss.$SRAM_DTC)
       *(.bss.$RAM2.*)
       *(.bss.$SRAM_DTC.*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM2 = .) ;
    } > SRAM_DTC

    /* BSS section for SRAM_ITC */
    .bss_RAM3 :
    {
       . = ALIGN(4) ;
       PROVIDE(__start_bss_RAM3 = .) ;
       *(.bss.$RAM3)
       *(.bss.$SRAM_ITC)
       *(.bss.$RAM3.*)
       *(.bss.$SRAM_ITC.*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM3 = .) ;
    } > SRAM_ITC

    /* BSS section for SRAM_OC */
    .bss_RAM4 :
    {
       . = ALIGN(4) ;
       PROVIDE(__start_bss_RAM4 = .) ;
       *(.bss.$RAM4)
       *(.bss.$SRAM_OC)
       *(.bss.$RAM4.*)
       *(.bss.$SRAM_OC.*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM4 = .) ;
    } > SRAM_OC

    /* MAIN BSS SECTION */
    .bss :
    {
        . = ALIGN(4) ;
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
    } > BOARD_SDRAM

    /* NOINIT section for SRAM_DTC */
    .noinit_RAM2 (NOLOAD) :
    {
       . = ALIGN(4) ;
       *(.noinit.$RAM2)
       *(.noinit.$SRAM_DTC)
       *(.noinit.$RAM2.*)
       *(.noinit.$SRAM_DTC.*)
       . = ALIGN(4) ;
    } > SRAM_DTC

    /* NOINIT section for SRAM_ITC */
    .noinit_RAM3 (NOLOAD) :
    {
       . = ALIGN(4) ;
       *(.noinit.$RAM3)
       *(.noinit.$SRAM_ITC)
       *(.noinit.$RAM3.*)
       *(.noinit.$SRAM_ITC.*)
       . = ALIGN(4) ;
    } > SRAM_ITC

    /* NOINIT section for SRAM_OC */
    .noinit_RAM4 (NOLOAD) :
    {
       . = ALIGN(4) ;
       *(.noinit.$RAM4)
       *(.noinit.$SRAM_OC)
       *(.noinit.$RAM4.*)
       *(.noinit.$SRAM_OC.*)
       . = ALIGN(4) ;
    } > SRAM_OC

    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD):
    {
         . = ALIGN(4) ;
        _noinit = .;
        *(.noinit*)
         . = ALIGN(4) ;
        _end_noinit = .;
    } > BOARD_SDRAM

    /* Reserve and place Heap within memory map */
    _HeapSize = 0x200000;
    .heap :  ALIGN(4)
    {
        _pvHeapStart = .;
        . += _HeapSize;
        . = ALIGN(4);
        _pvHeapLimit = .;
    } > BOARD_SDRAM

     _StackSize = 0x1000;
     /* Reserve space in memory for Stack */
    .heap2stackfill  :
    {
        . += _StackSize;
    } > BOARD_SDRAM
    /* Locate actual Stack in memory map */
    .stack ORIGIN(BOARD_SDRAM) + LENGTH(BOARD_SDRAM) - _StackSize - 0:  ALIGN(4)
    {
        _vStackBase = .;
        . = ALIGN(4);
        _vStackTop = . + _StackSize;
    } > BOARD_SDRAM

    /* Provide basic symbols giving location and size of main text
     * block, including initial values of RW data sections. Note that
     * these will need extending to give a complete picture with
     * complex images (e.g multiple Flash banks).
     */
    _image_start = LOADADDR(.boot_hdr);
    _image_end = LOADADDR(.data) + SIZEOF(.data);
    _image_size = _image_end - _image_start;
}