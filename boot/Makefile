# unix.img Makefile

all : Unix360.img unix0_rk.img unix.img

# Build boot sector and generate floppy image
Unix360.img: ../unix.com bootsect.asm
	wasm -bt=DOS -mt -1 bootsect.asm -fo=bootsect.obj
	wlink $(LDFLAGS) SYSTEM dos com FILE bootsect.obj OPTION nodefaultlibs NAME boot.com
	# HEADS = 2 * TRACKS = 40 * SECTORS_PER_TRACK = 9 = 720
	dd if=/dev/zero of=Unix360.img bs=512 count=720
	dd if=boot.com of=Unix360.img conv=notrunc
	dd if=../unix.com of=Unix360.img conv=notrunc bs=512 seek=1

../tools/mkfs: ../tools/mkfs.c
	gcc -Wno-implicit-function-declaration -Wno-implicit-int -Wno-parentheses -Wno-return-type -Wno-incompatible-pointer-types ../tools/mkfs.c -o ../tools/mkfs

unix.img: proto ../tools/mkfs
	../tools/mkfs unix.img proto
	dd if=/dev/zero of=unix.img bs=1 count=1 seek=10321919 conv=notrunc

b: Unix360.img unix.img
	-bochs -qf bochs.rc

q: Unix360.img unix.img
	qemu-system-i386 -curses -drive file=Unix360.img,format=raw,if=floppy -drive file=unix.img,format=raw,if=ide -boot a

unix0_rk.img:
	gunzip -c unix0_rk.img.gz > $@

bochs: Unix360.img unix0_rk.img
	bochs -qf bochsrc.bxrc

qemu: Unix360.img unix0_rk.img
	qemu-system-i386 -curses -drive file=Unix360.img,format=raw,if=floppy -drive file=unix0_rk.img,format=raw,if=ide -boot a

qemu-uart: Unix360.img unix0_rk.img
	qemu-system-i386 -curses -serial stdio -drive file=Unix360.img,format=raw,if=floppy -drive file=unix0_rk.img,format=raw,if=ide -boot a

clean:
	rm -f *.obj *.err *.com *.img *.txt ../tools/mkfs
