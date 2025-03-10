import numpy as np
import matplotlib.pyplot as plt
import cv2
from noise import snoise2
from .tools import *
from .simp_noise import Noise
from .erosion import *
from .display import Display
import pickle

width = 1024
height = 1024
noise = Noise(seed=42, width=1024, height=1024)

def volcanoe(density=7, radius=0.3, tau=20, c=0.01):
        worley = noise.worley_noise(density=density, k=1, p=2, distribution="poisson", radius=radius)
        sinusoidal_worley = ((worley.max() - worley)**tau)*np.sin(worley*c)
        return normalize(sinusoidal_worley, a=0, b=1)

def terrace(x, num_terraces=5, steepness=3):
        x = normalize(x, a=0, b=num_terraces)
        return normalize((np.round(x) + np.sign(x-np.round(x))*0.5*(np.abs(2*(x-np.round(x))))**steepness), 0, 1)

def dune(x, p, xm):
    s = np.where((x > 0) & (x < xm), 0.0, 1.0)
    ps1 = p * s + 1.0
    part1 = ps1 * 0.5
    part2 = 1 - np.cos((np.pi / ps1) * ((x - s) / (xm - s)))
    return (part1 * part2) - 1

def generate_dunes(self, frequency=10, noise_scale=5.0, noise_strength=200.0, rotation=-np.pi/4, amplitude=1.5, gap=100):
    # https://www.florisgroen.com/creating-sandy-desert-first-try/
    x = np.arange(width)
    y = np.arange(height)
    X, Y = np.meshgrid(x, y)
    noise_values = np.vectorize(lambda x, y: snoise2(x, y))(noise_scale * X / self.width, noise_scale * Y / self.height)
    shift = noise_strength * noise_values  

    xb = X * np.cos(rotation) - Y * np.sin(rotation)
    xa = (frequency * xb + shift) % (width + gap)
    xa = np.clip(xa, 0, width)
    normalized_xa = xa / width

    heightmap = amplitude * self.dune(normalized_xa, 1, 0.72)
    
    return heightmap


### Volcanoes:
# sinusoidal_worley = volcanoe(density=5, radius=0.4, tau=18, c=0.01)
# simplex = normalize(noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=256, octaves=10, persistence=0.5, lacunarity=2.0))
# heightmap = blend(sinusoidal_worley, simplex, alpha=0.3)

### Terraces:
# simplex = normalize(noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=512, octaves=10, persistence=0.42, lacunarity=2.0))
# heightmap = terrace(simplex, num_terraces=10, steepness=3)

### Dunes:
# dunes1 = normalize(generate_dunes(frequency=3, noise_scale=2.0, noise_strength=200.0, rotation=-np.pi/4, amplitude=1, gap=3))
# dunes2 = normalize(generate_dunes(frequency=6, noise_scale=2.0, noise_strength=500.0, rotation=-np.pi/4, amplitude=1, gap=0.5))
# dunes3 = normalize(generate_dunes(frequency=20, noise_scale=8.0, noise_strength=100.0, rotation=-np.pi/4, amplitude=1, gap=1))
# heightmap = normalize(1.3*dunes1 + 0.2*dunes2 + 0.05*dunes3, 0, 0.5)

# phasor_noise = normalize(noise.phasor_noise(num_phasors=20, freq_range=(30, 40), amplitude=1.0, direction_bias=np.pi/4, anisotropy=1))
# phasor_noise = normalize(sawtooth(phasor_noise), 0, 0.007)
# heightmap = heightmap + phasor_noise

### Mountains
# worley = (1-normalize(noise.worley_noise(density=50, k=1, p=2)))**2
# simplex = normalize(noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=512, octaves=10, persistence=0.5, lacunarity=2.0))
# heightmap = normalize(blend(simplex, worley, alpha=0.5), a=0, b=1)

# warpx = noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=128, octaves=2, persistence=0.5, lacunarity=2.0)
# warpy = noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=128, octaves=2, persistence=0.5, lacunarity=2.0)
# heightmap = domain_warp(heightmap, warpx, warpy, warp_strength=20)

### Sheer Mountains
# worley1 = normalize(noise.worley_noise(seed=1, density=30, k=1, p=1), -1, 1)
# worley2 = normalize(noise.worley_noise(seed=2, density=30, k=1, p=1), -1, 1)

