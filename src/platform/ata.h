#pragma once
#include "../common/types.h"
#include "gfx.h"


fn ata_wait_bsy() {
    while (inb(0x1F7) & 0x80);
}

static inline int ata_wait_drq_safe() {
    u8 status;
    while (1) {
        status = inb(0x1F7);
        if (status & 0x01) return -1; // ERR
        if (status & 0x20) return -1; // DF
        if (!(status & 0x80) && (status & 0x08))
            return 0;
    }
}

fn ata_delay_400ns() {
    inb(0x1F7);
    inb(0x1F7);
    inb(0x1F7);
    inb(0x1F7);
}

fn ata_read_sector(u32 lba, void* buffer) {
    u16* buf = (u16*)buffer;

    ata_wait_bsy();

    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    ata_delay_400ns();

    outb(0x1F2, 1);
    outb(0x1F3, lba);
    outb(0x1F4, lba >> 8);
    outb(0x1F5, lba >> 16);
    outb(0x1F7, 0x20);   // READ SECTOR

    ata_wait_drq_safe();

    insw(0x1F0, buf, 256);

    ata_wait_bsy();
}


fn ata_write_sector(u32 lba, const void* buffer) {
    const u16* buf = (const u16*)buffer;

    ata_wait_bsy();

    // Drive select (MANDATORY delay after)
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    ata_delay_400ns();

    outb(0x1F2, 1);
    outb(0x1F3, (u8) lba);
    outb(0x1F4, (u8)(lba >> 8));
    outb(0x1F5, (u8)(lba >> 16));
    outb(0x1F7, 0x30);   // WRITE SECTOR

    ata_wait_drq_safe();

    outsw(0x1F0, buf, 256);
    
    
    // Wait for completion
    ata_wait_bsy();
}

