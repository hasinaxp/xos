#ifndef XOS_PLATFORM_DISPLAY_TT_H
#define XOS_PLATFORM_DISPLAY_TT_H

#include "../common/str.h"

#define XOS_DISPLAY_TT_VDEOMEM 0xB8000
#define XOS_DISPLAY_TT_COLS 80
#define XOS_DISPLAY_TT_ROWS 25
#define XOS_DISPLAY_TT_CELLSIZE 2

enum XOS_DISPLAY_TT_COLOR {
    XOS_DISPLAY_TT_COLOR_BLACK = 0,
    XOS_DISPLAY_TT_COLOR_BLUE = 1,
    XOS_DISPLAY_TT_COLOR_GREEN = 2,
    XOS_DISPLAY_TT_COLOR_CYAN = 3,
    XOS_DISPLAY_TT_COLOR_RED = 4,
    XOS_DISPLAY_TT_COLOR_MAGENTA = 5,
    XOS_DISPLAY_TT_COLOR_BROWN = 6,
    XOS_DISPLAY_TT_COLOR_LIGHT_GREY = 7,
    XOS_DISPLAY_TT_COLOR_DARK_GREY = 8,
    XOS_DISPLAY_TT_COLOR_LIGHT_BLUE = 9,
    XOS_DISPLAY_TT_COLOR_LIGHT_GREEN = 10,
    XOS_DISPLAY_TT_COLOR_LIGHT_CYAN = 11,
    XOS_DISPLAY_TT_COLOR_LIGHT_RED = 12,
    XOS_DISPLAY_TT_COLOR_LIGHT_MAGENTA = 13,
    XOS_DISPLAY_TT_COLOR_LIGHT_BROWN = 14,
    XOS_DISPLAY_TT_COLOR_WHITE = 15
};

static int disp_tt_cursor_x = 0;
static int disp_tt_cursor_y = 0;

static void disp_tt_clear_screen() {
    volatile char *video = (volatile char*)XOS_DISPLAY_TT_VDEOMEM;
    for(int i=0; i < XOS_DISPLAY_TT_COLS * XOS_DISPLAY_TT_ROWS * XOS_DISPLAY_TT_CELLSIZE; i++) {
        video[i] = 0;
    }
}



static void disp_tt_print_col(
    const char * str,
    int len /* -1 for strlen */,
    enum XOS_DISPLAY_TT_COLOR fg,
    enum XOS_DISPLAY_TT_COLOR bg
) {
    if(len == -1) {
        len = kstrlen(str);
    }
    volatile char *video = (volatile char*)XOS_DISPLAY_TT_VDEOMEM;
    char color_code = (bg << 4) | (fg & 0x0F);
    int offset = (disp_tt_cursor_y * XOS_DISPLAY_TT_COLS + disp_tt_cursor_x) * XOS_DISPLAY_TT_CELLSIZE;
    for(int i=0; i < len; i++) {
        if(str[i] == '\n' || disp_tt_cursor_x >= XOS_DISPLAY_TT_COLS) {
            disp_tt_cursor_x = 0;
            disp_tt_cursor_y++;
            if(disp_tt_cursor_y >= XOS_DISPLAY_TT_ROWS) {
                disp_tt_cursor_y = 0;
                
            }
            offset = (disp_tt_cursor_y * XOS_DISPLAY_TT_COLS + disp_tt_cursor_x) * XOS_DISPLAY_TT_CELLSIZE;
            continue;
        }
        video[offset] = str[i];
        video[offset + 1] = color_code;
        disp_tt_cursor_x++;
        offset += XOS_DISPLAY_TT_CELLSIZE;
    }
}

static void disp_tt_set_cursor(int x, int y) {
    if(x < 0 || x >= XOS_DISPLAY_TT_COLS || y < 0 || y >= XOS_DISPLAY_TT_ROWS) {
        return;
    }
    disp_tt_cursor_x = x;
    disp_tt_cursor_y = y;
}

static void disp_tt_print(const char * str) {
    disp_tt_print_col(
        str,
        -1,
        XOS_DISPLAY_TT_COLOR_LIGHT_GREY,
        XOS_DISPLAY_TT_COLOR_BLACK);
}






#endif // XOS_PLATFORM_DISPLAY_TT_H