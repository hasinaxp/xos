#include "kernel.h"
#include "platform/display_tt.h"
#include "platform/multiboot.h"
#include "common/str.h"
#include "platform/platform_init.h"
#include "platform/gfx.h"
#include "platform/ata.h"
#include "platform/memory.h"

void print_multiboot_header(multiboot_info_t *mbi) {
    char buffer[512] = {0};
    append_str(buffer, "Multiboot Info:\n");
    append_str(buffer, "  Flags: 0x");
    char hexbuf[9] = {0};
    to_hex_str(mbi->flags, hexbuf, 8);
    append_str(buffer, hexbuf);
    append_str(buffer, "\n");
    append_str(buffer, "  Memory Lower: ");
    append_int(buffer, mbi->mem_lower, 10);
    append_str(buffer, "\n");
    append_str(buffer, "  Memory Upper: ");
    append_int(buffer, mbi->mem_upper, 10);
    append_str(buffer, "\n");
    append_str(buffer, "  Boot Device: ");
    append_int(buffer, mbi->boot_device, 10);
    append_str(buffer, "\n");
    append_str(buffer, "  Command Line: ");
    append_str(buffer, (char *)mbi->cmdline);
    append_str(buffer, "\n");
    append_str(buffer, "  Modules Count: ");
    append_int(buffer, mbi->mods_count, 10);
    append_str(buffer, "\n");
    append_str(buffer, "  Modules Address: 0x");
    to_hex_str(mbi->mods_addr, hexbuf, 8);
    append_str(buffer, hexbuf);
    append_str(buffer, "\n");
    append_str(buffer, "  Memory Map Length: ");
    append_int(buffer, mbi->mmap_length, 10);
    append_str(buffer, "\n");
    append_str(buffer, "  Memory Map Address: 0x");
    to_hex_str(mbi->mmap_addr, hexbuf, 8);
    append_str(buffer, hexbuf);
    append_str(buffer, "\n");
    append_str(buffer, "  Drives Length: ");
    append_int(buffer, mbi->drives_length, 10);
    append_str(buffer, "\n");
    append_str(buffer, "  Drives Address: 0x");
    to_hex_str(mbi->drives_addr, hexbuf, 8);
    append_str(buffer, hexbuf);
    append_str(buffer, "\n");
    append_str(buffer, "  Config Table: 0x");
    to_hex_str(mbi->config_table, hexbuf, 8);
    append_str(buffer, hexbuf);
    append_str(buffer, "\n");
    append_str(buffer, "  Boot Loader Name: ");
    append_str(buffer, (char *)mbi->boot_loader_name);
    append_str(buffer, "\n");
    append_str(buffer, "  Frame buffer Address: 0x");
    to_hex_str(mbi->framebuffer_addr, hexbuf, 8);
    append_str(buffer, hexbuf);
    append_str(buffer, "\n");
    append_str(buffer, "  Frame buffer width: ");
    append_int(buffer, mbi->framebuffer_width, 10);
    append_str(buffer, "\n");
    append_str(buffer, "  Frame buffer height: ");
    append_int(buffer, mbi->framebuffer_height, 10);
    append_str(buffer, "\n");
    draw_text(400, 10, buffer, 12, COLOR_YELLOW);

}



void print_memory_info(int x, int y) {
    char buffer[256] = {0};
    char hexbuf[17] = {0};

    append_str(buffer, "Memory Info:\n");


    append_str(buffer, "  Start Address: 0x");
    to_hex64_str(sys_meminfo.start_address, hexbuf, 16);
    append_str(buffer, hexbuf);
    append_str(buffer, "\n");

    append_str(buffer, "  End Address:   0x");
    to_hex64_str(sys_meminfo.end_address, hexbuf, 16);
    append_str(buffer, hexbuf);
    append_str(buffer, "\n");

    append_str(buffer, "  Size: ");
    append_int(buffer, sys_meminfo.size / 1024, 10);
    append_str(buffer, " KB\n");

    draw_text(x, y, buffer, 12, COLOR_YELLOW);
}


static u32 last_ticks = 0;

void draw_clock() {
    
    if(ticks - last_ticks > 10) {
        char buf[64] = {0};
    
        int s = (ticks / 100) % 60;
        int m = (ticks / 100 / 60) % 60;
        int h = (ticks / 100 / 3600) % 24;
    
        append_int(buf, h, 10);
        append_str(buf, ":");
        append_int(buf, m, 10);
        append_str(buf, ":");
        append_int(buf, s, 10);
        int font_size = 20;
        int padding = 4;
        int text_width = font_size * .65f * kstrlen(buf);
      
        fill_rect(1,
                  FB_HEIGHT - (font_size + padding * 2) - 2,
                  text_width + padding * 2,
                  font_size + padding * 2 - 2,
                  COLOR_BLUE, 8);
        draw_text(padding,
                  FB_HEIGHT - (font_size + padding) - 4,
                  buf,
                  font_size,
                  COLOR_YELLOW);
    
        last_ticks = ticks;
    }
}

static u16 ata_test_buf[256];


void kernel_main(unsigned long magic, multiboot_info_t* mb_info) {
    


    platform_init(mb_info);
    memory_init();


    print_multiboot_header(mb_info);
    print_memory_info(400, 190);
    


    while(true) {
        draw_clock();
        if(ticks % 200 == 0) {
            sys_memory_commit();
        } 
    }

    
    while(1) {
        __asm__ __volatile__("hlt");
    }
}

