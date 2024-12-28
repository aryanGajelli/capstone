#ifndef __LED_H__
#define __LED_H__

// Header file for Adafruit 1484 32x32 RGB LED Matrix
// https://learn.adafruit.com/32x16-32x32-rgb-led-matrix/overview
// https://learn.adafruit.com/32x16-32x32-rgb-led-matrix/how-the-matrix-works
// https://web.archive.org/web/20121201205905/http://www.hobbypcb.com/blog/item/3-16x32-rgb-led-matrix-technical-details.html

#include <stdint.h>
#include "stm32f4xx_hal.h"

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} pixel;

#define LED_WIDTH 32
#define LED_HEIGHT 32
extern uint16_t frame[LED_HEIGHT][LED_WIDTH];

HAL_StatusTypeDef ledInit(void);
void test_led(void);
void draw_frame(void);

#endif  // __LED_H__