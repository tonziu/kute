#include "kute.h"

#include <stdbool.h>

#define KUTE_DEPTH_MAX (1 << 16)
#define KUTE_SWAP(a, b, T) do {T temp = *(a); *(a) = *(b);*(b) = temp;} while (0)
#define KUTE_ABS(a) ((a) >= 0) ? (a) : -(a)
#define KUTE_SIGN(a) (a) >= 0 ? 1 : -1
#define KUTE_MAT4_AT(m, i, j) (m).data[(i) + (j) * 4]

int kute_edge(int ax, int ay, int bx, int by, int x, int y)
{
    return (x - ax) * (by - ay) - (y - ay) * (bx - ax);
}

uint32_t kute_rgba32_pack(kute_color_t color)
{
    // Warning: this is intended for wasm use right now.
    return color.a << 24 | color.b << 16 | color.g << 8 | color.r;
}

void kute_pixel_put(kute_framebuffer_t* fb, int x, int y, int z, kute_color_t color)
{
    if (x < 0 || y < 0)
        return;
    if (x >= fb->width || y >= fb->height)
        return;

    int idx = x + y * fb->width;
    if (z < fb->depth[idx])
    {
        fb->depth[idx] = z;
        fb->pixels[idx] = kute_rgba32_pack(color);
    }
}

void kute_pixel_fill(kute_framebuffer_t *fb, kute_color_t color)
{
    for (int i = 0; i < fb->width * fb->height; ++i)
    {
        fb->pixels[i] = kute_rgba32_pack(color);
    }
}

void kute_pixel_clear(kute_framebuffer_t *fb, kute_color_t color)
{
    for (int i = 0; i < fb->width * fb->height; ++i)
    {
        fb->pixels[i] = kute_rgba32_pack(color);
        fb->depth[i] = KUTE_DEPTH_MAX;
    }
}

void kute_pixel_line(kute_framebuffer_t *fb, int ax, int ay, int bx, int by, kute_color_t color)
{
    if (ax > bx)
    {
        KUTE_SWAP(&ax, &bx, int);
        KUTE_SWAP(&ay, &by, int);
    }

    int dx = KUTE_ABS(bx - ax);
    int dy = KUTE_ABS(by - ay);
    int sy = KUTE_SIGN(by - ay);
    bool alongy = 0;

    if (dy > dx)
    {
        alongy = 1;
        KUTE_SWAP(&dx, &dy, int);
    }

    int e = 2 * dy - dx;
    int x = ax;
    int y = ay;

    while (1)
    {
        if (x == bx && y == by)
            break;
        if (x < 0 || y < 0)
            continue;
        if (x >= fb->width || y >= fb->height)
            break;

        kute_pixel_put(fb, x, y, 0, color);

        if (e >= 0)
        {
            x += 1 * alongy;
            y += sy * !alongy;
            e += (dy - dx);
        }
        else
        {
            e += dy;
        }

        x += 1 * !alongy;
        y += sy * alongy;
    }
}

void kute_pixel_rect(kute_framebuffer_t *fb, int x, int y, int rw, int rh, kute_color_t color)
{
    for (int col = y; col < y + rh; ++col)
    {
        for (int row = x; row < x + rw; ++row)
        {
            kute_pixel_put(fb, row, col, 0, color);
        }
    }
}

void kute_pixel_triangle(kute_framebuffer_t* fb, int ax, int ay, int bx, int by, int cx, int cy, kute_color_t color)
{
    int minx = ax < bx ? (ax < cx ? ax : cx) : (bx < cx ? bx : cx);
    int maxx = ax > bx ? (ax > cx ? ax : cx) : (bx > cx ? bx : cx);
    int miny = ay < by ? (ay < cy ? ay : cy) : (by < cy ? by : cy);
    int maxy = ay > by ? (ay > cy ? ay : cy) : (by > cy ? by : cy);

    int area = kute_edge(ax, ay, bx, by, cx, cy);
    if (area == 0) return;

    int abs_area = KUTE_ABS(area);

    for (int y = miny; y < maxy; ++y)
    {
        for (int x = minx; x < maxx; ++x)
        {

            float w0 = kute_edge(bx, by, cx, cy, x, y);
            float w1 = kute_edge(cx, cy, ax, ay, x, y);
            float w2 = kute_edge(ax, ay, bx, by, x, y);
            bool is_inside = (area > 0) ? (w0 >= 0 && w1 >= 0 && w2 >= 0) 
                                     : (w0 <= 0 && w1 <= 0 && w2 <= 0);

            if (is_inside) kute_pixel_put(fb, x, y, 0, color);
        }
    }
}

