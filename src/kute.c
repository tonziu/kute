#include "kute.h"

uint32_t kute_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return a << 24 | b << 16 | g << 8 | r;
}

void kute_clear(uint32_t *pixels, int pw, int ph, uint32_t color)
{
    for (int i = 0; i < pw * ph; ++i)
    {
        pixels[i] = color;
    }
}