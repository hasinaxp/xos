#pragma once
#include "types.h"

static inline int kstrlen(const char *str) {
    int len = 0;
    while (str && str[len]) len++;
    return len;
}

static inline char *kstrcpy(char *dest, const char *src) {
    char *rd = dest;
    while ((*dest++ = *src++));
    return rd;
}

static inline char *kstrcat(char *dest, const char *src) {
    char *rd = dest;
    while (*dest) dest++;
    while ((*dest++ = *src++));
    return rd;
}

static inline int to_hex_str(unsigned long value, char *str, int width) {
    if (!str || width < 1) return 0;
    const char *hex_chars = "0123456789abcdef";
    char *p = str;
    for (int i = (width - 1) * 4; i >= 0; i -= 4) {
        *p++ = hex_chars[(value >> i) & 0xF];
    }
    *p = '\0';
    return width;
}

void to_hex64_str(u64 value, char* out, int digits) {
    static const char hex[] = "0123456789ABCDEF";

    for (int i = digits - 1; i >= 0; i--) {
        out[i] = hex[value & 0xF];
        value >>= 4;
    }
    out[digits] = '\0';
}

/* Integer to string: handles negative numbers for base 10 safely.
   Returns the pointer to the buffer (str).
   base: supported 2..36 (digits >9 use 'a'..'z'). */
static inline char *kitoa(int value, char *str, int base) {
    if (!str) return (char *)NULL;
    if (base < 2 || base > 36) {   /* invalid base -> empty string */
        *str = '\0';
        return str;
    }

    char *p = str;
    unsigned int u;

    if (base == 10 && value < 0) {
        *p++ = '-';
        u = (unsigned int)(-(long)value);
    } else {
        u = (unsigned int)value;
    }

    char *start = p;

    if (u == 0) {
        *p++ = '0';
        *p = '\0';
        return str;
    }

    while (u) {
        unsigned int rem = u % (unsigned)base;
        *p++ = (rem < 10) ? (char)('0' + rem) : (char)('a' + (rem - 10));
        u /= base;
    }

    *p = '\0';
    char *lo = start;
    char *hi = p - 1;
    while (lo < hi) {
        char tmp = *lo;
        *lo++ = *hi;
        *hi-- = tmp;
    }

    return str;
}

static inline char *kftoa(float value, char *str, int precision) {
    if (!str) return (char *)0;
    if (precision < 0) precision = 0;

    char *p = str;

    if (value < 0.0f) {
        *p++ = '-';
        value = -value;
    }

    int ipart = (int)value;
    float fpart = value - (float)ipart;

    char tmp[16];
    kitoa(ipart, tmp, 10);
    char *t = tmp;
    while (*t)
        *p++ = *t++;

    if (precision == 0) {
        *p = '\0';
        return str;
    }

    *p++ = '.';

    while (precision--) {
        fpart *= 10.0f;
        int digit = (int)fpart;
        *p++ = (char)('0' + digit);
        fpart -= digit;
    }

    *p = '\0';
    return str;
}


static inline void append_str(char *dest, const char *text) {
    kstrcat(dest, text);
}

static inline void append_line(char *dest, const char *text) {
    kstrcat(dest, text);
    kstrcat(dest, "\n");
}

static inline void append_int(char *dest, int value, int base) {
    char buf[36];
    kitoa(value, buf, base);
    kstrcat(dest, buf);
}

static inline void append_float(char *dest, float value, int pricision) {
    char buf[36];
    kftoa(value, buf, pricision);
    kstrcat(dest, buf);
}

