#Produces the specified number of images of dla and cdla

import dla 
import dla_coalesce_parallel as cdla 
import numpy as np
import matplotlib.pyplot as plt

#Create an image of the specified grid size with n_particles using dla_func
def image_prod(grid_size, n_particles, dla_func): 

    grid = np.zeros((grid_size, grid_size), dtype=np.int32) 

    seed = (grid_size // 2, grid_size // 2)
    grid[seed] = 1

    arrivals = np.zeros((n_particles, 2), dtype=np.int32) 

    dla_func.compute_grid(grid, n_particles, arrivals)

    image = np.zeros((grid_size, grid_size, 3), dtype=np.uint8)
    image[:, :, 0] = 255 * grid
    image[:, :, 1] = 255 * grid
    image[:, :, 2] = 255 * grid

    return image

#Create the specified number of images using dla_func
def run_image_prod(n_images, dla_func):
    grid_size = 256
    n_particles = 2500

    for i in range(n_images):
        image = image_prod(grid_size, n_particles, dla_func)
        plt.imsave(f"dla_images/image_{i}.png", image)

# run_image_prod(10, dla)