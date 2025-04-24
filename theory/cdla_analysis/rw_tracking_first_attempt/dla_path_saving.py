##Implements a simple version of DLA on the 
##2D lattice with no optimization
##**some jit compiler optimizations because
##dla is very slow

##Output done using pygame

import random
import numpy as np 
import numba as nb ##for speed
import matplotlib as mpl ##for colors
import time
import sys
import pickle as pkl


n = 7## do not put above 10!!
grid_size = 2 ** n

##Number of particles to be added
n_particles = (n**2) * grid_size
n_particles = 250

grid = np.zeros((grid_size, grid_size), dtype=np.int32) 


##Initialise the seed

#For a random seed
# seed = (random.randrange(grid_size), random.randrange(grid_size))
seed = (grid_size // 2, grid_size // 2)
grid[seed] = 1


rw = np.zeros((n_particles, 5000_00, 2))

##Store the particles in order of arrival
arrivals = np.zeros((n_particles, 2), dtype=np.int32)


# @nb.njit(fastmath=True)
def compute_grid(grid, n_particles, arrivals, rw):
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

        j = 0 
        rw[i, j] = (x, y)
        
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

            j += 1

            rw[i, j] = (x, y)


        ##Fill in the particle
        grid[x, y] = 1
        arrivals[i,0] = x
        arrivals[i,1] = y

        # rw = rw[:, :j+1]


    

np.random.seed(42)
random.seed(42)
t0 = time.time()
compute_grid(grid, n_particles, arrivals, rw)
print("Time taken: ", time.time() - t0)

#Save the grid
pkl.dump(grid, open('./dla_grid.pkl', 'wb'))

pkl.dump(rw, open("rw.pkl", "wb"))