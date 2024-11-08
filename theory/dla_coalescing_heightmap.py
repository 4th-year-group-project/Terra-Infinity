##Implements a simple version of DLA on the 
##2D lattice with no optimization
##**some jit compiler optimizations because
##dla is very slow

##Output done using pygame


import pygame as pg ##for visuals
import random
import numpy as np 
import numba as nb ##for speed
import matplotlib as mpl ##for colors
import time
import matplotlib.pyplot as plt


@nb.njit(fastmath=True)
def check_adjacent(grid, x, y):
    ##See if any of the 8 adjacent cells are occupied
    for i in range(-1, 2):
        for j in range(-1, 2):
            if grid[(x + i) % grid_size, (y + j) % grid_size] != 0:
                return True
    return False

@nb.njit(fastmath=True)
def check_adjacent1(grid, x, y):
    grid_size = grid.shape[0]
    ##See if any of the 8 adjacent cells are occupied
    for i in range(-1, 2):
        for j in range(-1, 2):
            if x+i < grid_size and x+i >= 0 and y+j < grid_size and y+j >= 0 and grid[(x + i), (y + j)] == 1:
                return True
    return False

@nb.njit(fastmath=True)
def check_adjacent4(grid, x, y):
    ##See if any of the 4 adjacent cells are occupied
    for i in [-1, 1]:
            if grid[(x + i) % grid_size, y] == 1:
                return True
    for j in [-1, 1]:
            if grid[x, (y + j) % grid_size] == 1:
                return True
    return False

@nb.njit(fastmath=True)
def check_adjacent2(grid, x, y):
    ##See if any of the 8 adjacent cells are occupied
    for i in range(-1, 2):
        for j in range(-1, 2):
            if grid[(x + i) % grid_size, (y + j) % grid_size] == 2:
                return True
    return False



n = 8 ## do not put above 10!!
desired_grid_size = 2 ** n

arrivals = np.zeros((int((0.3 * desired_grid_size) ** 2), 2), dtype=np.int32)


@nb.njit(fastmath=True, parallel=True)
def compute_grid(grid, n_particles, arrivals, particle_tracker):
    #setup 
    grid_size = grid.shape[0]
    particle_gen = set()

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

    plt.show()

scale_factor = 2
n = 4
grid_size = 2 ** n
t1 = time.time()
grid = np.zeros((grid_size, grid_size), dtype=np.int32)
seed = (grid_size // 2, grid_size // 2)
grid[seed] = 1
new_particles = 0
while grid_size <= desired_grid_size:
    
    n_particles = int((0.3 * grid_size) ** 2) 

    particle_tracker = np.zeros((n_particles, 2), dtype=np.int32)
    for i in range(new_particles):
        particle_tracker[i][0] = -1
    n_particles = n_particles - new_particles
    compute_grid(grid, n_particles, arrivals, particle_tracker)

    plot_grid(grid)
    

    if grid_size < desired_grid_size:
        grid_size *= scale_factor
        new_grid = np.zeros((grid_size, grid_size), dtype=np.int32)

        arrivals = increase_resolution(grid, new_grid,arrivals, scale_factor)

        n_particles = n_particles - new_particles
        grid = new_grid
        plot_grid(grid)
        
        new_particles = np.count_nonzero(grid)

    else:
        break
    n += 1



t2 = time.time()

print("Time to compute grid: ", t2 - t1)
