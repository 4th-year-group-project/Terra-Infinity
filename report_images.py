import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import RegularGridInterpolator
from noise import pnoise2
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

def generate_perlin_noise(width, height, scale, octaves, persistence, lacunarity):
    noise_grid = np.zeros((height, width))
    for y in range(height):
        for x in range(width):
            nx = x / scale
            ny = y / scale
            noise_val = pnoise2(nx, ny, octaves=octaves, persistence=persistence, lacunarity=lacunarity, repeatx=width, repeaty=height, base=0)
            noise_grid[y][x] = noise_val
    return noise_grid

s = 128
noise = generate_perlin_noise(512, 512, s, 8, 0.4, 2.2)
noise = (noise - noise.min()) / (noise.max() - noise.min()) 

# plt.figure(figsize=(6, 6))
# plt.imshow(noise, cmap="gray", origin="lower")
# plt.axis("off")
# plt.savefig(f"perlin_s_{s}.png", bbox_inches="tight", pad_inches=0, dpi=300)

display = Display(noise, height_scale=250, colormap='terrain')
#display.display_heightmap()
display.save_heightmap("perlin_noise_fractal2.png")