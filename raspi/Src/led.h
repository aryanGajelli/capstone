#ifndef __LED_H__
#define __LED_H__

#include <stdint.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} pixel;

#define LED_WIDTH 128
#define LED_HEIGHT 64
extern uint16_t frame[LED_HEIGHT][LED_WIDTH];

void test_led(void);

#endif  // __LED_H__