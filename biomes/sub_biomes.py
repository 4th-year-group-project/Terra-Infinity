from generation import Noise
import numpy as np
from generation import Display
from generation import tools
class Sub_Biomes:
    def __init__(self, seed, width, height):
        self.seed = seed
        self.noise = Noise(seed=seed, width=width, height=height)
        self.width = width
        self.height = height

    def normalise(self, heightmap, low, high):
        return (heightmap - np.min(heightmap)) / (np.max(heightmap) - np.min(heightmap)) * (high - low) + low

    def flats(self, x_offset, y_offset, min_height, max_height, variation):
        base_noise = self.noise.fractal_simplex_noise(seed=self.seed, width=self.width, height=self.height, x_offset=x_offset, y_offset=y_offset, scale=1024, octaves=3, persistence=0.4, lacunarity=2.0)
        #base_noise = self.noise.warped_simplex_noise(seed=self.seed, width=self.width, height=self.height, x_offset=x_offset, y_offset=y_offset, scale=512, octaves=8, persistence=0.45, lacunarity=2.0, warp_x=0, warp_y=0)
        #base_noise = self.noise.billow_noise(seed=self.seed, width=self.width, height=self.height, x_offset=x_offset, y_offset=y_offset, scale=512, octaves=8, persistence=0.45, lacunarity=2.0)
        base_noise = self.normalise(base_noise, 0, 1)

        base_noise = base_noise**variation
        texture_noise = self.noise.fractal_simplex_noise(seed=self.seed, width=self.width, height=self.height, x_offset=x_offset, y_offset=y_offset, scale=1024, octaves=8, persistence=0.5, lacunarity=2.0)
        texture_noise = self.normalise(texture_noise, 0, 1)
        add_noise = base_noise + texture_noise
        #base_noise =(base_noise - np.min(base_noise)) / (np.max(base_noise) - np.min(base_noise)) * (0.5 - 0.3) + 0.3
    # base_noise = normalize(base_noise, 0, 1)
        add_noise = self.normalise(add_noise, min_height, max_height)
        return add_noise

    # def hills(self, x_offset, y_offset, min_height, max_height):
    #     base_noise = noise.fractal_simplex_noise(seed=seed, width=width, height=height, x_offset=x_offset, y_offset=y_offset, scale=1024, octaves=2, persistence=0.45, lacunarity=2.0)
    #     base_noise = normalize(base_noise, 0.2, 1)
    #     hills = noise.fractal_simplex_noise(seed=seed, width=width, height=height, x_offset=x_offset, y_offset=y_offset, scale=1024, octaves=8, persistence=0.5, lacunarity=2.0)
    #     hills = normalize(hills, 0, 1)
    #     noise_map = base_noise + hills*0.3
    #     noise_map = normalize(noise_map, 0, 1)

    def hills(self, x_offset, y_offset, min_height, max_height, ruggedness=2):
        base_noise = self.noise.billow_noise(seed=self.seed, width=self.width, height=self.height, x_offset=x_offset, y_offset=y_offset, scale=1024, octaves=ruggedness, persistence=0.5, lacunarity=2.0)
        texture_noise = self.noise.fractal_simplex_noise(seed=self.seed, width=self.width, height=self.height, x_offset=x_offset, y_offset=y_offset, scale=1024, octaves=8, persistence=0.5, lacunarity=2.0)
        texture_noise = self.normalise(texture_noise, 0, 1)
        base_noise = self.normalise(base_noise, 0, 1)
        base_noise = tools.smooth(base_noise)
        add_noise = base_noise + texture_noise
        add_noise = self.normalise(add_noise, min_height, max_height)
        return add_noise


    def dla_mountains(self):
        pass    

    def volcanoes(self):
        pass

#...

# def flat_land(seed, width, height, x_offset, y_offset):
#     base_noise = noise.fractal_simplex_noise(seed=seed, width=width, height=height, x_offset=x_offset, y_offset=y_offset, scale=512, octaves=8, persistence=0.45, lacunarity=2.0)
#     base_noise = normalize(base_noise, 0, 1)
#     base_noise = base_noise**2
#     #base_noise =(base_noise - np.min(base_noise)) / (np.max(base_noise) - np.min(base_noise)) * (0.5 - 0.3) + 0.3
#    # base_noise = normalize(base_noise, 0, 1)
#     return base_noise


# def bumpy_land(seed, width, height, x_offset, y_offset):
#     base_noise = noise.fractal_simplex_noise(seed=seed, width=width, height=height, x_offset=x_offset, y_offset=y_offset, scale=512, octaves=8, persistence=0.45, lacunarity=2.0)
#     base_noise = normalize(base_noise, 0.2, 1)
#     base_noise = base_noise**2
#     #base_noise =(base_noise - np.min(base_noise)) / (np.max(base_noise) - np.min(base_noise)) * (0.5 - 0.3) + 0.3
#    # base_noise = normalize(base_noise, 0, 1)
#     return base_noise

# def rolling_hills(seed, width, height, x_offset, y_offset):

#     base_noise = noise.fractal_simplex_noise(seed=seed, width=width, height=height, x_offset=x_offset, y_offset=y_offset, scale=1024, octaves=2, persistence=0.45, lacunarity=2.0)
#     base_noise = normalize(base_noise, 0.2, 1)
#     hills = noise.fractal_simplex_noise(seed=seed, width=width, height=height, x_offset=x_offset, y_offset=y_offset, scale=1024, octaves=8, persistence=0.5, lacunarity=2.0)
#     hills = normalize(hills, 0, 1)
#     noise_map = base_noise + hills*0.3
#     noise_map = normalize(noise_map, 0, 1)
#     return noise_map
    
# if __name__ == "__main__":
#     # Example usage
#     seed = 12345
#     width = 1024
#     height = 1024
#     sub_biome_generator = Sub_Biomes(seed, width, height)
    
#     # Generate hilly land
#     hilly_land = sub_biome_generator.hills(0, 0, 0.3, 0.5)