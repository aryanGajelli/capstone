#ifndef __SLICEMAP_H__
#define __SLICEMAP_H__

#include "mathc.h"
#include "voxel.h"

#define SLICE_COUNT 100
#define SLICE_QUADRANT (SLICE_COUNT / 4)
#define SLICE_WRAP(slice) ((slice) % (SLICE_COUNT))
#define SLICE_2_RAD(slice) ((slice) * (2 * MPI / SLICE_COUNT))

#if SLICE_COUNT <= 256
typedef uint8_t slice_index_t;
#else
typedef uint16_t slice_index_t;
#endif

typedef struct {
    slice_index_t slice;
    uint8_t column;
} slice_polar_t;
typedef struct {
    voxel_index_t x, y;
} voxel_2D_t;

extern voxel_2D_t slice_map[SLICE_COUNT][PANEL_WIDTH][PANEL_COUNT];

void slicemap_init();
#endif  // __SLICEMAP_H__