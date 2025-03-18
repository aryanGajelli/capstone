#include "volumetric.h"

#include <stdio.h>
#include <led-matrix-c.h>
#include <string.h>

int volumetric_test(int argc, char **argv) {
    struct RGBLedMatrixOptions options;
    struct RGBLedRuntimeOptions rt_options;
    struct RGBLedMatrix *matrix;
    struct LedCanvas *offscreen_canvas;
    int width, height;
    int x, y, i;

    memset(&options, 0, sizeof(options));
    options.rows = 64;
    options.cols = 128;
    options.chain_length = 1;
    options.parallel = 3;

    options.hardware_mapping = "counter";
    options.row_address_type = 5;

    options.pwm_bits = 1;
    options.pwm_dither_bits = 2;
    options.pwm_lsb_nanoseconds = 20;

    // options.show_refresh_rate = true;

    memset(&rt_options, 0, sizeof(rt_options));
    rt_options.gpio_slowdown = 3;

    volatile uint32_t *read_reg = NULL;
    init_photo(read_reg);

    /* This supports all the led commandline options. Try --led-help */
    matrix = led_matrix_create_from_options_and_rt_options(&options, &rt_options);
    if (matrix == NULL)
        return 1;

    /* Let's do an example with double-buffering. We create one extra
     * buffer onto which we draw, which is then swapped on each refresh.
     * This is typically a good aproach for animations and such.
     */
    offscreen_canvas = led_matrix_create_offscreen_canvas(matrix);

    // led_canvas_get_size(offscreen_canvas, &width, &height);

    fprintf(stderr, "Size: %dx%d. Hardware gpio mapping: %s\n",
            width, height, options.hardware_mapping);
    height = 50;
    width = 50;
    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            led_canvas_set_pixel(matrix, x, y, 255, 0, 0);
        }
    }

    while (true) {
        bool sync = (*read_reg) & (1UL << PHOTO_PIN);
        printf("%d\n", sync);
    }

    /*
     * Make sure to always call led_matrix_delete() in the end to reset the
     * display. Installing signal handlers for defined exit is a good idea.
     */
    led_matrix_delete(matrix);

    return 0;
}