    .arch   i8086, nojumps
    .code16 
    .text
    .globl clock_isr, trap_isr, getps, setps, save, use_resume_stack, do_resume
    .globl move_to_user_mode, ide_isr, kbd_isr, uart_isr, common_isr
    .globl fmemcpy, fmemset
    .globl rdbyte, rdword, wrbyte, wrword
    .globl outport, outportb, inport, inportb, idle
    .globl bios_getc, bios_putc
    .extern main
    .extern isr_savuar, isr_router, clock, check_runrun
    .extern trap0, trap
    .data
    .extern u, resume_SI
    .extern _edata, _end

    .text
    .globl _start
_start:                 // org 0x100
     mov %cs, %ax
     mov %ax, %ds
     mov %ax, %es
     mov $u, %ax
     add $1020, %ax
     mov %ax, %sp

// Reset uninitialized data area
     xor %ax, %ax
     mov $_edata, %di
     mov $_end, %cx
     sub %di, %cx
     cld
     rep stosb

     call main
     jmp .

#define ENTERISR        \
     push %bp;          \
     push %si;          \
     push %di;          \
     push %ax;          \
     push %bx;          \
     push %cx;          \
     push %dx;          \
     push %es;          \
     push %ds;          \
     mov %cs, %ax;      \
     mov %ax, %ds

#define EXITISR         \
     pop %ds;           \
     pop %es;           \
     pop %dx;           \
     pop %cx;           \
     pop %bx;           \
     pop %ax;           \
     pop %di;           \
     pop %si;           \
     pop %bp;           \
     iret

#define SWITCHTOKERNELSTACK \
     mov %sp, %cx;          \
     mov %ss, %dx;          \
     mov %cs, %ax;          \
     mov %ax, %ss;          \
     mov $u, %ax;           \
     add $1024, %ax;        \
     mov %ax, %sp;          \
     push %dx;              \
     push %cx

#define SWITCHTOUSERSTACK   \
     cli;                   \
     pop %cx;               \
     pop %dx;               \
     mov %cx, %sp;          \
     mov %dx, %ss

clock_isr:
     ENTERISR
     xor %di, %di
     jmp common_isr

ide_isr:
     ENTERISR
     mov $0x20, %al     // send EOI to slave PIC
     out %al, $0xa0
     mov $1, %di
     jmp common_isr

kbd_isr:
     ENTERISR
     mov $2, %di
     jmp common_isr

uart_isr:
     ENTERISR
     mov $3, %di
     jmp common_isr

common_isr:
     mov $0x20, %al      // send EOI
     out %al, $0x20
     mov %cs, %ax
     mov %ss, %si
     sub %ax, %si
     jz 1f              // ss == cs intr in kernel mode
     call isr_savuar
     SWITCHTOKERNELSTACK
1:
     push %si
     push %di
     call isr_router
     pop %di
     pop %si
     or %si, %si
     jz 1f
     call check_runrun
     SWITCHTOUSERSTACK
1:
     EXITISR

trap_isr:
     ENTERISR
     call trap0
     SWITCHTOKERNELSTACK
     sti
     call trap
     call check_runrun
     SWITCHTOUSERSTACK
     EXITISR

getps:
     pushf
     pop %ax
     ret

setps:
     push %bp
     mov %sp, %bp
     mov 4(%bp), %ax
     pop %bp
     and $0x200, %ax
     jz 1f
     sti
     ret
1:
     cli
     ret

save:
     push %bp
     mov %sp, %bp
     push %si
     push %di
     mov $1, %ax
     push %ax
     push %bx
     push %cx
     push %dx
     push %es
     push %ds
     mov %ds, %ax
     mov %ax, %es
     mov 4(%bp), %di
     mov %sp, %si
     mov $10, %cx       // copies DS,ES,DX,DX,BX,AX,DI,SI,BP,IP
     cld
     rep movsw
     mov 4(%bp), %bx
     mov %cs, 20(%bx)
     pushf
     pop %ax
     mov %ax, 22(%bx)
     mov %ss, 24(%bx)
     mov %bp, %ax
     add $4, %ax
     mov %ax, 26(%bx)
     pop %ds
     pop %es
     pop %dx
     pop %cx
     pop %bx
     pop %ax
     pop %di
     pop %si
     pop %bp
     xor %ax, %ax
     ret

