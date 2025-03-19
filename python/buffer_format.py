import numpy as np
from typing import TypeAlias

# Constants
VOXELS_X = 256
VOXELS_Y = 256
VOXELS_Z = 128

# the format of the pixel should be 
# first 3 bits for red, next 3 bits for green, last 2 bits for blue
pixel: TypeAlias = np.uint8

class volume_buffer: 
    def __init__(self, voxels_z=VOXELS_Z, voxels_x=VOXELS_X, voxels_y=VOXELS_Y):
        self.volume: np.ndarray = np.zeros((voxels_z, voxels_x, voxels_y), dtype=pixel)

# checks if a voxel on the x-y plane is inside the circle
def voxel_inside_cylinder(x, y): 
    x = x*2 - VOXELS_X-1
    y = y*2 - VOXELS_Y-1
    return x*x + y*y <= (VOXELS_X*VOXELS_Y/2)*(VOXELS_X*VOXELS_Y/2)

# read a .obj file and fill the volume buffer with its associated color
def read_obj_file(file_path: str, volume: volume_buffer):
    with open(file_path, 'r') as f:
        for line in f:
            if line.startswith('v '):
                x, y, z, r, g, b = map(float, line[2:].split())
                x = int((x+1)/2 * VOXELS_X)
                y = int((y+1)/2 * VOXELS_Y)
                z = int((z+1)/2 * VOXELS_Z)
                if voxel_inside_cylinder(x, y):
                    volume.volume[z, x, y] = np.uint8((r*7)<<5 | (g*7)<<2 | (b*3))