import numpy as np
import random
import pickle as pkl


N = 2 ** 9 #NxN grid
M = 100 #Number of Particles
L = 100_00 #Length of trajectory 

rws = np.zeros((M, L, 2)) 

for rw in rws: 
    rw[0] = (random.randint(0, N), 0) 
    for i in range(1, L): 
        x, y = 0, 0
        if random.random() < 0.5:
            x = 1
        else:
            y = 1
        rw[i] = (x, y)

with open('rw_trajectory.pkl', 'wb') as f:
    pkl.dump(rws, f)


