import matplotlib.pyplot as plt
import numpy as np 
import pickle as pkl

dla_grid = pkl.load(open('./dla_grid.pkl', 'rb')) 


final_cdla_grid = np.zeros((128, 128), dtype=np.int32)  
n = 100
c = 0.25

for i in range(1, n+1):
    cdla_grid = pkl.load(open(f'./cdla_approx/cdla_grid{i}.pkl', 'rb')) 
    final_cdla_grid += cdla_grid

#Threshold the grid so anything non zero is 1
final_cdla_grid[final_cdla_grid < c*n] = 0
final_cdla_grid[final_cdla_grid >= c*n] = 1


final_cdla_grid = pkl.load(open("./cdla_grid.pkl", 'rb'))

# final_cdla_grid = pkl.load(open(f'./cdla_approx/cdla_grid{10}.pkl', 'rb')) 

fig, ax = plt.subplots(1, 2, figsize=(10, 5))
ax[0].imshow(dla_grid, cmap='gray')
ax[0].set_title('DLA')
ax[1].imshow(final_cdla_grid, cmap='gray')
ax[1].set_title('CDLA')
plt.show()