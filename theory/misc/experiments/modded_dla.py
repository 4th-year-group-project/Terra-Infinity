##Implements a modified version of DLA which attempts
##to avoid simulating the random walk of the particles
##Output done using pygame

import pygame as pg 
import random
import numpy as np 
import numba as nb
import matplotlib as mpl

@nb.njit(fastmath=True)
def check_adjacent(grid, x, y):
    ##See if any of the 8 adjacent cells are occupied
    for i in range(-1, 2):
        for j in range(-1, 2):
            if grid[(x + i) % grid_size, (y + j) % grid_size] == 1:
                return True
    return False

@nb.njit(fastmath=True)
def get_adjacent(grid, x, y):
    ##Return any adjacent cells that are not occupied
    ##Otherwise returns the empty list
    adj_cells = []

    for i in range(-1, 2):
        for j in range(-1, 2):
            if grid[(x + i) % grid_size, (y + j) % grid_size] == 0:
                adj_cells.append(((x + i)%grid_size, (y + j)%grid_size))
                
    return adj_cells




n = 8
grid_size = 2 ** n

grid = np.zeros((grid_size, grid_size), dtype=np.int32) 

#For a random seed
# seed = (random.randrange(grid_size), random.randrange(grid_size))

seed = (grid_size // 2, grid_size // 2)
grid[seed] = 1

# grid[grid_size//2, :] = 1
# grid[:, grid_size//2] = 1

n_particles = (n**2) * grid_size

arrivals = np.zeros((n_particles + 1, 2), dtype=np.int32)


ones = [(i, j) for i in range(grid_size) for j in range(grid_size) if grid[i, j] == 1]


# @nb.njit(fastmath=True)
def compute_grid(grid, n_particles, arrivals):
    ws = [1] * len(ones)
    for i in range(n_particles):
        if i % 100 == 0:
            print(i)
        planted = False
        
        while not planted: 
            x,y = random.choices(ones, weights= ws, k=1)[0]

            ##Find out if any of the adjacent cells are free 
            adj_cells = get_adjacent(grid, x, y)
            if len(adj_cells) > 0:
                x, y = random.choice(adj_cells)
                # x += random.choice([-1, 0, 1])
                # y += random.choice([-1, 0, 1])
                # x = x % grid_size
                # y = y % grid_size
                grid[x, y] = 1
                ones.append((x, y))
                arrivals[i,0] = x
                arrivals[i,1] = y
                planted = True
        x-=grid_size//2
        y-=grid_size//2
        ws += [x**2 + y**2]

    
        

compute_grid(grid, n_particles, arrivals)


x = np.linspace(0.0, 1.0, n_particles)
nice_colors = mpl.colormaps['plasma'](x)[np.newaxis, :, :3]


pg.init()
screen = pg.display.set_mode((grid_size, grid_size))
running = True
count = 0
for i in range(n_particles):
    x, y = arrivals[i] 
    color = 255 * nice_colors[0][i]
    screen.set_at((x, y), color[:])
    count += 255 / n_particles
pg.display.flip()
while running:
    for event in pg.event.get():
        if event.type == pg.QUIT:
            running = False



pg.quit()


