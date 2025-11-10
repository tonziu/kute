#include "kute.h"

#define WIDTH 600
#define HEIGHT 400

static kute_pixel_t pixel_buffer[WIDTH * HEIGHT];
static float depth_buffer[WIDTH * HEIGHT];
static kute_framebuffer_t fb = {.pixels = pixel_buffer, 
                                .depth  = depth_buffer,
                                .width  = WIDTH, 
                                .height = HEIGHT};

kute_pixel_t *wasm_get_pixels()
{
    return fb.pixels;
}

int wasm_get_width(void)
{
    return fb.width;
}

int wasm_get_height(void)
{
    return fb.height;
}

kute_vec4_t triangle1[3] = {
    { -0.5f, -0.5f, 0.0f, 1.0f },
    {  0.5f, -0.5f, 0.0f, 1.0f },
    {  0.0f,  0.5f, 0.0f, 1.0f }
};

kute_vec4_t triangle2[3] = {
    {  0.0f, -0.5f,  0.5f, 1.0f },
    {  0.0f, -0.5f, -0.5f, 1.0f },
    {  0.0f,  0.5f,  0.0f, 1.0f }
};

kute_camera_t cam = {
    .pos = {0.0f, 1.0f, 1.0f}, 
    .target = {0.0f, 0.0f, 0.0f},
    .up = {0.0f, 1.0f, 0.0f}
};

float fov_y = 60.0f * (3.14159265f / 180.0f);
float aspect = (float)WIDTH / (float)HEIGHT;
float near = 0.1f, far = 10.0f;
float angle = 0.0f;
float delta = 0.02f;

void wasm_loop(void)
{
    angle += delta;

    kute_mat4_t projection = kute_mat4_perspective(fov_y, aspect, near, far);
    kute_mat4_t view = kute_mat4_look_at(cam.pos, cam.target, cam.up);
    kute_mat4_t model = kute_mat4_rotation_y(angle);
    kute_mat4_t mvp = kute_mat4_mul(projection, kute_mat4_mul(view, model));

    kute_vertex_t vertices1[3];
    kute_vertex_t vertices2[3];
    kute_color_t colors[] = {KUTE_RED, KUTE_GREEN, KUTE_BLUE};

    for (int i = 0; i < 3; i++)
    {
        kute_vec4_t ndc1 = kute_vec4_to_ndc(mvp, triangle1[i]);
        kute_vec4_t ndc2 = kute_vec4_to_ndc(mvp, triangle2[i]);
        vertices1[i].pos = kute_vec4_to_screen(ndc1, fb.width, fb.height);
        vertices2[i].pos = kute_vec4_to_screen(ndc2, fb.width, fb.height);
        vertices1[i].color = colors[i];
        vertices2[i].color = colors[i];
    }

    kute_pixel_clear(&fb, KUTE_BLACK);

    kute_pixel_triangle_interp(&fb, vertices1[0], vertices1[1], vertices1[2]);
    kute_pixel_triangle_interp(&fb, vertices2[0], vertices2[1], vertices2[2]);
}