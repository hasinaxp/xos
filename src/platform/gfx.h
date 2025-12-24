#pragma once
#include "../common/types.h"
#include "gfx_font32.h"
#include "gfx_color.h"



static volatile u32* FRAMEBUFFER = (u32*)0xE0000000;
static volatile int FB_WIDTH = 800;
static volatile int FB_HEIGHT = 600;



fn framebuffer_init(u32 phys_addr, int width, int height, int pitch) {
    FRAMEBUFFER = (u32*)phys_addr;
    FB_WIDTH  = width;
    FB_HEIGHT = height;
}


fn putpixel(int x, int y, u32 color) {
    FRAMEBUFFER[y * FB_WIDTH + x] = color;
}

u32 getpixel(int x, int y) {
    return FRAMEBUFFER[y * FB_WIDTH + x];
}

static u32 create_color(u8 r, u8 g, u8 b) {
    return (r << 16) | (g << 8) | b;
}



fn clear_screen(u32 color) {
    for (int y = 0; y < FB_HEIGHT; y++) {
        for (int x = 0; x < FB_WIDTH; x++) {
            putpixel(x, y, color);
        }
    }
}


static char font32_glyph_indices[256];



// Store per glyph: 32x32 alpha values
static u8 font32_alpha[95][32][32];

void font_preprocess() {
    for (int ch = 32; ch <= 126; ch++) {
        int index = font32_glyph_indices[ch];
        if (index == -1) continue;

        const u32* glyph = FONT_BITMAPS[index];
        int wordsPerRow = 32 / 8;

        for (int row = 0; row < 32; row++) {
            const u32* glyphRow = glyph + row * wordsPerRow;

            for (int word = 0; word < wordsPerRow; word++) {
                u32 data = glyphRow[word];
                for (int p = 0; p < 8; p++) {
                    int col = word * 8 + p;
                    u32 pixelVal = (data >> ((7 - p) * 4)) & 0xF;
                    font32_alpha[ch - 32][row][col] = pixelVal * 17; // 0–255
                }
            }
        }
    }
}

fn font32_init() {
    for (int i = 0; i < 256; i++) {
        font32_glyph_indices[i] = -1; // Mark all as unsupported
    }
    for (int i = 0; i < FONT_GLYPH_COUNT; i++) {
        char character = FONT_CHARSET[i];
        font32_glyph_indices[(u8)character] = i;
    }
    font_preprocess();
}


// 16-level gamma-corrected table (0-15 -> 0-255)
static const u8 gamma16[16] = {
    0, 5, 10, 18, 28, 42, 60, 80,
    105, 130, 160, 190, 215, 230, 245, 255
};

fn draw_char(int x, int y, char c, int font_size, u32 color) {
    if ((unsigned)(c - 32) > 94) return;

    // Get glyph alpha (always stored as 32×32)
    const u8 (*glyphAlpha)[32] = font32_alpha[c - 32];

    // Precompute src color channels
    const u32 src_r = (color >> 16) & 0xFF;
    const u32 src_g = (color >> 8) & 0xFF;
    const u32 src_b = color & 0xFF;

    for (int row = 0; row < font_size; row++) {
        for (int col = 0; col < font_size; col++) {
            // Scale coordinates from font_size → 32
            int src_y = row * FONT_GLYPH_SIZE / font_size;
            int src_x = col * FONT_GLYPH_SIZE / font_size;

            u32 alpha = glyphAlpha[src_y][src_x];
            if (alpha < 60) continue;   // skip transparent

            if (alpha >= 250) {  // fast opaque path
                putpixel(x + col, y + row, color);
                continue;
            }

            u32 dst = getpixel(x + col, y + row);
            u32 dst_r = (dst >> 16) & 0xFF;
            u32 dst_g = (dst >> 8) & 0xFF;
            u32 dst_b = dst & 0xFF;

            u32 inv_alpha = 255 - alpha;

            u32 r = (src_r * alpha + dst_r * inv_alpha + 127) / 255;
            u32 g = (src_g * alpha + dst_g * inv_alpha + 127) / 255;
            u32 b = (src_b * alpha + dst_b * inv_alpha + 127) / 255;

            putpixel(x + col, y + row, (r << 16) | (g << 8) | b);
        }
    }
}

