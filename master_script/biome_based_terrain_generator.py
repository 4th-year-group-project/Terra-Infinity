import warnings
import time

import cv2
import matplotlib.pyplot as plt
import numpy as np
from scipy.ndimage import gaussian_filter
from skimage.filters.rank import entropy
from skimage.morphology import disk
from scipy.ndimage import uniform_filter
from scipy.ndimage import laplace

from cellular_automata.scaling_heightmap import ca_in_mask
from generation import Noise

warnings.filterwarnings("ignore")

class BBTG:
    """Biome Based Terrain Generator

    Generates terrain based on the biome number given.

    Numbering:  10: temperate rainforest,
                20: boreal forest,
                30: grassland,
                40: tundra,
                50: savanna,
                60: woodland,
                70: tropical rainforest,
                80: temperate seasonal forest,
                90: subtropical desert
                100: ocean
    """
    def __init__(self, binary_mask, spread_mask, seed, x_offset, y_offset, parameters):
        self.seed = seed
        self.binary_mask = binary_mask
        self.spread_mask = spread_mask
        self.x_offset = x_offset
        self.y_offset = y_offset
        self.width = spread_mask.shape[1]
        self.height = spread_mask.shape[0]
        self.noise = Noise(seed=seed, width=self.width, height=self.height)
        self.parameters = parameters
        self.global_max_height = parameters.get("global_max_height", 100)
        self.global_max_height = self.global_max_height / 100

    def normalise(self, heightmap, low, high):
        return (heightmap - np.min(heightmap)) / (np.max(heightmap) - np.min(heightmap)) * (high - low) + low

    def temperate_rainforest(self):
        temperate_rainforest_max_height = self.parameters.get("temperate_rainforest").get("max_height", 100) / 100
        temperate_rainforest_max_height = self.global_max_height * temperate_rainforest_max_height
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                     scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0.22, temperate_rainforest_max_height)
        return noise_map * self.spread_mask

    def boreal_forest(self):
        boreal_forest_max_height = self.parameters.get("boreal_forest").get("max_height", 100) / 100
        boreal_forest_max_height = self.global_max_height * boreal_forest_max_height
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0.32, boreal_forest_max_height)
        return noise_map * self.spread_mask

    def grassland(self):
        grassland_max_height = self.parameters.get("grassland").get("max_height", 100) / 100
        grassland_max_height = self.global_max_height * grassland_max_height
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0.33, grassland_max_height)
        return noise_map * self.spread_mask

    def tundra(self):
        tundra_max_height = self.parameters.get("tundra").get("max_height", 100) / 100
        tundra_max_height = self.global_max_height * tundra_max_height
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0.22, tundra_max_height)
        return noise_map * self.spread_mask

    def savanna(self):
        savanna_max_height = self.parameters.get("savanna").get("max_height", 100) / 100
        savanna_max_height = self.global_max_height * savanna_max_height
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0.21, savanna_max_height)
        return noise_map * self.spread_mask

    def woodland(self):
        woodland_max_height = self.parameters.get("woodland").get("max_height", 100) / 100
        woodland_max_height = self.global_max_height * woodland_max_height
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0.31, woodland_max_height)
        return noise_map * self.spread_mask

    def tropical_rainforest(self):
        tropical_rainforest_max_height = self.parameters.get("tropical_rainforest").get("max_height", 100) / 100
        tropical_rainforest_max_height = self.global_max_height * tropical_rainforest_max_height
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0.22, tropical_rainforest_max_height)
        return noise_map * self.spread_mask
    
    def super_fake_entropy(self, image):
        return np.abs(laplace(image))

    def temperate_seasonal_forest(self):

        heightmap = ca_in_mask(self.seed, self.binary_mask)

        # start_time = time.time()
        heightmap_to_entropy = self.normalise(heightmap, 0, 1)
        image_std = self.super_fake_entropy(heightmap_to_entropy)
        image_std = self.normalise(image_std, 0, 1)
        image_std = gaussian_filter(image_std, sigma=3)
        inverted_image_std = 1 - image_std
        # inverted_image_std = gaussian_filter(inverted_image_std, sigma=5)
        # print(f"Time taken for entropy: {time.time() - start_time} seconds")

        #Make them less smooth by adding low amplitude high frequency noise
        noise_to_add = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=30, octaves=4, persistence=0.5, lacunarity=2, start_freq=9)
        noise_to_add = self.normalise(noise_to_add, 0, 1)
        noise_overlay_scale = 0.2
        heightmap = heightmap + (noise_to_add*noise_overlay_scale*image_std)
        heightmap = self.normalise(heightmap, 0, 1)
        
        #Bring out peaks (parameterize this)
        heightmap = heightmap**2

        #Add some noise where the mountains did not reach
        negative_space_noise = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        negative_space_noise = self.normalise(negative_space_noise, 0, 1)
        inverted_image_std = self.normalise(inverted_image_std, 0, 1)
        heightmap = heightmap + (negative_space_noise*0.05*inverted_image_std)

        #Add some low frequency noise to the mountains for less peak height uniformity
        perturbing_noise = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=200, octaves=1, persistence=0.5, lacunarity=2)

        heightmap = heightmap + perturbing_noise*0.3
        heightmap = self.normalise(heightmap, 0.22, 1)
        

        return heightmap * self.spread_mask

        # subtropical_desert_max_height = self.parameters.get("subtropical_desert").get("max_height", 100) / 100
        # subtropical_desert_max_height = self.global_max_height * subtropical_desert_max_height
        # noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
        #                                             scale=100, octaves=8, persistence=0.5, lacunarity=2)
        # noise_map = self.normalise(noise_map, 0.22, subtropical_desert_max_height)
        # return noise_map * self.spread_mask


    def subtropical_desert(self):
        subtropical_desert_max_height = self.parameters.get("subtropical_desert").get("max_height", 100) / 100
        subtropical_desert_max_height = self.global_max_height * subtropical_desert_max_height
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0.22, subtropical_desert_max_height)
        return noise_map * self.spread_mask

    def ocean(self):
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.07
        return noise_map * self.spread_mask


    def default(self):
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.22
        return noise_map * self.spread_mask

    def generate_terrain(self, biome_number):
        match biome_number:
            case 10:
                return self.temperate_rainforest()
            case 20:
                return self.boreal_forest()
            case 30:
                return self.grassland()
            case 40:
                return self.tundra()
            case 50:
                return self.savanna()
            case 60:
                return self.woodland()
            case 70:
                return self.tropical_rainforest()
            case 80:
                return self.temperate_seasonal_forest()
            case 90:
                return self.subtropical_desert()
            case 100:
                return self.ocean()
            case _:
                return self.default()


