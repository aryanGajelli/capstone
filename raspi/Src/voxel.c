#include "voxel.h"
#include <stdio.h>

pixel_t volume[VOXELS_Z][VOXELS_Y][VOXELS_X];

void volume_draw_plane() {
    printf("Filling cartesian volume buffer \n");
    for (int z = 0; z < VOXELS_Z; z++) {
        for (int y = 0; y < VOXELS_Y; y++) {
            for (int x = 0; x < VOXELS_X; x++) {
                volume[z][y][x] = 0;
            }
        }
    }

    for (int z = 50; z < 100; z++) {
        for (int y = 0; y < 50; y++) {
            for (int x = 0; x < 50; x++) {
                volume[z][y + PANEL_WIDTH / 2][x + PANEL_WIDTH / 2] = 0b111;
            }
        }
    }

}