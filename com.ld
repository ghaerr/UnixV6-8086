OUTPUT_FORMAT(binary)

SECTIONS {
    . = 0x0100;
    .text : {
        *(.text .text.*)
    }

    .data : {
        *(.rodata .rodata.*)
        *(.data .data.*)
    }

    _bdata = ALIGN(2);
    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    }
    _edata = ALIGN(2);

    /DISCARD/ : { *(.comment) }

    _heap = ALIGN(2);

    PROVIDE (_start = DEFINED (entry) ? entry : 0x100);
}
