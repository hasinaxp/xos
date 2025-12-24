#pragma once
#include "idt.h"
#include "clock.h"
#include "keyboard.h"
#include "multiboot.h"
#include "mouse.h"
#include "gfx.h"

fn platform_init(multiboot_info_t* mb_info) {

    
    sys_init_mem_info(mb_info);
    
    framebuffer_init(
        mb_info->framebuffer_addr,
        mb_info->framebuffer_width,
        mb_info->framebuffer_height,
        mb_info->framebuffer_pitch
    );

    font32_init();

    idt_init();
    
    clock_init();
    keyboard_init();
    mouse_init();
    
    idt_install();
    
    __asm__ volatile("sti");
    
    
    
    clear_screen(COLOR_DEEP_CYAN);

    draw_text(4, 14, "  XOS Initialized >.<", 12, COLOR_WHITE);

}