void kute_pixel_line_interp(kute_framebuffer_t* fb, int ax, int ay, int bx, int by, kute_color_t c0, kute_color_t c1)
{
    if (ax > bx)
    {
        KUTE_SWAP(&ax, &bx, int);
        KUTE_SWAP(&ay, &by, int);
    }

    int dx = KUTE_ABS(bx - ax);
    int dy = KUTE_ABS(by - ay);
    int sy = KUTE_SIGN(by - ay);
    bool alongy = 0;

    if (dy > dx)
    {
        alongy = 1;
        KUTE_SWAP(&dx, &dy, int);
    }

    int e = 2 * dy - dx;
    int x = ax;
    int y = ay;
    int length = dx > dy ? dx : dy;
    int step = 0;

    while (1)
    {
        if (x == bx && y == by)
            break;
        if (x < 0 || y < 0)
            continue;
        if (x >= fb->width || y >= fb->height)
            break;

        kute_color_t c;
        float t = (float) step / (float) length;
        c.r = (uint8_t)((1-t)*c0.r + t*c1.r);
        c.g = (uint8_t)((1-t)*c0.g + t*c1.g);
        c.b = (uint8_t)((1-t)*c0.b + t*c1.b);
        c.a = (uint8_t)((1-t)*c0.a + t*c1.a);
        kute_pixel_put(fb, x, y, 0, c);

        if (e >= 0)
        {
            x += 1 * alongy;
            y += sy * !alongy;
            e += (dy - dx);
        }
        else
        {
            e += dy;
        }

        x += 1 * !alongy;
        y += sy * alongy;
        step++;
    }
}

void kute_pixel_triangle_interp(kute_framebuffer_t* fb, kute_vertex_t a, kute_vertex_t b, kute_vertex_t c)
{
    int minx = a.pos.x < b.pos.x ? (a.pos.x < c.pos.x ? a.pos.x : c.pos.x) : (b.pos.x < c.pos.x ? b.pos.x : c.pos.x);
    int maxx = a.pos.x > b.pos.x ? (a.pos.x > c.pos.x ? a.pos.x : c.pos.x) : (b.pos.x > c.pos.x ? b.pos.x : c.pos.x);
    int miny = a.pos.y < b.pos.y ? (a.pos.y < c.pos.y ? a.pos.y : c.pos.y) : (b.pos.y < c.pos.y ? b.pos.y : c.pos.y);
    int maxy = a.pos.y > b.pos.y ? (a.pos.y > c.pos.y ? a.pos.y : c.pos.y) : (b.pos.y > c.pos.y ? b.pos.y : c.pos.y);

    int area = kute_edge(a.pos.x, a.pos.y, b.pos.x, b.pos.y, c.pos.x, c.pos.y);
    if (area == 0) return;

    int abs_area = KUTE_ABS(area);

    for (int y = miny; y < maxy; ++y)
    {
        for (int x = minx; x < maxx; ++x)
        {
            float w0 = kute_edge(b.pos.x, b.pos.y, c.pos.x, c.pos.y, x, y);
            float w1 = kute_edge(c.pos.x, c.pos.y, a.pos.x, a.pos.y, x, y);
            float w2 = kute_edge(a.pos.x, a.pos.y, b.pos.x, b.pos.y, x, y);
            bool is_inside = (area > 0) ? (w0 >= 0 && w1 >= 0 && w2 >= 0) 
                                     : (w0 <= 0 && w1 <= 0 && w2 <= 0);

            if (is_inside) 
            {
                float w0_inv = 1.0f / a.pos.w;
                float w1_inv = 1.0f / b.pos.w;
                float w2_inv = 1.0f / c.pos.w;
                
                float denom = w0 * w0_inv + w1 * w1_inv + w2 * w2_inv;
                float inv_denom = 1.0f / denom;

                float l0 = (w0 * w0_inv) * inv_denom;
                float l1 = (w1 * w1_inv) * inv_denom;
                float l2 = (w2 * w2_inv) * inv_denom;

                kute_color_t color;
                color.r = l0 * a.color.r + l1 * b.color.r + l2 * c.color.r;
                color.g = l0 * a.color.g + l1 * b.color.g + l2 * c.color.g;
                color.b = l0 * a.color.b + l1 * b.color.b + l2 * c.color.b;
                color.a = l0 * a.color.a + l1 * b.color.a + l2 * c.color.a;

                float zf = l0 * a.pos.z + l1 * b.pos.z + l2 * c.pos.z;
                int z = (int)(zf * KUTE_DEPTH_MAX);
                kute_pixel_put(fb, x, y, z, color);
            } 
        }
    }
}

