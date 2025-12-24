#pragma once
#include "gfx_color.h"
#include "gfx_font32.h"
#include "multiboot.h"
#include "memory.h"

static volatile u32* FRAMEBUFFER = (u32*)0xE0000000;
static volatile u32* BACKBUFFER = (u32*)NULL;
static volatile int FB_WIDTH = 800;
static volatile int FB_HEIGHT = 600;

static char font32_glyph_indices[256];
static u8 font32_alpha[95][32][32];


// 16-level gamma-corrected table (0-15 -> 0-255)
static const u8 gamma16[16] = {
    0, 5, 10, 18, 28, 42, 60, 80,
    105, 130, 160, 190, 215, 230, 245, 255
};


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


fn graphics_init(multiboot_info_t * mbi) {
    FRAMEBUFFER = (u32*) mbi->framebuffer_addr;
    FB_WIDTH = mbi->framebuffer_width;
    FB_HEIGHT = mbi->framebuffer_height;
    BACKBUFFER = sys_graphics_back_buffer;

    font32_init();
}



fn graphics_present() {
    u32 size = FB_WIDTH * FB_HEIGHT * sizeof(int);
    memcpy((void*)FRAMEBUFFER, (void*)BACKBUFFER, size);
}

fn putpixel(int x, int y, u32 color) {
    BACKBUFFER[y * FB_WIDTH + x] = color;
}