# heightmap = normalize(
#       noise.uber_noise(width=1024, height=1024, scale=256, octaves=10, persistence=0.5, lacunarity=2.0,
#                        sharpness=0.5, feature_amp=1, altitude_erosion=0.15, slope_erosion=0.9, ridge_erosion=0,
#                        warp_x=worley1, warp_y=worley2, warp_strength=70)
# )

### Worley Warped DLA
# from cellular_automata.scaling_heightmap import ca_in_mask
# binary_polygon = np.ones((1024, 1024))
# binary_polygon[:, 0], binary_polygon[:, -1], binary_polygon[0, :], binary_polygon[-1, :] = 0,0,0,0
# ca_map = normalize(ca_in_mask(40, binary_polygon), 0, 1)

# # from skimage.filters.rank import entropy
# # from skimage.morphology import disk
# # from skimage.util import img_as_ubyte
# # entropy_map = normalize(entropy(img_as_ubyte(ca_map), disk(10))) 
# # simplex = normalize(noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=512, octaves=10, persistence=0.5, lacunarity=2.0))
# # ca_map = ca_map + simplex*entropy_map*0.4
# # ca_map = normalize(ca_map, 0, 1)**2

# worley1 = normalize(noise.worley_noise(seed=1, density=100, k=1, p=2), -1, 1)
# worley2 = normalize(noise.worley_noise(seed=2, density=100, k=1, p=2), -1, 1)

# warped = domain_warp(ca_map, worley1, worley2, warp_strength=40)

### Ocean Trenches and Ravines
# simplex = normalize(
#         noise.uber_noise(width=1024, height=1024, 
#                          scale=128, octaves=10, persistence=0.5, lacunarity=2.0,
#                          sharpness=0, feature_amp=1, 
#                          altitude_erosion=0, slope_erosion=0.9, ridge_erosion=0.2),
# -1, 1)

# heightmap = normalize(1-np.abs(simplex))**2
# heightmap = high_smooth(low_smooth(normalize((1-normalize(heightmap, 0, 1))), a=20, b=0.15), a=15, b=0.9)
# heightmap = normalize(heightmap, 0, 0.2)

### Terraced plains
# simplex = normalize(
#       noise.fractal_simplex_noise(
#             noise="simplex", x_offset=0, y_offset=0, 
#             scale=1024, octaves=2, persistence=0.4, lacunarity=1.5
#       )
# )
# heightmap = normalize(terrace(simplex, num_terraces=10, steepness=3))

# add = normalize(
#         noise.fractal_simplex_noise(
#                 noise="simplex", x_offset=0, y_offset=0, 
#                 scale=512, octaves=5, persistence=0.5, lacunarity=2.0
#         )
# )

# heightmap = normalize(
#       0.5*heightmap + 0.1*add,
# 0.2, 0.7)

### Basic Plains
# base1 = normalize(
#       noise.fractal_simplex_noise(
#             seed=100,
#             noise="simplex", x_offset=0, y_offset=0, 
#             scale=768, octaves=2, persistence=0.4, lacunarity=1.5
#       ),
# -1, 1)

# base2 = normalize(
#       noise.fractal_simplex_noise(
#             seed=200,
#             noise="simplex", x_offset=0, y_offset=0, 
#             scale=256, octaves=3, persistence=0.5, lacunarity=2.0
#       )
# )

# heightmap = normalize(normalize(low_smooth(normalize(abs(base1), 0, 1), a=10, b=0.3)) + 0.1*base2, 0.2, 0.5)

# warp_x = normalize(noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=512, octaves=2, persistence=0.5, lacunarity=2.0), -1, 1)
# warp_y = normalize(noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=512, octaves=2, persistence=0.5, lacunarity=2.0), -1, 1)

# heightmap = normalize(noise.warped_simplex_noise(x_offset=0, y_offset=0, scale=512, octaves=3, persistence=0.5, lacunarity=2.0,
#                                                  warp_x=warp_x, warp_y=warp_y, warp_strength=50))
      

# dist_worley, angular_worley = noise.angular_noise(seed=10, density=10, k=1, p=2)
# from noise import pnoise2
# perlin_noise = np.array([[pnoise2(x / 50.0, y / 50.0, octaves=4) for x in range(width)] for y in range(height)])
# angular_worley = angular_worley + 0.5 * perlin_noise  # Adjust 0.3 for more/less perturbation

# dist_worley = normalize(dist_worley, 0.2, 1)
# angular_worley = (normalize(np.sin(10*angular_worley),0,1))

