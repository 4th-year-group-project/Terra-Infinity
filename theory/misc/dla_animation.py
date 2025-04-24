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

@nb.njit(fastmath=True)
def check_adjacent(grid, x, y):
    ##See if any of the 8 adjacent cells are occupied
    for i in range(-1, 2):
        for j in range(-1, 2):
            if grid[(x + i) % grid_size, (y + j) % grid_size] == 1:
                return True
    return False



n = 9 ## do not put above 10!!
grid_size = 2 ** n

##Number of particles to be added
n_particles = (n**2) * grid_size
n_particles = 100_000

grid = np.zeros((grid_size, grid_size), dtype=np.int32) 


##Initialise the seed

#For a random seed
# seed = (random.randrange(grid_size), random.randrange(grid_size))
seed = (grid_size // 2, grid_size // 2)
grid[seed] = 1




##Store the particles in order of arrival
arrivals = np.zeros((n_particles, 2), dtype=np.int32)


@nb.njit(fastmath=True)
def compute_grid(grid, n_particles, arrivals):
    for i in range(n_particles):

        if i % 100 == 0:
            print(i)

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
        
        while not check_adjacent(grid, x, y):
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


t0 = time.time()
compute_grid(grid, n_particles, arrivals)
print("Time taken: ", time.time() - t0)


##Set up the color map
##It will let us color the particles based on arrival time
x = np.linspace(0.0, 1.0, n_particles)
nice_colors = mpl.colormaps['plasma'](x)[np.newaxis, :, :3]


##Set up pygame
pg.init()
screen = pg.display.set_mode((grid_size, grid_size))
running = True

##Set the screen to white
screen.fill((255, 255, 255))
# screen.fill((164, 194, 223))

##Draw the particles with the colors
for i in range(n_particles):
    x, y = arrivals[i] 
    color = 255 * nice_colors[0][i]
    # color = [255, 255, 255]
    screen.set_at((x, y), color[:])

    pg.display.flip()
    if i % 10000 == 0:
        pg.image.save(screen, f"fractal_images_dla_large/fractal{i}.png")

# ##Run the game loop (very simple)
# while running:
#     for event in pg.event.get():
#         if event.type == pg.QUIT:
#             running = False
# pg.quit()


