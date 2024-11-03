import numpy as np
from PIL import Image
import cv2
import matplotlib.pyplot as plt

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

def plot_heightmap(heightmap):
    heightmap_data = np.array(heightmap)

    x = np.arange(0, heightmap_data.shape[1], 1)
    y = np.arange(0, heightmap_data.shape[0], 1)
    x, y = np.meshgrid(x, y)

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.plot_surface(x, y, heightmap_data, cmap='viridis')

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Height')
    ax.set_title('Heightmap in 3D')
    plt.show()

