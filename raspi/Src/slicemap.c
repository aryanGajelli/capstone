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

        int i_p = 0;  // Front panel
        double d = 0; // Offset
        for (int i_w = 0; i_w < PANEL_WIDTH; i_w++)
        {
            // Position of point in frame p
            double x_p = i_w - PANEL_WIDTH/2;

            // Position of panel in frame r
            double p_x = -d*sin_angle;
            double p_y = d*cos_angle;

            // Position of point in frame r
            double x_r = x_p*cos_angle + p_x;
            double y_r = x_p*sin_angle + p_y;

            // Convert rectangular coordinate to index
            int index_x = x_r + PANEL_WIDTH/2;
            int index_y = y_r + PANEL_WIDTH/2;

            slice_map[slice][i_w][i_p].x = index_x;
            slice_map[slice][i_w][i_p].y = index_y;
        }

        i_p = 1; // Back panel
        d = -PANEL_BACK_OFFSET;  // Offset
        for (int i_w = 0; i_w < PANEL_WIDTH; i_w++)
        {
            // Position of point in frame p
            double x_p = i_w - PANEL_WIDTH/2;

            // Position of panel in frame r
            double p_x = -d*sin_angle;
            double p_y = d*cos_angle;

            // Position of point in frame r
            double x_r = x_p*cos_angle + p_x;
            double y_r = x_p*sin_angle + p_y;

            // Convert rectangular coordinate to index
            int index_x = x_r + PANEL_WIDTH/2;
            int index_y = y_r + PANEL_WIDTH/2;

            slice_map[slice][i_w][i_p].x = index_x;
            slice_map[slice][i_w][i_p].y = index_y;
        }
    }
}