kute_mat4_t kute_mat4_identity()
{
    kute_mat4_t result = {0};
    for (int i = 0; i < 4; ++i)
    {
        KUTE_MAT4_AT(result, i, i) = 1;
    }
    return result;
}

kute_mat4_t kute_mat4_add(kute_mat4_t a, kute_mat4_t b)
{
    kute_mat4_t result;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            KUTE_MAT4_AT(result, i, j) = KUTE_MAT4_AT(a, i, j) + KUTE_MAT4_AT(b, i, j);
        }
    }
    return result;
}

kute_mat4_t kute_mat4_sub(kute_mat4_t a, kute_mat4_t b)
{
    kute_mat4_t result;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            KUTE_MAT4_AT(result, i, j) = KUTE_MAT4_AT(a, i, j) - KUTE_MAT4_AT(b, i, j);
        }
    }
    return result;
}

kute_mat4_t kute_mat4_mul(kute_mat4_t a, kute_mat4_t b)
{
    kute_mat4_t result;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            float sum = 0;
            for (int r = 0; r < 4; ++r)
            {
                sum += KUTE_MAT4_AT(a, i, r) * KUTE_MAT4_AT(b, r, j);
            }
            KUTE_MAT4_AT(result, i, j) = sum;
        }
    }
    return result;
}

kute_vec3_t kute_vec3_add(kute_vec3_t a, kute_vec3_t b)
{
    return (kute_vec3_t) {a.x + b.x, a.y + b.y, a.z + b.z};
}

kute_vec3_t kute_vec3_sub(kute_vec3_t a, kute_vec3_t b)
{
    return (kute_vec3_t) {a.x - b.x, a.y - b.y, a.z - b.z};
}

