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
    kute_fill_circle(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT/2, 40, kute_rgba(60, 60, 60, 128));
    kute_draw_line(pixels, WIDTH, HEIGHT, 325, 300, 475, 300, kute_rgba(0, 255, 0, 255));
    kute_draw_line(pixels, WIDTH, HEIGHT, 400, 245, 400, 355, kute_rgba(0, 255, 0, 255));
}