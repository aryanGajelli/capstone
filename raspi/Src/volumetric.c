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

#define NON_SET_PIXEL_US 100
int volumetric_test(struct LedCanvas *canvas, volatile uint32_t *read_reg) {
    // fclose(fptr);
    // volume_draw_plane();
    slicemap_init();

    // --------- Slicing Setup -----------
    const float rpm = 600;
    const float us_per_rev = 1e6 * 60 / rpm;
    const uint32_t us_per_slice = us_per_rev / SLICE_COUNT;  // Microseconds per slice
    const uint32_t SET_PIXEL_MAX_TIME_US = us_per_slice - NON_SET_PIXEL_US > 0 ? us_per_slice - NON_SET_PIXEL_US : 0;

    bool rising_edge = false;
    bool falling_edge = false;
    printf("us/slice: %d\n", us_per_slice);
    if (SET_PIXEL_MAX_TIME_US == 0) {
        fprintf(stderr, "SET_PIXEL_MAX_TIME_US is 0\n");
        return 1;
    }


    int slice = 0;
    uint32_t start;

    uint32_t last_rising_edge = get_micros_counter();
    int panel_z = 0;
    while (true) {
        start = get_micros_counter();
        // led_canvas_clear(canvas);
        getRisingFallingPhoto(read_reg, &rising_edge, &falling_edge);
        if (rising_edge) {
            slice = 0;
            uint32_t end = get_micros_counter();
            printf("us/rev: %d, rpm: %.2f\n", end - last_rising_edge, 1e6 * 60 / (end - last_rising_edge));
            last_rising_edge = end;
        }
        if (falling_edge) {
            slice = SLICE_COUNT / 2;
        }

        while (get_micros_counter() - start < us_per_slice - NON_SET_PIXEL_US) {
            int panel_index = panel_z / PANEL_HEIGHT;
            for (int r = 0; r < PANEL_WIDTH; r++) {
                voxel_2D_t voxel_2D = slice_map[slice][r][panel_index];
                pixel_t color = volume[panel_2_voxel_z(panel_z)][voxel_2D.y][voxel_2D.x];
                uint8_t r_ = ((color & 0b11100000) >> 5) > 0b111/2 ? 255 : 0;
                uint8_t g_ = ((color & 0b00011100) >> 2) > 0b111/2 ? 255 : 0;
                uint8_t b_ = (color & 0b00000011) > 0b11/2 ? 255 : 0;
                led_canvas_set_pixel(offscreen_canvas, r, panel_z, r_, g_, b_);
            }
            panel_z = (panel_z + 1) % (PANEL_HEIGHT * PANEL_COUNT);
        }

        uint32_t duration_us = get_micros_counter() - start;
        if (duration_us > us_per_slice) {
            fprintf(stderr, "Slice took too long: %d\n", duration_us);
            duration_us = 0;
        }

        usleep(us_per_slice - duration_us);
        slice = (slice + 1) % SLICE_COUNT;
    }

    /*
     * Make sure to always call led_matrix_delete() in the end to reset the
     * display. Installing signal handlers for defined exit is a good idea.
     */

    return 0;
}