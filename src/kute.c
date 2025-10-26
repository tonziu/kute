#include "kute.h"

#define MAX_WIDTH 1024
#define MAX_HEIGHT 1024

static uint32_t pixels[MAX_WIDTH * MAX_HEIGHT];
static int pw;
static int ph;

uint32_t* kute_get_pixels() 
{
    return pixels;
}

void kute_init(int width, int height) 
{
    if (width > MAX_WIDTH) width = MAX_WIDTH;
    if (height > MAX_HEIGHT) height = MAX_HEIGHT;
    
    pw = width;
    ph = height;

    for (int i = 0; i < pw * ph; i++) 
    {
        pixels[i] = 0;
    }
}

void kute_clear(uint32_t color)
{
    if (pw <= 0 || ph <= 0) return;

    for (int i = 0; i < pw * ph; ++i)
    {
        pixels[i] = color;
    }
}

void kute_rect(int x0, int y0, int rw, int rh, uint32_t color)
{
    if (pw <= 0 || ph <= 0) return;

    for (int y = y0; y < y0 + rh; ++y)
    {
        if (y >= ph) break;
        for (int x = x0; x < x0 + rw; ++x)
        {
            if (x >= pw) break;
            pixels[x + y * pw] = color;
        }
    }
}