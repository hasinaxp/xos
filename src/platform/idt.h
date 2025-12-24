#pragma once
#include "../common/types.h"

#define IDT_SIZE 256

#define IRQ_CLOCK_IRQ 32
#define IRQ_KEYBOARD_IRQ 33
#define IRQ_MOUSE_IRQ 44

#define IRQ_CALLBACK __attribute__((naked)) void

#define IRQ_BEGIN_CODE()      \
    asm volatile("pusha");    \
    asm volatile("push %ds"); \
    asm volatile("push %es"); \
    asm volatile("push %fs"); \
    asm volatile("push %gs");

#define IRQ_END_CODE()       \
    asm volatile("pop %gs"); \
    asm volatile("pop %fs"); \
    asm volatile("pop %es"); \
    asm volatile("pop %ds"); \
    asm volatile("popa");    \
    asm volatile("iret");

struct gate_desc
{
    u16 offset_low;
    u16 selector;
    u8 zero;
    u8 type_attr;
    u16 offset_high;
} __attribute__((packed));

struct idt_desc
{
    u16 limit;
    u32 base;
} __attribute__((packed));

struct gate_desc idt[IDT_SIZE] = {0};
struct idt_desc idt_descriptor = {0};
int ix = 0;
char buf[32];

IRQ_CALLBACK ignore_interrupt()
{
    IRQ_BEGIN_CODE();
    outb(0x20, 0x20); // EOI
    IRQ_END_CODE();
}

fn set_idt_gate(u8 num, u32 handler)
{
    idt[num].offset_low = (u16)(handler & 0xFFFF);
    idt[num].selector = 0x08;
    idt[num].zero = 0;
    idt[num].type_attr = 0x8E;
    idt[num].offset_high = (u16)((handler >> 16) & 0xFFFF);
}

fn enable_interrupts()
{
    asm volatile("sti");
}

fn disable_interrupts()
{
    asm volatile("cli");
}

fn idt_init()
{
    for (int i = 0; i < 32; i++)
    {
        set_idt_gate(i, (u32)ignore_interrupt);
    }
}

fn remap_pic()
{
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    // mask IRQ14 (slave PIC), needed for ATA to work properly
    outb(0xA1, inb(0xA1) | (1 << 6));
}

fn activate_interrupts()
{
    remap_pic();
    enable_interrupts();
}

fn idt_install()
{
    idt_descriptor.limit = sizeof(idt) - 1;
    idt_descriptor.base = (u32)&idt;

    asm volatile("lidt %0" : : "m"(idt_descriptor));
    activate_interrupts();
}
