// C startup code for ia16-elf-gcc programs

        .arch   i8086, nojumps
        .code16
        .text

        .data
        .word   0           // don't allow legal NULL ptr
        .global errno,r0,r1,r3
        .comm   errno,2
        .comm   r0,2
        .comm   r1,2
        .comm   r3,2

        .text
        .global _start
        .extern main
_start: jmp     1f
        .byte   'F', 'a', 'n', 'g'
        jmp    _callsig     // hardcoded in ken/sig.c to be address 6

1:      call    main
        mov     $1,%dx      // SYS_exit
        int     $0x81

_callsig:
        call *%si           // call signal handler
        pop %ax             // don't touch ds
        pop %ax             // don't touch es
        pop %dx
        pop %cx
        pop %bx
        pop %ax
        pop %di
        pop %si
        pop %bp
        pop %ax             // ip, cs, flag = ax, flag, return address
        popf
        ret

        .global syscall
syscall:
        pop %cx             // return address
        pop %dx             // syscall number
        pop %ax             // r0
        int $0x81
        or %dx,%dx
        jz 1f               // no error
        mov %dx,errno
        mov $-1,%dx
1:      mov %ax,r0
        mov %bx,r1
        mov %dx,r3
        sub $4,%sp
        push %cx
        ret
