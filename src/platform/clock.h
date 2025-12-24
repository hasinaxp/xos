#pragma once
#include "../common/types.h"
#include "idt.h"

#define SYS_PIT_CONTROL_PORT 0x43
#define SYS_PIT_DATA_PORT 0x40
#define SYS_PIT_FREQUENCY 1193182 // Base frequency of PIT
#define SYS_DESIRED_FREQUENCY 100 // 100Hz for the OS clock

static u32 ticks = 0;

char buffer[256];

IRQ_CALLBACK clock_handler()
{
    IRQ_BEGIN_CODE();
    ticks++;
    outb(0x20, 0x20);
    IRQ_END_CODE();
}

void clock_init()
{
    u32 divisor = SYS_PIT_FREQUENCY / SYS_DESIRED_FREQUENCY;

    outb(SYS_PIT_CONTROL_PORT, 0x36); // Channel 0, lobyte/hibyte, rate generator
    outb(SYS_PIT_DATA_PORT, (u8)(divisor & 0xFF));
    outb(SYS_PIT_DATA_PORT, (u8)((divisor >> 8) & 0xFF));

    set_idt_gate(IRQ_CLOCK_IRQ, (u32)clock_handler);
    u8 mask = inb(0x21); // Read the current mask
    mask &= ~0x01;       // Unmask IRQ0 (clock)
    outb(0x21, mask);
};
