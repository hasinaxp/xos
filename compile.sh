#!/bin/bash
set -e

# Clean
rm -rf build iso
mkdir -p build iso/boot/grub

# Assemble
nasm -f elf32 src/main.asm -o build/main.o
nasm -f elf32 src/gdt.asm -o build/gdt.o

# Compile with cross-compiler
i686-elf-gcc -m32 -ffreestanding -c src/kernel.c -o build/kernel.o

# Link kernel
i686-elf-ld -m elf_i386 -T linker.ld -o build/kernel.elf build/main.o build/gdt.o build/kernel.o

# Copy files for ISO
cp build/kernel.elf iso/boot/
cp boot/grub/grub.cfg iso/boot/grub/

# Make bootable ISO
grub-mkrescue -o xos.iso iso
