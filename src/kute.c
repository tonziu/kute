#include "kute.h"
#include <stdbool.h>

void kute_unpack_rgba32(uint32_t color, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t* a)
{
    *r = (color >>  0) & 0xFF;
    *g = (color >>  8) & 0xFF;
    *b = (color >> 16) & 0xFF;
    *a = (color >> 24) & 0xFF;
}

uint32_t kute_blend_color(uint32_t src, uint32_t dest)
{
    uint8_t src_r, src_g, src_b, src_a;
    uint8_t dest_r, dest_g, dest_b, dest_a;

    kute_unpack_rgba32(src, &src_r, &src_g, &src_b, &src_a);
    kute_unpack_rgba32(dest, &dest_r, &dest_g, &dest_b, &dest_a);
    
    uint32_t alpha = src_a;
    uint32_t invalpha = 255 - alpha;

    uint8_t r = (src_r * alpha + dest_r * invalpha) / 255;
    uint8_t g = (src_g * alpha + dest_g * invalpha) / 255;
    uint8_t b = (src_b * alpha + dest_b * invalpha) / 255;
    uint8_t a = src_a > dest_a ? src_a : dest_a;
    
    return kute_rgba(r, g, b, a);
}

uint32_t kute_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return a << 24 | b << 16 | g << 8 | r;
}

void kute_clear(uint32_t *pixels, int pw, int ph, uint32_t color)
{
    if (!pixels || pw <= 0 || ph <= 0) return;

    for (int i = 0; i < pw * ph; ++i)
    {
        pixels[i] = color;
    }
}

void kute_fill_rect(uint32_t *pixels, int pw, int ph, int x0, int y0, int rw, int rh, uint32_t color)
{
    if (!pixels || pw <= 0 || ph <= 0) return;

    if (rw < 0)
    {
        rw *= -1;
        x0 -= rw;
    }

    if (rh < 0)
    {
        rh *= -1;
        y0 -= rh;
    }

    for (int y = y0; y < y0 + rh; ++y)
    {
        if (y < 0 || y > ph) continue;
        for (int x = x0; x < x0 + rw; ++x)
        {
            if (x < 0 || x > pw) continue;
            uint32_t curr = pixels[x + y * pw];
            pixels[x + y * pw] = kute_blend_color(color, curr);
        }
    }
}

void kute_fill_circle(uint32_t *pixels, int pw, int ph, int cx, int cy, int radius, uint32_t color)
{
    if (!pixels || pw <= 0 || ph <= 0) return;

    uint8_t r, g, b, a;
    kute_unpack_rgba32(color, &r, &g, &b, &a);

    int aa_samples = 4;
    float aa_step = 1.0f / aa_samples;
    float aa2 = (float)(aa_samples * aa_samples);

    for (int y = cy - radius - 1; y <= cy + radius + 1; ++y)
    {
        if (y < 0 || y >= ph) continue;
        
        for (int x = cx - radius - 1; x <= cx + radius + 1; ++x)
        {
            if (x < 0 || x >= pw) continue;
            
            int aa_count = 0;
            
            for (int i = 0; i < aa_samples; i++)
            {
                for (int j = 0; j < aa_samples; j++)
                {
                    float sx = x + (i + 0.5f) * aa_step;
                    float sy = y + (j + 0.5f) * aa_step;
                    
                    float dx = sx - cx;
                    float dy = sy - cy;
                    
                    if (dx*dx + dy*dy <= radius * radius)
                    {
                        aa_count++;
                    }
                }
            }

            if (aa_count > 0)
            {
                float coverage = aa_count / aa2;
                
                uint8_t final_alpha = (uint8_t)(a * coverage);
                
                uint32_t blended = kute_rgba(r, g, b, final_alpha);

                uint32_t curr = pixels[x + y * pw];
                
                pixels[x + y * pw] = kute_blend_color(blended, curr);
            }
        }
    }
}

int abs(int value)
{
    return value < 0 ? -value : value;
}

void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void kute_draw_line(uint32_t *pixels, int pw, int ph, int x0, int y0, int x1, int y1, uint32_t color)
{
    if (!pixels || pw <= 0 || ph <= 0) return;

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;

    bool is_vert = dy > dx;

    if (is_vert)
        swap(&dx, &dy);

    int d = 2 * dy - dx;
    int x = x0;
    int y = y0;

    while (1)
    {
        if (x >= 0 && x < pw && y > 0 && y < ph)
        {
            uint32_t curr = pixels[x + y * pw];
            pixels[x + y * pw] = kute_blend_color(color, curr);
        }

        if (x == x1 && y == y1)
            break;

        if (d > 0)
        {
            if (is_vert)
                x += sx;
            else
                y += sy;
            d += 2 * (dy - dx);
        }
        else
        {
            d += 2 * dy;
        }
        if (is_vert)
            y += sy;
        else
            x += sx;
    }
}