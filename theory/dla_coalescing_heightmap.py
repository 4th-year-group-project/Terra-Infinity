
import cv2
import random
import numpy as np 
import numba as nb ##for speed
import matplotlib as mpl ##for colors
import time
import matplotlib.pyplot as plt
import PIL.Image as Image

@nb.njit(fastmath=True)
def check_adjacent1(grid, x, y):
    grid_size = grid.shape[0]
    ##See if any of the 8 adjacent cells are occupied
    for i in range(-1, 2):
        for j in range(-1, 2):
            # don't allow wrapping
            if x+i < grid_size and x+i >= 0 and y+j < grid_size and y+j >= 0 and grid[(x + i), (y + j)] == 1:
                return True
    return False


n = 8 ## do not put above 10!!
desired_grid_size = 2 ** n

arrivals = np.zeros((int((0.3 * desired_grid_size) ** 2), 2), dtype=np.int32)


@nb.njit(fastmath=True, parallel=True)
def compute_grid(grid, n_particles, arrivals):
    #setup 
    grid_size = grid.shape[0]
    particle_gen = set()
    particle_tracker = np.zeros((n_particles, 2), dtype=np.int32)
    for i in range(n_particles): 
        x,y = (random.randrange(grid_size), random.randrange(grid_size))
        while (x,y) in particle_gen or grid[x, y] == 1:
            x,y = (random.randrange(grid_size), random.randrange(grid_size))
        particle_gen.add((x,y)) 
        particle_tracker[i][0] = x
        particle_tracker[i][1] = y

    while True:
        flag = True
        for j in nb.prange(n_particles):
            x, y = particle_tracker[j]


            if x == -1:
                continue
            elif check_adjacent1(grid, x, y):
                # print(check_adjacent1(grid, x, y), x, y)
                
                ##Fill in the particle
                grid[x, y] = 1
                e = 0 
                while arrivals[e][0] != 0:
                    e += 1
                arrivals[e,0] = x
                arrivals[e,1] = y

                particle_tracker[j][0] = -1
   
            else:
            ##Randomly move 
                if random.random() < 0.5:
                    if random.random() < 0.5:
                        x = x + 1
                        if x == grid_size: x = grid_size - 1
                    else:
                        x = x - 1
                        if x < 0: x = 0 
                else:
                    if random.random() < 0.5:
                        y = y + 1
                        if y == grid_size: y = grid_size - 1
                    else:
                        y = y - 1
                        if y < 0: y = 0


                particle_tracker[j][0] = x
                particle_tracker[j][1] = y
        

        for k in range(n_particles):
            if particle_tracker[k][0] != -1:
                flag = False
                break
        if flag:
            break
         
def scale_coordinates(x, y, scale_factor, old_grid_size, new_grid_size):
    if (x == old_grid_size - 1):
        scaled_x = new_grid_size - 1
    else:
        scaled_x = x * scale_factor
    if (y == old_grid_size - 1):
        scaled_y = new_grid_size - 1
    else:
        scaled_y = y * scale_factor
    return scaled_x, scaled_y


def dfs(grid, new_grid, x, y, visited, scale_factor, adjacency_offsets, arrivals, index):

    visited.add((x, y))
    
    
    old_grid_size = grid.shape[0]
    for dx, dy in adjacency_offsets:
        new_x, new_y = x + dx, y + dy
        if new_x < old_grid_size and new_y < old_grid_size and new_x >= 0 and new_y >=0 and grid[(new_x) % grid_size, (new_y) % grid_size] != 0 and (new_x, new_y) not in visited:

            
            scaled_x, scaled_y = scale_coordinates(x, y, scale_factor, old_grid_size, new_grid.shape[0])
            scaled_new_x, scaled_new_y = scale_coordinates(new_x, new_y, scale_factor, old_grid_size, new_grid.shape[0])
            abs_dx = abs(scaled_new_x - scaled_x)
            abs_dy = abs(scaled_new_y - scaled_y)
            sx = 1 if scaled_x < scaled_new_x else -1
            sy = 1 if scaled_y < scaled_new_y else -1
            err = abs_dx - abs_dy
            while scaled_x != scaled_new_x or scaled_y != scaled_new_y:
                e2 = 2 * err
                if e2 > -abs_dy:
                    err -= abs_dy
                    scaled_x += sx
                if e2 < abs_dx:
                    err += abs_dx
                    scaled_y += sy

                if (scaled_x, scaled_y) != (scaled_new_x, scaled_new_y) :
                    new_grid[scaled_x, scaled_y] = 1

                    arrivals[index][0] = scaled_x
                    arrivals[index][1] = scaled_y
                    index += 1

            arrivals, index = dfs(grid, new_grid, new_x, new_y, visited, scale_factor, adjacency_offsets, arrivals, index)
    return arrivals, index

