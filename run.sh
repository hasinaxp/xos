#   qemu-img create -f raw disk.img 10M
qemu-system-i386 -m 1G -cdrom xos.iso -drive file=disk.img,format=raw,if=ide
