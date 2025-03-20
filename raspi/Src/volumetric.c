#include "volumetric.h"

#include <led-matrix-c.h>
#include <math.h>
#include <mathc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "gpio.h"
#include "slicemap.h"
#include "voxel.h"

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

    volume_draw_plane();
    slicemap_init();

    // --------- Slicing Setup -----------
    const float rpm = 600;
    const float us_per_rev = 1e6 * 60 / rpm;
    const uint32_t us_per_slice = us_per_rev / SLICE_COUNT;  // Microseconds per slice

    bool rising_edge = false;
    bool falling_edge = false;
    printf("us/slice: %d\n", us_per_slice);

    int slice = 0;
    uint32_t start;

    uint32_t last_rising_edge = get_micros_counter();
    while (true) {
        start = get_micros_counter();
        // led_canvas_clear(canvas);
        getRisingFallingPhoto(read_reg, &rising_edge, &falling_edge);
        if (rising_edge) {
            slice = 0;
            uint32_t duration = get_micros_counter() - last_rising_edge;
            printf("us/rev: %d, rpm: %.2f\n", duration, 1e6 * 60 / duration);
            last_rising_edge = get_micros_counter();
        }
        if (falling_edge) {
            slice = SLICE_COUNT / 2;
        }

        for (int panel_z = 0; panel_z < PANEL_HEIGHT * PANEL_COUNT; panel_z++) {
            int panel_index = panel_z / PANEL_HEIGHT;
            for (int r = 0; r < PANEL_WIDTH; r++) {
                voxel_2D_t voxel_2D = slice_map[slice][r][panel_index];
                pixel_t color = volume[panel_2_voxel_z(panel_z)][voxel_2D.y][voxel_2D.x];
                uint8_t r_ = (color & 0b100) ? 255 : 0;
                uint8_t g_ = (color & 0b010) ? 255 : 0;
                uint8_t b_ = (color & 0b001) ? 255 : 0;
                led_canvas_set_pixel(canvas, r, panel_z, r_, g_, b_);
            }
        }

        slice = (slice + 1) % SLICE_COUNT;
        uint32_t duration_us = get_micros_counter() - start;
        if (duration_us > us_per_slice) {
            fprintf(stderr, "Slice took too long: %d\n", duration_us);
            duration_us = 0;
        }

        usleep(us_per_slice - duration_us);
    }

    /*
     * Make sure to always call led_matrix_delete() in the end to reset the
     * display. Installing signal handlers for defined exit is a good idea.
     */
    led_matrix_delete(matrix);

    return 0;
}