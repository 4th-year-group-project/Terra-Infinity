import numpy as np
import vedo
from PIL import Image

# Step 1: Load the heightmap image
image_path = "master_script/imgs/combined2.png"  # Replace with your image file path
image = Image.open(image_path)  # Open the image
height_array = np.array(image)  # Convert image to a 2D NumPy array
print(height_array.shape)

# Step 2: Normalize and Scale the Heights
height_scale = 200  # Adjust this value for more or less exaggerated terrain
height_array = (height_array / 65535) * height_scale  # Scale height between 0 and `height_scale`

# Step 3: Create a 3D surface mesh from the heightmap
rows, cols = height_array.shape
x, y = np.meshgrid(np.arange(cols), np.arange(rows))
z = height_array  # The height values

# Flatten the x, y, z arrays to create a list of vertices
vertices = np.c_[x.ravel(), y.ravel(), z.ravel()]

# Create the faces of the mesh by connecting each grid cell
faces = []
for i in range(rows - 1):
    for j in range(cols - 1):
        p1 = i * cols + j
        p2 = p1 + 1
        p3 = p1 + cols
        p4 = p3 + 1
        faces.append([p1, p2, p4])
        faces.append([p1, p4, p3])

# Step 4: Create the mesh and color it based on height
terrain_mesh = vedo.Mesh([vertices, faces])
terrain_mesh.cmap("terrain", z.ravel())  # Apply a colormap based on the z (height) values

# Step 4.5: Add a plane at 0.2 * 65536 height (scaled)
target_raw_height = 0.2 * 65536
target_height = (target_raw_height / 65535) * height_scale

plane = vedo.shapes.Plane(
    pos=(cols / 2, rows / 2, target_height), normal=(0, 0, 1), s=(cols, rows), c="green", alpha=0.4, res=(1, 1)
)

# Step 5: Render the 3D heightmap terrain
plotter = vedo.Plotter()
plotter.show(terrain_mesh, plane, "3D Heightmap Terrain with Plane", axes=1, viewup="z", zoom=True)
