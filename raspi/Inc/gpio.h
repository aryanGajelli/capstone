#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdint.h>
#include <stdbool.h>

#define PHOTO_PIN (44 - 32)

#define readPhoto(read_reg) ((*read_reg) & (1UL << PHOTO_PIN))

void getRisingFallingPhoto(volatile uint32_t *read_reg, bool *rising_edge, bool *falling_edge);
#endif // __GPIO_H__