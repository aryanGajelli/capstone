#ifndef __VOLUMETRIC_H__
#define __VOLUMETRIC_H__

#include <stdint.h>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

int volumetric_test(int argc, char **argv);
void init_photo(volatile uint32_t *read_reg);
uint32_t read_gpio(volatile uint32_t *read_reg, int bit);
#endif  // __VOLUMETRIC_H__