import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import RegularGridInterpolator
from generation import Display, Noise, normalize

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

# grid_size = 8
# fine_size = 512  

# np.random.seed(42)  
# coarse_noise = np.random.rand(grid_size, grid_size)  
# noise = generate_value_noise(grid_size, fine_size, method='linear',
#                              octaves=3, persistence=0.5, lacunarity=1.75)
# noise = (noise - np.min(noise)) / (np.max(noise) - np.min(noise))

#p=0.45, scale=256
#p=0.45, scale=350, lacunarity=1.5, octaves=8
# noise_gen = Noise(0, 512, 512, 0, 0)
# noise = noise_gen.fractal_simplex_noise(noise="open", scale=128, octaves=1, lacunarity=2, persistence=0.5)
# noise = normalize(noise, 0, 1)

# plt.figure(figsize=(6, 6))
# plt.imshow(noise, cmap="gray", origin="lower")
# plt.axis("off")
# #plt.show()
# plt.savefig("opensimplex_noise_128.png", bbox_inches="tight", pad_inches=0, dpi=300)

# display = Display(noise, height_scale=250, colormap='gray')
# display.display_heightmap()
# display.save_heightmap("opensimplex_noise_32.png")



F2 = (np.sqrt(3) - 1) / 2

def skew_point(x, y):
    s = (x + y) * F2
    return np.array([x + s, y + s])


print(skew_point(0, 0)) 
print(skew_point(1, 0))
print(skew_point(0, 1))
print(skew_point(1, 1))
print(skew_point(2, 0))
print(skew_point(2, 1))


grid_size = 5
points = [(x, y) for x in range(grid_size) for y in range(grid_size)]

# Assign each point a unique color
num_points = len(points)
colors = plt.cm.viridis(np.linspace(0, 1, num_points))  # use a nice colormap

# Apply the skew transformation
skewed_points = [skew_point(x, y) for x, y in points]

# Convert to numpy for plotting
points = np.array(points)
skewed_points = np.array(skewed_points)

# Plot
plt.figure(figsize=(10, 5))

# Original grid
plt.subplot(1, 2, 1)
for i, (x, y) in enumerate(points):
    plt.scatter(x, y, color=colors[i])
plt.title("Original Grid")
plt.gca().set_aspect('equal')

# Skewed grid
plt.subplot(1, 2, 2)
for i, (x, y) in enumerate(skewed_points):
    plt.scatter(x, y, color=colors[i])
plt.title("Skewed to Triangle Grid")
plt.gca().set_aspect('equal')

plt.tight_layout()
plt.show()