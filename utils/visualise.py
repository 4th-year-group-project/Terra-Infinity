import numpy as np
from PIL import Image
import vedo

# Step 1: Load the heightmap image
image_path = 'master_script/imgs/24_0_0.png'  # Replace with your image file path
image = Image.open(image_path)  # Convert to grayscale (L mode)
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
        # Define the corners of each grid cell
        p1 = i * cols + j
        p2 = p1 + 1
        p3 = p1 + cols
        p4 = p3 + 1
        # Create two triangles for each grid cell
        faces.append([p1, p2, p4])
        faces.append([p1, p4, p3])

# Step 4: Create the mesh and color it based on height

terrain_mesh = vedo.Mesh([vertices, faces])
terrain_mesh.cmap("summer", z.ravel())  # Apply a colormap based on the z (height) values

# make green colourmap


# Step 5: Render the 3D heightmap terrain
plotter = vedo.Plotter()
plotter.show(terrain_mesh, "3D Heightmap Terrain", axes=1, viewup="z", zoom=True)