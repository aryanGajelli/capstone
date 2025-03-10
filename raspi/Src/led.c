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

#ifndef USE_COUNTER
#define USE_COUNTER
#endif

#define MIN_CNTR_CLK_DELAY 50
#define MIN_RGB_CLK_DELAY 50

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

uint16_t frame[LED_HEIGHT * 2][LED_WIDTH * 2] = {0};

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO_UNSAFE(x)
#define INP_GPIO(g) *(gpio + ((g) / 10)) &= ~(7 << (((g) % 10) * 3))
#define OUT_GPIO_UNSAFE(g) *(gpio + ((g) / 10)) |= (1 << (((g) % 10) * 3))

#define OUT_GPIO(g) \
    INP_GPIO(g);    \
    OUT_GPIO_UNSAFE(g)

#define GPIO_SET_REG *(gpio + 0x1c / sizeof(uint32_t))  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR_REG *(gpio + 0x28 / sizeof(uint32_t))  // clears bits which are 1 ignores bits which are 0

#define GPIO1_SET_REG *(gpio + 0x20 / sizeof(uint32_t))  // sets   bits which are 1 ignores bits which are 0
#define GPIO1_CLR_REG *(gpio + 0x2c / sizeof(uint32_t))  // clears bits which are 1 ignores bits which are 0

#define GPIO_BIT(g) (1 << (g))
#define GPIO_SET(g) (GPIO_SET_REG = 1 << (g))
#define GPIO_CLR(g) (GPIO_CLR_REG = 1 << (g))

#define GPIO1_SET(g) (GPIO1_SET_REG = 1 << (g))
#define GPIO1_CLR(g) (GPIO1_CLR_REG = 1 << (g))

#define GET_GPIO(g) (*(gpio + 13) & (1 << g))  // 0 if LOW, (1<<g) if HIGH

#define GPIO_PULL *(gpio + 37)      // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio + 38)  // Pull up/pull down clock

// TODO: Choose GPIO definitions
#define MAT_CLK_Pin (4)
#define MAT_LAT_Pin (2)
#define MAT_OE_Pin (18)
// #define MAT_A_Pin (14)
// #define MAT_B_Pin (15)
// #define MAT_C_Pin (18)
// #define MAT_D_Pin (23)
// #define MAT_E_Pin (24)
#define p0_r1_pin (17)
#define p0_g1_pin (27)
#define p0_b1_pin (22)
#define p0_r2_pin (14)
#define p0_g2_pin (15)
#define p0_b2_pin (23)

#define p1_r1_pin (10)
#define p1_g1_pin (9)
#define p1_b1_pin (11)
#define p1_r2_pin (24)
#define p1_g2_pin (25)
#define p1_b2_pin (8)

#define p2_r1_pin (0)
#define p2_g1_pin (5)
#define p2_b1_pin (6)
#define p2_r2_pin (7)
#define p2_g2_pin (1)
#define p2_b2_pin (12)

#define p3_r1_pin (13)
#define p3_g1_pin (19)
#define p3_b1_pin (26)
#define p3_r2_pin (16)
#define p3_g2_pin (20)
#define p3_b2_pin (21)

#ifdef USE_SHIFT_REGISTER
#define SR_CLK_Pin (27)
#define SR_LAT_Pin (26)
#define SR_DAT_Pin (25)
#endif

#ifdef USE_COUNTER
#define CNTR_CLK_Pin (3)
#define CNTR_CLR_Pin (45 - 32)  // clear pin is shared but thats ok as the r1 pin is not used when selecting address and doesn't matter
#endif

// Control pins

// #define sr_clk_low() GPIO_CLR(SR_CLK_Pin)
// #define sr_clk_high() GPIO_SET(SR_CLK_Pin)

// #define sr_lat_low() GPIO_CLR(SR_CLK_Pin)
// #define sr_lat_high() GPIO_SET(SR_CLK_Pin)

// #define sr_dat_low() GPIO_CLR(SR_CLK_Pin)
// #define sr_dat_high() GPIO_SET(SR_CLK_Pin)

#define clk_en() GPIO_CLR(MAT_CLK_Pin)  // Active low
#define clk_dis() GPIO_SET(MAT_CLK_Pin)

#define latch_en() GPIO_SET(MAT_LAT_Pin)
#define latch_dis() GPIO_CLR(MAT_LAT_Pin)

#define mat_en() GPIO_CLR(MAT_OE_Pin)  // Active low
#define mat_dis() GPIO_SET(MAT_OE_Pin)

// #define A_low() GPIO_CLR(MAT_A_Pin)
// #define A_high() GPIO_SET(MAT_A_Pin)

// #define B_low() GPIO_CLR(MAT_B_Pin)
// #define B_high() GPIO_SET(MAT_B_Pin)

// #define C_low() GPIO_CLR(MAT_C_Pin)
// #define C_high() GPIO_SET(MAT_C_Pin)

