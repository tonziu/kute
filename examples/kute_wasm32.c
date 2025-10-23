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
    kute_fill_circle(pixels, WIDTH, HEIGHT, 400, 300, 200, kute_rgba(200, 200, 200, 255));
}