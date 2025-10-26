#ifndef H_KUTE
#define H_KUTE

#include <stdint.h>

uint32_t* kute_get_pixels();

void kute_init(int width, int height);
void kute_clear(uint32_t color);
void kute_rect(int x0, int y0, int rw, int rh, uint32_t color);

#endif // H_KUTE