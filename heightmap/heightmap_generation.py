import numpy as np
from PIL import Image
import cv2



def generate_heightmap(image):
    im = np.asarray(image.convert('RGB'))  # Convert PIL image to RGB format
    im = 255 - cv2.cvtColor(im, cv2.COLOR_RGB2GRAY)
    heightmap = np.zeros((image.size[1], image.size[0]))
    
    initial_radius = 16
    while initial_radius <= 256:
        blurred = cv2.GaussianBlur(im, (1023,1023), initial_radius)
        heightmap = heightmap + blurred

        initial_radius = initial_radius * 2

    normalised_heightmap = cv2.normalize(heightmap, None, 0, 255, cv2.NORM_MINMAX)

    return normalised_heightmap