// #define D_low() GPIO_CLR(MAT_D_Pin)
// #define D_high() GPIO_SET(MAT_D_Pin)

// #define E_low() GPIO_CLR(MAT_E_Pin)
// #define E_high() GPIO_SET(MAT_E_Pin)

#define pulse_clk() \
    do {            \
        clk_en();   \
        clk_dis();  \
    } while (0)

// #define select_row_direct(row)               \
//     do {                                     \
//         ((row) & 0x01) ? A_high() : A_low(); \
//         ((row) & 0x02) ? B_high() : B_low(); \
//         ((row) & 0x04) ? C_high() : C_low(); \
//         ((row) & 0x08) ? D_high() : D_low(); \
//         ((row) & 0x10) ? E_high() : E_low(); \
//     } while (0)

// Data pins
#define r1_high() GPIO_SET(p0_r1_pin)
#define r1_low() GPIO_CLR(p0_r1_pin)

#define g1_high() GPIO_SET(p0_g1_pin)
#define g1_low() GPIO_CLR(p0_g1_pin)

#define b1_high() GPIO_SET(p0_b1_pin)
#define b1_low() GPIO_CLR(p0_b1_pin)

#define r2_high() GPIO_SET(p0_r2_pin)
#define r2_low() GPIO_CLR(p0_r2_pin)

#define g2_high() GPIO_SET(p0_g2_pin)
#define g2_low() GPIO_CLR(p0_g2_pin)

#define b2_high() GPIO_SET(p0_b2_pin)
#define b2_low() GPIO_CLR(p0_b2_pin)

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

    OUT_GPIO(p0_r1_pin);
    OUT_GPIO(p0_g1_pin);
    OUT_GPIO(p0_b1_pin);
    OUT_GPIO(p0_r2_pin);
    OUT_GPIO(p0_g2_pin);
    OUT_GPIO(p0_b2_pin);

    OUT_GPIO(p1_r1_pin);
    OUT_GPIO(p1_g1_pin);
    OUT_GPIO(p1_b1_pin);
    OUT_GPIO(p1_r2_pin);
    OUT_GPIO(p1_g2_pin);
    OUT_GPIO(p1_b2_pin);

    OUT_GPIO(p2_r1_pin);
    OUT_GPIO(p2_g1_pin);
    OUT_GPIO(p2_b1_pin);
    OUT_GPIO(p2_r2_pin);
    OUT_GPIO(p2_g2_pin);
    OUT_GPIO(p2_b2_pin);

    OUT_GPIO(p3_r1_pin);
    OUT_GPIO(p3_g1_pin);
    OUT_GPIO(p3_b1_pin);
    OUT_GPIO(p3_r2_pin);
    OUT_GPIO(p3_g2_pin);
    OUT_GPIO(p3_b2_pin);

#ifdef USE_SHIFT_REGISTER
    OUT_GPIO(SR_CLK_Pin);
    OUT_GPIO(SR_LAT_Pin);
    OUT_GPIO(SR_DAT_Pin);
#endif

#ifdef USE_COUNTER
    OUT_GPIO(CNTR_CLK_Pin);
    OUT_GPIO(CNTR_CLR_Pin);
#endif
}

void led_init(void) {
    // Initialize the LED matrix
    // This function should be called before any other functions
    // that interact with the LED matrix

    clk_dis();
    latch_dis();
    mat_dis();
    reset_row_cntr();
    return;
}

void clear_frame() {
    // Clear the LED matrix
    memset((void *)frame, 0, sizeof(frame));
}

uint8_t curr_row = 0;

void reset_row_cntr() {
    // Reset the row counter
    GPIO1_CLR(CNTR_CLR_Pin);
    GPIO_SET(CNTR_CLK_Pin);
    delay_loop(50);
    GPIO_CLR(CNTR_CLK_Pin);
    GPIO1_SET(CNTR_CLR_Pin);
    delay_loop(50);
    curr_row = 0;
}

#define pulse_cntr_clk()                \
    do {                                \
        GPIO_SET(CNTR_CLK_Pin);         \
        delay_loop(MIN_CNTR_CLK_DELAY); \
        GPIO_CLR(CNTR_CLK_Pin);         \
        delay_loop(MIN_CNTR_CLK_DELAY); \
    } while (0)

static inline void inc_row_cntr() {
    // Increment the row counter
    pulse_cntr_clk();
    curr_row = (curr_row + 1) % LED_ROW_HEIGHT;
}
/**
 * Select a row on the LED matrix by incrementing the row counter
 * to the desired row.
 * @param row The row to select (must be between 0-31 inclusive)
 */
static inline void select_row_cntr(uint8_t row) {
    if (row == curr_row) {
        return;
    }

    if (row >= LED_ROW_HEIGHT) {
        perror("Invalid row\n");
        printf("Invalid row given [%d] must be < %d inclusive\n", row, LED_ROW_HEIGHT);
        return;
    }
    if (row > curr_row) {
        for (uint8_t i = 0; i < row - curr_row; row++) {
            pulse_cntr_clk();
        }
    } else if (row < curr_row) {
        for (uint8_t i = 0; i < row; i++) {
            pulse_cntr_clk();
        }
    }
    curr_row = row;
}

