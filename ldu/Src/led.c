#include "led.h"

#include <stdio.h>
#include <string.h>

#include "bsp.h"
#include "delay_us.h"
#include "main.h"
#include "stm32f4xx_hal.h"

volatile pixel frame[LED_HEIGHT][LED_WIDTH];

// fast gpio set and reset

#define set_pin(port, pin) (port->BSRR = pin)
#define reset_pin(port, pin) (port->BSRR = (uint32_t)pin << 16U)

// Control pins
#define clk_en()  reset_pin(MAT_CLK_GPIO_Port, MAT_CLK_Pin) // Active low
#define clk_dis() set_pin(MAT_CLK_GPIO_Port, MAT_CLK_Pin)

#define latch_en() set_pin(MAT_LAT_GPIO_Port, MAT_LAT_Pin)
#define latch_dis() reset_pin(MAT_LAT_GPIO_Port, MAT_LAT_Pin)

#define mat_en() reset_pin(MAT_OE_GPIO_Port, MAT_OE_Pin) // Active low
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

// Data pins
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

void selectRow(uint8_t row) {
    if (row > 0x0F) {
        printf("Invalid row: got [%u] expected between [0-15]\n", row);
        return;
    }

    HAL_GPIO_WritePin(MAT_A_GPIO_Port, MAT_A_Pin, row & 0x01);
    HAL_GPIO_WritePin(MAT_B_GPIO_Port, MAT_B_Pin, row & 0x02);
    HAL_GPIO_WritePin(MAT_C_GPIO_Port, MAT_C_Pin, row & 0x04);
    HAL_GPIO_WritePin(MAT_D_GPIO_Port, MAT_D_Pin, row & 0x08);
}

void clearMatrix() {
    // Clear the LED matrix
    memset((void*)frame, 0, sizeof(frame));
    draw_frame();
}

void draw_frame() {
    // Draw the frame to the LED matrix
    // mat_en();
    for (int y = 0; y < LED_HEIGHT / 2; y++) {
        mat_dis();
        latch_en();
        selectRow(y);
        for (int x = 0; x < LED_WIDTH; x++) {
            pixel p1 = frame[y][x];
            pixel p2 = frame[y + LED_HEIGHT / 2][x];
            HAL_GPIO_WritePin(MAT_R1_GPIO_Port, MAT_R1_Pin, p1.r > 0);
            HAL_GPIO_WritePin(MAT_G1_GPIO_Port, MAT_G1_Pin, p1.g > 0);
            HAL_GPIO_WritePin(MAT_B1_GPIO_Port, MAT_B1_Pin, p1.b > 0);
            HAL_GPIO_WritePin(MAT_R2_GPIO_Port, MAT_R2_Pin, p2.r > 0);
            HAL_GPIO_WritePin(MAT_G2_GPIO_Port, MAT_G2_Pin, p2.g > 0);
            HAL_GPIO_WritePin(MAT_B2_GPIO_Port, MAT_B2_Pin, p2.b > 0);

            pulse_clk();
        }
        latch_dis();
        mat_en();
        delayUS(10);
    }
    mat_dis();
}

uint8_t matrixRow = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    if (htim->Instance == PIXEL_TIMER.Instance) {
        mat_dis();
        latch_en();
        selectRow(matrixRow);
        for (int x = 0; x < LED_WIDTH; x++) {
            pixel p1 = frame[matrixRow][x];
            pixel p2 = frame[matrixRow + LED_HEIGHT / 2][x];
            if (p1.r > 0) r1_high();
            else r1_low();

            if (p1.g > 0) g1_high();
            else g1_low();

            if (p1.b > 0) b1_high();
            else b1_low();

            if (p2.r > 0) r2_high();
            else r2_low();

            if (p2.g > 0) g2_high();
            else g2_low();

            if (p2.b > 0) b2_high();
            else b2_low();

            pulse_clk();
        }
        latch_dis();
        mat_en();
        matrixRow = (matrixRow + 1) % (LED_HEIGHT / 2);
    }
}
void test_led() {
    clearMatrix();

    uint32_t start = HAL_GetTick(), curr;
    pixel p = {1, 0, 0};
    uint16_t radius = 1;
    int sign = 1;
    while (1) {
        // draw circle with changing radius
        curr = HAL_GetTick();
        if (curr - start > 100) {
            start = curr;
            if (radius > LED_HEIGHT / 2) {
                sign *= -1;
            }
            if (radius <= 0) {
                sign *= -1;
                if (p.r) {
                    p = (pixel){0, 1, 0};
                } else if (p.g) {
                    p = (pixel){0, 0, 1};
                } else {
                    p = (pixel){1, 0, 0};
                }
            }

            radius += sign;
        }

        for (int i = 0; i < LED_HEIGHT; i++) {
            for (int j = 0; j < LED_WIDTH; j++) {
                if ((i - LED_HEIGHT / 2) * (i - LED_HEIGHT / 2) + (j - LED_WIDTH / 2) * (j - LED_WIDTH / 2) < radius * radius) {
                    frame[i][j] = p;
                } else {
                    frame[i][j] = (pixel){0, 0, 0};
                }
            }
        }
    }
}