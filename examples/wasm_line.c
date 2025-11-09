#include "kute.h"

#define WIDTH 600
#define HEIGHT 400

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

void wasm_loop(void) {
    kute_pixel_fill(&fb, KUTE_BLACK);
    kute_pixel_line(&fb, 200, 300, 400, 300, KUTE_RED);
    kute_pixel_line(&fb, 400, 300, 300, 100, KUTE_GREEN);
    kute_pixel_line(&fb, 300, 100, 200, 300, KUTE_BLUE);
}