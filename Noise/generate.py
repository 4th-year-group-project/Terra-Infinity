# from Noise.simplex import SimplexNoise 
# from Noise.colormap import ColorMap
# from coastline.fractal_coastline import FractalCoastline
# from coastline.primitives import shapes

# import numpy as np
# import cv2
# import matplotlib.pyplot as plt

# #UK detailed, 69, r=0.6, 

# def noise_in_mask(spread_mask, seed, scale, biome, x_offset=0, y_offset=0, octaves=8, start_frequency=1):
        
#     if biome == 10 or biome == 40 or biome == 70 or biome == 80 or biome == 30:
#         noise = SimplexNoise(seed=seed, width=spread_mask.shape[1], height=spread_mask.shape[0], scale=scale, octaves=octaves, persistence=0.5, lacunarity=2)
#         noise_map = noise.fractal_noise(noise="open", x_offset=x_offset, y_offset=y_offset, reason="heightmap")
#         #crop to just the shape of the mask
#         # noise_map = noise_map[:spread_mask.shape[0], :spread_mask.shape[1]]
#         noise_map = (noise_map + 1) / 2
#         noise_map *= 0.15
#         noise_map += np.random.uniform(0.21, 0.24)
#         terrain_map = noise_map * spread_mask
#     elif biome == 50 or biome == 60:
#         noise = SimplexNoise(seed=seed, width=spread_mask.shape[1], height=spread_mask.shape[0], scale=scale, octaves=octaves, persistence=0.4, lacunarity=2)
#         noise_map = noise.fractal_noise(noise="open", x_offset=x_offset, y_offset=y_offset, reason="heightmap")
#         noise_map = (noise_map + 1) / 2
#         noise_map = noise_map**(0.1)
#         noise_map = (noise_map - np.min(noise_map)) / (np.max(noise_map) - np.min(noise_map))
#         noise_map *= 0.2
#         noise_map += np.random.uniform(0.24, 0.26)
#         terrain_map = noise_map * spread_mask
#     elif biome == 20: # this noise would be good for desert
#         noise = SimplexNoise(seed=seed, width=spread_mask.shape[1], height=spread_mask.shape[0], scale=200, octaves=5, persistence=0.35, lacunarity=2)
#         noise_map = noise.fractal_noise(noise="open", x_offset=x_offset, y_offset=y_offset, reason="heightmap")
#         noise_map = (noise_map + 1) / 2
#         noise_map *= 0.15
#         noise_map += np.random.uniform(0.21, 0.24)
#         terrain_map = noise_map * spread_mask
#     else:
#         noise = SimplexNoise(seed=seed, width=spread_mask.shape[1], height=spread_mask.shape[0], scale=scale, octaves=octaves, persistence=0.5, lacunarity=2)
#         noise_map = noise.fractal_noise(noise="open", x_offset=x_offset, y_offset=y_offset, reason="heightmap")
#         noise_map = np.abs(noise_map)
#         noise_map = (noise_map + 1) / 2
#         noise_map *= 0.15
#         noise_map += np.random.uniform(0.21, 0.24)
#         terrain_map = noise_map * spread_mask










#     return terrain_map

# def generate_noise(seed, scale, x_offset=0, y_offset=0, octaves=8, start_frequency=1):
#     noise = SimplexNoise(seed=seed, width=1024, height=1024, scale=scale, octaves=octaves, persistence=0.5, lacunarity=2)
#     noise_map = noise.fractal_noise(noise="open", x_offset=x_offset, y_offset=y_offset, reason="heightmap")
#     noise_map = (noise_map + 1) / 2
#     return noise_map
