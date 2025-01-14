
import cv2
import random
import numpy as np 
import numba as nb ##for speed
import time
import matplotlib.pyplot as plt

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

@nb.njit(fastmath=True)
def get_random_xy(grid_size, seed):
    np.random.seed(seed)
    x = np.random.randint(0, grid_size)
    y = np.random.randint(0, grid_size)
    return (x, y)


@nb.njit(fastmath=True)
def get_three_random(seed):

    #np.random.seed(seed)
    return np.random.random(), np.random.random(), np.random.random()

@nb.njit(fastmath=True, parallel=True)
def compute_grid(grid, n_particles, mask, seed):
    #setup
    seed = 42
    grid_size = grid.shape[0]
    particle_gen = set()
    particle_tracker = np.zeros((n_particles, 2), dtype=np.int32)
    #new_seed = seed + 5
    for i in range(n_particles): 
        #a = new_seed + 3
        new_seed = seed
        x,y = get_random_xy(grid_size, new_seed)
        k = i
        while (x,y) in particle_gen or grid[x, y] == 1 or mask[x, y] == 0:
            k = k + 1
            new_seed = seed + k
            x,y = get_random_xy(grid_size, new_seed)
        particle_gen.add((x,y)) 
        particle_tracker[i][0] = x
        particle_tracker[i][1] = y
    seeds = [seed + i for i in range(n_particles)] 
    count = 1
    #print(seed)
    
    while True:
        #print(seed)
        flag = True
        
        
        
        for j in nb.prange(n_particles):
            
            
            #seed = nb.get_thread_id() 
            
            print(seed)
            #seed = thread + count + seed
            #print(seed)
            x, y = particle_tracker[j]
            #print(x, y)
            #print(j)
            # if j % 100 == 0:
            #     print(j)
            #     print(seed)
            if x == -1:
                continue
            elif check_adjacent1(grid, x, y):
                # print(check_adjacent1(grid, x, y), x, y)
                
                ##Fill in the particle
                grid[x, y] = 1

                particle_tracker[j][0] = -1
   
            else:
            ##Randomly move 
                before_x = x
                before_y = y
                
                #print(seed)
                np.random.seed(seed + count)
                if np.random.random() < 0.5:
                    if np.random.random() < 0.5:
                        x = x + 1
                        if x == grid_size: x = grid_size - 1
                    else:
                        x = x - 1
                        if x < 0: x = 0 
                else:
                    if np.random.random() < 0.5:
                        y = y + 1
                        if y == grid_size: y = grid_size - 1
                    else:
                        y = y - 1
                        if y < 0: y = 0
                
                if mask[x, y] == 0:
                    x = before_x
                    y = before_y


                particle_tracker[j][0] = x
                particle_tracker[j][1] = y
            #print(seed)
        count += 1

        for k in range(n_particles):
            if particle_tracker[k][0] != -1:
                flag = False
                break
        if flag:
            break

           
    return grid
         
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

def iterative_dfs(grid, new_grid, x, y, visited, scale_factor, adjacency_offsets, index):
    stack = [(x, y)]
    while stack:
        x, y = stack.pop()
        for dx, dy in adjacency_offsets:
            new_x, new_y = x + dx, y + dy
            if new_x < grid.shape[0] and new_y < grid.shape[0] and new_x >= 0 and new_y >=0 and grid[(new_x) % grid.shape[0], (new_y) % grid.shape[0]] != 0 and (new_x, new_y) not in visited:
                scaled_x, scaled_y = scale_coordinates(x, y, scale_factor, grid.shape[0], new_grid.shape[0])
                scaled_new_x, scaled_new_y = scale_coordinates(new_x, new_y, scale_factor, grid.shape[0], new_grid.shape[0])
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
                
                stack.append((new_x, new_y))
                visited.add((new_x, new_y))
    return index, visited

