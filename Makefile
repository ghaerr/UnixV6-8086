# kernel Makefile using ia16-elf-gcc

CC = ia16-elf-gcc
CLBASE = -Os -mtune=i8086 -mcmodel=small
CLBASE += -fno-inline -fno-builtin -ffreestanding
CLBASE += -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-defer-pop
CLBASE += -fpack-struct=2 -fcommon
INCLUDES = -Ih
WARNINGS = -Wno-int-conversion -Wno-incompatible-pointer-types -Wno-implicit-int
CFLAGS = $(CLBASE) $(WARNINGS) $(INCLUDES) $(DEFINES) $(LOCALFLAGS)
LD = ia16-elf-gcc
LDFLAGS = -nostdlib

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $^

%.o: %.s
	$(CC) -c -o $@ $(CFLAGS) $^

.PHONY: all usr boot b x

all: unix.com usr boot

usr:
	make -C usr

boot: unix.com
	make -C boot

b: all
	make -C boot b

x: bclean boot b

######### kernel build #########

OBJS =              \
    dmr/bio.o       \
    dmr/ide.o       \
    dmr/kbd.o       \
    dmr/kl.o        \
    dmr/mem.o       \
    dmr/pc.o        \
    dmr/rk.o        \
    dmr/tty.o       \
    dmr/uart.o      \
    ken/alloc.o     \
    ken/clock.o     \
    ken/fio.o       \
    ken/iget.o      \
    ken/main.o      \
    ken/malloc.o    \
    ken/nami.o      \
    ken/pipe.o      \
    ken/prf.o       \
    ken/rdwri.o     \
    ken/sig.o       \
    ken/slp.o       \
    ken/subr.o      \
    ken/sys1.o      \
    ken/sys2.o      \
    ken/sys3.o      \
    ken/sys4.o      \
    ken/sysent.o    \
    ken/text.o      \
    ken/trap.o      \

GCCOBJS = gcc/ashlsi3.o gcc/ashrsi3.o
OBJS += $(GCCOBJS)

unix.com: dmr/m86.o $(OBJS)
	$(LD) -o $@ -T com.ld -Wl,-Map=unix.map $(LDFLAGS) dmr/m86.o $(OBJS)

bclean:
	rm -f ken/*.o dmr/*.o gcc/*.o *.com *.map
	make -C boot clean

clean: bclean
	make -C usr clean
