#include "led.h"

#include <stdio.h>
#include <string.h>

#include "delay_us.h"
#include "main.h"
#include "bsp.h"
#include "stm32f4xx_hal.h"

volatile pixel frame[LED_HEIGHT][LED_WIDTH];

// Control pins
#define clk_en() HAL_GPIO_WritePin(MAT_CLK_GPIO_Port, MAT_CLK_Pin, GPIO_PIN_RESET)  // Active low
#define clk_dis() HAL_GPIO_WritePin(MAT_CLK_GPIO_Port, MAT_CLK_Pin, GPIO_PIN_SET)

#define latch_en() HAL_GPIO_WritePin(MAT_LAT_GPIO_Port, MAT_LAT_Pin, GPIO_PIN_SET)
#define latch_dis() HAL_GPIO_WritePin(MAT_LAT_GPIO_Port, MAT_LAT_Pin, GPIO_PIN_RESET)

#define mat_en() HAL_GPIO_WritePin(MAT_OE_GPIO_Port, MAT_OE_Pin, GPIO_PIN_RESET)  // Active low
#define mat_dis() HAL_GPIO_WritePin(MAT_OE_GPIO_Port, MAT_OE_Pin, GPIO_PIN_SET)

#define A_low() HAL_GPIO_WritePin(MAT_A_GPIO_Port, MAT_A_Pin, GPIO_PIN_RESET)
#define A_high() HAL_GPIO_WritePin(MAT_A_GPIO_Port, MAT_A_Pin, GPIO_PIN_SET)

#define B_low() HAL_GPIO_WritePin(MAT_B_GPIO_Port, MAT_B_Pin, GPIO_PIN_RESET)
#define B_high() HAL_GPIO_WritePin(MAT_B_GPIO_Port, MAT_B_Pin, GPIO_PIN_SET)

#define C_low() HAL_GPIO_WritePin(MAT_C_GPIO_Port, MAT_C_Pin, GPIO_PIN_RESET)
#define C_high() HAL_GPIO_WritePin(MAT_C_GPIO_Port, MAT_C_Pin, GPIO_PIN_SET)

#define D_low() HAL_GPIO_WritePin(MAT_D_GPIO_Port, MAT_D_Pin, GPIO_PIN_RESET)
#define D_high() HAL_GPIO_WritePin(MAT_D_GPIO_Port, MAT_D_Pin, GPIO_PIN_SET)

#define pulse_clk() \
    do {            \
        clk_en();   \
        clk_dis();  \
    } while (0)

// Data pins
#define r1_high() HAL_GPIO_WritePin(MAT_R1_GPIO_Port, MAT_R1_Pin, GPIO_PIN_SET)
#define r1_low() HAL_GPIO_WritePin(MAT_R1_GPIO_Port, MAT_R1_Pin, GPIO_PIN_RESET)

#define g1_high() HAL_GPIO_WritePin(MAT_G1_GPIO_Port, MAT_G1_Pin, GPIO_PIN_SET)
#define g1_low() HAL_GPIO_WritePin(MAT_G1_GPIO_Port, MAT_G1_Pin, GPIO_PIN_RESET)

#define b1_high() HAL_GPIO_WritePin(MAT_B1_GPIO_Port, MAT_B1_Pin, GPIO_PIN_SET)
#define b1_low() HAL_GPIO_WritePin(MAT_B1_GPIO_Port, MAT_B1_Pin, GPIO_PIN_RESET)

#define r2_high() HAL_GPIO_WritePin(MAT_R2_GPIO_Port, MAT_R2_Pin, GPIO_PIN_SET)
#define r2_low() HAL_GPIO_WritePin(MAT_R2_GPIO_Port, MAT_R2_Pin, GPIO_PIN_RESET)

#define g2_high() HAL_GPIO_WritePin(MAT_G2_GPIO_Port, MAT_G2_Pin, GPIO_PIN_SET)
#define g2_low() HAL_GPIO_WritePin(MAT_G2_GPIO_Port, MAT_G2_Pin, GPIO_PIN_RESET)

#define b2_high() HAL_GPIO_WritePin(MAT_B2_GPIO_Port, MAT_B2_Pin, GPIO_PIN_SET)
#define b2_low() HAL_GPIO_WritePin(MAT_B2_GPIO_Port, MAT_B2_Pin, GPIO_PIN_RESET)

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
    memset((void *)frame, 0, sizeof(frame));
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance == PIXEL_TIMER.Instance) {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        draw_frame();
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
        // draw_frame();
        // memset((void *)frame, 0, sizeof(frame));
    }
}