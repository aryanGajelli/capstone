import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from mpl_toolkits.mplot3d import Axes3D

# Parameters
frames_per_rotation = 60  # One frame per degree
fps = 60
volume_size = 10  # Cube dimensions (10x10x10)
rectangle_size = (6, 4)  # Width and height of the rectangle
output_file = "z_axis_rotating_rectangle_facing_x.mp4"

# Generate rectangle positions
def get_rectangle(frame, size, rect_size):
    angle = (frame % frames_per_rotation) * (2 * np.pi / frames_per_rotation)
    cx, cy, cz = size / 2, size / 2, size / 2  # Center of the volume
    w, h = rect_size
    # Rotation matrix for Z-axis rotation
    rotation_matrix = np.array([
        [np.cos(angle), -np.sin(angle), 0],
        [np.sin(angle), np.cos(angle), 0],
        [0, 0, 1]
    ])
    # Define rectangle corners initially aligned with the X-axis
    corners = np.array([
        [0, -h / 2, -w / 2],  # Bottom-left
        [0, -h / 2, w / 2],   # Bottom-right
        [0, h / 2, w / 2],    # Top-right
        [0, h / 2, -w / 2]    # Top-left
    ])
    # Rotate and translate the rectangle
    rotated_corners = np.dot(corners, rotation_matrix.T)
    rotated_corners[:, 0] += cx
    rotated_corners[:, 1] += cy
    rotated_corners[:, 2] += cz
    return rotated_corners

# Initialize figure
fig = plt.figure()
ax = fig.add_subplot(111, projection="3d")
ax.set_xlim([0, volume_size])
ax.set_ylim([0, volume_size])
ax.set_zlim([0, volume_size])

# Update function for animation
def update(frame):
    ax.cla()
    ax.set_xlim([0, volume_size])
    ax.set_ylim([0, volume_size])
    ax.set_zlim([0, volume_size])
    corners = get_rectangle(frame, volume_size, rectangle_size)

    # Plot rectangle edges
    for i in range(len(corners)):
        start = corners[i]
        end = corners[(i + 1) % len(corners)]
        ax.plot([start[0], end[0]], [start[1], end[1]], [start[2], end[2]], color="blue")

    # Highlight bottom-right corner every 60 frames
    if frame % (frames_per_rotation/2) == 0:
        bottom_right = corners[1 if (frame%frames_per_rotation==0) else 2]  # Index 1 is the bottom-right corner
        ax.scatter(bottom_right[0], bottom_right[1], bottom_right[2], color="red", s=100)

    ax.set_title(f"Frame {frame}")

# Create animation
ani = animation.FuncAnimation(fig, update, frames=frames_per_rotation*5, interval=100/fps)

# Save animation as video
ani.save(output_file, fps=fps, writer="ffmpeg")
print(f"Video saved as {output_file}")