kute_vec3_t kute_vec3_cross(kute_vec3_t a, kute_vec3_t b)
{
    kute_vec3_t result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

float kute_vec3_dot(kute_vec3_t a, kute_vec3_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float kute_vec4_dot(kute_vec4_t a, kute_vec4_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float kute_vec3_length(kute_vec3_t a)
{
    return sqrtf(kute_vec3_dot(a, a));
}

kute_vec3_t kute_vec3_normalize(kute_vec3_t a)
{
    float len = kute_vec3_length(a);
    if (len == 0.0f) return (kute_vec3_t) {0, 0, 0};
    float inv = 1.0f / len;
    return (kute_vec3_t) {a.x * inv, a.y * inv, a.z * inv};
}

kute_mat4_t kute_mat4_look_at(kute_vec3_t eye, kute_vec3_t target, kute_vec3_t up)
{
    kute_vec3_t zaxis = kute_vec3_normalize(kute_vec3_sub(eye, target));
    kute_vec3_t xaxis = kute_vec3_normalize(kute_vec3_cross(up, zaxis));
    kute_vec3_t yaxis = kute_vec3_cross(zaxis, xaxis);

    kute_mat4_t view = kute_mat4_identity();

    KUTE_MAT4_AT(view, 0, 0) = xaxis.x;
    KUTE_MAT4_AT(view, 0, 1) = xaxis.y;
    KUTE_MAT4_AT(view, 0, 2) = xaxis.z;
    KUTE_MAT4_AT(view, 0, 3) = -kute_vec3_dot(xaxis, eye);

    KUTE_MAT4_AT(view, 1, 0) = yaxis.x;
    KUTE_MAT4_AT(view, 1, 1) = yaxis.y;
    KUTE_MAT4_AT(view, 1, 2) = yaxis.z;
    KUTE_MAT4_AT(view, 1, 3) = -kute_vec3_dot(yaxis, eye);

    KUTE_MAT4_AT(view, 2, 0) = zaxis.x;
    KUTE_MAT4_AT(view, 2, 1) = zaxis.y;
    KUTE_MAT4_AT(view, 2, 2) = zaxis.z;
    KUTE_MAT4_AT(view, 2, 3) = -kute_vec3_dot(zaxis, eye);

    KUTE_MAT4_AT(view, 3, 3) = 1.0f;

    return view;
}

kute_mat4_t kute_mat4_perspective(float fov, float aspect, float near, float far)
{
    float f = 1.0f / tanf(fov * 0.5f);
    
    kute_mat4_t result = {0};
    KUTE_MAT4_AT(result, 0, 0) = f / aspect;
    KUTE_MAT4_AT(result, 1, 1) = f;
    KUTE_MAT4_AT(result, 2, 2) = (far + near) / (near - far);
    KUTE_MAT4_AT(result, 2, 3) = (2.0f * far * near) / (near - far);
    KUTE_MAT4_AT(result, 3, 2) = -1.0f;
    
    return result;
}

kute_vec4_t kute_mat4_row(kute_mat4_t m, int row)
{
    kute_vec4_t result;

    result.x = KUTE_MAT4_AT(m, row, 0);
    result.y = KUTE_MAT4_AT(m, row, 1);
    result.z = KUTE_MAT4_AT(m, row, 2);
    result.w = KUTE_MAT4_AT(m, row, 3);

    return result;
}

kute_vec4_t kute_mat4_mul_vec4(kute_mat4_t m, kute_vec4_t v)
{
    kute_vec4_t result;

    kute_vec4_t x = kute_mat4_row(m, 0);
    kute_vec4_t y = kute_mat4_row(m, 1);
    kute_vec4_t z = kute_mat4_row(m, 2);
    kute_vec4_t w = kute_mat4_row(m, 3);

    result.x = kute_vec4_dot(x, v);
    result.y = kute_vec4_dot(y, v);
    result.z = kute_vec4_dot(z, v);
    result.w = kute_vec4_dot(w, v);

    return result;
}

kute_vec4_t kute_vec4_to_ndc(kute_mat4_t mvp, kute_vec4_t pos)
{
    kute_vec4_t clip = kute_mat4_mul_vec4(mvp, pos);
    kute_vec4_t ndc;
    
    ndc.x = clip.x / clip.w;
    ndc.y = clip.y / clip.w;
    ndc.z = clip.z / clip.w;
    ndc.w = clip.w;

    return ndc;
}

kute_vec4_t kute_vec4_to_screen(kute_vec4_t ndc, int width, int height)
{
    kute_vec4_t result;
    result.x = (ndc.x + 1.0f) * 0.5f * width;
    result.y = (1.0f - ndc.y) * 0.5f * height;
    result.z = (ndc.z + 1.0f) * 0.5f;
    result.w = ndc.w;
    return result;
}

kute_mat4_t kute_mat4_translation(float x, float y, float z)
{
    kute_mat4_t m = kute_mat4_identity();

    KUTE_MAT4_AT(m, 0, 3) = x;
    KUTE_MAT4_AT(m, 1, 3) = y;
    KUTE_MAT4_AT(m, 2, 3) = z;

    return m;
}

kute_mat4_t kute_mat4_rotation_y(float radians)
{
    kute_mat4_t m = kute_mat4_identity();

    float c = cosf(radians);
    float s = sinf(radians);

    KUTE_MAT4_AT(m, 0, 0) = c;
    KUTE_MAT4_AT(m, 0, 2) = s;
    KUTE_MAT4_AT(m, 2, 0) = -s;
    KUTE_MAT4_AT(m, 2, 2) = c;

    return m;
}