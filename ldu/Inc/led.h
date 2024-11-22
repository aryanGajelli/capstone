#ifndef __LED_H__
#define __LED_H__

// Header file for Adafruit 1484 32x32 RGB LED Matrix
// https://learn.adafruit.com/32x16-32x32-rgb-led-matrix/overview
// https://learn.adafruit.com/32x16-32x32-rgb-led-matrix/how-the-matrix-works
// https://web.archive.org/web/20121201205905/http://www.hobbypcb.com/blog/item/3-16x32-rgb-led-matrix-technical-details.html

#include <stdint.h>

typedef struct {
    uint16_t x;
    uint16_t y;
} pos;

typedef struct {
    uint16_t r;
    uint16_t g;
    uint16_t b;
} pixel;

#define LED_WIDTH 32
#define LED_HEIGHT 32
extern pixel frame[LED_HEIGHT][LED_WIDTH];

void test_led(void);

#endif  // __LED_H__