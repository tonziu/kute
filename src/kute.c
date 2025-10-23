#include "kute.h"

uint32_t kute_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return a << 24 | b << 16 | g << 8 | r;
}

void kute_clear(uint32_t *pixels, int pw, int ph, uint32_t color)
{
    if (!pixels || pw <= 0 || ph <= 0) return;

    for (int i = 0; i < pw * ph; ++i)
    {
        pixels[i] = color;
    }
}

void kute_fill_rect(uint32_t *pixels, int pw, int ph, int x0, int y0, int rw, int rh, uint32_t color)
{
    if (!pixels || pw <= 0 || ph <= 0) return;

    if (rw < 0)
    {
        rw *= -1;
        x0 -= rw;
    }

    if (rh < 0)
    {
        rh *= -1;
        y0 -= rh;
    }

    for (int x = x0; x < x0 + rw; ++x)
    {
        for (int y = y0; y < y0 + rh; ++y)
        {
            if (x >= 0 && x < pw && y >= 0 && y < ph)
            {
                pixels[x + y * pw] = color;
            }
        }
    }
}