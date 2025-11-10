#ifndef H_KUTE
#define H_KUTE

#include <stdint.h>

typedef uint32_t kute_pixel_t;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} kute_color_t;

typedef struct
{
    kute_pixel_t* pixels;
    float *depth;
    int width;
    int height;
} kute_framebuffer_t;

typedef struct
{
    float x;
    float y;
} kute_vec2_t;

typedef struct
{
    float x;
    float y;
    float z;
} kute_vec3_t;

typedef struct
{
    float x;
    float y;
    float z;
    float w;
} kute_vec4_t;

typedef struct
{
    float data[16];
} kute_mat4_t;

typedef struct 
{
    kute_vec3_t pos;
    kute_vec3_t target;
    kute_vec3_t up;
} kute_camera_t;

typedef struct
{
    kute_vec4_t pos;
    kute_color_t color;
} kute_vertex_t;


#define KUTE_RED   (kute_color_t) {255,   0,   0, 255}
#define KUTE_GREEN (kute_color_t) {  0, 255,   0, 255}
#define KUTE_BLUE  (kute_color_t) {  0,   0, 255, 255}
#define KUTE_WHITE (kute_color_t) {255, 255, 255, 255}
#define KUTE_BLACK (kute_color_t) {  0,   0,   0, 255}

kute_vec3_t kute_vec3_add(kute_vec3_t a, kute_vec3_t b);
kute_vec3_t kute_vec3_sub(kute_vec3_t a, kute_vec3_t b);
kute_vec3_t kute_vec3_cross(kute_vec3_t a, kute_vec3_t b);
kute_vec3_t kute_vec3_normalize(kute_vec3_t a);

float kute_vec3_dot(kute_vec3_t a, kute_vec3_t b);
float kute_vec3_length(kute_vec3_t a);

float kute_vec4_dot(kute_vec4_t a, kute_vec4_t b);

kute_vec4_t kute_mat4_row(kute_mat4_t m, int row);

kute_mat4_t kute_mat4_identity();
kute_mat4_t kute_mat4_add(kute_mat4_t a, kute_mat4_t b);
kute_mat4_t kute_mat4_sub(kute_mat4_t a, kute_mat4_t b);
kute_mat4_t kute_mat4_mul(kute_mat4_t a, kute_mat4_t b);
kute_mat4_t kute_mat4_translation(float x, float y, float z);
kute_mat4_t kute_mat4_rotation_y(float radians);

kute_vec4_t kute_mat4_mul_vec4(kute_mat4_t m, kute_vec4_t v);

kute_mat4_t kute_mat4_look_at(kute_vec3_t eye, kute_vec3_t target, kute_vec3_t up);
kute_mat4_t kute_mat4_perspective(float fov, float aspect, float near, float far);

kute_vec4_t kute_vec4_to_ndc(kute_mat4_t mvp, kute_vec4_t pos);
kute_vec4_t kute_vec4_to_screen(kute_vec4_t ndc, int width, int height);

uint32_t kute_rgba32_pack(kute_color_t color);

void kute_pixel_put(kute_framebuffer_t* fb, int x, int y, int z, kute_color_t color);
void kute_pixel_clear(kute_framebuffer_t *fb, kute_color_t color);
void kute_pixel_fill(kute_framebuffer_t* fb, kute_color_t color);
void kute_pixel_line(kute_framebuffer_t* fb, int ax, int ay, int bx, int by, kute_color_t color);
void kute_pixel_rect(kute_framebuffer_t* fb, int x, int y, int rw, int rh, kute_color_t color);
void kute_pixel_triangle(kute_framebuffer_t* fb, int ax, int ay, int bx, int by, int cx, int cy, kute_color_t color);

void kute_pixel_line_interp(kute_framebuffer_t* fb, int ax, int ay, int bx, int by, kute_color_t c0, kute_color_t c1);
void kute_pixel_triangle_interp(kute_framebuffer_t* fb, kute_vertex_t a, kute_vertex_t b, kute_vertex_t c);

#endif // H_KUTE