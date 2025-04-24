import numpy as np
import pickle as pkl


N = 2 ** 9 #NxN grid
M = 100 #Number of Particles
L = 100_000 #Length of trajectory

rws = pkl.load(open('./rw_trajectory.pkl', 'rb'))  

grid = np.zeros((N, N), dtype=np.int32) 

for i in range(M):
    start = rws[i][0]
    for j in range(1, L):
        move = rws[i][j]
        start += move 
        x, y = start
        x, y = int(x), int(y)   

        if x < 0: break 
        if y < 0: break
        if x >= N: break
        if y == N-1: grid[y][x] = 1; break 

        if grid[y+1][x] == 1 or (y != 0 and grid[y-1][x] == 1) or (x != N-1 and grid[y][x+1] == 1) or (x!= 0 and grid[y][x-1] == 1):
            grid[y][x] = 1
            break
            

pkl.dump(grid, open('./dla_grid_10k.pkl', 'wb'))
          
       