def linear_interpolation(grid, scale_factor):
    orig_rows, orig_cols = grid.shape
    new_rows = int(orig_rows * scale_factor)
    new_cols = int(orig_cols * scale_factor)

    scaled_grid = np.zeros((new_rows, new_cols))

    for y in range(new_rows):
        for x in range(new_cols):

            orig_y = y / scale_factor
            orig_x = x / scale_factor

            y0 = int(np.floor(orig_y))
            y1 = min(y0 + 1, orig_rows - 1)
            x0 = int(np.floor(orig_x))
            x1 = min(x0 + 1, orig_cols - 1)
            
            # Get weights
            wy = orig_y - y0
            wx = orig_x - x0

            f00 = grid[y0, x0]  # top left
            f10 = grid[y0, x1]  # top right
            f01 = grid[y1, x0]  # bottom left
            f11 = grid[y1, x1]  # bottom right
            
            # Interpolate
            # First interpolate in x direction
            top = f00 * (1 - wx) + f10 * wx  # top row
            bottom = f01 * (1 - wx) + f11 * wx  # bottom row
            
            # Then interpolate in y direction
            scaled_grid[y, x] = top * (1 - wy) + bottom * wy

    return scaled_grid

def blur(grid):
    initial_radius = 4
    radius_step = 16

    kernel_size = initial_radius + radius_step
    if kernel_size % 2 == 0:
        kernel_size += 1
        
    blurred = cv2.GaussianBlur(grid, (kernel_size, kernel_size), initial_radius, radius_step)

    return blurred

def increase_resolution(grid, new_grid,arrivals, scale_factor):

    past_arrivals = arrivals.copy()

    arrivals.fill(0)

    old_grid_size = grid.shape[0]

    index = 0
    new_grid_size = new_grid.shape[0]

    for x in range(old_grid_size):
        for y in range(old_grid_size):
            if grid[x, y] == 1:
                scaled_x, scaled_y = scale_coordinates(x, y, scale_factor, old_grid_size, new_grid_size)

                new_grid[scaled_x, scaled_y] = 1
                arrivals[index][0] = scaled_x
                arrivals[index][1] = scaled_y
                index += 1

    adjacency_offsets = [(0, 1), (0, -1), (1, 0), (-1, 0), (1, 1), (-1, -1), (1, -1), (-1, 1)]

    visited = set()
    for a, b in past_arrivals:
        if (a, b) not in visited:
            
            arrivals, index = dfs(grid, new_grid, a, b, visited, scale_factor, adjacency_offsets, arrivals, index)

    return arrivals

    
def plot_grid(grid):

    plt.imshow(grid, cmap='gray')
    plt.colorbar()
    plt.show()

scale_factor = 2
n = 4
grid_size = 2 ** n

grid = np.zeros((grid_size, grid_size), dtype=np.int32)
seed = (grid_size // 2, grid_size // 2)
grid[seed] = 1
new_particles = 0

blurred = np.zeros((grid_size, grid_size), dtype=np.float32)
iteration = 0
t1 = time.time()
n_particles = int((0.3 * grid_size) ** 2) 
while grid_size <= desired_grid_size:


    compute_grid(grid, n_particles, arrivals)

    weighted_grid = grid.copy() * (1 / (2 ** iteration))

    blurred += weighted_grid

    if grid_size < desired_grid_size:
        scaled_grid_low_res = linear_interpolation(blurred, scale_factor)
        blurred = blur(scaled_grid_low_res)
        grid_size *= scale_factor
        new_grid = np.zeros((grid_size, grid_size), dtype=np.int32)

        n_particles = int((0.3 * grid_size) ** 2) - n_particles

        arrivals = increase_resolution(grid, new_grid,arrivals, scale_factor)
        
        grid = new_grid
        

    else:
        # Blur and add detail for the final time
        blurred = blur(blurred)
        blurred += (weighted_grid * 0.2)

        break

    iteration += 1
    
    n += 1

t2 = time.time()
print(n_particles)
plt.imshow(blurred, cmap='gray', vmin=0,vmax=1)
plt.axis('off')
plt.gca().set_position([0, 0, 1, 1])
plt.savefig('heightmap.png', bbox_inches='tight', pad_inches=0)
plt.close()


print("Time to compute grid: ", t2 - t1)
