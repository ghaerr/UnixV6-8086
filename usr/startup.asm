        name    cstart
        assume  nothing

DGROUP group _TEXT,CONST,STRINGS,_DATA,DATA,_BSS

_TEXT   segment word public 'CODE'

        extrn   _main                 : near

        assume  ds:DGROUP

CONST   segment word public 'DATA'
CONST   ends

STRINGS segment word public 'DATA'
STRINGS ends

DATA    segment word public 'DATA'
        extrn _errno: word, _r0: word, _r1: word, _r3: word
DATA    ends

_BSS    segment word public 'BSS'
        extrn   _edata                  : byte  ; end of DATA (start of BSS)
        extrn   _end                    : byte  ; end of BSS (start of STACK)
_BSS    ends

        assume  nothing
        public  startx, _syscall

        assume  cs:_TEXT

        org     0100h

startx proc near
        jmp     @f
        db      'F', 'a', 'n', 'g'
        jmp    _callsig

; Reset uninitialized data area
@@: 
        assume  ds:DGROUP
        assume  es:DGROUP

        mov     cx,offset DGROUP:_end   ; end of _BSS segment (start of STACK)
        mov     di,offset DGROUP:_edata ; start of _BSS segment
        sub     cx,di                   ; calc # of bytes in _BSS segment
        mov     al,0                    ; zero the _BSS segment
        rep     stosb                   ; . . .

        call    _main
        mov     dx, 1                   ; SYS_exit
        int     81h
startx endp

_callsig  proc    near
        call si             ; call signal handler
        pop ax              ; don't touch ds
        pop ax              ; don't touch es
        pop dx
        pop cx
        pop bx
        pop ax
        pop di
        pop si
        pop bp
        pop ax              ; ip, cs, flag = ax, flag, return address
        popf
        ret
_callsig  endp

_syscall proc near
    pop cx     ; return address
    pop dx     ; syscall number
    pop ax     ; r0
    int 0x81
    or dx, dx
    jz @f      ; no error
    mov _errno, dx
    mov dx, -1
@@:
    mov _r0, ax
    mov _r1, bx
    mov _r3, dx
    sub sp, 4
    push cx
    ret
_syscall endp

_TEXT   ends

        end     startx
