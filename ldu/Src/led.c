#include "led.h"

#include <stdio.h>
#include <string.h>

#include "bsp.h"
#include "delay_us.h"
#include "main.h"
#include "stm32f4xx_hal.h"

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

#define selectRow(row)    \
    do {                  \
        if ((row) & 0x01) \
            A_high();     \
        else              \
            A_low();      \
        if ((row) & 0x02) \
            B_high();     \
        else              \
            B_low();      \
        if ((row) & 0x04) \
            C_high();     \
        else              \
            C_low();      \
        if ((row) & 0x08) \
            D_high();     \
        else              \
            D_low();      \
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
    return HAL_OK;
    // return HAL_TIM_Base_Start_IT(&PIXEL_TIMER);
}

void clearMatrix() {
    // Clear the LED matrix
    memset((void*)frame, 0, sizeof(frame));
    delayUS(1100);  // wait for 1ms to allow auto frame clear by the timer
}

uint8_t matrixRow = 0;
// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
//     if (htim->Instance == PIXEL_TIMER.Instance) {
//         return;
//         /*
//         mat_dis();
//         latch_en();
//         selectRow(matrixRow);
//         for (int x = 0; x < LED_WIDTH; x++) {
//             pixel p1 = frame[matrixRow][x];
//             pixel p2 = frame[matrixRow + LED_HEIGHT / 2][x];
//             if (p1.r)
//                 r1_high();
//             else
//                 r1_low();

//             if (p1.g)
//                 g1_high();
//             else
//                 g1_low();

//             if (p1.b)
//                 b1_high();
//             else
//                 b1_low();

//             if (p2.r)
//                 r2_high();
//             else
//                 r2_low();

//             if (p2.g)
//                 g2_high();
//             else
//                 g2_low();

//             if (p2.b)
//                 b2_high();
//             else
//                 b2_low();

//             pulse_clk();
//         }
//         latch_dis();
//         mat_en();
//         matrixRow = (matrixRow + 1) % (LED_HEIGHT / 2);
//         */
//     }
// }
void test_led() {
    clearMatrix();

    // draw 9 bit color gradient

    for (int y = 0; y < LED_HEIGHT; y++) {
        for (int x = 0; x < LED_WIDTH; x++) {
            frame[y][x] = x << 10 | y;
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

    const int bits = 5;
    uint16_t p1, p2;
    const int period = 200;

    while (1) {
        // rotate the frame
        // for (int y = 0; y < LED_HEIGHT / 2; y++) {
        //     for (int x = 0; x < LED_WIDTH; x++) {
        //         frame[y][x] = frame[y + 1][x];
        //         frame[y + LED_HEIGHT / 2][x] = frame[y + LED_HEIGHT / 2 + 1][x];
        //     }
        // }
        if (matrixRow == 0)
            selectRow(1);
        else if (matrixRow == 1)
            selectRow(0);
        else
            selectRow(matrixRow);

        for (int bitplane = bits; bitplane >= 0; bitplane--) {
            __HAL_TIM_SET_COUNTER(&US_DELAY_TIMER, 0);
            uint16_t bitplane_mask = 1u << bitplane;
            uint16_t mask1 = bitplane_mask << 9;
            uint16_t mask2 = bitplane_mask << 4;
            for (uint8_t x = 0; x < LED_WIDTH; x++) {
                p1 = frame[matrixRow][x];
                p2 = frame[matrixRow + LED_HEIGHT / 2][x];
                // MAT_RGB1_GPIO_Port->BSRR = ((1 << bitplane) & p1.r) ? MAT_R1_Pin : ((uint32_t)MAT_R1_Pin << 16u);
                (mask1 & p1) ? r1_high() : r1_low();
                (mask2 & p1) ? g1_high() : g1_low();
                (bitplane_mask & p1) ? b1_high() : b1_low();

                (mask1 & p2) ? r2_high() : r2_low();
                (mask2 & p2) ? g2_high() : g2_low();
                (bitplane_mask & p2) ? b2_high() : b2_low();

                pulse_clk();
            }
            
            mat_dis();
            latch_en();
            latch_dis();
            mat_en();
            uint32_t delay = __HAL_TIM_GET_COUNTER(&US_DELAY_TIMER);
            // printf("%d: %d\n", bitplane, delay);
            uint32_t p = period * bitplane_mask / (1 << bits);
            if (p > delay)
                delayUS(p - delay);
        }
        matrixRow = (matrixRow + 1) % (LED_HEIGHT / 2);
    }
}