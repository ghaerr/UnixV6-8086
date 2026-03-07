// init code for ken/main.c
// to see produced binary, use:
//  ia16-elf-gcc -melks-libc -nostdlib boots.s
//  disasm a.out
//
        .code16
        .text

        .global _start
_start: mov $argv,%ax
        push %ax
        mov $argv0,%ax
        push %ax
        mov $11,%dx
        int $0x81

        .data
        .word   0
argv0:  .asciz  "init"
argv:   .word   argv0
        .word   0
