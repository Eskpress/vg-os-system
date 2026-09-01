CC = gcc
CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -Wall -Wextra
AS = as
ASFLAGS = --32
LD = ld
LDFLAGS = -m elf_i386 -T linker.ld

OBJS = boot.o kernel.o render.o keyboard.o strutil.o logo.o

all: meuos.bin

boot.o: boot.s
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

meuos.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

iso: meuos.bin
	mkdir -p iso_root/boot/grub
	cp meuos.bin iso_root/boot/meuos.bin
	cp grub.cfg iso_root/boot/grub/grub.cfg
	grub-mkrescue -o vgos.iso iso_root

clean:
	rm -f $(OBJS) meuos.bin vgos.iso
