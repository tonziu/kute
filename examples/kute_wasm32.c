#include "kute.h"

#define WIDTH 800
#define HEIGHT 600

static uint32_t pixels[WIDTH * HEIGHT];

uint32_t *wasm_get_pixels()
{
    return pixels;
}

void wasm_update(int t)
{
    (void) t;
    kute_clear(pixels, WIDTH, HEIGHT, kute_rgba(22, 22, 22, 255));
    kute_fill_triangle(pixels, WIDTH, HEIGHT, 385, 400, 415, 400, 400, 425, kute_rgba(175, 0, 0, 255));
    kute_fill_triangle(pixels, WIDTH, HEIGHT, 385, 200, 415, 200, 400, 175, kute_rgba(175, 0, 0, 255));
    kute_fill_circle(pixels, WIDTH, HEIGHT, 400, 300, 50, kute_rgba(0, 0, 0, 128));
}