#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "led.h"
#include "vol_test.h"

// Raspberry Pi 2 or 1 ? Since this is a simple example, we don't
// bother auto-detecting but have it a compile-time option.
#ifndef PI_VERSION
#define PI_VERSION 4
#endif

#define BCM2708_PI1_PERI_BASE 0x20000000
#define BCM2709_PI2_PERI_BASE 0x3F000000
#define BCM2711_PI4_PERI_BASE 0xFE000000
#define BCM2712_PI5_PERI_BASE 0x1f000d0000

// --- General, Pi-specific setup.
#if PI_VERSION == 1
#define PERI_BASE BCM2708_PI1_PERI_BASE
#elif PI_VERSION == 2 || PI_VERSION == 3
#define PERI_BASE BCM2709_PI2_PERI_BASE
#elif PI_VERSION == 4
#define PERI_BASE BCM2711_PI4_PERI_BASE
#else
#define PERI_BASE BCM2712_PI5_PERI_BASE
#endif

#define PAGE_SIZE 4096

// ---- GPIO specific defines
#define CLOCK_BASE 0x101000 /* Clocks */
#define GPIO_REGISTER_BASE 0x200000
#define GPIO_SET_OFFSET 0x1C
#define GPIO_CLR_OFFSET 0x28
#define GPIO_READ_OFFSET 0x34
#define GPIO_SET1_OFFSET 0x20
#define GPIO_CLR1_OFFSET 0x2C
#define GPIO_READ1_OFFSET 0x38

#define PHYSICAL_GPIO_BUS (0x7E000000 + GPIO_REGISTER_BASE)

// Return a pointer to a periphery subsystem register.
static void *mmap_bcm_register(off_t register_offset) {
    const off_t base = PERI_BASE;

    int mem_fd;
    if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        perror("can't open /dev/mem: ");
        fprintf(stderr, "You need to run this as root!\n");
        return NULL;
    }

    uint32_t *result = (uint32_t *)mmap(
        NULL,  // Any adddress in our space will do
        PAGE_SIZE,
        PROT_READ | PROT_WRITE,  // Enable r/w on GPIO registers.
        MAP_SHARED,
        mem_fd,                 // File to map
        base + register_offset  // Offset to bcm register
    );

    close(mem_fd);

    if (result == MAP_FAILED) {
        fprintf(stderr, "mmap error %p\n", result);
        return NULL;
    }

    return result;
}

void initialize_gpio_for_output(volatile uint32_t *gpio_registerset, int bit) {
    *(gpio_registerset + (bit / 10)) &= ~(7 << ((bit % 10) * 3));  // prepare: set as input
    *(gpio_registerset + (bit / 10)) |= (1 << ((bit % 10) * 3));   // set as output.
}

void initialize_gpio_for_input(volatile uint32_t *gpio_registerset, int bit) {
    *(gpio_registerset + (bit / 10)) &= ~(7 << ((bit % 10) * 3));  // prepare: set as input
}

void set_gpio_alt(volatile uint32_t *gpio_registerset, int bit, int alt) {
    *(gpio_registerset + (bit / 10)) |= (alt <= 3 ? alt + 4 : alt == 4 ? 3 : 2) << ((bit % 10) * 3);
}
static inline void my_sleep(uint32_t nops) {
    for (uint32_t i = 0; i < nops; i++)
        __asm volatile("nop\n");
}

void gpio_test() {
    // Prepare GPIO
    volatile uint32_t *gpio_port = mmap_bcm_register(GPIO_REGISTER_BASE);
    volatile uint32_t *set_reg = gpio_port + (GPIO_SET_OFFSET / sizeof(uint32_t));
    volatile uint32_t *clr_reg = gpio_port + (GPIO_CLR_OFFSET / sizeof(uint32_t));

    const unsigned CLK_PIN = 27;
    const unsigned DAT_PIN = 3;
    const unsigned LAT_PIN = 22;
    initialize_gpio_for_output(gpio_port, CLK_PIN);
    initialize_gpio_for_output(gpio_port, DAT_PIN);
    initialize_gpio_for_output(gpio_port, LAT_PIN);

    // clock out the alternating high and low
    // Do it. Endless loop, directly setting.
    printf(
        "1) CPU: Writing to GPIO directly in tight loop\n"
        "== Press Ctrl-C to exit.\n");

    uint8_t addr = 0;

    for (;;) {
        uint8_t ctrl = 0;
        uint8_t data = (ctrl << 5) | addr;
        addr = (addr + 1) % 32;
        for (uint8_t i = 0; i < 8; i++) {
            if (data & (1 << i))
                *set_reg = 1 << DAT_PIN;
            else
                *clr_reg = 1 << DAT_PIN;
            *set_reg = 1 << CLK_PIN;
            *clr_reg = 1 << CLK_PIN;
        }
        *set_reg = 1 << LAT_PIN;
        *clr_reg = 1 << LAT_PIN;
        //      printf("%d\n", addr);
        //      my_sleep(50000);
    }
}

