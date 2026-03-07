# kernel Makefile

CC = wcc
CFLAGS = -i=h -ms -0 -s -zls -ecc -bt=dos -ohs -zq -j -zl
LD = wlink
LDFLAGS = SYSTEM dos com OPTION map,nodefaultlibs

%.obj: %.c
	$(CC) $(CFLAGS) $^ -fo=$@

all: unix.com

dmr/m86.obj: dmr/m86.asm
	wasm -bt=DOS -mt -0 $< -fo=$@

OBJS =              \
    dmr/bio.obj     \
    dmr/ide.obj     \
    dmr/kbd.obj     \
    dmr/kl.obj      \
    dmr/mem.obj     \
    dmr/pc.obj      \
    dmr/rk.obj      \
    dmr/tty.obj     \
    dmr/uart.obj    \
    ken/alloc.obj   \
    ken/clock.obj   \
    ken/fio.obj     \
    ken/iget.obj    \
    ken/main.obj    \
    ken/malloc.obj  \
    ken/nami.obj    \
    ken/pipe.obj    \
    ken/prf.obj     \
    ken/rdwri.obj   \
    ken/sig.obj     \
    ken/slp.obj     \
    ken/subr.obj    \
    ken/sys1.obj    \
    ken/sys2.obj    \
    ken/sys3.obj    \
    ken/sys4.obj    \
    ken/sysent.obj  \
    ken/text.obj    \
    ken/trap.obj    \

unix.com: dmr/m86.obj $(OBJS)
	$(LD) $(LDFLAGS) NAME unix.com FILE dmr/m86.obj, $(OBJS:obj=obj,)

clean:
	rm -f ken/*.obj dmr/*.obj *.com *.map *.err
