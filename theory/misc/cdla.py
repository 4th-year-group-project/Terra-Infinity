##Implements a simple version of DLA on the 
##2D lattice with no optimization
##**some jit compiler optimizations because
##dla is very slow

##Output done using pygame
import random
import numpy as np 
import numba as nb ##for speed
import matplotlib.pyplot as plt 

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
# n_particles = grid_size * (n**2)
n_particles = 10_000
print(n_particles)


grid = np.zeros((grid_size, grid_size), dtype=np.int32) 


##Initialise the seed

#For a random seed
# seed = (random.randrange(grid_size), random.randrange(grid_size))
seed = (grid_size // 2, grid_size // 2)
grid[seed] = 1

##Store the particles in order of arrival
arrivals = np.zeros((n_particles, 2), dtype=np.int32)

@nb.njit(fastmath=True, parallel=True)
def compute_grid(grid, n_particles, arrivals):
    #setup 
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
        for j in nb.prange(n_particles):
            x, y = particle_tracker[j]


            if x == -1:
                continue
            elif check_adjacent(grid, x, y):
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


t0 = time.time()
compute_grid(grid, n_particles, arrivals)
t1 = time.time()
print("Time taken: ", t1 - t0)



##Plot the grid
plt.imshow(grid, cmap='gray')
plt.show()