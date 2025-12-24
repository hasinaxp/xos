#pragma once
#include "multiboot.h"
#include "gfx.h"
#include "ata.h"
#include "clock.h"
#include "../common/str.h"


#define SYS_MEM_RESERVE_SPACE   (1024 * 1024)
#define PROG_MEM_RESERVE_SPACE  (64 * 1024 * 1024)
#define PROG_MEM_BLOCK_SIZE     64
#define PROG_MEM_STACK_SIZE     (1024 * 1024)

#define SYS_MEM_FILE_LBA        10      

#define SYS_MEM_MAGIC           123124

typedef struct {
    u32 magic;
    u32 clock;
    struct {
        u32 background_color;
        u32 forground_color;
        u32 primary_color;
        u32 font_size;
    } ui;
} sys_mem_data_t;




typedef struct {
    u8 stack[PROG_MEM_STACK_SIZE]; /* grows downwards*/
    u32 stack_top;
} prog_mem_data_t;



static void * sys_mem_base_ptr = NULL;
static void * prog_mem_base_ptr = NULL;

sys_mem_data_t * sys_mem_data = (sys_mem_data_t *)NULL;
prog_mem_data_t * prog_mem_data = (prog_mem_data_t *)NULL;




fn memory_init() {
    sys_mem_base_ptr = (void *)(sys_meminfo.start_address);
    prog_mem_base_ptr = (void *)(sys_meminfo.start_address + SYS_MEM_RESERVE_SPACE);

    sys_mem_data = (sys_mem_data_t *)(sys_mem_base_ptr);
    prog_mem_data = (prog_mem_data_t *)(prog_mem_base_ptr);
    prog_mem_data->stack_top = PROG_MEM_STACK_SIZE;

    /* system memory initialization */
    ata_read_sector(SYS_MEM_FILE_LBA, (void*) sys_mem_data);
    for(int i = 0; i < 10000; i++);
    if(sys_mem_data->magic != SYS_MEM_MAGIC) {
        
        draw_text(4, 100, "could not read from disk. creating new entry!!", 12, COLOR_WHITE);
        sys_mem_data->magic = SYS_MEM_MAGIC;
        sys_mem_data->clock = 0;
        sys_mem_data->ui.background_color = COLOR_DEEP_CYAN;
        sys_mem_data->ui.primary_color = COLOR_BLUE;
        sys_mem_data->ui.forground_color = COLOR_WHITE;
        sys_mem_data->ui.font_size = 12;
    } 
   
    ticks = sys_mem_data->clock;

}

static int tempy = 100;
fn sys_memory_commit() {
    sys_mem_data->clock = ticks;
    ata_write_sector(SYS_MEM_FILE_LBA, (void*) sys_mem_data);
}


void * prog_stack_push(u32 size, void * data) {
    if((i32)prog_mem_data->stack_top - size <= 0) {
        return NULL;
    }
    prog_mem_data->stack_top -= size;
    void * stk_ptr = prog_mem_data->stack + prog_mem_data->stack_top;
    memcpy(stk_ptr, data, size);
    return stk_ptr;
}

boolean prog_stack_pop(u32 size) {
    if(prog_mem_data->stack_top + size > PROG_MEM_STACK_SIZE)
        return false;
    prog_mem_data->stack_top += size;
    return true;
}
