#pragma once
#include "../platform/memory.h"
#include "../platform/graphics.h"

#define UI_DEFAULT_PADDING              4
#define UI_DEFAULT_LINE_WIDTH           2
#define UI_DEFAULT_BORDER_RADIUS        8
#define UI_DEFAULT_STATUS_BAR_HEIGHT    20

static inline int get_default_font_size() {
    return sys_mem_data ? sys_mem_data->ui.font_size : 12;
}


static inline int get_default_bg_color() {
    return sys_mem_data ? sys_mem_data->ui.background_color: COLOR_CHARCOAL;
}


static inline int get_default_fg_size() {
    return sys_mem_data ? sys_mem_data->ui.forground_color: COLOR_WHITE;
}

static inline int get_default_primary_color() {
    return sys_mem_data ? sys_mem_data->ui.primary_color: COLOR_DARK_CYAN;
}


