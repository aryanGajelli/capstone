#include "led.h"

#include <stdio.h>
#include <string.h>

#include "bsp.h"
#include "delay_us.h"
#include "main.h"
#include "cmsis_gcc.h"
#include "stm32f4xx_hal.h"


#define BITS 5
#define PERIOD_US 50

uint16_t frame[LED_HEIGHT][LED_WIDTH];

// fast gpio set and reset

#define set_pin(port, pin) (port->BSRR = pin)
#define reset_pin(port, pin) (port->BSRR = pin << 16U)

// Control pins
#define clk_en() reset_pin(MAT_CLK_GPIO_Port, MAT_CLK_Pin)  // Active low
#define clk_dis() set_pin(MAT_CLK_GPIO_Port, MAT_CLK_Pin)

#define latch_en() set_pin(MAT_LAT_GPIO_Port, MAT_LAT_Pin)
#define latch_dis() reset_pin(MAT_LAT_GPIO_Port, MAT_LAT_Pin)

#define mat_en() reset_pin(MAT_OE_GPIO_Port, MAT_OE_Pin)  // Active low
#define mat_dis() set_pin(MAT_OE_GPIO_Port, MAT_OE_Pin)

#define A_low() reset_pin(MAT_A_GPIO_Port, MAT_A_Pin)
#define A_high() set_pin(MAT_A_GPIO_Port, MAT_A_Pin)

#define B_low() reset_pin(MAT_B_GPIO_Port, MAT_B_Pin)
#define B_high() set_pin(MAT_B_GPIO_Port, MAT_B_Pin)

#define C_low() reset_pin(MAT_C_GPIO_Port, MAT_C_Pin)
#define C_high() set_pin(MAT_C_GPIO_Port, MAT_C_Pin)

#define D_low() reset_pin(MAT_D_GPIO_Port, MAT_D_Pin)
#define D_high() set_pin(MAT_D_GPIO_Port, MAT_D_Pin)

#define pulse_clk() \
    do {            \
        clk_en();   \
        clk_dis();  \
    } while (0)

#define selectRow(row)                       \
    do {                                     \
        ((row) & 0x01) ? A_high() : A_low(); \
        ((row) & 0x02) ? B_high() : B_low(); \
        ((row) & 0x04) ? C_high() : C_low(); \
        ((row) & 0x08) ? D_high() : D_low(); \
    } while (0)

// Data pins
#define MAT_RGB1_GPIO_Port MAT_R1_GPIO_Port

#define r1_high() set_pin(MAT_R1_GPIO_Port, MAT_R1_Pin)
#define r1_low() reset_pin(MAT_R1_GPIO_Port, MAT_R1_Pin)

#define g1_high() set_pin(MAT_G1_GPIO_Port, MAT_G1_Pin)
#define g1_low() reset_pin(MAT_G1_GPIO_Port, MAT_G1_Pin)

#define b1_high() set_pin(MAT_B1_GPIO_Port, MAT_B1_Pin)
#define b1_low() reset_pin(MAT_B1_GPIO_Port, MAT_B1_Pin)

#define r2_high() set_pin(MAT_R2_GPIO_Port, MAT_R2_Pin)
#define r2_low() reset_pin(MAT_R2_GPIO_Port, MAT_R2_Pin)

#define g2_high() set_pin(MAT_G2_GPIO_Port, MAT_G2_Pin)
#define g2_low() reset_pin(MAT_G2_GPIO_Port, MAT_G2_Pin)

#define b2_high() set_pin(MAT_B2_GPIO_Port, MAT_B2_Pin)
#define b2_low() reset_pin(MAT_B2_GPIO_Port, MAT_B2_Pin)

HAL_StatusTypeDef ledInit(void) {
    // Initialize the LED matrix
    // This function should be called before any other functions
    // that interact with the LED matrix
    clk_dis();
    latch_dis();
    mat_dis();
    return HAL_TIM_Base_Start_IT(&PIXEL_TIMER);
}

void clearMatrix() {
    // Clear the LED matrix
    memset((void*)frame, 0, sizeof(frame));
    delayUS(2 * PERIOD_US);  // wait for 100us to allow auto frame clear by the timer
}

uint8_t matrixRow = 0;
uint8_t bitplane = 0;
uint32_t delay = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    if (htim->Instance == PIXEL_TIMER.Instance) {
        if (bitplane == 0) {
            selectRow(matrixRow);
        }

        uint32_t start = __HAL_TIM_GET_COUNTER(&PIXEL_TIMER);

        // bitplane masks to check if r g b should be high or low
        uint16_t bitplane_mask = 1u << bitplane;
        uint16_t r_mask = bitplane_mask << (2 * BITS - 1);
        uint16_t g_mask = bitplane_mask << (BITS - 1);

        // send data serially
        for (uint8_t x = 0; x < LED_WIDTH; x++) {
            uint16_t p1 = frame[matrixRow][x];
            uint16_t p2 = frame[matrixRow + LED_HEIGHT / 2][x];

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

        // delay for respective portion of the period so that the row is visible
        delay = __HAL_TIM_GET_COUNTER(&PIXEL_TIMER) - start;
        uint32_t p = PERIOD_US * bitplane_mask / (1 << BITS);
        if (p > delay)
            __HAL_TIM_SET_AUTORELOAD(&PIXEL_TIMER, p - delay);

        // advance to next bitplane or row
        if (bitplane == BITS) {
            bitplane = 0;
            matrixRow = (matrixRow + 1) % (LED_HEIGHT / 2);
        } else {
            bitplane++;
        }
    }
}
void test_led() {
    clearMatrix();

    // draw 9 bit color gradient

    for (int y = 0; y < LED_HEIGHT; y++) {
        for (int x = 0; x < LED_WIDTH; x++) {
            frame[y][x] = x << (2 * BITS) | y;
        }
    }
    // swap 1st and 2nd rows
    // for (int x = 0; x < LED_WIDTH; x++) {
    //     uint16_t temp = frame[0][x];
    //     frame[0][x] = frame[1][x];
    //     frame[1][x] = temp;
    // }
    // print frame
    // for (int y = 0; y < LED_HEIGHT; y++) {
    //     for (int x = 0; x < LED_WIDTH; x++) {
    //         printf("%0x%0x%0x ", frame[y][x].r, frame[y][x].g, frame[y][x].b);
    //     }
    //     printf("\n");
    // }
    // int h = 2, w = 2, dx = 2, dy = -3;
    // int x = LED_WIDTH/2 - w/2, y = LED_HEIGHT/2 - h/2;
    while (1) {
        // clearMatrix();
        // // draw rotating red square
        // for (int i = 0; i < w; i++) {
        //     for (int j = 0; j < h; j++) {
        //         frame[y + j][x + i] = 0x11<<(2*BITS);
        //     }
        // }
        // x += dx;
        // y += dy;
        // if (x + w >= LED_WIDTH || x < 0) {
        //     dx = -dx;
        //     x += dx;
        // }
        // if (y + h >= LED_HEIGHT || y < 0) {
        //     dy = -dy;
        //     y += dy;
        // }
        // printf("%lu\n", delay);
        HAL_Delay(1000);
    }
}