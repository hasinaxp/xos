#pragma once
#include "../common/types.h"
#include "idt.h"


static const char scancode_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b', // 0x0E = Backspace
    '\t', // 0x0F = Tab
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',    // 0x1C = Enter
    0,    // 0x1D = Ctrl
    'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,    // 0x2A = Left Shift
    '\\','z','x','c','v','b','n','m',',','.','/',
    0,    // 0x36 = Right Shift
    '*',
    0,    // Alt
    ' ',  // Spacebar
    0,    // Caps Lock
    // rest are function keys, etc. → ignore for now
};

static char input_buffer[256] = {0};
static int input_buffer_index = 0;

fn clear_input_buffer() {
    input_buffer_index = 0;
    for(int i=0; i<sizeof(input_buffer); i++) input_buffer[i] = 0;
}
fn parse_keyboard_input(char c) {
    if (!c) return;
    input_buffer[input_buffer_index++] = c;
    if (input_buffer_index >= sizeof(input_buffer) -1)  {
        clear_input_buffer();
    }
}

IRQ_CALLBACK keyboard_handler()
{
    IRQ_BEGIN_CODE();
    u8 scancode = inb(0x60);

    if (scancode & 0x80) {
        // Key released → ignore for now
    } else {
        char c = scancode_ascii[scancode];
        parse_keyboard_input(c);
    }
    outb(0x20, 0x20);
    IRQ_END_CODE();
}

fn keyboard_init() {
    set_idt_gate(IRQ_KEYBOARD_IRQ, (u32)keyboard_handler);
    u8 mask = inb(0x21); // Read the current mask
    mask &= ~0x02;       // Unmask IRQ1 (keyboard)
    outb(0x21, mask);
}