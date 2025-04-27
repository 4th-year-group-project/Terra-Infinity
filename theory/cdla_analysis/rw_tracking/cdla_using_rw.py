import numpy as np 
import pickle as pkl

N = 2 ** 7 #NxN grid
M = 100_00 #Number of Particles
L = 500 #Length of trajectory

#Simulate CDLA using a predifined set of generated random walks

rws = pkl.load(open('./pickles/rw_trajectory.pkl', 'rb'))  

grid = np.zeros((N, N), dtype=np.int32)  
active_walks = np.ones(M, dtype=np.int32) 
time_step = 1

#Whilst there are active walks
while np.sum(active_walks) > 0: 

    #For each particle
    for i in range(M):
        if active_walks[i] == 0: continue
        #Move it one position according to the random walk
        start = rws[i][time_step - 1]
        move = rws[i][time_step] 
        new_pos = start + move
        rws[i][time_step] = new_pos
        x, y = new_pos
        x, y = int(x), int(y)

        #If it is out of bounds kill it
        if x < 0 or y < 0 or x >= N: 
            active_walks[i] = 0
            continue  

        if y == N-1 or grid[y+1][x] == 1 or (y != 0 and grid[y-1][x] == 1) or (x != N-1 and grid[y][x+1] == 1) or (x!= 0 and grid[y][x-1] == 1):
            grid[y][x] = 1
            active_walks[i] = 0
    
    time_step += 1

pkl.dump(grid, open('./pickles/cdla_grid_10k.pkl', 'wb'))


    

