import numpy as np
import matplotlib.pyplot as plt
from generation import Display, Noise, tools

noise_gen = Noise(seed=0, width=1024, height=1024)
heightmap = np.zeros((1024, 1024))

#radii = [200, 100, 50, 25, 10]
radii = [10, 50, 100, 500, 2000]
for i in range(len(radii)):
    heightmap += noise_gen.worley_noise(seed=noise_gen.seed,
                                        density=radii[i], k=1, p=2, 
                                        #distribution="poisson", radius=radii[i],
                                        )

# plt.imshow(heightmap, cmap='viridis', origin='lower')
# plt.axis('off')
# plt.show()

display = Display(tools.normalize(heightmap), height_scale=250, colormap='terrain')
#display.display_heightmap()
display.save_heightmap("worley_noise_terrain3.png")
#plt.savefig("worley_noise_f3divf2.png", bbox_inches='tight', pad_inches=0, dpi=300)