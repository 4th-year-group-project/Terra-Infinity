import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import RegularGridInterpolator
from generation import Display

def generate_value_noise(grid_size, fine_size, octaves=4, persistence=0.5, lacunarity=2.0, method='linear'):
    noise = np.zeros((fine_size, fine_size))
    amplitude = 1.0 
    for octave in range(octaves):
        coarse_noise = np.random.rand(grid_size, grid_size)  
        x_coarse = np.linspace(0, 1, grid_size)
        y_coarse = np.linspace(0, 1, grid_size)
        interpolator = RegularGridInterpolator((x_coarse, y_coarse), coarse_noise, method=method)

        x_fine = np.linspace(0, 1, fine_size)
        y_fine = np.linspace(0, 1, fine_size)
        X_fine, Y_fine = np.meshgrid(x_fine, y_fine, indexing='ij') 

        fine_noise = interpolator((X_fine, Y_fine)) 

        noise += fine_noise * amplitude
        amplitude *= persistence
        grid_size = int(grid_size * lacunarity)

    return noise

grid_size = 8
fine_size = 512  

np.random.seed(42)  
coarse_noise = np.random.rand(grid_size, grid_size)  
noise = generate_value_noise(grid_size, fine_size, method='cubic',
                             octaves=8, persistence=0.5, lacunarity=1.75)
noise = (noise - np.min(noise)) / (np.max(noise) - np.min(noise))

plt.figure(figsize=(6, 6))
plt.imshow(noise, cmap="gray", origin="lower")
plt.axis("off")
#plt.savefig("value_noise_32.png", bbox_inches="tight", pad_inches=0, dpi=300)

display = Display(noise, height_scale=250, colormap='gray')
display.display_heightmap()
display.save_heightmap("value_noise_lacunarity_175.png", "both")