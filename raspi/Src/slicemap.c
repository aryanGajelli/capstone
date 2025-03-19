#include "slicemap.h"

#include <math.h>

#include "mathc.h"
#include "voxel.h"

voxel_2D_t slice_map[SLICE_COUNT][PANEL_WIDTH][PANEL_COUNT];
/**
 * @brief Inintialize the slicemap
 *
 *          Slices the cartesian volume buffer and
 *          finds the closest cylindrical coordinates to the cartestian coordinates and records that index
 */
void slicemap_init() {
    double offsets[PANEL_COUNT] = {PANEL_BACK_OFFSET, PANEL_FRONT_OFFSET, PANEL_FRONT_OFFSET};

    for (int slice = 0; slice < SLICE_COUNT; slice++) {
        double angle = SLICE_2_RAD(slice);
        double sin_angle = sin(angle);
        double cos_angle = cos(angle);
        // do lookup
    }
}