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
    ##See if any of the 8 adjacent cells are occupied
    for i in range(-1, 2):
        for j in range(-1, 2):
            if grid[(x + i) % grid_size, (y + j) % grid_size] == 1:
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



n = 10 ## do not put above 10!!
grid_size = 2 ** n

grid = np.zeros((grid_size, grid_size), dtype=np.int32) 


##Initialise the seed

#For a random seed
# seed = (random.randrange(grid_size), random.randrange(grid_size))
seed = (grid_size // 2, grid_size // 2)
grid[seed] = 1

##Number of particles to be added
n_particles = (n**2) * grid_size

##Store the particles in order of arrival
arrivals = np.zeros((n_particles, 2), dtype=np.int32)


@nb.njit(fastmath=True)
def compute_grid(grid, n_particles, arrivals):
    #setup 
    particle_tracker = set()
    for i in range(n_particles): 
        x,y = (random.randrange(grid_size), random.randrange(grid_size))
        while (x,y) in particle_tracker:
            x,y = (random.randrange(grid_size), random.randrange(grid_size))
        particle_tracker.add((x,y)) 
    i = 0
    particle_tracker = list(particle_tracker)
    while len(particle_tracker) > 0:
        new_particle_tracker = []

        for j in range(len(particle_tracker)):
            x, y = particle_tracker[j]

            if check_adjacent1(grid, x, y):
                ##Fill in the particle
                grid[x, y] = 1
                arrivals[i,0] = x
                arrivals[i,1] = y
   
                
                i += 1
                if i % 100 == 0:
                    print(i)
            else:
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

                new_particle_tracker.append((x,y))
        particle_tracker = new_particle_tracker

                


compute_grid(grid, n_particles, arrivals)
##Set up the color map
##It will let us color the particles based on arrival time
x = np.linspace(0.0, 1.0, n_particles)
nice_colors = mpl.colormaps['plasma'](x)[np.newaxis, :, :3]


##Set up pygame
pg.init()
screen = pg.display.set_mode((grid_size, grid_size))
running = True

##Draw the particles with the colors
for i in range(n_particles):
    x, y = arrivals[i] 
    color = 255 * nice_colors[0][i]
    screen.set_at((x, y), color[:])

pg.display.flip()

pg.image.save(screen, "fractal_new.png")

##Run the game loop (very simple)
while running:
    for event in pg.event.get():
        if event.type == pg.QUIT:
            running = False
pg.quit()


