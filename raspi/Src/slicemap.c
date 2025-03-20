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
    printf("Initializing slicemap --- ");
    double offsets[PANEL_COUNT] = {PANEL_BACK_OFFSET, PANEL_FRONT_OFFSET, PANEL_FRONT_OFFSET};

    for (int slice = 0; slice < SLICE_COUNT; slice++) {
        double angle = SLICE_2_RAD(slice);
        double sin_angle = sin(angle);
        double cos_angle = cos(angle);
        // do lookup
        for (int panel = 0; panel < PANEL_COUNT; panel++) {
            for (int i_w = 0; i_w < PANEL_WIDTH; i_w++) {
                int acc_w = panel == 0 ? PANEL_WIDTH - i_w : i_w;

                // Position of point in frame p
                double x_p = acc_w - PANEL_WIDTH / 2;
                

                // Position of panel in frame r
                double p_x = -offsets[panel] * sin_angle;
                double p_y = offsets[panel] * cos_angle;

                // Position of point in frame r
                double x_r = x_p * cos_angle + p_x;
                double y_r = x_p * sin_angle + p_y;

                // Convert rectangular coordinate to index
                int index_x = clampf(x_r + PANEL_WIDTH / 2, 0, PANEL_WIDTH - 1);
                int index_y = clampf(y_r + PANEL_WIDTH / 2, 0, PANEL_WIDTH - 1);
                
                slice_map[slice][i_w][panel] = (voxel_2D_t){index_x, index_y};
            }
        }
    }
    printf(" done\n");
}