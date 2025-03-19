#ifndef __VOLUMETRIC_H__
#define __VOLUMETRIC_H__

#include <stdint.h>

int volumetric_test(int argc, char **argv);
uint32_t read_gpio(volatile uint32_t *read_reg, int bit);
#endif  // __VOLUMETRIC_H__