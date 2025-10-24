#include "kute.h"

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

    for (int x = x0; x < x0 + rw; ++x)
    {
        for (int y = y0; y < y0 + rh; ++y)
        {
            if (x >= 0 && x < pw && y >= 0 && y < ph)
            {
                pixels[x + y * pw] = color;
            }
        }
    }
}

void kute_fill_circle(uint32_t *pixels, int pw, int ph, int cx, int cy, int radius, uint32_t color)
{
    if (!pixels || pw <= 0 || ph <= 0) return;

    uint8_t r = (color >> 0)  & 0xFF;
    uint8_t g = (color >> 8)  & 0xFF;
    uint8_t b = (color >> 16) & 0xFF;
    uint8_t a = (color >> 24) & 0xFF;

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
                
                pixels[x + y * pw] = blended;
            }
        }
    }
}