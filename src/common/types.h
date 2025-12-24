#pragma once

#define fn static inline void 


typedef int i32;
typedef unsigned int u32;
typedef short i16;
typedef unsigned short u16;
typedef char i8;
typedef unsigned char u8;
typedef long long i64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;

#define NULL ((void*)0)
#define true 1
#define false 0

typedef u8 boolean;


typedef struct {
    f32 x;
    f32 y;
} point_t;


typedef struct {
    f32 x;
    f32 y;
    f32 w;
    f32 h;
} rect_t;


#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)


static inline void outb(u16 port, u8 val) {
    asm volatile ("outb %0, %1" : : "a"(val), "d"(port));
}
static inline u8 inb(u16 port) {
    u8 ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}
static inline void outw(u16 port, u16 val) {
    asm volatile ("outw %0, %1"
                  :
                  : "a"(val), "d"(port)
                  : "memory");
}

static inline void outsw(u16 port, const void* addr, u32 count) {
    asm volatile (
        "rep outsw"
        : "+S"(addr), "+c"(count)
        : "d"(port)
        : "memory"
    );
}

static inline u16 inw(u16 port) {
    u16 ret;
    asm volatile ("inw %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}


static inline void insw(u16 port, void* addr, u32 count) {
    asm volatile (
        "rep insw"
        : "+D"(addr), "+c"(count)
        : "d"(port)
        : "memory"
    );
}

static inline void io_wait(void) {
    __asm__ __volatile__ ("outb %%al, $0x80" : : "a"(0));
}

static void memset(void* dest, int val, u32 len) {
    u8* ptr = (u8*)dest;
    while(len--) {
        *ptr++ = (u8)val;
    }
}

static void memcpy(void* dest, const void* src, u32 len) {
    u8* d = (u8*)dest;
    const u8* s = (const u8*)src;
    while(len--) {
        *d++ = *s++;
    }
}

