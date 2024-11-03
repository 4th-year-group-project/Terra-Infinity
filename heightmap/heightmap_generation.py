import numpy as np
from PIL import Image
import cv2
import matplotlib.pyplot as plt
from matplotlib import cm

# Function to generate a heightmap from an image
def generate_heightmap(image_name):
    image = Image.open(image_name).convert('L')
    im = np.asarray(image.convert('RGB'))
    im = 255 - cv2.cvtColor(im, cv2.COLOR_RGB2GRAY)
    heightmap = np.zeros((image.size[1], image.size[0]))
    
    # Apply Gaussian blur to the image with increasing smoothing radius
    initial_radius = 16
    while initial_radius <= 256:
        blurred = cv2.GaussianBlur(im, (1023,1023), initial_radius)
        heightmap = heightmap + blurred
        initial_radius = initial_radius * 2

    normalised_heightmap = cv2.normalize(heightmap, None, 0, 255, cv2.NORM_MINMAX)

    heightmap_image = Image.fromarray(normalised_heightmap)
    heightmap_image.show()
    heightmap_image = heightmap_image.convert('RGB')
    heightmap_image.save('heightmap/images/heightmap.png')
    return normalised_heightmap

def plot_heightmap(heightmap, z_scale=0.9):
    heightmap = np.array(heightmap)
    
    # Create coordinate matrices
    y, x = np.mgrid[:heightmap.shape[0], :heightmap.shape[1]]
    
    # Create the 3D plot
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')
    
    # Plot the surface with a color gradient
    surf = ax.plot_surface(x, y, heightmap, 
                          cmap=cm.terrain,
                          linewidth=0,
                          antialiased=True,
                          rcount=100,
                          ccount=100)
    
    # Set the aspect ratio of the plot
    # This controls the relative height without changing the values
    max_range = max(heightmap.shape[0], heightmap.shape[1], heightmap.max())
    ax.set_box_aspect((
        heightmap.shape[1] / max_range,  # x-axis
        heightmap.shape[0] / max_range,  # y-axis
        heightmap.max() / max_range * z_scale  # z-axis (scaled down)
    ))
    
    # Add a color bar
    fig.colorbar(surf, ax=ax, shrink=0.5, aspect=5)
    
    # Set labels
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Height')
    
    # Set title
    plt.title('3D Heightmap Visualization')
    
    # Show the plot
    plt.show()