fn draw_text(int x, int y, const char* str, int font_size, u32 color) {
    
    int orig_x = x;
    int adv = font_size * .65f;
    int line_height = font_size * .9f;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            y += line_height;
            x = orig_x;
            continue;
        }
        draw_char(x, y, str[i], font_size, color);
        x += adv; // advance by font size
    } 
}

point_t get_text_size(const char* str, int font_size) {
    point_t p;

    int adv = font_size * .65f;
    int line_height = font_size * .9f;
    int cur_x = 0;
    int max_x = 0;
    int lines = 1;

    char c;
    while ((c = *str++)) {
        if (c == '\n') {
            if (cur_x > max_x)
                max_x = cur_x;
            cur_x = 0;
            lines++;
        } else {
            cur_x += adv;
        }
    }

    if (cur_x > max_x)
        max_x = cur_x;

    p.x = max_x;
    p.y = lines * line_height;
    return p;
}


fn draw_rect(int x, int y, int w, int h, int line_width, u32 color, int radius) {
    if (radius < 0) radius = 0;
    if (radius > w/2) radius = w/2;
    if (radius > h/2) radius = h/2;

    for (int i = 0; i < line_width; i++) {
        int inner_r = radius - i;  // shrinking radius for inner borders
        int outer_r = radius;      // outer arc radius

        // --- Top ---
        for (int dx = radius; dx < w - radius; dx++) {
            putpixel(x + dx, y + i, color);
        }

        // --- Bottom ---
        for (int dx = radius; dx < w - radius; dx++) {
            putpixel(x + dx, y + h - 1 - i, color);
        }

        // --- Left ---
        for (int dy = radius; dy < h - radius; dy++) {
            putpixel(x + i, y + dy, color);
        }

        // --- Right ---
        for (int dy = radius; dy < h - radius; dy++) {
            putpixel(x + w - 1 - i, y + dy, color);
        }

        // --- Corners (quarter circles) ---
        for (int dy = 0; dy < radius; dy++) {
            for (int dx = 0; dx < radius; dx++) {
                int dist2 = dx*dx + dy*dy;

                // Outer boundary circle check
                if (dist2 >= (inner_r*inner_r) && dist2 <= (outer_r*outer_r)) {
                    // Top-left
                    putpixel(x + radius - 1 - dx, y + radius - 1 - dy, color);
                    // Top-right
                    putpixel(x + w - radius + dx, y + radius - 1 - dy, color);
                    // Bottom-left
                    putpixel(x + radius - 1 - dx, y + h - radius + dy, color);
                    // Bottom-right
                    putpixel(x + w - radius + dx, y + h - radius + dy, color);
                }
            }
        }
    }
}


fn fill_rect(int x, int y, int w, int h, u32 color, int radius) {
    if (radius < 0) radius = 0;
    if (radius > w/2) radius = w/2;
    if (radius > h/2) radius = h/2;

    for (int dy = 0; dy < h; dy++) {
        for (int dx = 0; dx < w; dx++) {
            boolean inside = true;

            // --- Top-left corner ---
            if (dx < radius && dy < radius) {
                int rx = radius - dx - 1;
                int ry = radius - dy - 1;
                if (rx*rx + ry*ry >= radius*radius) inside = false;
            }

            // --- Top-right corner ---
            else if (dx >= w - radius && dy < radius) {
                int rx = dx - (w - radius);
                int ry = radius - dy - 1;
                if (rx*rx + ry*ry >= radius*radius) inside = false;
            }

            // --- Bottom-left corner ---
            else if (dx < radius && dy >= h - radius) {
                int rx = radius - dx - 1;
                int ry = dy - (h - radius);
                if (rx*rx + ry*ry >= radius*radius) inside = false;
            }

            // --- Bottom-right corner ---
            else if (dx >= w - radius && dy >= h - radius) {
                int rx = dx - (w - radius);
                int ry = dy - (h - radius);
                if (rx*rx + ry*ry >= radius*radius) inside = false;
            }

            if (inside) {
                putpixel(x + dx, y + dy, color);
            }
        }
    }
}
