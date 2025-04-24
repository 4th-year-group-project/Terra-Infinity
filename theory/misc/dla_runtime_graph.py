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
import matplotlib.pyplot as plt
import time

@nb.njit(fastmath=True)
def check_adjacent(grid, grid_size, x, y):
    ##See if any of the 8 adjacent cells are occupied
    for i in range(-1, 2):
        for j in range(-1, 2):
            if grid[(x + i) % grid_size, (y + j) % grid_size] == 1:
                return True
    return False

@nb.njit(fastmath=True)
def compute_grid(grid, grid_size, n_particles, arrivals):
    for i in range(n_particles):

        # if i % 100 == 0:
        #     print(i)

        ##Pick a starting point
        # x, y = (random.randrange(grid_size), random.randrange(grid_size))
        # while grid[x, y] == 1:
        #     x, y = (random.randrange(grid_size), random.randrange(grid_size))
        ##While there is not a particle in the adjacent cells
        # a = int(random.random() * 4)
        # x,y = [(1,1), (1, grid_size - 2), (grid_size - 2, 1), (grid_size - 2, grid_size -2)][a]
        if random.random() < 0.25: 
            x,y = 1, random.randint(1, grid_size - 2)
        elif random.random() < 0.5:
            x,y = grid_size - 2, random.randint(1, grid_size - 2)
        elif random.random() < 0.75:
            x,y = random.randint(1, grid_size - 2), grid_size - 2
        else:
            x,y = random.randint(1, grid_size - 2), 1
        
        while not check_adjacent(grid, grid_size, x, y):
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
        arrivals[i,0] = x
        arrivals[i,1] = y




def runtime_info():
    times = []
    for n in range(3, 500, 10):
        grid_size = n 
        print(grid_size)

        ##Number of particles to be added
        n_particles = int(((n/4)**2) * grid_size)

        grid = np.zeros((grid_size, grid_size), dtype=np.int32) 


        ##Initialise the seed

        #For a random seed
        # seed = (random.randrange(grid_size), random.randrange(grid_size))
        seed = (grid_size // 2, grid_size // 2)
        grid[seed] = 1
        ##Store the particles in order of arrival
        arrivals = np.zeros((n_particles, 2), dtype=np.int32)

        t0 = time.time()
        compute_grid(grid, grid_size, n_particles, arrivals)
        t1 = time.time()
        times.append(t1 - t0) 
    return times

def produce_graph():
    sizes = [i * i for i in range(3, 500, 10)]
    times = runtime_info()
    plt.plot(sizes, times)
    plt.xlabel("Number of Grid Points")
    plt.ylabel("Time to Compute (s)")
    plt.show()

produce_graph()


# ##Set up the color map
# ##It will let us color the particles based on arrival time
# x = np.linspace(0.0, 1.0, n_particles)
# nice_colors = mpl.colormaps['plasma'](x)[np.newaxis, :, :3]


# ##Set up pygame
# pg.init()
# screen = pg.display.set_mode((grid_size, grid_size))
# running = True

# ##Draw the particles with the colors
# for i in range(n_particles):
#     x, y = arrivals[i] 
#     color = 255 * nice_colors[0][i]
#     screen.set_at((x, y), color[:])

# pg.display.flip()

# pg.image.save(screen, "fractal.png")

# ##Run the game loop (very simple)
# while running:
#     for event in pg.event.get():
#         if event.type == pg.QUIT:
#             running = False
# pg.quit()


