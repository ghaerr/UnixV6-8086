# ==========================================================================
# NOTE: This Makefile requires Open Watcom 'wmake'.
# It uses specific syntax (like .symbolic, %make, & line continuation)
# that is not compatible with GNU make or Microsoft nmake.
# ==========================================================================

all : .symbolic Unix360.img unix0_rk.img

# Build boot sector and generate floppy image
Unix360.img:
	wasm -bt=DOS -mt -1 bootsect.asm -fo=bootsect.obj
	wlink $(LDFLAGS) SYSTEM dos com FILE bootsect.obj OPTION nodefaultlibs NAME boot.com
	python genimage.py

unix0_rk.img:
	gunzip -c unix0_rk.img.gz > $@

bochs: .symbolic Unix360.img unix0_rk.img
	bochs -qf bochsrc.bxrc

qemu: .symbolic Unix360.img unix0_rk.img
	qemu-system-i386 -curses -drive file=Unix360.img,format=raw,if=floppy -drive file=unix0_rk.img,format=raw,if=ide -boot a

qemu-uart: .symbolic Unix360.img unix0_rk.img
	qemu-system-i386 -curses -serial stdio -drive file=Unix360.img,format=raw,if=floppy -drive file=unix0_rk.img,format=raw,if=ide -boot a

clean: .symbolic
	rm -f *.obj *.err *.com *.img *.txt