void counter_test() {
    // Prepare GPIO
    volatile uint32_t *gpio_port = mmap_bcm_register(GPIO_REGISTER_BASE);
    volatile uint32_t *set_reg = gpio_port + (GPIO_SET_OFFSET / sizeof(uint32_t));
    volatile uint32_t *clr_reg = gpio_port + (GPIO_CLR_OFFSET / sizeof(uint32_t));

    const unsigned CLK_PIN = 4;
    const unsigned CLR_PIN = 3;
    initialize_gpio_for_output(gpio_port, CLK_PIN);
    initialize_gpio_for_output(gpio_port, CLR_PIN);

#define MIN_CLK_DELAY 15
    *clr_reg = (1 << CLR_PIN);
    *set_reg = 1 << CLK_PIN;
    my_sleep(MIN_CLK_DELAY);
    *clr_reg = 1 << CLK_PIN;
    *set_reg = 1 << CLR_PIN;
    for (int i = 0; i < 19; i++) {
        *set_reg = 1 << CLK_PIN;
        my_sleep(MIN_CLK_DELAY);
        *clr_reg = 1 << CLK_PIN;
        my_sleep(MIN_CLK_DELAY);
    }
}

void scl0_test() {
    // Prepare GPIO
    volatile uint32_t *gpio_port = mmap_bcm_register(GPIO_REGISTER_BASE);
    volatile uint32_t *set_reg = gpio_port + (GPIO_SET_OFFSET / sizeof(uint32_t));
    volatile uint32_t *set1_reg = gpio_port + (GPIO_SET1_OFFSET / sizeof(uint32_t));
    volatile uint32_t *clr_reg = gpio_port + (GPIO_CLR_OFFSET / sizeof(uint32_t));
    volatile uint32_t *clr1_reg = gpio_port + (GPIO_CLR1_OFFSET / sizeof(uint32_t));

    const unsigned SCL0_PIN = 45 - 32;
    const unsigned CLK_PIN = 4;
    initialize_gpio_for_output(gpio_port, SCL0_PIN);
    initialize_gpio_for_output(gpio_port, CLK_PIN);

    for (;;) {
        *set1_reg = 1 << SCL0_PIN;
        *set_reg = 1 << CLK_PIN;
        my_sleep(20);
        *clr1_reg = 1 << SCL0_PIN;
        *clr_reg = 1 << CLK_PIN;
        my_sleep(20);
    }
}

#define GZ_CLK_5MHz 0
#define GZ_CLK_125MHz 1
#define GZ_CLK_BUSY (1 << 7)
#define GP_CLK0_CTL_OFFSET (0x1C)
#define GP_CLK0_DIV_OFFSET (0x1D)

void gclk_test() {
    volatile uint32_t *gpio_port = mmap_bcm_register(GPIO_REGISTER_BASE);
    volatile uint32_t *clk_gpio_port = mmap_bcm_register(CLOCK_BASE);
    volatile uint32_t *gpclk0_ctl_reg = clk_gpio_port + (GP_CLK0_CTL_OFFSET / sizeof(uint32_t));
    volatile uint32_t *gpclk0_div_reg = clk_gpio_port + (GP_CLK0_DIV_OFFSET / sizeof(uint32_t));
    
    // change the next 2 for different results
    int speed = 0;
    int divisor = 2;

    int speed_id = 6;
    int mem_fd;
    if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        printf("\rError initializing IO. Consider using sudo.\n");
        exit(-1);
    }
    if (speed < GZ_CLK_5MHz || speed > GZ_CLK_125MHz) {
        printf("gz_clock_ena: Unsupported clock speed selected.\n");
        printf("Supported speeds: GZ_CLK_5MHz (0) and GZ_CLK_125MHz (1).\n");
        exit(-1);
    }
    if (speed == 0) {
        speed_id = 1;
    }
    if (divisor < 2) {
        printf("gz_clock_ena: Minimum divisor value is 2.\n");
        exit(-1);
    }
    if (divisor > 0xfff) {
        printf("gz_clock_ena: Maximum divisor value is %d.\n", 0xfff);
        exit(-1);
    }
    close(mem_fd);  // No need to keep mem_fd open after mmap
    usleep(1000);
    initialize_gpio_for_input(gpio_port, 4);
    set_gpio_alt(gpio_port, 4, 0);
    *gpclk0_ctl_reg = 0x5A000000 | speed_id;  // GPCLK0 off
    while (*gpclk0_ctl_reg & GZ_CLK_BUSY) {
    }  // Wait for BUSY low
    *gpclk0_div_reg = 0x5A002000 | (divisor << 12);  // set DIVI
    *gpclk0_ctl_reg = 0x5A000010 | speed_id;         // GPCLK0 on
    char aChar;
    printf("\nPress any key to stop test.");
    scanf("%c", &aChar);
    initialize_gpio_for_input(gpio_port, 4);
    return 0;
}


