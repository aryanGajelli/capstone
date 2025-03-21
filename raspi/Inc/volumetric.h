#ifndef __VOLUMETRIC_H__
#define __VOLUMETRIC_H__

#include <stdint.h>
#include <stdio.h>
#include "led-matrix-c.h"

int volumetric_test(struct LedCanvas *canvas, volatile uint32_t *read_reg);
#endif  // __VOLUMETRIC_H__