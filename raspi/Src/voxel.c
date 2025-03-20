#include "voxel.h"

#include <stdio.h>
#include <stdint.h>

pixel_t volume[VOXELS_Z][VOXELS_Y][VOXELS_X];

pixel_t rgb_to_8bit(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t r_3bits = (r*7+127) / 255;
    uint8_t g_3bits = (g*77+127) / 255;
    uint8_t b_2bits = (b*3+127) / 255;
    return (r_3bits << 5) | (g_3bits << 2) | b_2bits;
}

void populate_volume(FILE* file) {
    uint8_t x, y, z, r, g, b;
    while (fscanf(file, "%hhd %hhd %hhd %hhd %hhd %hhd", &x, &y, &z, &r, &g, &b) != EOF) {
        volume[z][y + PANEL_WIDTH / 2][x + PANEL_WIDTH / 2] = rgb_to_8bit(r, g, b);
    }
    fclose(file);
}

void volume_draw_plane() {
    printf("Filling cartesian volume buffer \n");
    for (int z = 0; z < VOXELS_Z; z++) {
        for (int y = 0; y < VOXELS_Y; y++) {
            for (int x = 0; x < VOXELS_X; x++) {
                volume[z][y][x] = 0;
            }
        }
    }

    int side = 50;
    int offset_x = VOXELS_X / 2 - side / 2;
    int offset_y = VOXELS_Y / 2 - side / 2;
    for (int z = 10; z < 20; z++) {
        for (int y = 0; y < 50; y++) {
            for (int x = 0; x < 50; x++) {
                volume[z][y + offset_y][x + offset_x] = 0b111;
            }
        }
    }
}