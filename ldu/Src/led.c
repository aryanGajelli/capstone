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
    delayUS(1100);  // wait for 1ms to allow auto frame clear by the timer
}

uint8_t matrixRow = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    if (htim->Instance == PIXEL_TIMER.Instance) {
        /*
        mat_dis();
        latch_en();
        selectRow(matrixRow);
        for (int x = 0; x < LED_WIDTH; x++) {
            pixel p1 = frame[matrixRow][x];
            pixel p2 = frame[matrixRow + LED_HEIGHT / 2][x];
            if (p1.r)
                r1_high();
            else
                r1_low();

            if (p1.g)
                g1_high();
            else
                g1_low();

            if (p1.b)
                b1_high();
            else
                b1_low();

            if (p2.r)
                r2_high();
            else
                r2_low();

            if (p2.g)
                g2_high();
            else
                g2_low();

            if (p2.b)
                b2_high();
            else
                b2_low();

            pulse_clk();
        }
        latch_dis();
        mat_en();
        matrixRow = (matrixRow + 1) % (LED_HEIGHT / 2);
        */
    }
}
void test_led() {
    HAL_Delay(1000);
    clearMatrix();

    // draw all 8 colors in 12 pixel blocks
    // pixel p = {1, 0, 0};

    // for (int y = 0; y < LED_HEIGHT; y++) {
    //     if (y % 4 == 0) {
    //         p = (pixel){(y/4)&0x01, (y/4)&0x02, (y/4)&0x04};
    //     }

    //     for (int x = 0; x < LED_WIDTH; x++) {
    //         frame[y][x] = p;
    //     }
    // }
    int color = 0;

    // draw all 4bit colors every 2 pixels
    for (int y = 0; y < LED_HEIGHT; y++) {
        for (int x = 0; x < LED_WIDTH; x++) {
            frame[y][x] = (pixel){x,0, y};
            // color = (color + 1) % 512; // Cycle through all 512 colors (9-bit)
        }
    }
    // print frame
    // for (int y = 0; y < LED_HEIGHT; y++) {
    //     for (int x = 0; x < LED_WIDTH; x++) {
    //         printf("%0x%0x%0x ", frame[y][x].r, frame[y][x].g, frame[y][x].b);
    //     }
    //     printf("\n");
    // }

    const int bits = 5;
    // uint8_t bitplane = 0;
    pixel p1;
    const int period = 10;
    while (1) {
        selectRow(matrixRow);
        for (int bitplane = 0; bitplane < (1 << bits); bitplane++){
            latch_dis();
            mat_dis();
            
            for (int x = 0; x < LED_WIDTH; x++) {
                p1 = frame[matrixRow][x];
                pixel p2 = frame[matrixRow + LED_HEIGHT / 2][x];
                if (p1.r == bitplane && p1.r)
                    r1_high();
                else
                    r1_low();
                
                if (p1.g == bitplane && p1.g)
                    g1_high();
                else
                    g1_low();
                
                if (p1.b == bitplane && p1.b)
                    b1_high();
                else
                    b1_low();
                
                if (p2.r == bitplane && p2.r)
                    r2_high();
                else
                    r2_low();
                
                if (p2.g == bitplane && p2.g)
                    g2_high();
                else
                    g2_low();
                
                if (p2.b == bitplane && p2.b)
                    b2_high();
                else
                    b2_low();
    /*
                if (p1.g)
                    g1_high();
                else
                    g1_low();

                if (p1.b)
                    b1_high();
                else
                    b1_low();

                if (p2.r)
                    r2_high();
                else
                    r2_low();

                if (p2.g)
                    g2_high();
                else
                    g2_low();

                if (p2.b)
                    b2_high();
                else
                    b2_low();
    */
                pulse_clk();
            }
            latch_en();
            mat_en();
            delayUS(period * bitplane / (1 << bits));
        }
        
        
        // printf("%u %d\n",p1.r, bitplane);
       
        matrixRow = (matrixRow + 1) % (LED_HEIGHT / 2);        
    }
}