def dfs(grid, new_grid, x, y, visited, scale_factor, adjacency_offsets, arrivals, index):

    visited.add((x, y))
    
    
    old_grid_size = grid.shape[0]
    new_grid_size = new_grid.shape[0]
    for dx, dy in adjacency_offsets:
        new_x, new_y = x + dx, y + dy
        if new_x < old_grid_size and new_y < old_grid_size and new_x >= 0 and new_y >=0 and grid[(new_x) % new_grid_size, (new_y) % new_grid_size] != 0 and (new_x, new_y) not in visited:

            
            scaled_x, scaled_y = scale_coordinates(x, y, scale_factor, old_grid_size, new_grid_size)
            scaled_new_x, scaled_new_y = scale_coordinates(new_x, new_y, scale_factor, old_grid_size, new_grid_size)
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

def increase_resolution(grid, new_grid, scale_factor):

    old_grid_size = grid.shape[0]
    index = 0
    new_grid_size = new_grid.shape[0]

    for x in range(old_grid_size):
        for y in range(old_grid_size):
            if grid[x, y] == 1:
                scaled_x, scaled_y = scale_coordinates(x, y, scale_factor, old_grid_size, new_grid_size)

                new_grid[scaled_x, scaled_y] = 1

                index += 1

    adjacency_offsets = [(0, 1), (0, -1), (1, 0), (-1, 0), (1, 1), (-1, -1), (1, -1), (-1, 1)]
    one_args = np.argwhere(grid == 1)
    visited = set()
    for a, b in one_args:
        if (a, b) not in visited:

            index, visited = iterative_dfs(grid, new_grid, a, b, visited, scale_factor, adjacency_offsets, index)
    return new_grid
    
def plot_grid(grid):

    plt.imshow(grid, cmap='gray')
    plt.colorbar()
    plt.show()


def main(desired_grid, seed):
    
    desired_grid_size = desired_grid.shape[0]

    scale_factor = 2

    grid_size = 16

    mask = cv2.resize(desired_grid, (grid_size, grid_size), interpolation=cv2.INTER_NEAREST)
    available_cells = int(np.sqrt(np.sum(mask == 1)))
    grid = np.zeros((grid_size, grid_size), dtype=np.int32)

    indices = np.argwhere(mask == 1)
    (x,y) = np.mean(indices, axis=0).astype(np.int32)
    seed = (x,y)
    grid[seed] = 1

    blurred = np.zeros((grid_size, grid_size), dtype=np.float32)
    iteration = 0
    t1 = time.time()
    n_particles = int((0.3 * available_cells) ** 2) 

    while grid_size <= desired_grid_size:

        np.random.seed(seed)
        grid = compute_grid(grid, n_particles, mask, seed)
        plot_grid(grid)
        weighted_grid = grid.copy() * (1 / (2 ** iteration))

        blurred += weighted_grid

        if grid_size < desired_grid_size:
            scaled_grid_low_res = linear_interpolation(blurred, scale_factor)
            blurred = blur(scaled_grid_low_res)
            grid_size *= scale_factor
            new_grid = np.zeros((grid_size, grid_size), dtype=np.int32)

            new_grid = increase_resolution(grid, new_grid, scale_factor)
            
            grid = new_grid
            mask = cv2.resize(desired_grid, (grid_size, grid_size), interpolation=cv2.INTER_NEAREST)
            available_cells = available_cells * scale_factor
            n_particles = int((0.3 * available_cells) ** 2) - n_particles

            

        else:
            # Blur and add detail for the final time
            blurred = blur(blurred)
            blurred += (weighted_grid * 0.4)
            blurred = cv2.GaussianBlur(blurred, (3, 3), 1)

            break

        iteration += 1

    
    t2 = time.time()
    print(n_particles)
    plt.figure(figsize=(512/100, 512/100), dpi=100)
    plt.imshow(blurred, cmap='gray', vmin=0,vmax=1)
    plt.axis('off')
    plt.gca().set_position([0, 0, 1, 1])
    plt.savefig('heightmap.png', bbox_inches='tight', pad_inches=0)

    plt.close()


    print("Time to compute grid: ", t2 - t1)

if __name__ == "__main__":
    mask = np.ones((512, 512))
    num = 42

    main(mask, 42)