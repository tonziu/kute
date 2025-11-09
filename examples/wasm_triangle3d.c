#include "kute.h"

#define WIDTH 600
#define HEIGHT 400

static kute_pixel_t buffer[WIDTH * HEIGHT];
static kute_framebuffer_t fb = {buffer, WIDTH, HEIGHT};

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

kute_vec4_t triangle[3] = {
    { -0.5f, -0.5f, 0.0f, 1.0f },
    {  0.5f, -0.5f, 0.0f, 1.0f },
    {  0.0f,  0.5f, 0.0f, 1.0f }
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

    kute_vec2_t screen_tri[3];

    for (int i = 0; i < 3; i++)
    {
        kute_vec3_t ndc = kute_vec4_to_ndc(mvp, triangle[i]);
        screen_tri[i] = kute_ndc_to_screen(ndc, fb.width, fb.height);
    }

    kute_pixel_fill(&fb, KUTE_BLACK);

    kute_pixel_triangle_interp(&fb, screen_tri[0].x, screen_tri[0].y,
                                    screen_tri[1].x, screen_tri[1].y,
                                    screen_tri[2].x, screen_tri[2].y,
                                    KUTE_RED, KUTE_GREEN, KUTE_BLUE);
}