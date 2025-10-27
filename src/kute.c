#include "kute.h"
#include <stdbool.h>

#define MAX_WIDTH 1024
#define MAX_HEIGHT 1024

#define KUTE_SWAP(a, b, T) do {T temp = *a; *a = *b; *b = temp;} while (0)
#define KUTE_ABS(x) (x) > 0 ? (x) : -(x)
#define KUTE_SIGN(x) (x < 0 ? -1 : 1)

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

void kute_circle(int cx, int cy, int radius, uint32_t color)
{
    if (pw <= 0 || ph <= 0) return;
    for (int x = cx - radius - 1; x <= cx + radius; ++x)
    {
        if (x >= pw) break;
        if (x < 0) continue;
        for (int y = cy - radius - 1; y <= cy + radius; ++y)
        {
            if (y >= ph) break;
            if (y < 0) continue;
            int dx = x - cx;
            int dy = y - cy;
            if (dx * dx + dy * dy <= radius * radius)
            {
                pixels[x + y * pw] = color;
            }
        }
    }
}

void kute_line(int x0, int y0, int x1, int y1, uint32_t color)
{
    if (pw <= 0 || ph <= 0) return;
    if (x0 > x1) 
    {
        KUTE_SWAP(&x0, &x1, int);
        KUTE_SWAP(&y0, &y1, int);
    }
    
    int dx = KUTE_ABS(x1 - x0);
    int dy = KUTE_ABS(y1 - y0);
    int sy = KUTE_SIGN(y1 - y0);
    bool alongy = 0;

    if (dy > dx) 
    {
        alongy = 1;
        KUTE_SWAP(&dx, &dy, int);
    }

    int e = 2 * dy - dx;
    int x = x0;
    int y = y0;

    while (1)
    {
        if (x == x1 && y == y1) break;
        if (x < 0 || y < 0) continue;
        if (x >= pw || y >= ph) break;

        pixels[x + y * pw] = color;

        if (e >= 0)
        {
            x += 1 * alongy;
            y += sy * !alongy;
            e += (dy - dx);
        }
        else
        {
            e += dy;
        }
        
        x += 1 * !alongy;
        y += sy * alongy;
    }
}