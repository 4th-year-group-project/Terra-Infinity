import warnings

import cv2
import matplotlib.pyplot as plt
import numpy as np
from skimage.filters.rank import entropy
from skimage.morphology import disk

from cellular_automata.scaling_heightmap import ca_in_mask
from Noise.simplex import SimplexNoise
from scipy.ndimage import gaussian_filter

from flora.flora_placement import place_plants

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
    def __init__(self, binary_mask, spread_mask, seed, x_offset, y_offset, tree_density):
        self.seed = seed
        self.binary_mask = binary_mask
        self.spread_mask = spread_mask
        self.x_offset = x_offset
        self.y_offset = y_offset
        self.width = spread_mask.shape[1]
        self.height = spread_mask.shape[0]
        self.tree_density = tree_density 

    def normalise(self, heightmap, low, high):
        return (heightmap - np.min(heightmap)) / (np.max(heightmap) - np.min(heightmap)) * (high - low) + low
        
    def get_sparseness(self, tree_density, low, high):
        sparseness = 100 - tree_density
        return (((sparseness) / 100) * (high - low)) + low

    def temperate_rainforest(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.22
        heightmap = noise_map * self.spread_mask

        sparseness = self.get_sparseness(self.tree_density, 5, 15)

        placed_plants = place_plants(heightmap, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.4, sparseness=sparseness, lower_bound=0.2)
        return heightmap, placed_plants

    def boreal_forest(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.32
        heightmap = noise_map * self.spread_mask
        placed_plants = place_plants(heightmap, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.4, sparseness=6)
        return heightmap, placed_plants

    def grassland(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.33
        heightmap = noise_map * self.spread_mask
        placed_plants = place_plants(heightmap, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.8, sparseness=20)
        return heightmap, placed_plants

    def tundra(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.22
        heightmap = noise_map * self.spread_mask
        placed_plants = place_plants(heightmap, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.6, sparseness=50)
        return heightmap, placed_plants

    def savanna(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.31
        heightmap = noise_map * self.spread_mask
        placed_plants = place_plants(heightmap, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.65, sparseness=20)
        return heightmap, placed_plants

    def woodland(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.31
        heightmap = noise_map * self.spread_mask
        placed_plants = place_plants(heightmap, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.4, sparseness=10)
        return heightmap, placed_plants

    def tropical_rainforest(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.22
        heightmap = noise_map * self.spread_mask
        placed_plants = place_plants(heightmap, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.2, sparseness=6, lower_bound=0.2)
        return heightmap, placed_plants

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

        heightmap_to_entropy = self.normalise(heightmap, 0, 1)
        image_entropy = entropy(heightmap_to_entropy, disk(5))
        image_entropy = self.normalise(image_entropy, 0, 1)
        inverted_image_entropy = 1 - image_entropy
        #gaussian filter on inverted_image_entropy
        inverted_image_entropy = gaussian_filter(inverted_image_entropy, sigma=10)


        heightmap = heightmap + (noise_to_add*noise_overlay_scale*image_entropy)
        # heightmap = heightmap + (noise_to_add*noise_overlay_scale)
        heightmap = self.normalise(heightmap, 0, 1)
        heightmap = heightmap**2

        negative_space_noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        negative_space_noise = negative_space_noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap", start_frequency=1)
        negative_space_noise = self.normalise(negative_space_noise, 0, 1)
        heightmap = heightmap + (negative_space_noise*0.2*inverted_image_entropy)

        perturbing_noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=200, octaves=1, persistence=0.5, lacunarity=2)
        perturbing_noise = perturbing_noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap", start_frequency=1)

        heightmap = heightmap + perturbing_noise*0.3
        heightmap = self.normalise(heightmap, 0.26, 1*ca_scale)
        heightmap *= self.spread_mask

        placed_plants = place_plants(heightmap, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.3, sparseness=6)
        return heightmap, placed_plants


    def subtropical_desert(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.22
        heightmap = noise_map * self.spread_mask
        placed_plants = []
        return heightmap, placed_plants

    def ocean(self):
        noise = SimplexNoise(seed=self.seed, width=self.width, height=self.height, scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = noise.fractal_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset, reason="heightmap")
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.07
        return noise_map * self.spread_mask, []


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


