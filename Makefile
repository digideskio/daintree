BISON = /usr/local/opt/bison/bin/bison
CC = i686-elf-gcc
LD = i686-elf-ld
AS = i686-elf-as
CFLAGS := -O0 -ansi -std=gnu99 -g -m32 -nodefaultlibs -nostdinc -nostdlib \
	-static -ffreestanding -fleading-underscore -fno-builtin \
	-fno-stack-protector -fomit-frame-pointer -Wall -Wcast-qual \
	-Wwrite-strings -Wno-unused-parameter
ASFLAGS := -gstabs --32
LDFLAGS := -melf_i386

BUILDDIR := build
TARGET := $(BUILDDIR)/daintree
COPYDEST := C:/daintree
CSRCS := entry.c interrupts.c console.c mem.c arch.c string.c ctype.c stdlib.c program.c ast.c crc32.c dict.c math.c task.c console_task.c manager_task.c
ASRCS := entry.s interrupts.s
LDFILE := daintree.ld
OBJS := $(BUILDDIR)/parse.tab.o $(BUILDDIR)/lex.sv.o $(CSRCS:%.c=$(BUILDDIR)/%.c.o) $(ASRCS:%.s=$(BUILDDIR)/%.s.o)
DEPS := $(BUILDDIR)/parse.tab.d $(BUILDDIR)/lex.sv.d $(CSRCS:%.c=$(BUILDDIR)/%.c.d)
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

$(TARGET): $(OBJS) $(LDFILE)
	$(LD) $(LDFLAGS) -T$(LDFILE) $(OBJS) -o $(TARGET)

-include $(DEPS)

$(BUILDDIR)/%.c.o: %.c
	$(CC) $(CFLAGS) -I. -c $< -o $@ -MMD

$(BUILDDIR)/%.s.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

$(BUILDDIR)/parse.tab.o: $(BUILDDIR)/parse.tab.c
	$(CC) $(CFLAGS) -I. -c $< -o $@ -MMD

$(BUILDDIR)/lex.sv.o: $(BUILDDIR)/lex.sv.c
	$(CC) $(CFLAGS) -I. -Ibuild -DSONAVARA_NO_SELF_CHAIN -c $(BUILDDIR)/lex.sv.c -o $@ -MMD

$(BUILDDIR)/parse.tab.h: $(BUILDDIR)/parse.tab.c

$(BUILDDIR)/parse.tab.c: parse.y
	$(BISON) -v --report=state -d $<
	mv parse.tab.? $(BUILDDIR)/

$(BUILDDIR)/lex.sv.c: lex.sv
	sonavara < $< > $@ || (rm $@; false)

clean:
	-rm $(OBJS) $(DEPS) $(TARGET) $(BUILDDIR)/{parse.tab.c,parse.tab.h,lex.sv.c}
