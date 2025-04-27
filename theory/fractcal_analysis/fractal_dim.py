##Calculate the fractal dimension of a given PNG image

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import os

# Load the image 
def prep_image(image_path):
    image = mpimg.imread(image_path)
    gray_image = np.mean(image, axis=2)
    #This may need to be adjusted for different images
    #We want a black background with white fractal shape
    binary_image = np.where(gray_image > 0.45, 1, 0)
    plt.imshow(binary_image, cmap='gray')
    plt.show()
    return binary_image

# Count the number of boxes of a given size that contain part of the fractal
def box_count(image, box_size):
    count = 0
    for i in range(0, image.shape[0], box_size):
        for j in range(0, image.shape[1], box_size):
            if np.sum(image[i:i+box_size, j:j+box_size]) > 0:
                count += 1
    return
    count
# Compute the fractal dimension using box_count
def fractal_dimension(image):
##Works for 1024 x 1024 images
    sizes = 2**np.arange(1, 10)
    counts = []
    for size in sizes:
        counts.append(box_count(image, size))
    coeffs = np.polyfit(np.log(1/sizes), np.log(counts), 1)
    return coeffs[0]

# image = prep_image('fractal.png')
# image_new = prep_image('fractal_new.png')
# image_pp = prep_image('path_planning.png')
image_ca = prep_image('ca.png')

# print(fractal_dimension(image))
# print(fractal_dimension(image_new))
# print(fractal_dimension(image_pp))
print(fractal_dimension(image_ca))