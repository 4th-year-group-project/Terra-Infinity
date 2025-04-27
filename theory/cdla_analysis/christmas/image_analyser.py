#Analyse the images gained from the DLA and CDLA simulations


import numpy as np
import matplotlib.pyplot as plt
import pickle as pkl

n = 10_000 
grid_size = 256 
n_particles = 2500 

# Read in n images and create a grid of the frequency of each pixel
def get_freq_grid(file_name):
    freq_grid = np.zeros((grid_size, grid_size), dtype=np.int32)
    for i in range(n):
        grid = np.array(plt.imread(f"{file_name}/image_{str(i)}.png"))[:, :, 1]
        # grid = [x.strip() for x in f]    
        for y in range(grid_size):
            for x in range(grid_size):
                if int(grid[y][x]) == 1:
                    freq_grid[y][x] += 1
    return freq_grid

#To create the pickles
# freq_grid = get_freq_grid("dla_images")
# freq_grid_c = get_freq_grid("cdla_images10000")
# pkls = [freq_grid, freq_grid_c]
# for i in range(2):
#     with open(f"freq_grid{i}.pkl", "wb") as f:
#         pkl.dump(pkls[i], f)

#Load in pre-existing freq grids
with open("freq_grid0.pkl", "rb") as f:
    freq_grid = pkl.load(f)
    f.close()
with open("freq_grid1.pkl", "rb") as f:
    freq_grid_c = pkl.load(f)
    f.close()

print("Obtained Grids")


#Compute the percentage of pixes which deviate by more than 10%
c = 0
d = 0
for y in range(grid_size):
    for x in range(grid_size):
        if freq_grid[y][x] != 0 or freq_grid_c[y][x] != 0:
            d += 1
        if abs(freq_grid[y][x] - freq_grid_c[y][x]) > 0.1 * max(freq_grid[y][x], freq_grid_c[y][x]):
            # print(freq_grid[y][x], freq_grid_c[y][x])
            c += 1
           
print(100 * c / d)


#Produce a plot of both grids
fig, ax = plt.subplots(1, 2, figsize=(10, 5))
ax[0].imshow(freq_grid, cmap='gist_earth', interpolation='nearest')
ax[0].set_title('DLA')
ax[1].imshow(freq_grid_c, cmap='gist_earth', interpolation='nearest')
ax[1].set_title('CDLA')
#Turn axes ticks off
ax[0].set_xticks([])
ax[0].set_yticks([])
ax[1].set_xticks([])
ax[1].set_yticks([])
plt.show()
