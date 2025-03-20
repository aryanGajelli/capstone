#ifndef __VOLUMETRIC_H__
#define __VOLUMETRIC_H__

#include <stdint.h>

int volumetric_test(int argc, char **argv);
uint32_t read_gpio(volatile uint32_t *read_reg, int bit);
pixel_t rgb_to_8bit(unsigned char r, unsigned char g, unsigned char b);
void populate_volume(FILE *file);
#endif  // __VOLUMETRIC_H__