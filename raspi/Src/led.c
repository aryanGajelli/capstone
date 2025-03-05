#include "led.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define BITS 5
#define PERIOD_US 50

#define PAGE_SIZE (4 * 1024)
#define BLOCK_SIZE (4 * 1024)

#ifndef PI_VERSION
#define PI_VERSION 4
#endif

#define BCM2708_PI1_PERI_BASE 0x20000000
#define BCM2709_PI2_PERI_BASE 0x3F000000
#define BCM2711_PI4_PERI_BASE 0xFE000000
#define BCM2712_PI5_PERI_BASE 0x1f000d0000

// Pi-specific setup.
#if PI_VERSION == 1
#define PERI_BASE BCM2708_PI1_PERI_BASE
#elif PI_VERSION == 2 || PI_VERSION == 3
#define PERI_BASE BCM2709_PI2_PERI_BASE
#elif PI_VERSION == 4
#define PERI_BASE BCM2711_PI4_PERI_BASE
#else
#define PERI_BASE BCM2712_PI5_PERI_BASE
#endif

#define GPIO_BASE (PERI_BASE + 0x200000) /* GPIO controller */

volatile unsigned *gpio = NULL;

uint16_t frame[LED_HEIGHT][LED_WIDTH] = {0};

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO_UNSAFE(x)
#define INP_GPIO(g) *(gpio + ((g) / 10)) &= ~(7 << (((g) % 10) * 3))
#define OUT_GPIO_UNSAFE(g) *(gpio + ((g) / 10)) |= (1 << (((g) % 10) * 3))

#define OUT_GPIO(g) \
    INP_GPIO(g);    \
    OUT_GPIO_UNSAFE(g)

#define GPIO_SET_REG *(gpio + 7)   // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR_REG *(gpio + 10)  // clears bits which are 1 ignores bits which are 0

#define GPIO_SET(g) (GPIO_SET_REG = 1 << (g))
#define GPIO_CLR(g) (GPIO_CLR_REG = 1 << (g))

#define GET_GPIO(g) (*(gpio + 13) & (1 << g))  // 0 if LOW, (1<<g) if HIGH

#define GPIO_PULL *(gpio + 37)      // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio + 38)  // Pull up/pull down clock

// TODO: Choose GPIO definitions
#define MAT_CLK_Pin (4)
#define MAT_LAT_Pin (3)
#define MAT_OE_Pin (2)
#define MAT_A_Pin (14)
#define MAT_B_Pin (15)
#define MAT_C_Pin (18)
#define MAT_D_Pin (23)
#define MAT_E_Pin (24)
#define MAT_R1_Pin (10)
#define MAT_G1_Pin (9)
#define MAT_B1_Pin (11)
#define MAT_R2_Pin (5)
#define MAT_G2_Pin (6)
#define MAT_B2_Pin (13)

#define SR_CLK_Pin (27)
#define SR_LAT_Pin (26)
#define SR_DAT_Pin (25)

// Control pins

#define sr_clk_low() GPIO_CLR(SR_CLK_Pin)
#define sr_clk_high() GPIO_SET(SR_CLK_Pin)

#define sr_lat_low() GPIO_CLR(SR_CLK_Pin)
#define sr_lat_high() GPIO_SET(SR_CLK_Pin)

#define sr_dat_low() GPIO_CLR(SR_CLK_Pin)
#define sr_dat_high() GPIO_SET(SR_CLK_Pin)

#define clk_en() GPIO_CLR(MAT_CLK_Pin)  // Active low
#define clk_dis() GPIO_SET(MAT_CLK_Pin)

#define latch_en() GPIO_SET(MAT_LAT_Pin)
#define latch_dis() GPIO_CLR(MAT_LAT_Pin)

#define mat_en() GPIO_CLR(MAT_OE_Pin)  // Active low
#define mat_dis() GPIO_SET(MAT_OE_Pin)

#define A_low() GPIO_CLR(MAT_A_Pin)
#define A_high() GPIO_SET(MAT_A_Pin)

#define B_low() GPIO_CLR(MAT_B_Pin)
#define B_high() GPIO_SET(MAT_B_Pin)

#define C_low() GPIO_CLR(MAT_C_Pin)
#define C_high() GPIO_SET(MAT_C_Pin)

#define D_low() GPIO_CLR(MAT_D_Pin)
#define D_high() GPIO_SET(MAT_D_Pin)

#define E_low() GPIO_CLR(MAT_E_Pin)
#define E_high() GPIO_SET(MAT_E_Pin)

#define pulse_clk() \
    do {            \
        clk_en();   \
        clk_dis();  \
    } while (0)

#define select_row_direct(row)               \
    do {                                     \
        ((row) & 0x01) ? A_high() : A_low(); \
        ((row) & 0x02) ? B_high() : B_low(); \
        ((row) & 0x04) ? C_high() : C_low(); \
        ((row) & 0x08) ? D_high() : D_low(); \
        ((row) & 0x10) ? E_high() : E_low(); \
    } while (0)

