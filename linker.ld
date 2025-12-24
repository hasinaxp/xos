ENTRY(_start)

SECTIONS
{
    /* Kernel load address */
    . = 1M;

    /* Multiboot header must be early in the file */
    .multiboot ALIGN(4) :
    {
        PROVIDE(multiboot_header = .);
        KEEP(*(.multiboot))
    }

    .text ALIGN(4) :
    {
        *(.text*)
    }

    .rodata ALIGN(4) :
    {
        *(.rodata*)
    }

    .data ALIGN(4) :
    {
        *(.data*)
    }

    .bss ALIGN(4) :
    {
        PROVIDE(_bss_start = .);
        *(.bss*)
        *(COMMON)
        PROVIDE(_bss_end = .);
    }

    /* End of kernel image */
    PROVIDE(_end = .);
}
