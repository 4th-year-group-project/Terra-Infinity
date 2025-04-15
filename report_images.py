import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import RegularGridInterpolator
from noise import pnoise2
from generation import Display, Noise, tools
from scipy.signal import find_peaks

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

# s = 128
# noise = generate_perlin_noise(512, 512, s, 8, 0.4, 2.2)
# noise = (noise - noise.min()) / (noise.max() - noise.min()) 

# plt.figure(figsize=(6, 6))
# plt.imshow(noise, cmap="gray", origin="lower")
# plt.axis("off")
# plt.savefig(f"perlin_s_{s}.png", bbox_inches="tight", pad_inches=0, dpi=300)

# display = Display(noise, height_scale=250, colormap='terrain')
# #display.display_heightmap()
# display.save_heightmap("perlin_noise_fractal2.png")

noise_gen = Noise(seed=0, width=1024, height=1024)
heightmap = noise_gen.fractal_simplex_noise(noise="open", scale=100, octaves=3, persistence=0.5, lacunarity=2.0)

slice_index = heightmap.shape[0] // 2
slice_data = tools.normalize(heightmap[slice_index, :], -1, 1)

def cus(cycles=12, points_per_cycle=100):
    # Set up the x-axis
    total_points = cycles * points_per_cycle
    x = np.linspace(0, cycles * 2 * np.pi, total_points)
    y = np.sin(x)

    # Scale every 3rd positive lobe (0 to π, 2π to 3π, etc.)
    for i in range(cycles):
        # Find indices for this cycle
        start_angle = i * 2 * np.pi
        end_angle = (i + 1) * 2 * np.pi
        start_idx = np.searchsorted(x, start_angle)
        end_idx = np.searchsorted(x, end_angle)

        # Middle of cycle is the positive half (first half)
        mid_idx = start_idx + (end_idx - start_idx) // 2

        # Boost only the positive half if it's the 3rd cycle
        if (i) % 3 == 0:
            y[start_idx:mid_idx] *= 2  # Boost whole lobe

    return x,y/2

x, y = cus(cycles=5, points_per_cycle=100)

billow = 2*np.abs(y)-1
ridged = 2*(1-np.abs(y))-1


plt.figure(figsize=(8, 6))
plt.axhline(y=0, color='black', linestyle='--', linewidth=1)
plt.plot(x, ridged, color='blue')
plt.gca().xaxis.set_visible(False)


plt.savefig("default.png", bbox_inches="tight", pad_inches=0, dpi=300)
plt.show()