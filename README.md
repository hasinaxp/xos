# XOS OPERATING SYSTEM

This is a simple `32-bit` gui opetrating system implementation as a hobby project. This is no way an actual operating system, and in most cases where I could cheat without following any actual specs I did. I kept most of the code in C as much as possible but, we can't avoid a little bit of Assembly.

This is `not a linux distro`. The implementation uses `grab` for booloader as I felt for this project bootloader is out of scope 😝.


## Build

### Requirements:
- A cross compiler - i686-elf-gcc and i686-elf-ld
- nasm
- qemu (for testing)
- grub-mkrescue (for making bootable iso)

### Compile / build iso image
```
sh ./compile.sh
```

### Run
```
sh ./run.sh
```
