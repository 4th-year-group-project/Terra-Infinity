import numpy as np
from .display import Display
from .simp_noise import Noise
from .tools import *
from numpy.fft import fft2, ifft2
# from pyamg import ruge_stuben_solver, smoothed_aggregation_solver
# from scipy.sparse import diags
import time

def quantize(x, num_levels):
    x_min = np.min(x)
    x_max = np.max(x)

    delta = (x_max - x_min) / num_levels
    quantized_values = np.round((x - x_min) / delta) * delta + x_min

    return quantized_values

def apply_smoothing1(h, gx, gy, dt):
    tmp = np.copy(h)
    tmp[:, 1:] = h[:, 1:] - (h[:, 1:] - h[:, :-1] + gx[:, 1:]) * dt
    h[:] = tmp
    tmp[:, :-1] = h[:, :-1] - (h[:, :-1] - h[:, 1:] - gx[:, :-1]) * dt
    h[:] = tmp
    tmp[1:, :] = h[1:, :] - (h[1:, :] - h[:-1, :] + gy[1:, :]) * dt
    h[:] = tmp
    tmp[:-1, :] = h[:-1, :] - (h[:-1, :] - h[1:, :] - gy[:-1, :]) * dt
    h[:] = tmp
    return h

def apply_smoothing2(h, gx, gy, dt):
    h[:, 1:] = h[:, 1:] - (h[:, 1:] - h[:, :-1] + gx[:, 1:]) * dt
    h[:, :-1] = h[:, :-1] - (h[:, :-1] - h[:, 1:] - gx[:, :-1]) * dt
    h[1:, :] = h[1:, :] - (h[1:, :] - h[:-1, :] + gy[1:, :]) * dt
    h[:-1, :] = h[:-1, :] - (h[:-1, :] - h[1:, :] - gy[:-1, :]) * dt
    return h

# def poisson_solver_multigrid(gx, gy, shape):
#     start = time.time()
#     div_g = np.gradient(gx, axis=1) + np.gradient(gy, axis=0)
#     N, M = shape
#     laplacian = diags([1, 1, -4, 1, 1], [-M, -1, 0, 1, M], shape=(N*M, N*M))
#     solver = smoothed_aggregation_solver(laplacian)
#     h_flat = solver.solve(div_g.ravel(), tol=1e-1)
#     return h_flat.reshape(shape)

def poisson_solver_fft(gx, gy, shape):
    div_g = np.zeros(shape)
    div_g[:, :-1] += gx[:, :-1]
    div_g[:, 1:] -= gx[:, :-1]
    div_g[:-1, :] += gy[:-1, :]
    div_g[1:, :] -= gy[:-1, :]

    N, M = shape
    pi = np.pi

    x = np.arange(M)
    y = np.arange(N)
    denom = (
        2 * (np.cos(pi * x / M) - 1)[None, :] + 
        2 * (np.cos(pi * y[:, None] / N) - 1)
    )
    denom[0, 0] = 1 

    div_g_hat = fft2(div_g)  
    z_hat = div_g_hat / denom  
    z = np.real(ifft2(z_hat))
    z = restore_fixed_point(z, heightmap, ref_idx=(0, 0))

    return z

def restore_fixed_point(h_computed, h_original, ref_idx=(0, 0)):
    i, j = ref_idx
    offset = h_original[i, j] - h_computed[i, j]
    return h_computed + offset

noise = Noise(seed=42, width=512, height=512)
heightmap = noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=256, octaves=2, persistence=0.5, lacunarity=2.0)

gx = np.zeros_like(heightmap, dtype=np.float32)
gy = np.zeros_like(heightmap, dtype=np.float32)

dx = heightmap[1:-1, 2:] - heightmap[1:-1, :-2]  # Difference in x-direction
dy = heightmap[2:, 1:-1] - heightmap[:-2, 1:-1]

num_levels = 5
gx[1:-1, 1:-1] = quantize(dx, num_levels)
gy[1:-1, 1:-1] = quantize(dy, num_levels)

## 1D case
noise = Noise(seed=42, width=1024, height=1024)
heightmap = noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=512, octaves=2, persistence=0.5, lacunarity=2.0)

heightmap_slice = heightmap[10, :]

slice_diff = np.diff(heightmap_slice)
slice_diff_quantized = quantize(slice_diff, 5)

reconstruction = np.zeros_like(heightmap_slice)
reconstruction[0] = heightmap_slice[0]
for i in range(1, len(slice_diff_quantized)):
    reconstruction[i] = reconstruction[i-1] + slice_diff_quantized[i]

import matplotlib.pyplot as plt
plt.figure(figsize=(6, 6))
plt.plot(reconstruction, 'r')
plt.axis('off')
plt.savefig("1d_slice2.png", bbox_inches='tight', pad_inches=0)

plt.plot(reconstruction), plt.grid()
plt.show()

### 2D case
# start = time.time()
# reconstructed = poisson_solver_multigrid(gx, gy, heightmap.shape)
# reconstructed = restore_fixed_point(reconstructed, heightmap, ref_idx=(0, 0))
# print(time.time() - start)
# # display = Display(normalize(reconstructed,0,1), height_scale=250, colormap="gray")
# # display.display_heightmap()

# # start = time.time()
# # reconstructed = heightmap.copy()

# # for i in range(200):
# #     reconstructed = apply_smoothing2(reconstructed, gx, gy, 0.6)
# # print(time.time() - start)

# display = Display(normalize(reconstructed), height_scale=250, colormap="gray")
# #display.display_heightmap()
# display.save_heightmap("flatten3.png")