u32 getpixel(int x, int y) {
    return BACKBUFFER[y * FB_WIDTH + x];
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

#define FP_SHIFT 16
#define FP_ONE   (1 << FP_SHIFT)

static inline u32 blend255(u32 src, u32 dst, u32 a) {
    return (src * a + dst * (255 - a) + 128) >> 8;
}

void draw_char(int x, int y, char c, int font_size, u32 color)
{
    if ((unsigned)(c - 32) > 94) return;

    const u8 (*glyph)[32] = font32_alpha[c - 32];

    const u32 src_r = (color >> 16) & 0xFF;
    const u32 src_g = (color >> 8)  & 0xFF;
    const u32 src_b =  color        & 0xFF;

    /* Precompute scaling tables */
    int sx[64];   // supports font_size up to 64
    int sy[64];

    const int step = (FONT_GLYPH_SIZE << FP_SHIFT) / font_size;

    int acc = 0;
    for (int i = 0; i < font_size; i++) {
        sx[i] = acc >> FP_SHIFT;
        acc += step;
    }

    acc = 0;
    for (int i = 0; i < font_size; i++) {
        sy[i] = acc >> FP_SHIFT;
        acc += step;
    }

    for (int row = 0; row < font_size; row++) {
        const u8 *g = glyph[sy[row]];
        int py = y + row;

        for (int col = 0; col < font_size; col++) {
            u32 a = g[sx[col]];
            if (a < 60) continue;

            int px = x + col;

            if (a >= 250) {
                putpixel(px, py, color);
                continue;
            }

            u32 dst = getpixel(px, py);

            u32 r = blend255(src_r, (dst >> 16) & 0xFF, a);
            u32 g = blend255(src_g, (dst >>  8) & 0xFF, a);
            u32 b = blend255(src_b,  dst        & 0xFF, a);

            putpixel(px, py, (r << 16) | (g << 8) | b);
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

fn draw_rect(int x, int y, int w, int h, int lw, u32 color, int r)
{
    if (lw <= 0) return;

    if (r < 0) r = 0;
    if (r > w / 2) r = w / 2;
    if (r > h / 2) r = h / 2;

    /* ---------- Fast straight edges ---------- */
    for (int i = 0; i < lw; i++) {
        int ty = y + i;
        int by = y + h - 1 - i;

        for (int dx = r; dx < w - r; dx++) {
            putpixel(x + dx, ty, color);
            putpixel(x + dx, by, color);
        }

        int lx = x + i;
        int rx = x + w - 1 - i;

        for (int dy = r; dy < h - r; dy++) {
            putpixel(lx, y + dy, color);
            putpixel(rx, y + dy, color);
        }
    }

    if (r == 0) return;

    /* ---------- Precompute radius² ---------- */
    int r2[64];  // supports radius up to 63
    for (int i = 0; i < r; i++)
        r2[i] = i * i;

    /* ---------- Rounded corners ---------- */
    for (int dy = 0; dy < r; dy++) {
        int dy2 = r2[dy];

        for (int dx = 0; dx < r; dx++) {
            int dist2 = dy2 + r2[dx];

            /* Outer circle test once */
            if (dist2 > r * r)
                continue;

            /* Draw thickness layers */
            for (int i = 0; i <= lw; i++) {
                int ir = r - i;
                if (ir <= 0) break;
                if (dist2 < ir * ir) continue;

                int px1 = x + r - 1 - dx;
                int px2 = x + w - r + dx;
                int py1 = y + r - 1 - dy;
                int py2 = y + h - r + dy;

                putpixel(px1, py1, color); // TL
                putpixel(px2, py1, color); // TR
                putpixel(px1, py2, color); // BL
                putpixel(px2, py2, color); // BR
            }
        }
    }
}

void fill_rect(int x, int y, int w, int h, u32 color, int r)
{
    if (w <= 0 || h <= 0) return;

    if (r < 0) r = 0;
    if (r > w / 2) r = w / 2;
    if (r > h / 2) r = h / 2;

    /* No radius: plain fill */
    if (r == 0) {
        for (int dy = 0; dy < h; dy++) {
            int py = y + dy;
            for (int px = x; px < x + w; px++)
                putpixel(px, py, color);
        }
        return;
    }

    int r2 = r * r;

    /* Precompute left cut for each row of the corner */
    int cut[64];  // r <= 64 assumed

    for (int dy = 0; dy < r; dy++) {
        int cy = r - 1 - dy;          // distance from circle center
        int cy2 = cy * cy;

        int dx = 0;
        while ((r - 1 - dx) * (r - 1 - dx) + cy2 > r2)
            dx++;

        cut[dy] = dx;
    }

    /* Top rounded part */
    for (int dy = 0; dy < r; dy++) {
        int py = y + dy;
        int c  = cut[dy];

        for (int px = x + c; px < x + w - c; px++)
            putpixel(px, py, color);
    }

    /* Middle rectangle */
    for (int dy = r; dy < h - r; dy++) {
        int py = y + dy;
        for (int px = x; px < x + w; px++)
            putpixel(px, py, color);
    }

    /* Bottom rounded part */
    for (int dy = 0; dy < r; dy++) {
        int py = y + h - 1 - dy;
        int c  = cut[dy];

        for (int px = x + c; px < x + w - c; px++)
            putpixel(px, py, color);
    }
}

static inline int isqrt(int n)
{
    int x = n;
    int y = (x + 1) >> 1;
    while (y < x) {
        x = y;
        y = (x + n / x) >> 1;
    }
    return x;
}

void draw_circle(int cx, int cy, int r, int lw, u32 color)
{
    if (r <= 0 || lw <= 0) return;

    int r_outer = r;
    int r_inner = r - lw;
    if (r_inner < 0) r_inner = 0;

    int r_outer2 = r_outer * r_outer;
    int r_inner2 = r_inner * r_inner;

    for (int y = -r_outer; y <= r_outer; y++) {
        int y2 = y * y;
        if (y2 > r_outer2) continue;

        int xo = isqrt(r_outer2 - y2);
        int xi = (y2 < r_inner2) ? isqrt(r_inner2 - y2) : -1;

        int py = cy + y;

        /* left span */
        for (int x = -xo; x <= -xi - 1; x++)
            putpixel(cx + x, py, color);

        /* right span */
        for (int x = xi + 1; x <= xo; x++)
            putpixel(cx + x, py, color);
    }
}

void draw_line(int x0, int y0, int x1, int y1, int t, u32 color)
{
    if (t <= 0) return;

    int dx = ABS(x1 - x0);
    int dy = ABS(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    int half = t >> 1;

    for (;;) {
        /* Draw thickness span */
        if (dx >= dy) {
            /* shallow line → vertical thickness */
            for (int i = -half; i <= half; i++)
                putpixel(x0, y0 + i, color);
        } else {
            /* steep line → horizontal thickness */
            for (int i = -half; i <= half; i++)
                putpixel(x0 + i, y0, color);
        }

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = err << 1;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
}







