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
	qemu-system-i386 -boot order=c -hda $(IMGFILE) -smp 1,cores=2 -net none -s

debug:
	gdb $(TARGET) --eval-command='target remote localhost:1234'

$(TARGET)-copy: $(TARGET)
	MTOOLSRC=mtoolsrc mcopy -D o $(TARGET) $(COPYDEST)
	MTOOLSRC=mtoolsrc mcopy -D o $(MENU) $(MENUDEST)

$(TARGET): $(OBJS) $(LDFILE)
	$(LD) $(LDFLAGS) -T$(LDFILE) $(OBJS) -o $(TARGET)

$(BUILDDIR)/%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.s.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	-rm $(OBJS) $(TARGET)

