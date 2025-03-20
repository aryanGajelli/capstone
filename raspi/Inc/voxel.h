#ifndef __VOXEL_H__
#define __VOXEL_H__

#include <errno.h>
#include <stdint.h>

#define PANEL_HEIGHT 64
#define PANEL_WIDTH 128
#define PANEL_COUNT 3  // Panel 0 is on the bottom offset, Panel 1 is on the bottom centered, Panel 2 is on the top centered

// Horizontal offset of the panels from the true center plane of the rotating structure,
// measured in pixels. Note 1 pixel is 2mm wide
#define PANEL_2_PANEL_DIST_MM 25.37  // measured
#define PIXEL_WIDTH_MM 2.f
#define PANEL_FRONT_OFFSET_MM -5  // likely 0 but should be experimentally determined
#define PANEL_BACK_OFFSET_MM (PANEL_2_PANEL_DIST_MM + PANEL_FRONT_OFFSET_MM)

#define PANEL_FRONT_OFFSET (PANEL_FRONT_OFFSET_MM / PIXEL_WIDTH_MM)
#define PANEL_BACK_OFFSET (PANEL_BACK_OFFSET_MM / PIXEL_WIDTH_MM)

// 128 * 128 * 128 cartesian space
#define VOXELS_X PANEL_WIDTH
#define VOXELS_Y PANEL_WIDTH
#define VOXELS_Z (PANEL_HEIGHT * 2)

#if (VOXELS_X <= 256) && (VOXELS_Y <= 256) && (VOXELS_Z <= 256)
typedef uint8_t voxel_index_t;
#else
typedef uint16_t voxel_index_t;
#endif

typedef uint8_t pixel_t;  // RRRGGGBB is what we want but rn its 00000RGB

extern pixel_t volume[VOXELS_Z][VOXELS_Y][VOXELS_X];

void volume_draw_plane();

static inline int panel_2_voxel_z(int panel_z) {
    if (panel_z < PANEL_HEIGHT) {
        return PANEL_HEIGHT - panel_z - 1;
    } else if (panel_z < PANEL_HEIGHT * 2) {
        return PANEL_HEIGHT - (panel_z - PANEL_HEIGHT) - 1;
    } else if (panel_z < PANEL_HEIGHT * 3) {
        return PANEL_HEIGHT * 2 - (panel_z - PANEL_HEIGHT * 2) - 1;
    } else {
        perror("Invalid panel_z");
        return -1;
    }
}

#endif  // __VOXEL_H__