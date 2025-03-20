import numpy as np
import matplotlib.pyplot as plt

PANEL_WIDTH = 128
PANEL_HEIGHT = 64
NUM_SLICES = 360
VOXELS_X, VOXELS_Y, VOXELS_Z = PANEL_WIDTH, PANEL_WIDTH, PANEL_HEIGHT*2

volume = np.zeros((VOXELS_X, VOXELS_Y, VOXELS_Z), dtype=np.uint8)
print(volume.shape)

# prepare some coordinates
x, y, z = np.indices((VOXELS_X//10, VOXELS_Y//10, VOXELS_Z//10))

# draw cuboids in the top left and bottom right corners, and a link between
# them
cube1 = (x < 3) & (y < 3) & (z < 3)
cube2 = (x >= 5) & (y >= 5) & (z >= 5)
link = abs(x - y) + abs(y - z) + abs(z - x) <= 2

# combine the objects into a single boolean array
voxelarray = cube1 | cube2 | link

# set the colors of each object
colors = np.empty(voxelarray.shape, dtype=object)
colors[link] = 'red'
colors[cube1] = 'blue'
colors[cube2] = 'green'

# and plot everything
ax = plt.figure().add_subplot(projection='3d')
ax.voxels(voxelarray, facecolors=colors, edgecolor='k')

plt.show()
