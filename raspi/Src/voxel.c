#include "voxel.h"

pixel_t volume[VOXELS_Z][VOXELS_Y][VOXELS_X];

void volume_draw_plane() {
    for (int z = 0; z < VOXELS_Z; z++) {
        for (int y = 0; y < VOXELS_Y; y++) {
            for (int x = 0; x < VOXELS_X; x++) {
                volume[z][y][x] = 0;
            }
        }
    }

    int plane_z = 10;
    for (int y = 0; y < 50; y++) {
        for (int x = 0; x < 50; x++) {
            volume[plane_z][y][x] = 0b111;
        }
    }
}