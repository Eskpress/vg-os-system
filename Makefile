CC = gcc
CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-asynchronous-unwind-tables -Wall -Wextra
AS = as
ASFLAGS = --32
LD = ld
LDFLAGS = -m elf_i386 -T linker.ld

OBJS = boot.o kernel.o Render/render.o keyboard/keyboard.o strutil.o logo.o comandos/comandos.o

all: meuos.bin

boot.o: boot.s
	$(AS) $(ASFLAGS) $< -o $@

kernel.o: kernel.c
	$(CC) $(CFLAGS) -IRender -Ikeyboard -Icomandos -c $< -o $@

strutil.o: strutil.c
	$(CC) $(CFLAGS) -c $< -o $@

logo.o: logo.c
	$(CC) $(CFLAGS) -c $< -o $@

Render/render.o: Render/render.c
	$(CC) $(CFLAGS) -I. -IRender -c $< -o $@

keyboard/keyboard.o: keyboard/keyboard.c
	$(CC) $(CFLAGS) -Ikeyboard -c $< -o $@

comandos/comandos.o: comandos/comandos.c
	$(CC) $(CFLAGS) -Icomandos -IRender -I. -c $< -o $@

meuos.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

iso: meuos.bin
	mkdir -p iso_root/boot/grub
	cp meuos.bin iso_root/boot/meuos.bin
	cp grub.cfg iso_root/boot/grub/grub.cfg
	grub-mkrescue -o vgos.iso iso_root

clean:
	rm -f $(OBJS) meuos.bin vgos.iso