use_resume_stack:
     cli

     push %si
     push %di
     push %es

     push %ds
     pop %es
     mov $resume_stack+256, %di
     mov %sp, %si
     mov $16, %cx
     cld
     rep movsw
     mov %bp, %ax               // AX = BP
     sub %sp, %ax               // AX = (BP - SP) = positive difference BP is above SP
     mov $resume_stack+256, %sp
     add %sp, %ax               // AX = (BP - SP) + NEWSP
     mov %ax, %bp               // BP points to args in resume stack frame

     pop %es
     pop %di
     pop %si
     ret

do_resume:
     mov resume_SI, %bx
     mov 24(%bx), %es       // ((struct ctx *)resume_SI)->ss
     mov 26(%bx), %di
     sub $24, %di           // ((struct ctx *)resume_SI)->sp - 24
     mov %bx, %si
     mov $12, %cx
     cld
     rep movsw
     mov %es, %ax
     mov %ax, %ss
     sub $24, %di
     mov %di, %sp
     pop %ds
     pop %es
     pop %dx
     pop %cx
     pop %bx
     pop %ax
     pop %di
     pop %si
     pop %bp
     iret

move_to_user_mode:
     mov %sp, %bp
     mov 2(%bp), %dx
     cli
     mov %dx, %ss
     mov $0xFC00, %sp       // USTACK
     sti
     mov %dx, %ds
     add $0x1000, %dx       // CS = DSIZE * (PAGESIZ/16)
     push %dx
     xor %ax, %ax
     push %ax
     retf


fmemcpy:
     push %bp
     mov %sp, %bp
     push %si
     push %di
     push %ds
     push %es
     les 4(%bp), %di
     lds 8(%bp), %si
     mov 12(%bp), %cx
     shr $1, %cx
     cld
     rep movsw
     jnc 1f
     movsb
1:
     pop %es
     pop %ds
     pop %di
     pop %si
     pop %bp
     ret

fmemset:
     push %bp
     mov %sp, %bp
     push %di
     push %es
     les 4(%bp), %di
     mov 8(%bp), %al
     mov %al, %ah
     mov 10(%bp), %cx
     shr $1, %cx
     cld
     rep stosw
     jnc 1f
     stosb
1:
     pop %es
     pop %di
     pop %bp
     ret

rdbyte:
     mov %ds, %dx
     mov %bx, %cx
     mov %sp, %bx
     lds 2(%bx), %bx
     mov (%bx), %al
     xor %ah, %ah
     mov %cx, %bx
     mov %dx, %ds
     ret

rdword:
     mov %ds, %dx
     mov %bx, %cx
     mov %sp, %bx
     lds 2(%bx), %bx
     mov (%bx), %ax
     mov %cx, %bx
     mov %dx, %ds
     ret

wrbyte:
     mov %ds, %dx
     mov %bx, %cx
     mov %sp, %bx
     mov %ss:6(%bx), %ax
     lds %ss:2(%bx), %bx
     mov %al, (%bx)
     mov %cx, %bx
     mov %dx, %ds
     ret

wrword:
     mov %ds, %dx
     mov %bx, %cx
     mov %sp, %bx
     mov %ss:6(%bx), %ax
     lds %ss:2(%bx), %bx
     mov %ax, (%bx)
     mov %dx, %ds
     mov %cx, %bx
     ret

outport:
     mov %bx, %cx
     mov %sp, %bx
     mov %ss:2(%bx), %dx
     mov %ss:4(%bx), %ax
     out %ax, %dx
     mov %cx, %bx
     ret

outportb:
     mov %bx, %cx
     mov %sp, %bx
     mov %ss:2(%bx), %dx
     mov %ss:4(%bx), %al
     out %al, %dx
     mov %cx, %bx
     ret

inport:
     mov %bx, %cx
     mov %sp, %bx
     mov %ss:2(%bx), %dx
     in %dx, %ax
     mov %cx, %bx
     ret

inportb:
     mov %bx, %cx
     mov %sp, %bx
     mov %ss:2(%bx), %dx
     in %dx, %al
     xor %ah, %ah
     mov %cx, %bx
     ret

idle:
     sti
     hlt
     cli
     ret

bios_getc:
     mov $1, %ah
     int $0x16
     jnz 1f
     mov $-1, %ax
     ret
1:
     mov $0, %ah
     int $0x16
     mov $0, %ah
     ret

bios_putc:
     push %bp
     mov %sp, %bp
     mov 4(%bp), %al
     mov $0x0e, %ah
     mov $0, %bh
     int $0x10
     pop %bp
     ret

     .bss
resume_stack:
     .skip 288

