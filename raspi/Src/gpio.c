#include "gpio.h"

void getRisingFallingPhoto(volatile uint32_t *read_reg, bool *rising_edge, bool *falling_edge) {
    static bool photo_prev1 = 0;
    static bool photo_prev2 = 0;
    static bool photo_prev3 = 0;

    bool photo = readPhoto(read_reg);
    *rising_edge = !photo_prev3 && !photo_prev2 && photo_prev1 && photo;
    *falling_edge = photo_prev3 && photo_prev2 && !photo_prev1 && !photo;

    photo_prev3 = photo_prev2;
    photo_prev2 = photo_prev1;
    photo_prev1 = photo;
}