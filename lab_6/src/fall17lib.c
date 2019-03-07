//#include "miscutil.h"
#include "fall17lib.h"
#include "stdint.h"
#include "ctype.h"
#include "string.h"

static const char *conversionchars = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz";

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 * Modified from http://www.strudel.org.uk/itoa
 */
char *itoa(int value, char *result, int base)
{
    char *ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    // check that the base if valid
    if (base < 2 || base > 36) {
        *result = '\0';
        return result;
    }

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = conversionchars[35 + (tmp_value - value * base)];
    } while (value);

    // Apply negative sign
    if (tmp_value < 0)
        *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

char *utoa(unsigned int value, char *result, int base)
{
    char *ptr = result, *ptr1 = result, tmp_char;
    unsigned int tmp_value;

    // check that the base is valid
    if (base < 2 || base > 36) {
        *result = '\0';
        return result;
    }

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = conversionchars[35 + (tmp_value - value * base)];
    } while (value);

    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

char *utoar(unsigned int value, char *result, int base, unsigned int width)
{
    unsigned int ui_len;
    utoa(value, result, base);

    ui_len = strlen(result);

    // Right-align by shifting (including the null terminator!) and filling with '0's.
    memmove(result + width - ui_len, result, ui_len + 1);
    memset(result, '0', width - ui_len);

    return result;
}

char *upperstring(char *psz_str)
{
    while (*psz_str) {
        *psz_str = toupper(*psz_str);
        ++psz_str;
    }
}

uint8_t lut3pt[] = {
    0b00000, 0b00011, 0b00110, 0b01001, 0b01100, 0b10000, 0b10011, 0b10110, 0b11001, 0b11100,
};

uint8_t dtou3p5(uint8_t ipart, uint8_t fpart)
{
    uint8_t fpart3p5;

    fpart = MIN(fpart, 9);

    return ipart << 5 | lut3pt[fpart];
}

uint8_t u3p5toipart(uint8_t fixedpt)
{
    return fixedpt >> 5;
}

uint8_t u3p5tofpart(uint8_t fixedpt)
{
    uint8_t fpart = (fixedpt & 0b11111);
    uint8_t i;

    for (i = 0; i < NELEM(lut3pt); ++i) {
        if (lut3pt[i] >= fpart)
            return i;
    }

    return 9;
}