# angular_worley = np.sin(angular_worley + 0.2 * perlin_noise)
#angular_worley = np.arcsin(angular_worley)
#angular_worley = low_smooth(angular_worley, a=100, b=0.1)

# import cv2
# import matplotlib.pyplot as plt
# from scipy.ndimage import distance_transform_edt

# mask = np.zeros((height, width), dtype=np.uint8)
# cv2.circle(mask, (512, 512), 120, 1, thickness=-1)
# edt_mask = normalize(distance_transform_edt(mask))**1.2

# simplex = normalize(
#         noise.fractal_simplex_noise(
#                 noise="simplex", x_offset=0, y_offset=0, 
#                 scale=1024, octaves=10, persistence=0.5, lacunarity=2
#         )
# )
# heightmap = normalize(high_smooth(normalize(edt_mask*simplex), a=15, b=0.1))

# mask = np.zeros((height, width), dtype=np.uint8)
# cv2.circle(mask, (572, 552), 100, 1, thickness=-1)
# edt_mask = normalize(distance_transform_edt(mask))**1.2

# heightmap2 = normalize(high_smooth(normalize(edt_mask*simplex), a=15, b=0.1), 0, 0.7)

# heightmap = np.maximum(heightmap, heightmap2)

# display = Display(height_array=heightmap, height_scale=250, colormap="terrain")
# display.display_heightmap()

def compute_normals(heightmap):
    # Compute gradients (dx, dy) using Sobel filter (or finite difference method)
    dx = cv2.Sobel(heightmap, cv2.CV_64F, 1, 0, ksize=3)  # Gradient in X direction
    dy = cv2.Sobel(heightmap, cv2.CV_64F, 0, 1, ksize=3)  # Gradient in Y direction
    
    # Calculate the normal map as (dx, dy, 1), where 1 is the Z component (perpendicular to the surface)
    norm_map = np.dstack((-dx, -dy, np.ones_like(heightmap)))  # Inverted direction for standard normal map
    norm_map = norm_map / np.linalg.norm(norm_map, axis=2, keepdims=True)  # Normalize the vectors
    return norm_map


def quantize_normals(norm_map, num_steps=5):
    # Quantize normals by limiting the range of gradients (stepping the surface steepness)
    # Quantize the angle of the gradient in each direction
    angle_map = np.arctan2(norm_map[:, :, 1], norm_map[:, :, 0])  # Get the angle from the X and Y components
    
    # Normalize the angle to be in [0, 2*pi] range
    angle_map = (angle_map + np.pi) % (2 * np.pi)
    
    # Quantize the angles into discrete steps
    quantized_angles = np.round(angle_map / (2 * np.pi) * num_steps) * (2 * np.pi / num_steps)
    
    # Recompute normals from the quantized angles
    quantized_norm_map = np.zeros_like(norm_map)
    quantized_norm_map[:, :, 0] = np.cos(quantized_angles)
    quantized_norm_map[:, :, 1] = np.sin(quantized_angles)
    quantized_norm_map[:, :, 2] = np.ones_like(quantized_norm_map[:, :, 2])
    
    # Normalize the quantized normal vectors
    quantized_norm_map = quantized_norm_map / np.linalg.norm(quantized_norm_map, axis=2, keepdims=True)
    
    return quantized_norm_map


def reconstruct_heightmap_from_normals(norm_map, initial_heightmap):
    # Integrate the normal map back into a heightmap
    # We will use a simple cumulative sum approach here (this is a basic form of "integration")
    
    heightmap = np.zeros_like(initial_heightmap)
    
    # Start with the initial heightmap
    heightmap[0, 0] = initial_heightmap[0, 0]
    
    # Reconstruct by using the normals and an approximation method
    for y in range(1, heightmap.shape[0]):
        for x in range(1, heightmap.shape[1]):
            # Use the normal map to estimate the height change
            heightmap[y, x] += 0.5*(heightmap[y-1, x] + norm_map[y, x, 0])
            heightmap[y, x] += 0.5*(heightmap[y, x-1] + norm_map[y, x, 1])
        
    
    return heightmap

heightmap = normalize(noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=512, octaves=2, persistence=0.5, lacunarity=2.0))
norm_map = compute_normals(heightmap)
quantized_norm_map = quantize_normals(norm_map, num_steps=5)
reconstructed_heightmap = reconstruct_heightmap_from_normals(quantized_norm_map, heightmap)

display = Display(height_array=normalize(reconstructed_heightmap), height_scale=250, colormap="gray")
display.display_heightmap()