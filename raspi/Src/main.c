#include "rpi-rgb-led-matrix/include/led-matrix-c.h"

int main(int argc, char **argv) {
    struct RGBLedMatrixOptions options = {
        .rows = 64,
        .cols = 128,
        .parallel = 3,
        .chain_length = 1,
        .pwm_lsb_nanoseconds = 100,
        .pwm_bits = 1,
    };
    struct RGBLedRuntimeOptions rt_options = {
        .gpio_slowdown = 3,

    };
    struct RGBLedMatrix *matrix = led_matrix_create_from_options_and_rt_options(&options, &rt_options);
    if (matrix == NULL) {
        led_matrix_print_flags(stderr);
        return 1;
    }

    struct LedCanvas *canvas = led_matrix_get_canvas(matrix);
    // draw red vertical line 10 pixels thick, 32 pixels from the left
    led_canvas_clear(canvas);

    while (1) {
        led_canvas_set_pixels(canvas, 32, 0, 10, 64, (struct Color[]){{0, 255, 0}});
    }

    free(matrix);
    free(canvas);
}