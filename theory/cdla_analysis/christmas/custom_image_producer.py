# A version of the image produceer that is optimised for DLA
# See the documenetation of the functions in image_producer.py and DLA.py
import numpy as np
import matplotlib.pyplot as plt
import random
import numba as nb

#Embedding the grid generation code inside the image production greatly 
#increases performance
@nb.njit(fastmath=True)
def image_prod(grid_size, n_particles): 

    grid = np.zeros((grid_size, grid_size), dtype=np.int32) 

    seed = (grid_size // 2, grid_size // 2)
    grid[seed] = 1

    for i in range(n_particles):
        r = random.random()
        if r < 0.25: 
            x,y = 1, random.randint(1, grid_size - 2)
        elif r < 0.5:
            x,y = grid_size - 2, random.randint(1, grid_size - 2)
        elif r < 0.75:
            x,y = random.randint(1, grid_size - 2), grid_size - 2
        else:
            x,y = random.randint(1, grid_size - 2), 1
        
        while not (grid[(x + 1) % grid_size, y] or grid[x, (y + 1) % grid_size] or grid[(x - 1) % grid_size, y] or grid[x, (y - 1) % grid_size]):
        ##Randomly move 
            if random.random() < 0.5:
                if random.random() < 0.5:
                    x = x + 1
                else:
                    x = x - 1
            else:
                if random.random() < 0.5:
                    y = y + 1
                else:
                    y = y - 1
            
            ##Boundary checks
            ##We do not want to wrap!
            if x < 0: x = 0 
            if x == grid_size: x = grid_size - 1
            if y < 0: y = 0
            if y == grid_size: y = grid_size - 1


        ##Fill in the particle
        grid[x, y] = 1

    image = np.zeros((grid_size, grid_size, 3), dtype=np.uint8)
    image[:, :, 0] = 255 * grid
    image[:, :, 1] = 255 * grid
    image[:, :, 2] = 255 * grid

    return image

@nb.njit(fastmath=True, parallel=True)
def run_image_prod(n_images):
    grid_size = 256
    n_particles = 2500
    images = np.zeros((n_images, grid_size, grid_size, 3), dtype=np.uint8)

    for i in nb.prange(n_images):
        image = image_prod(grid_size, n_particles)
        images[i] = image 
    
    return images

n_images = 100_00
images = run_image_prod(n_images)

for i in range(n_images):
    plt.imsave(f"dla_images/image_{i}.png", images[i])
