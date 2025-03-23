import warnings

import cv2
import matplotlib.pyplot as plt
import numpy as np
from skimage.filters.rank import entropy
from skimage.morphology import disk

from cellular_automata.scaling_heightmap import ca_in_mask
from Noise.simplex import SimplexNoise

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
    def __init__(self, binary_mask, spread_mask, seed, x_offset, y_offset):
        self.seed = seed
        self.binary_mask = binary_mask
        self.spread_mask = spread_mask
        self.x_offset = x_offset
        self.y_offset = y_offset
        self.width = spread_mask.shape[1]
        self.height = spread_mask.shape[0]

    def normalise(self, heightmap, low, high):
        return (heightmap - np.min(heightmap)) / (np.max(heightmap) - np.min(heightmap)) * (high - low) + low

    def temperate_rainforest(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.22
        return noise_map * self.spread_mask

    def boreal_forest(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.32
        return noise_map * self.spread_mask

    def grassland(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.33
        return noise_map * self.spread_mask

    def tundra(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.22
        return noise_map * self.spread_mask

    def savanna(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.31
        return noise_map * self.spread_mask

    def woodland(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.31
        return noise_map * self.spread_mask

    def tropical_rainforest(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.22
        return noise_map * self.spread_mask

    def temperate_seasonal_forest(self):
        ca_scale = 0.85
        noise_overlay_scale = 0.028
        heightmap = ca_in_mask(self.seed, self.binary_mask)
        # archie method: heightmap normalize

        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=30, octaves=4, persistence=0.5, lacunarity=2)
        noise_to_add = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap", start_frequency=9)

        # archie method: noise normalize
        # archie method: normalize(alpha*dla + (1-alpha)*noise)

        noise_to_add = self.normalise(noise_to_add, 0, 1)
        noise_to_add = noise_to_add

        # heightmap_to_entropy = (heightmap - np.min(heightmap)) / (np.max(heightmap) - np.min(heightmap))
        # image_entropy = entropy(heightmap_to_entropy, disk(10))
        # image_entropy = (image_entropy - np.min(image_entropy)) / (np.max(image_entropy) - np.min(image_entropy))


        # heightmap = heightmap + (noise_to_add*noise_overlay_scale*image_entropy)
        heightmap = heightmap + (noise_to_add*noise_overlay_scale)
        heightmap = (heightmap - np.min(heightmap)) / (np.max(heightmap) - np.min(heightmap))
        heightmap = heightmap**2
        heightmap = self.normalise(heightmap, 0.26, 1*ca_scale)
        heightmap *= self.spread_mask

        return heightmap


    def subtropical_desert(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.22
        return noise_map * self.spread_mask

    def ocean(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.07
        return noise_map * self.spread_mask


    def default(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.22
        return noise_map * self.spread_mask

    def generate_terrain(self, biome_number):
        print("Generating terrain for biome number: ", biome_number)
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


