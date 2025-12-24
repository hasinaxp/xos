#include "kernel.h"
#include "platform/display_tt.h"
#include "platform/multiboot.h"
#include "common/str.h"
#include "platform/platform_init.h"
#include "platform/graphics.h"
#include "platform/ata.h"
#include "platform/memory.h"
#include "gui/renderer.h"

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



void kernel_main(unsigned long magic, multiboot_info_t* mb_info) {
    


    platform_init(mb_info);


    window_t window = {0};
    window.x = 20;
    window.y = 30;
    window.width = 400;
    window.height = 400;
    
    
    
    while(true) {
        if(ticks % 20 == 0) {
            clear_desktop();
            draw_status_bar();
            draw_window(&window);
            graphics_present();
            sys_memory_commit();
        } 
    }

    
    while(1) {
        __asm__ __volatile__("hlt");
    }
}

