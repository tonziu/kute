#ifndef H_KUTE
#define H_KUTE

#include <stdint.h>

uint32_t kute_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

void kute_clear(uint32_t *pixels, int pw, int ph, uint32_t color);

#endif // H_KUTE