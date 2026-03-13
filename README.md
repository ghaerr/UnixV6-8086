# UnixV6-8086

This is a fork of the fantastic work done in
[RealXV6](https://github.com/FounderSG/RealXV6)
porting UNIX V6 to the 8086 in Real Mode, with as few modifications as possible
to the original kernel source.

## Overview

The goal of this fork is to produce a system capable of reproducing itself,
by being able to compile the kernel, all utilities, and the compiler toolchain
itself on the running UNIX V6 system, thus producing what UNIX V6 accomplished,
except running on an 8086.

This is a work in progress. As many applications as possible, except
the C compiler itsef, will be replaced with the original V6 source, with
as few modifications as are necessary.

Finding a C compiler that can compile itself on the 8086 is somewhat tricky,
and for now, the [DeSMET C compiler and toolchain](https://github.com/ghaerr/dcc),
(DCC) released as version 3.10h in 1989, and recently ported to
[ELKS](https://github.com/ghaerr/elks) will be used.

## More technical details on changes from RealXV6

The RealXV6 project uses the simple DOS .com executable format for the kernel
and all applications. In order to host a larger C compiler able to compile itself,
the UNIX processes need to support seperate I & D (code and data), which allow
for a max 64K code segment and additional 64K data segment. For this reason,
the UNIX executable format has been enhanced to support a UNIX V7-like a.out format,
currently in use by ELKS and 16-bit MINIX applications.

Additionally, non-standard C language features including far pointers and inline
assembly code have been changed to lessen specific compiler dependencies.
In preparation for compilation by DCC, the applications and kernel are now able
to be compiled by ia16-elf-gcc, which also natively produces the a.out
executable format. DCC has also been enhanced to produce the ELKS a.out
executable format.

## How to build

The original OpenWatcom C `wmake` build has been replaced with GNU `make`,
and the kernel can currently be built by either OpenWatcom or ia16-elf-gcc.
The applications require building using GCC, since the a.out executable
format is being used.

The ia16-elf-gcc compiler can be installed by building ELKS.

To build the kernel, all applications and boot images using ia16-elf-gcc:
```
$ make
```
This will build the kernel executable unix.com,
and a floppy boot image in boot/Unix306.img and
root UNIX filesystem in boot/Unix.img. Either QEMU or Bochs can be used
to boot UNIX V6 on the PC:
```
$ cd boot
$ make b (for bochs emulation, see boot/bochs.rc for configuration)
$ make q (for qemu emulation)
```

# RealXV6

A Port of the UNIX Version 6 (V6) Kernel to the 8086 PC in Real Mode.

## Overview

This project is a port of the classic UNIX Version 6 (V6) kernel to the **Intel 8086/8088** architecture, targeting the original IBM PC and its compatibles. The kernel code is based on the version famously analyzed in the *Lions' Commentary on UNIX 6th Edition*.

The userspace is a hybrid, containing programs from the original V6 distribution as well as some from the `xv6` project.

## Philosophy: A Port, Not a Rewrite

Unlike projects like `xv6` which are modern rewrites inspired by V6, RealXV6 aims to be a faithful **port**. The goal is to preserve the original structure, algorithms, and spirit of the V6 kernel as much as possible. This makes it an excellent resource for studying the original design on a more accessible platform for modern study, typically through emulation.

Key aspects of the original design are intentionally retained:

*   **Original Scheduler Logic:** The logic at the famous `/* You are not expected to understand this */` comment in the scheduler retains its original meaning. As a direct result, the original V6 process swapping mechanism is also fully preserved.
*   **Process Switching:** The original PDP-11 `savu`/`retu` assembly primitives relied on the C compiler saving the environment on every function call. In this port, the `savu`/`retu` routines are still used, but their role is now limited to managing the switch of the user area (`u` area). Since the 8086 architecture lacks an MMU, this switch is handled by `memcpy` within these routines. The actual process context switch is handled by the more portable `save`/`resume` functions from UNIX V7.

## Architecture

The original V6 was designed for the 16-bit PDP-11. By targeting the native 16-bit architecture of the 8086, this port minimizes the architectural changes required, keeping the code as close to the original as possible.

While understanding the PDP-11 architecture is key to studying the original V6 source, the 8086 architecture is more widely understood. Furthermore, the widespread availability of mature 8086 PC emulators makes this platform highly accessible for development and study. For developers, knowledge of how a typical PC bootloader operates is sufficient to understand the low-level aspects of RealXV6.

The project uses the simple `.COM` executable format. This choice simplifies development as `.COM` files are memory-relocatable by design, which makes it easier to manage processes in memory and to implement swapping them to disk.

## Building the System

The project is built using the **Open Watcom** C compiler and toolchain. Open Watcom is a powerful cross-platform suite that can run on modern operating systems like Windows, Linux, and macOS, while still targeting the 8086's 16-bit architecture.

To build the kernel, please refer to the included `Makefile`.

## Code Structure

The source code is organized into several key directories:

*   `/ken`: Named for Ken Thompson, this directory contains the main, machine-independent kernel code (e.g., system calls, process management, file system).
*   `/dmr`: Named for Dennis M. Ritchie, this directory contains machine-dependent drivers and low-level code for the PC architecture (e.g., keyboard, IDE, TTY).
*   `/h`: Contains all kernel header files, defining the core data structures and constants.
