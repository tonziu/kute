#include <stdint.h>

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
}