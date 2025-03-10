import numpy as np
from .simp_noise import Noise
from .tools import *
from .display import Display

def quantize(x, num_levels):
    # Find the min and max values of the input signal
    x_min = np.min(x)
    x_max = np.max(x)
    
    # Calculate the step size (delta)
    delta = (x_max - x_min) / num_levels
    
    # Quantize each value
    quantized_values = np.round((x - x_min) / delta) * delta + x_min
    
    return quantized_values

### 1D case
# noise = Noise(seed=42, width=1024, height=1024)
# heightmap = noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=512, octaves=2, persistence=0.5, lacunarity=2.0)

# heightmap_slice = heightmap[10, :]

# slice_diff = np.diff(heightmap_slice)
# slice_diff_quantized = quantize(slice_diff, 5)

# reconstruction = np.zeros_like(heightmap_slice)
# reconstruction[0] = heightmap_slice[0]
# for i in range(1, len(slice_diff_quantized)):
#     reconstruction[i] = reconstruction[i-1] + slice_diff_quantized[i]

# import matplotlib.pyplot as plt
# plt.figure(figsize=(10, 5))
# plt.subplot(1,2,1), plt.plot(heightmap_slice), plt.title("Original"), plt.grid()
# plt.subplot(1,2,2), plt.plot(reconstruction), plt.title("Reconstructed, q=5"), plt.grid()
# plt.savefig("reconstructed_heightmap.png")
# plt.show()

### 2D case
# noise = Noise(seed=42, width=1024, height=1024)
# heightmap = noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=512, octaves=2, persistence=0.5, lacunarity=2.0)

# dy = np.diff(heightmap, axis=0)
# dx = np.diff(heightmap, axis=1)

# dy = np.pad(dy, ((1, 0), (0, 0)), mode='constant')
# dx = np.pad(dx, ((0, 0), (1, 0)), mode='constant')

# mag = np.sqrt(dy**2 + dx**2)
# ang = np.arctan2(dy, dx)

# mag_quantized = quantize(mag, 5)  
# ang_quantized = ang

# dy_quantized = quantize(dy, 50)
# dx_quantized = quantize(dx, 50)

# reconstruction_x = np.cumsum(dx_quantized, axis=1)
# reconstruction_y = np.cumsum(dy_quantized, axis=0)

# reconstruction = np.zeros_like(heightmap)
# reconstruction[:, 0] = heightmap[:, 0]
# reconstruction[0, :] = heightmap[0, :]
# for i in range(1, dy_quantized.shape[0]):
#     for j in range(1, dx_quantized.shape[1]):
#         reconstruction[i, j] += (1/2)*(reconstruction[i-1, j] + dy_quantized[i, j])
#         reconstruction[i, j] += (1/2)*(reconstruction[i, j-1] + dx_quantized[i, j])

# display = Display(heightmap, height_scale=250, colormap="gray")
# display.save_heightmap("original_heightmap.png")
# display = Display(0.5*(reconstruction_x+reconstruction_y), height_scale=250, colormap="gray")
# display.display_heightmap()
# display.save_heightmap("reconstructed_heightmap.png")