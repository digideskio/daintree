BISON = /usr/local/opt/bison/bin/bison
CC = i686-elf-gcc
LD = i686-elf-ld
AS = i686-elf-as
CFLAGS := -O0 -ansi -std=gnu99 -g -m32 -nodefaultlibs -nostdinc -nostdlib \
	-static -ffreestanding -fleading-underscore -fno-builtin \
	-fno-stack-protector -fomit-frame-pointer -Wall -Wcast-qual -Wextra \
	-Wwrite-strings
ASFLAGS := -gstabs --32
LDFLAGS := -melf_i386

BUILDDIR := build
TARGET := $(BUILDDIR)/daintree
COPYDEST := C:/daintree
CSRCS := entry.c cons.c mem.c arch.c
ASRCS := entry.s
LDFILE := daintree.ld
OBJS := $(CSRCS:%.c=$(BUILDDIR)/%.c.o) $(ASRCS:%.s=$(BUILDDIR)/%.s.o)
MENU := menu.lst
MENUDEST := C:/boot/grub/menu.lst
IMGFILE := daintree.img

all: $(TARGET)-copy

run: $(TARGET)-copy
	qemu-system-i386 -boot order=c -drive file=$(IMGFILE),index=0,media=disk,format=raw -smp 1,cores=2 -net none -s & \
	osascript -e "`cat foreground`"; \
	wait

debug:
	gdb $(TARGET) --eval-command='target remote localhost:1234'

$(TARGET)-copy: $(TARGET)
	MTOOLSRC=mtoolsrc mcopy -D o $(TARGET) $(COPYDEST)
	MTOOLSRC=mtoolsrc mcopy -D o $(MENU) $(MENUDEST)

$(TARGET): $(OBJS) $(LDFILE) $(BUILDDIR)/parse.tab.o $(BUILDDIR)/lex.yy.o
	$(LD) $(LDFLAGS) -T$(LDFILE) $(OBJS) $(BUILDDIR)/parse.tab.o $(BUILDDIR)/lex.yy.o -o $(TARGET)

$(BUILDDIR)/%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.s.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

$(BUILDDIR)/parse.tab.o: $(BUILDDIR)/parse.tab.c
	$(CC) $(CFLAGS) -I. -c $< -o $@

$(BUILDDIR)/lex.yy.o: $(BUILDDIR)/lex.yy.c $(BUILDDIR)/parse.tab.c
	$(CC) $(CFLAGS) -I. -c $(BUILDDIR)/lex.yy.c -o $@

$(BUILDDIR)/parse.tab.c: parse.y
	$(BISON) -v --report=state -d $<
	mv parse.tab.? $(BUILDDIR)/

$(BUILDDIR)/lex.yy.c: lex.l
	flex -t $< > $@

clean:
	-rm $(OBJS) $(TARGET)

