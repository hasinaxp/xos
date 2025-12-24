#pragma once
#include "defaults.h"
#include "window.h"



fn clear_desktop() {
    clear_screen(sys_mem_data->ui.desktop_color);
}



static u32 last_ticks = 0;

void draw_clock(u32 x, u32 y) {
    
    u32 fsize = get_default_font_size();
    char buf[64] = {0};

    int s = (ticks / 100) % 60;
    int m = (ticks / 100 / 60) % 60;
    int h = (ticks / 100 / 3600) % 24;

    if(h < 10) append_str(buf, "0");
    append_int(buf, h, 10);
    append_str(buf, ":");
    if(m < 10) append_str(buf, "0");
    append_int(buf, m, 10);
    append_str(buf, ":");
    if(s < 10) append_str(buf, "0");
    append_int(buf, s, 10);
    
    

    draw_text(x + fsize * 1.5,
                y,
                buf,
                fsize,
                COLOR_YELLOW);

    last_ticks = ticks;

}


fn draw_status_bar() {
    fill_rect(
        0,
        0,
        FB_WIDTH,
        UI_DEFAULT_STATUS_BAR_HEIGHT,
        COLOR_BLACK,
        0
    );

    u32 font_size = get_default_font_size();
    
    u32 clock_x_offset = font_size * 2;
    u32 clock_y_offset = (UI_DEFAULT_STATUS_BAR_HEIGHT - font_size) / 2;
    draw_text(UI_DEFAULT_PADDING,
                clock_y_offset,
                ">.</",
                font_size,
                COLOR_RED);
    draw_clock(clock_x_offset, clock_y_offset);
}


fn draw_window(window_t * window) {
    fill_rect(
        window->x,
        window->y,
        window->width,
        window->height,
        get_default_bg_color(),
        UI_DEFAULT_BORDER_RADIUS
    );
    
    
    draw_rect(
        window->x,
        window->y,
        window->width,
        window->height,
        UI_DEFAULT_LINE_WIDTH,
        get_default_primary_color(),
        UI_DEFAULT_BORDER_RADIUS
    );
}


