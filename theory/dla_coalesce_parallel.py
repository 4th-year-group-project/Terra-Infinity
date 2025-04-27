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
            if grid[(x + i) % grid_size, (y + j) % grid_size] != 0:
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
n_particles = 100_000

##Store the particles in order of arrival
arrivals = np.zeros((n_particles, 2), dtype=np.int32)


@nb.njit(fastmath=True, parallel=True)
def compute_grid(grid, n_particles, arrivals):
    #setup all of the particles at distinct random initial positions
    particle_gen = set()
    particle_tracker = np.zeros((n_particles, 2), dtype=np.int32)
    for i in range(n_particles): 
        x,y = (random.randrange(grid_size), random.randrange(grid_size))
        while (x,y) in particle_gen:
            x,y = (random.randrange(grid_size), random.randrange(grid_size))
        particle_gen.add((x,y)) 
        particle_tracker[i][0] = x
        particle_tracker[i][1] = y

    while True:
        flag = True
        #Go through each particle
        for j in nb.prange(n_particles):
            x, y = particle_tracker[j]

            #If the particle is done, skip it
            if x == -1:
                continue
            elif check_adjacent4(grid, x, y):
                
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
        
        #Check if all particles are done
        for k in range(n_particles):
            if particle_tracker[k][0] != -1:
                flag = False
                break
        if flag:
            break
                

t1 = time.time()
compute_grid(grid, n_particles, arrivals)
t2 = time.time()


##Set up the color map
##It will let us color the particles based on arrival time
x = np.linspace(0.0, 1.0, n_particles)
nice_colors = mpl.colormaps['plasma'](x)[np.newaxis, :, :3]


##Set up pygame
pg.init()
screen = pg.display.set_mode((grid_size, grid_size))
running = True




flag = True
##Run the game loop (very simple)
while running:
    for event in pg.event.get():
        if event.type == pg.QUIT:
            running = False
    
    if flag:

        ##Draw the particles with the colors
        for i in range(n_particles):
            x, y = arrivals[i] 
            color = 255 * nice_colors[0][i]
            screen.set_at((x, y), color[:])
            #update the screeen 

            if i % 100 == 0:
                # rect = pg.Rect(x, y, 1, 1)
                # pg.display.update(rect)
                pg.display.flip()


        # pg.display.flip()
        t3 = time.time()
        pg.image.save(screen, "fractal_new.png")
        flag = False
pg.quit()

print("Time to compute grid: ", t2 - t1)
print("Time to produce fractal: ", t3 - t2)



