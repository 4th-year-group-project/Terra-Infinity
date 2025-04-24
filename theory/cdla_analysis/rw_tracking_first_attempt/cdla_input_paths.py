import pickle as pkl 
import numpy as np
import numba as nb
import random


def fill_grid():
    rws = pkl.load(open('./rw.pkl', 'rb')) 

    time_step = 0
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
    inactive_particles = np.zeros(n_particles, dtype=np.int32)

    while True: 
        flag = True
        if time_step % 10_000 == 0:
            print(time_step)
        for i, rw in enumerate(rws): 
            if not inactive_particles[i]:
            
                x, y = rw[time_step]
                if x == 0 and y == 0 and rw[time_step+1][0] == 0 and rw[time_step+1][1] == 0:
                #     x, y = rw[time_step - 1]
                #     ##Randomly move 
                #     if random.random() < 0.5:
                #         if random.random() < 0.5:
                #             x = x + 1
                #         else:
                #             x = x - 1
                #     else:
                #         if random.random() < 0.5:
                #             y = y + 1
                #         else:
                #             y = y - 1
                    
                #     ##Boundary checks
                #     ##We do not want to wrap!
                #     if x < 0: x = 0 
                #     if x == grid_size: x = grid_size - 1
                #     if y < 0: y = 0
                #     if y == grid_size: y = grid_size - 1

        
                #     rw[time_step] = (x, y)
                    x, y = rw[time_step - 1]
                    x, y = int(x), int(y)
                    grid[x, y] = 1
                    inactive_particles[i] = 1
    


                flag = False
                # x, y = int(x), int(y)   
                # if (grid[(x + 1) % grid_size, y] or grid[x, (y + 1) % grid_size] or grid[(x - 1) % grid_size, y] or grid[x, (y - 1) % grid_size]):
                #     print("here")
                #     grid[x, y] = 1
                #     inactive_particles[i] = 1
        if flag:
            break

        time_step += 1
    return grid


# for i in range(1,101):
#     grid = fill_grid()
#     pkl.dump(grid, open(f'./cdla_approx/cdla_grid{i}.pkl', 'wb'))


grid = fill_grid()
pkl.dump(grid, open(f'./cdla_grid.pkl', 'wb'))