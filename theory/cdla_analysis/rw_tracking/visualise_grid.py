import matplotlib.pyplot as plt
import numpy as np 
import pickle as pkl

dla_grid = pkl.load(open('./pickles/dla_grid_10k.pkl', 'rb')) 
final_cdla_grid = pkl.load(open("./pickles/cdla_grid_10 k.pkl", 'rb'))

fig, ax = plt.subplots(1, 2, figsize=(10, 5))
ax[0].imshow(dla_grid, cmap='gray')
ax[0].set_title('DLA')
ax[1].imshow(final_cdla_grid, cmap='gray')
ax[1].set_title('CDLA')
ax[0].set_xticks([])
ax[0].set_yticks([])
ax[1].set_xticks([])
ax[1].set_yticks([])
plt.show()