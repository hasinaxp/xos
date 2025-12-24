[bits 32]

section .multiboot
align 4

MULTIBOOT_MAGIC      equ 0x1BADB002
MULTIBOOT_FLAGS      equ (1 << 0) | (1 << 1) | (1 << 2) ; page_align | mem_info | video
MULTIBOOT_CHECKSUM   equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

; extern _start
extern _end
extern _bss_end
extern multiboot_header

multiboot_header:
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM

    ; REQUIRED when VIDEO_MODE is set
    dd multiboot_header   ; header_addr
    dd _start             ; load_addr
    dd _end               ; load_end_addr
    dd _bss_end           ; bss_end_addr
    dd _start             ; entry_addr

    ; Video mode request
    dd 0                  ; mode_type = linear framebuffer
    dd 1024               ; width
    dd 768                ; height
    dd 32                 ; bpp


    
section .data
saved_magic:   dd 0
saved_mbi:     dd 0

section .text
global _start
extern gdt_flush
extern kernel_main

_start:
    cli
    mov esp, 0x90000            ; set up stack BEFORE anything else

    ; Save GRUB values
    mov [saved_magic], eax
    mov [saved_mbi], ebx

    call gdt_flush               ; optional — GRUB already sets a flat GDT

    ; Restore original values
    mov eax, [saved_magic]
    mov ebx, [saved_mbi]

    ; Push multiboot info for kernel_main
    push ebx                     ; multiboot info pointer
    push eax                     ; multiboot magic
    call kernel_main

.halt_loop:
    cli
    hlt
    jmp .halt_loop