// Data pins
#define r1_high() GPIO_SET(MAT_R1_Pin)
#define r1_low() GPIO_CLR(MAT_R1_Pin)

#define g1_high() GPIO_SET(MAT_G1_Pin)
#define g1_low() GPIO_CLR(MAT_G1_Pin)

#define b1_high() GPIO_SET(MAT_B1_Pin)
#define b1_low() GPIO_CLR(MAT_B1_Pin)

#define r2_high() GPIO_SET(MAT_R2_Pin)
#define r2_low() GPIO_CLR(MAT_R2_Pin)

#define g2_high() GPIO_SET(MAT_G2_Pin)
#define g2_low() GPIO_CLR(MAT_G2_Pin)

#define b2_high() GPIO_SET(MAT_B2_Pin)
#define b2_low() GPIO_CLR(MAT_B2_Pin)

void delay_loop(int16_t n) {
    while (n-- > 0) {
        __asm volatile("nop\n");
    }
}

// Set up memory region to access GPIO
void io_init() {
    int mem_fd = 0;

    /* open /dev/mem */
    if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        printf("can't open /dev/mem \n");
        exit(-1);
    }

    /* mmap GPIO */
    gpio = mmap(
        NULL,                    // Any address in our space will do
        BLOCK_SIZE,              // Map length
        PROT_READ | PROT_WRITE,  // Enable reading & writting to mapped memory
        MAP_SHARED,              // Shared with other processes
        mem_fd,                  // File to map
        GPIO_BASE                // Offset to GPIO peripheral
    );

    close(mem_fd);

    if (gpio == MAP_FAILED) {
        printf("mmap error %lu\n", (uint64_t)gpio);
        exit(-1);
    }

    // Set GPIO pins to output
    OUT_GPIO(MAT_CLK_Pin);
    OUT_GPIO(MAT_LAT_Pin);
    OUT_GPIO(MAT_OE_Pin);
    OUT_GPIO(MAT_A_Pin);
    OUT_GPIO(MAT_B_Pin);
    OUT_GPIO(MAT_C_Pin);
    OUT_GPIO(MAT_D_Pin);
    OUT_GPIO(MAT_E_Pin);

    OUT_GPIO(MAT_R1_Pin);
    OUT_GPIO(MAT_G1_Pin);
    OUT_GPIO(MAT_B1_Pin);
    OUT_GPIO(MAT_R2_Pin);
    OUT_GPIO(MAT_G2_Pin);
    OUT_GPIO(MAT_B2_Pin);
}

void led_init(void) {
    // Initialize the LED matrix
    // This function should be called before any other functions
    // that interact with the LED matrix
    clk_dis();
    latch_dis();
    mat_dis();
    return;
}

void clear_frame() {
    // Clear the LED matrix
    memset((void *)frame, 0, sizeof(frame));
}

void select_row(uint8_t row) {
    // Clock in 8 bits
    for (uint8_t i = 0; i < 8; i++) {
        // Set the bit
        if ( row & (1u << i) ) {
            sr_dat_high();
        } else {
            sr_dat_low();
        }

        // Clock it in
        sr_clk_high();
        sr_clk_low();
    }

    // Latch out the byte
    sr_lat_high();
    sr_lat_low();
}

void draw_row() {
    static uint8_t matrix_row = 0;
    static uint8_t bitplane = 0;

    if (bitplane == 0) {
        select_row(matrix_row);
    }

    // bitplane masks to check if r g b should be high or low
    uint16_t bitplane_mask = 1u << bitplane;
    uint16_t r_mask = bitplane_mask << (2 * BITS - 1);
    uint16_t g_mask = bitplane_mask << (BITS - 1);

    // send data serially
    for (uint8_t x = 0; x < LED_WIDTH; x++) {
        uint16_t p1 = frame[matrix_row][x];
        uint16_t p2 = frame[matrix_row + LED_HEIGHT / 2][x];

        (r_mask & p1) ? r1_high() : r1_low();
        (g_mask & p1) ? g1_high() : g1_low();
        (bitplane_mask & p1) ? b1_high() : b1_low();

        (r_mask & p2) ? r2_high() : r2_low();
        (g_mask & p2) ? g2_high() : g2_low();
        (bitplane_mask & p2) ? b2_high() : b2_low();

        pulse_clk();
    }

    // latch the data and display row
    mat_dis();
    latch_en();
    latch_dis();
    mat_en();

    // TODO: Determine appropriate delay
    delay_loop(5);

    // advance to next bitplane or row
    if (bitplane == BITS) {
        bitplane = 0;
        matrix_row = (matrix_row + 1) % (LED_HEIGHT / 2);
    } else {
        bitplane++;
    }
}

void test_led() {
    io_init();
    led_init();
    clear_frame();

    const uint8_t draw_height = 60;
    const uint8_t draw_width = 30;

    // Store a white rectangle
    for (int y = 0; y < draw_height; y++) {
        for (int x = 0; x < draw_width; x++) {
            frame[y][x] = 0xFFFF;
        }
    }

    while (1) {
        draw_row();
    }
}