_TEXT   segment word public 'CODE'
    ORG   100h
STARTX          PROC    NEAR
    mov ax, offset argv
    push ax
    mov ax, offset argv0
    push ax
    mov dx,11
    int 129
STARTX          ENDP

argv0 label byte
    db  'init'
    db  0
argv label word
    dw  argv0
    dw  0

_TEXT   ENDS

    END STARTX
