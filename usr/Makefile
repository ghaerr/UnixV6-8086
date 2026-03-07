# /usr Makefile

############# Standard Section for IA16 C ##############

CC = ia16-elf-gcc
LD = ia16-elf-gcc
CLBASE = -Os -melks-libc -mtune=i8086 -mcmodel=small
CLBASE += -mno-segment-relocation-stuff -fno-inline -fno-builtin -nostdlib
WARNINGS = -Wno-implicit-int -Wno-implicit-function-declaration
INCLUDES =
DEFINES =
CFLAGS = $(CLBASE) $(WARNINGS) $(INCLUDES) $(DEFINES) $(LOCALFLAGS)
LDLIBS =

%.com: %.c
	$(CC) $(CFLAGS) $(STARTUP) $^ -o $@ $(LIBS) $(LDLIBS)

SOURCES=$(PROGS:.com=.c)
#OBJECTS=$(SOURCES:.c=.o)

############# End of Standard Section ##############

LOCALFLAGS = -Wno-multichar

PROGS = init.com sh.com ls.com cat.com wc.com echo.com pwd.com kill.com date.com
PROGS += rm.com login.com cp.com chmod.com mkdir.com sync.com id.com zombie.com
PROGS += mount.com umount.com forktest.com sigtest.com stty.com more.com fib.com
PROGS += ln.com nice.com mknod.com sleep.com grep.com ps.com
PROGS += mkfs.com

STARTUP = startup.s
LIBS = syscall.c ulib.c ctime.c printf.c

.PHONY: all
all: $(OBJECTS) $(PROGS)

clean:
	rm -f *.com *.o
