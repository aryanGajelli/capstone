#include "volumetric.h"

#include <led-matrix-c.h>
#include <math.h>
#include <mathc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "gpio.h"

int volumetric_test(int argc, char **argv) {
    struct RGBLedMatrixOptions options;
    struct RGBLedRuntimeOptions rt_options;
    struct RGBLedMatrix *matrix;
    struct LedCanvas *offscreen_canvas;
    int width, height;

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
    options.disable_busy_waiting = true;

    memset(&rt_options, 0, sizeof(rt_options));
    rt_options.gpio_slowdown = 3;

    volatile uint32_t *read_reg = NULL;
    // init_photo(read_reg);

    /* This supports all the led commandline options. Try --led-help */
    matrix = led_matrix_create_from_ext(&options, &rt_options, &read_reg);
    if (matrix == NULL)
        return 1;

    /* Let's do an example with double-buffering. We create one extra
     * buffer onto which we draw, which is then swapped on each refresh.
     * This is typically a good aproach for animations and such.
     */
    offscreen_canvas = led_matrix_create_offscreen_canvas(matrix);

    led_canvas_get_size(offscreen_canvas, &width, &height);

    struct LedCanvas *canvas = (struct LedCanvas *)matrix;

    fprintf(stderr, "Size: %dx%d. Hardware gpio mapping: %s\n", width, height, options.hardware_mapping);

    const int rows_per_panel = 64;
    const int cols_per_panel = 128;

    // --------- Slicing Setup -----------
    const float rpm = 600;
    int num_slices = 128;
    int slice = 0;
    const float slice_to_rad = 2 * MPI / num_slices;
    const float us_per_rev = 1e6 * 60 / rpm;
    const uint32_t us_per_slice = us_per_rev / num_slices;  // Microseconds per slice

    // Angles that align with a square's diagonals
    const float diag1 = num_slices / 8;  // 45 degrees
    const float diag2 = diag1 * 3;
    const float diag3 = diag1 * 5;
    const float diag4 = diag1 * 7;

    const int cube_width = 50;
    const int cube_height = 50;

    bool rising_edge = false;
    bool falling_edge = false;
    printf("us/slice: %d\n", us_per_slice);

    uint32_t start;

    uint32_t last_rising_edge = get_micros_counter();
    while (true) {
        start = get_micros_counter();
        led_canvas_clear(canvas);
        getRisingFallingPhoto(read_reg, &rising_edge, &falling_edge);
        if (rising_edge) {
            slice = 0;
            uint32_t duration = get_micros_counter() - last_rising_edge;
            printf("us/rev: %d, rpm: %.2f\n", duration, 1e6 * 60 / duration);
            last_rising_edge = get_micros_counter();
        }
        if (falling_edge) {
            slice = num_slices / 2;
        }
        double angle = slice * slice_to_rad;
        double width = 0;
        // Calculate width of cube cross-section
        // Adjust formula when crossing the cubes's diagonals
        if ((diag1 < slice && slice < diag2) || (diag3 < slice && slice < diag4)) {
            width = fabsf(cube_width / sinf(angle));
        } else {
            width = fabsf(cube_width / cosf(angle));
        }

        int offset_x = cols_per_panel/2 - width / 2;
        int offset_y = (rows_per_panel - cube_height) / 2;
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < cube_height; y++) {
                led_canvas_set_pixel(canvas, x + offset_x , y + offset_y , 255, 255, 255);
            }
        }

        // offset_x = 128 - width / 2;
        offset_y = rows_per_panel  + (rows_per_panel - cube_height) / 2;
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < cube_height; y++) {
                led_canvas_set_pixel(canvas, x + offset_x, y + offset_y, 255, 255, 255);
            }
        }
        slice = (slice + 1) % num_slices;
        uint32_t duration_us = get_micros_counter() - start;
        if (duration_us > us_per_slice)
            duration_us = 0;

        usleep(us_per_slice - duration_us);
    }

    /*
     * Make sure to always call led_matrix_delete() in the end to reset the
     * display. Installing signal handlers for defined exit is a good idea.
     */
    led_matrix_delete(matrix);

    return 0;
}