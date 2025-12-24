#pragma once
#include "../common/types.h"


typedef struct {
    u32 flags;
    u32 mem_lower;
    u32 mem_upper;
    u32 boot_device;
    u32 cmdline;
    u32 mods_count;
    u32 mods_addr;
    union {
        struct { u32 tabsize, strsize, addr, reserved; } aout_sym;
        struct { u32 num, size, addr, shndx; } elf_sec;
    } u;
    u32 mmap_length;
    u32 mmap_addr;
    u32 drives_length;
    u32 drives_addr;
    u32 config_table;
    u32 boot_loader_name;
    u32 apm_table;
    u32 vbe_control_info;
    u32 vbe_mode_info;
    u16 vbe_mode;
    u16 vbe_interface_seg;
    u16 vbe_interface_off;
    u16 vbe_interface_len;
    u64 framebuffer_addr;
    u32 framebuffer_pitch;
    u32 framebuffer_width;
    u32 framebuffer_height;
    u8  framebuffer_bpp;
    u8  framebuffer_type;
    union {
        struct {
            u32 framebuffer_palette_addr;
            u16 framebuffer_palette_num_colors;
        };
        struct {
            u8 framebuffer_red_field_position;
            u8 framebuffer_red_mask_size;
            u8 framebuffer_green_field_position;
            u8 framebuffer_green_mask_size;
            u8 framebuffer_blue_field_position;
            u8 framebuffer_blue_mask_size;
        };
    };
} __attribute__((packed)) multiboot_info_t;



#define MULTIBOOT_HEADER_MAGIC       0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC   0x2BADB002

typedef struct {
    u32 size;       // size of this entry (not counting 'size' itself)
    u64 addr;       // base address of memory region
    u64 len;        // length of memory region
    u32 type;       // type of memory region
} __attribute__((packed)) multiboot_memory_map_t;

typedef struct {
    u32 start_address;
    u32 end_address;
    u32 size;
    u32 heap_offset;
    u32 effective_memory_address;
} ksystem_memory_info_t;


static ksystem_memory_info_t sys_meminfo = {0};


fn sys_init_mem_info(multiboot_info_t* mb) {
    if (!mb) return;

    /* mmap path (your screenshot shows this IS present) */
    if (mb->flags & (1 << 6)) {
        multiboot_memory_map_t* mmap =
            (multiboot_memory_map_t*)(u32)mb->mmap_addr;

        multiboot_memory_map_t* end =
            (multiboot_memory_map_t*)((u32)mb->mmap_addr + mb->mmap_length);

        u64 best_addr = 0;
        u64 best_len  = 0;

        while (mmap < end) {
            if (mmap->type == 1 && mmap->len > best_len) {
                best_addr = mmap->addr;
                best_len  = mmap->len;
            }
            mmap = (multiboot_memory_map_t*)((u32)mmap +
                    mmap->size + sizeof(mmap->size));
        }

        sys_meminfo.start_address = best_addr;
        sys_meminfo.size          = best_len;
        sys_meminfo.end_address   = best_addr + best_len;
    }

    sys_meminfo.heap_offset = 64ULL * 1024 * 1024;
    sys_meminfo.effective_memory_address =
        sys_meminfo.start_address + sys_meminfo.heap_offset;
}

