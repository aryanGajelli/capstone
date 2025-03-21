#include "volumetric.h"

#include <led-matrix-c.h>
#include <math.h>
#include <mathc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>

#include "gpio.h"
#include "slicemap.h"
#include "voxel.h"

static uint8_t a_speed = 1;
static uint8_t z_speed = 3;
static int16_t a_shift = 0;
static int16_t z_shift = 0;

void *inputThread(void *data)
{
    char *name = (char*)data;
 
    printf("Hi from thread name = %s\n", name);
    
	int key = 0;
	while(true) {
		usleep(100000);
		key = getch();
	
		// skip buffered repeats                                                                                     
		if (key != ERR) {
		  while (getch() == key);
		}
	
		switch (key) {
		case KEY_LEFT:
		  a_shift += a_speed; break;
		case KEY_RIGHT:
		  a_shift -= a_speed; break;
		case KEY_UP:
		  z_shift -= z_speed; break;
		case KEY_DOWN:
		  z_shift += z_speed; break;
		}
	
		fflush(stdout);
    }
 
    printf("Thread %s done!\n", name);
    return NULL;
}

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
    
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    pthread_t th_input;
    pthread_create(&th_input, NULL, inputThread, "input_thread");

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
                int a_adj = (slice + a_shift);
                if(a_adj > SLICE_COUNT){
                    a_adj -= SLICE_COUNT;
                }
                else if(a_adj < 0){
                    a_adj += SLICE_COUNT;
                }
                voxel_2D_t voxel_2D = slice_map[a_adj][r][panel_index];
                
                int z_adj = (panel_2_voxel_z(panel_z) + z_shift);
                if(z_adj > VOXELS_Z){
                    z_adj -= VOXELS_Z;
                }
                else if(z_adj < 0){
                    z_adj += VOXELS_Z;
                }
                pixel_t color = volume[z_adj][voxel_2D.y][voxel_2D.x];

                uint8_t r_ = ((color & 0b11100000) >> 5) > 0b111 / 2 ? 255 : 0;
                uint8_t g_ = ((color & 0b00011100) >> 2) > 0b111 / 2 ? 255 : 0;
                uint8_t b_ = (color & 0b00000011) > 0b11 / 2 ? 255 : 0;
                led_canvas_set_pixel(canvas, r, panel_z, r_, g_, b_);
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
    
    endwin();

    /*
     * Make sure to always call led_matrix_delete() in the end to reset the
     * display. Installing signal handlers for defined exit is a good idea.
     */

    return 0;
}