uint32_t read_gpio(volatile uint32_t *read_reg, int bit) {
    return (*read_reg & (1 << bit));
}

void input_test() {
    volatile uint32_t *gpio_port = mmap_bcm_register(GPIO_REGISTER_BASE);
    volatile uint32_t *read1_reg = gpio_port + (GPIO_READ1_OFFSET / sizeof(uint32_t));

    const uint32_t PHOTO_PIN = 44 - 32;
    initialize_gpio_for_input(gpio_port, PHOTO_PIN);

    for (;;) {
        printf("Photo pin: %d\n", read_gpio(gpio_port, PHOTO_PIN));
        my_sleep(100000);
    }
}

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
    options.pwm_bits = 1;
    options.pwm_lsb_nanoseconds = 20;
    options.pwm_dither_bits = 2;
    options.show_refresh_rate = true;

    memset(&rt_options, 0, sizeof(rt_options));
    rt_options.gpio_slowdown = 3;

    static uint32_t *gpio_reg;

    /* This supports all the led commandline options. Try --led-help */
    matrix = led_matrix_create_from_options_and_rt_options(&options, &rt_options, &gpio_reg);
    if (matrix == NULL)
      return 1;
  
    /* Let's do an example with double-buffering. We create one extra
     * buffer onto which we draw, which is then swapped on each refresh.
     * This is typically a good aproach for animations and such.
     */
    offscreen_canvas = led_matrix_create_offscreen_canvas(matrix);
  
    led_canvas_get_size(offscreen_canvas, &width, &height);
  
    fprintf(stderr, "Size: %dx%d. Hardware gpio mapping: %s\n",
            width, height, options.hardware_mapping);
    
    const int rows_per_panel = 64;
    const float rpm = 800;
    const float us_per_rev = 1e6 * 60 / rpm;
    
    const int cube_dim = 50;
    int slice = 0;
    const int num_slices = 100;
    const float slice_to_rad = 2 * 3.14159265 / num_slices;

    const uint32_t us_per_slice = us_per_rev / num_slices; // Microseconds per slice

    // Angles that align with a square's diagonals
    const float diag1 = 0.7854; // 45 degrees
    const float diag2 = diag1 * 3;
    const float diag3 = diag1 * 5;
    const float diag4 = diag1 * 7;

    bool prev_sync = false;

    while (true) {
        // Wait until panel has rotated to next slice
        usleep(us_per_slice/10);
        led_canvas_clear(offscreen_canvas);
        int ret = usleep(10*us_per_slice/9);
        if (ret) {
            printf("___USLEEP ERROR___ value: %d\n", ret);
        }


        bool read_sync = (uint32_t)(*gpio_reg) & (1UL << (44-32));
        if (!prev_sync && read_sync) {
        slice = 0;
        //printf("--SYNC--");
        }
        prev_sync = read_sync;
        
        float angle = slice * slice_to_rad;
        int height = cube_dim;
        int width = 0;

        // Calculate width of cube cross-section
        // Adjust formula when crossing the cubes's diagonals
        if ((diag1 < angle && angle < diag2) || (diag3 < angle && angle < diag4)) {
        width = abs(cube_dim / sinf(angle));
        }
        else {
        width = abs(cube_dim / cosf(angle));
        }
        
        // Display red cube cross-section
        height = 192;
        int offset_x = (128 - width)/2;
        int offset_y = 0;//(canvas_height - height)/2;
        for(int x = 0; x < width/2; x++) {
            for (int y = 0; y < height; y++) {
                led_canvas_set_pixel(offscreen_canvas, x + offset_x, y + offset_y, 255, 0, 0);
            }
        }
        offscreen_canvas = led_matrix_swap_on_vsync(matrix, offscreen_canvas);
    }
  
    /*
     * Make sure to always call led_matrix_delete() in the end to reset the
     * display. Installing signal handlers for defined exit is a good idea.
     */
    led_matrix_delete(matrix);
    return 0;
}

int main(int argc, char **argv) {
    // scl0_test();
    // counter_test();
    // test_led();
    // gclk_test();
    // input_test();
    volumetric_test(argc, argv);
    return 0;
}