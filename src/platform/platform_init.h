#pragma once
#include "idt.h"
#include "clock.h"
#include "keyboard.h"
#include "multiboot.h"
#include "mouse.h"
#include "graphics.h"

fn platform_init(multiboot_info_t* mb_info) {

    
    sys_init_mem_info(mb_info);
    
    idt_init();
    
    clock_init();
    keyboard_init();
    mouse_init();
    
    idt_install();

    memory_init();

    graphics_init(mb_info);
    

    __asm__ volatile("sti");
    
}
