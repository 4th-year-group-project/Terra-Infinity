
from .sub_biomes import *

# noise_gen = Noise(seed=44, width=1024, height=1024, x_offset=0, y_offset=0)
# sub = Sub_Biomes(seed=44, width=1024, height=1024, x_offset=0, y_offset=0)
# start = time.time()
# # warp_noise = sub.noise.fractal_simplex_noise(scale=512, octaves=4, persistence=0.5, lacunarity=1.9)
# # heightmap = sub.noise.uber_noise(scale=450, octaves=8, persistence=0.45, lacunarity=1.9,
# #                                  warp_x=warp_noise, warp_y=warp_noise*2, warp_strength=30,
# #                                  sharpness=0.7, slope_erosion=0.8, altitude_erosion=0.3, ridge_erosion=0)
# #heightmap = sub.step_desert(density=40, beta=0.2)
# heightmap = sub.noise.fractal_simplex_noise(scale=256, octaves=8, persistence=0.5, lacunarity=1.9, noise="open")
# scale_map = tools.normalize(sub.noise.fractal_simplex_noise(scale=1024, octaves=3, persistence=0.5, lacunarity=1.9, noise="open"))
# heightmap = 1/(1+np.exp(0-15*heightmap))
# heightmap *= scale_map
# heightmap = normalize(heightmap, 0, 1)

# print(time.time()-start)
# print(heightmap.min(), heightmap.max())

# display = Display(heightmap, height_scale=250, colormap="cliffs")
# display.display_heightmap()

