#include "kute.h"

#define WIDTH 400
#define HEIGHT 300

static kute_pixel_t buffer[WIDTH * HEIGHT];
static kute_framebuffer_t fb = {buffer, WIDTH, HEIGHT};

kute_pixel_t *wasm_get_pixels() {
    return fb.pixels;
}

int wasm_get_width(void) {
    return fb.width;
}

int wasm_get_height(void) {
    return fb.height;
}

void wasm_loop()
{
    kute_pixel_fill(&fb, KUTE_BLUE);
}