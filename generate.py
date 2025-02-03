from Noise import SimplexNoise, ColorMap
from Coastline import FractalCoastline, GeometryUtils, shapes
import numpy as np
import cv2
#UK detailed, 69, r=0.6, 

coastline = FractalCoastline(shape=shapes["square"],
                        displacement=shapes["square"].distance_triple[0], 
                        width=0.1,
                        roughness=0.6,
                        display=True,
                        seed=690
                        )

coastline.fractal(iterations=14)
coastline.polygonize()

width, height= 2048, 2048
mask = coastline.to_raster(width, height)
spread_mask = GeometryUtils.mask_transform(mask, spread_rate=0.2)

noise = SimplexNoise(seed=5, width=width, height=height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
noise_map = noise.fractal_noise(noise="open")
noise_map = (noise_map + 1) / 2

terrain_map = spread_mask*noise_map

colormap = ColorMap(ocean_threshold=0.2, 
                    coast_threshold=0.25, 
                    beach_threshold=0.29, 
                    lowland_threshold=0.5, 
                    highland_threshold=0.6)
terrain_colored_map = colormap.apply_terrain_colormap(terrain_map)


#coastline.plot.display_polygons(coastline.polygons)
# with open('coastline_mask.raw', 'wb') as f:
#     f.write(mask1.tobytes())

import matplotlib.pyplot as plt

plt.imshow(terrain_colored_map)
plt.title('Coastline Mask')
plt.axis('off')
plt.show()