// void select_row_sr(uint8_t row) {
//     // Clock in 8 bits
//     for (uint8_t i = 0; i < 8; i++) {
//         // Set the bit
//         if (row & (1u << i)) {
//             sr_dat_high();
//         } else {
//             sr_dat_low();
//         }

//         // Clock it in
//         sr_clk_high();
//         sr_clk_low();
//     }

//     // Latch out the byte
//     sr_lat_high();
//     sr_lat_low();
// }

void draw_row() {
    // static uint8_t matrix_row = 0;
    static uint8_t bitplane = 0;

    // if (bitplane == 0) {
    //     select_row(matrix_row);
    // }

    // bitplane masks to check if r g b should be high or low
    // uint16_t bitplane_mask = 1u << bitplane;
    // uint16_t r_mask = bitplane_mask << (2 * BITS - 1);
    // uint16_t g_mask = bitplane_mask << (BITS - 1);

    // send data serially
    for (uint8_t x = 0; x < LED_WIDTH; x++) {
        uint16_t p0_1 = frame[curr_row][x];
        uint16_t p0_2 = frame[curr_row + LED_ROW_HEIGHT][x];
        uint16_t p1_1 = frame[curr_row + LED_ROW_HEIGHT * 2][x];
        uint16_t p1_2 = frame[curr_row + LED_ROW_HEIGHT * 3][x];

        uint32_t set_mask = 0;
        uint32_t clr_mask = 0;

        if (p0_1 & 0b100)
            set_mask |= GPIO_BIT(p0_r1_pin);
        else
            clr_mask |= GPIO_BIT(p0_r1_pin);

        if (p0_1 & 0b010)
            set_mask |= GPIO_BIT(p0_g1_pin);
        else
            clr_mask |= GPIO_BIT(p0_g1_pin);

        if (p0_1 & 0b001)
            set_mask |= GPIO_BIT(p0_b1_pin);
        else
            clr_mask |= GPIO_BIT(p0_b1_pin);

        if (p0_2 & 0b100)
            set_mask |= GPIO_BIT(p0_r2_pin);
        else
            clr_mask |= GPIO_BIT(p0_r2_pin);

        if (p0_2 & 0b010)
            set_mask |= GPIO_BIT(p0_g2_pin);
        else
            clr_mask |= GPIO_BIT(p0_g2_pin);

        if (p0_2 & 0b001)
            set_mask |= GPIO_BIT(p0_b2_pin);
        else
            clr_mask |= GPIO_BIT(p0_b2_pin);

        if (p1_1 & 0b100)
            set_mask |= GPIO_BIT(p1_r1_pin);
        else
            clr_mask |= GPIO_BIT(p1_r1_pin);

        if (p1_1 & 0b010)
            set_mask |= GPIO_BIT(p1_g1_pin);
        else
            clr_mask |= GPIO_BIT(p1_g1_pin);

        if (p1_1 & 0b001)
            set_mask |= GPIO_BIT(p1_b1_pin);
        else
            clr_mask |= GPIO_BIT(p1_b1_pin);

        if (p1_2 & 0b100)
            set_mask |= GPIO_BIT(p1_r2_pin);
        else
            clr_mask |= GPIO_BIT(p1_r2_pin);

        if (p1_2 & 0b010)
            set_mask |= GPIO_BIT(p1_g2_pin);
        else
            clr_mask |= GPIO_BIT(p1_g2_pin);

        if (p1_2 & 0b001)
            set_mask |= GPIO_BIT(p1_b2_pin);
        else
            clr_mask |= GPIO_BIT(p1_b2_pin);

        GPIO_SET_REG = set_mask;
        GPIO_CLR_REG = clr_mask;
        // pulse_clk();
        clk_en();
        delay_loop(MIN_RGB_CLK_DELAY);
        clk_dis();
    }
    // latch the data and display row
    mat_dis();
    latch_en();
    delay_loop(MIN_RGB_CLK_DELAY);
    latch_dis();
    mat_en();

    // TODO: Determine appropriate delay
    // delay_loop(5);
    inc_row_cntr();

    // // advance to next bitplane or row
    // if (bitplane == BITS) {
    //     bitplane = 0;
    //     matrix_row = (matrix_row + 1) % (LED_HEIGHT / 2);
    // } else {
    //     bitplane++;
    // }
}

void test_led() {
    io_init();
    led_init();
    clear_frame();

    const uint8_t draw_height = 50;
    const uint8_t draw_width = 30;

    // Store a white rectangle
    for (int y = 0; y < draw_height; y++) {
        for (int x = 0; x < draw_width; x++) {
            frame[y][x] = 0b111;
        }
    }
    reset_row_cntr();
    while (1) {
        draw_row();
    }
}