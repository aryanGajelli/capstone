#include "led.h"

#include <stdio.h>

#include "delay_us.h"
#include "main.h"
#include "stm32f4xx_hal.h"

pixel frame[LED_HEIGHT][LED_WIDTH];

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
        clk_dis();  \
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

void ledInit(void) {
    // Initialize the LED matrix
    // This function should be called before any other functions
    // that interact with the LED matrix
    clk_dis();
    latch_dis();
    mat_dis();
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

void clearMatrix(void) {
    // Clear the LED matrix
    mat_dis();
    clk_dis();
    r1_low();
    g1_low();
    b1_low();
    r2_low();
    g2_low();
    b2_low();
    for (int i = 0; i < LED_WIDTH; i++) {
        pulse_clk();
    }
    clk_dis();
    mat_en();
}
/**
 * @brief Set the color of a pixel
 *
 * @param p The color to set the pixel to
 * @param loc The location of the pixel
 */
void write_pixel(pixel p, pos loc) {
    return;
}

void test_led() {
    HAL_Delay(250);
    ledInit();
    clearMatrix();

    mat_en();
    int row = 0;
    while (1) {
        // mat_dis();
        latch_en();
        selectRow(row);
        row = (row + 1) % 16;
        for (int i = 0; i < LED_WIDTH; i++) {
            r1_high();
            // r2_high();
            pulse_clk();
            r1_low();
            // r2_low();

            // b1_high();
            // b2_high();
            // pulse_clk();
            // b1_low();
            // b2_low();

            // g1_high();
            g2_high();
            pulse_clk();
            // g1_low();
            g2_low();
        }
        latch_dis();
        // mat_en();
        // HAL_Delay(1);
        delayUS(1);
    }

    //     mat_dis();
    //     latch_en();
    //     selectRow(10);
    //     for (int i = 0; i < LED_WIDTH; i += 3) {
    // r2_high();
    // pulse_clk();
    // r2_low();
    // b2_high();
    // pulse_clk();
    // b2_low();
    // g2_high();
    // pulse_clk();
    // g2_low();
    //     }
    //     latch_dis();
    //     mat_en();

    //     HAL_Delay(1);
    // }
    // clearMatrix();
    // latch_dis();
}