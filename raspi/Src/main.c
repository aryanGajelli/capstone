#define _GNU_SOURCE

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

#include "gpio.h"
#include "led-matrix-c.h"
#include "slicemap.h"
#include "volumetric.h"
#include "voxel.h"

static FILE *fptr;

int stick_thread_to_core(int core_id) {
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (core_id < 0 || core_id >= num_cores)
       return EINVAL;
 
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
 
    pthread_t current_thread = pthread_self();    
    return pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
}

void *writerThread(void *data)
{
    stick_thread_to_core(1);

    char *name = (char*)data;
 
    printf("Hi from thread name = %s\n", name);
    
    // populate_volume(fptr);
    volume_draw_continuous();
 
    printf("Thread %s done!\n", name);
    return NULL;
}

int main(int argc, char **argv) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <volume_file.xyzrgb>\n", argv[0]);
        return 1;
    }

    fptr = fopen(argv[1], "r");
    if (fptr == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    pthread_t th_writer;
    pthread_create(&th_writer, NULL, writerThread, "writer_thread");

    // populate_volume(fptr);

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

    options.led_rgb_sequence = "BRG";
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

    int r_val = volumetric_test(canvas, read_reg);
    led_matrix_delete(matrix);

    return r_val;
}