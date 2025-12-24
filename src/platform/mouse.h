#pragma once
#include "../common/types.h"
#include "idt.h"

static int mouse_x = 0;
static int mouse_y = 0;
static u8 mouse_cycle = 0;
static i8 mouse_bytes[3];
static u8 mouse_left = 0;
static u8 mouse_right = 0;
static u8 mouse_middle = 0;

static int mouse_get_x() { return mouse_x; }
static int mouse_get_y() { return mouse_y; }
static int mouse_get_left() { return mouse_left; }
static int mouse_get_right() { return mouse_right; }
static int mouse_get_middle() { return mouse_middle; }

fn mouse_wait(u8 type) {
    u32 timeout = 100000;
    if (type == 0) {
        while (timeout--) if (inb(0x64) & 1) return;
    } else {
        while (timeout--) if (!(inb(0x64) & 2)) return;
    }
}

fn mouse_write(u8 data) {
    mouse_wait(1);
    outb(0x64, 0xD4);
    mouse_wait(1);
    outb(0x60, data);
}

static int mouse_read() {
    mouse_wait(0);
    return inb(0x60);
}

IRQ_CALLBACK mouse_handler() {
    IRQ_BEGIN_CODE();

    u8 status = inb(0x64);
    if (!(status & 0x01)) {
        outb(0x20, 0x20);
        outb(0xA0, 0x20);
        IRQ_END_CODE();
    }

    i8 data = inb(0x60);

    switch (mouse_cycle) {
        case 0:
            if (data & 0x08) { // sync
                mouse_bytes[0] = data;
                mouse_cycle = 1;
            }
            break;
        case 1:
            mouse_bytes[1] = data;
            mouse_cycle = 2;
            break;
        case 2:
            mouse_bytes[2] = data;
            mouse_cycle = 0;

            i32 dx = mouse_bytes[1];
            i32 dy = mouse_bytes[2];

            if (mouse_bytes[0] & 0x10) dx |= 0xFFFFFF00; // sign extend
            if (mouse_bytes[0] & 0x20) dy |= 0xFFFFFF00;

            mouse_x += dx;
            mouse_y -= dy; // invert Y

            mouse_left   = mouse_bytes[0] & 0x01;
            mouse_right  = mouse_bytes[0] & 0x02;
            mouse_middle = mouse_bytes[0] & 0x04;
            break;
    }

    outb(0x20, 0x20);
    outb(0xA0, 0x20);
    IRQ_END_CODE();
}

fn mouse_init() {
    // Enable auxiliary device
    mouse_wait(1);
    outb(0x64, 0xA8);

    // Enable IRQ12
    mouse_wait(1);
    outb(0x64, 0x20);
    mouse_wait(0);
    u8 status = inb(0x60) | 2;
    mouse_wait(1);
    outb(0x64, 0x60);
    mouse_wait(1);
    outb(0x60, status);

    // Default settings
    mouse_write(0xF6);
    mouse_read(); // ack

    // Enable mouse
    mouse_write(0xF4);
    mouse_read(); // ack

    // Hook into IDT
    set_idt_gate(IRQ_MOUSE_IRQ, (u32)mouse_handler);

    // Unmask IRQ12 on slave PIC
    u8 mask = inb(0xA1);
    mask &= ~0x10;
    outb(0xA1, mask);
}
