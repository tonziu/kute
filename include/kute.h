#ifndef H_KUTE
#define H_KUTE

#include <stdint.h>

uint32_t kute_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

void kute_clear(uint32_t *pixels, int pw, int ph, uint32_t color);

void kute_fill_rect(uint32_t *pixels, int pw, int ph, int x0, int y0, int rw, int rh, uint32_t color);

void kute_fill_circle(uint32_t *pixels, int pw, int ph, int cx, int cy, int radius, uint32_t color);

void kute_draw_line(uint32_t *pixels, int pw, int ph, int x0, int y0, int x1, int y1, uint32_t color);

void kute_fill_triangle(uint32_t *pixels, int pw, int ph, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
#endif // H_KUTE