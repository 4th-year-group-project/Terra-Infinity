import warnings

import cv2
import matplotlib.pyplot as plt
import numpy as np
from scipy.ndimage import gaussian_filter
from skimage.filters.rank import entropy
from skimage.morphology import disk

from cellular_automata.scaling_heightmap import ca_in_mask
from generation import Noise

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
        
    def get_sparseness(self, tree_density, low, high):
        sparseness = 100 - tree_density
        return (((sparseness) / 100) * (high - low)) + low

    def temperate_rainforest(self):
        tree_density = self.parameters.get("temperate_rainforest").get("tree_density", 50)
        temperate_rainforest_max_height = self.parameters.get("temperate_rainforest").get("max_height", 50) / 100
        temperate_rainforest_max_height = self.global_max_height * temperate_rainforest_max_height
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                     scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0.22, temperate_rainforest_max_height)
        heightmap = noise_map * self.spread_mask

        sparseness = self.get_sparseness(tree_density, 7, 15)

        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.4, sparseness=sparseness, lower_bound=0.2, low=0.22, high=temperate_rainforest_max_height)
        return heightmap, placed_plants

    def boreal_forest(self):

        tree_density = self.parameters.get("boreal_forest").get("tree_density", 100)
        boreal_forest_max_height = self.parameters.get("boreal_forest").get("max_height", 100) / 100
        boreal_forest_max_height = self.global_max_height * boreal_forest_max_height
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0.32, boreal_forest_max_height)
        heightmap = noise_map * self.spread_mask
        sparseness = self.get_sparseness(tree_density, 7, 10)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.3, sparseness=sparseness, low=0.32, high=boreal_forest_max_height)
        return heightmap, placed_plants

    def grassland(self):
        grassland_max_height = self.parameters.get("grassland").get("max_height", 100) / 100
        grassland_max_height = self.global_max_height * grassland_max_height
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0.33, grassland_max_height)
        heightmap = noise_map * self.spread_mask
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.7, sparseness=20)
        return heightmap, placed_plants

    def tundra(self):
        tundra_max_height = self.parameters.get("tundra").get("max_height", 100) / 100
        tundra_max_height = self.global_max_height * tundra_max_height
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0.22, tundra_max_height)
        heightmap = noise_map * self.spread_mask
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.6, sparseness=50, low=0.22, high=tundra_max_height)
        return heightmap, placed_plants

    def savanna(self):
        savanna_max_height = self.parameters.get("savanna").get("max_height", 100) / 100
        savanna_max_height = self.global_max_height * savanna_max_height
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0.21, savanna_max_height)
        heightmap = noise_map * self.spread_mask
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.65, sparseness=20, low=0.21, high=savanna_max_height)
        return heightmap, placed_plants

    def woodland(self):
        woodland_max_height = self.parameters.get("woodland").get("max_height", 100) / 100
        woodland_max_height = self.global_max_height * woodland_max_height
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0.31, woodland_max_height)
        heightmap = noise_map * self.spread_mask

        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.4, sparseness=8, lower_bound=0.4, high=woodland_max_height, low=0.31)

        return heightmap, placed_plants

    def tropical_rainforest(self):
        tropical_rainforest_max_height = self.parameters.get("tropical_rainforest").get("max_height", 100) / 100
        tropical_rainforest_max_height = self.global_max_height * tropical_rainforest_max_height
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0.22, tropical_rainforest_max_height)
        heightmap = noise_map * self.spread_mask
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.2, sparseness=7, lower_bound=0.2, low=0.22, high=tropical_rainforest_max_height)
        return heightmap, placed_plants

    def temperate_seasonal_forest(self):
        temperate_seasonal_forest_max_height = self.parameters.get("temperate_seasonal_forest").get("max_height", 100) / 100
        temperate_seasonal_forest_max_height = self.global_max_height * temperate_seasonal_forest_max_height
        noise_overlay_scale = 0.028
        heightmap = ca_in_mask(self.seed, self.binary_mask)
        # archie method: heightmap normalize

        noise_to_add = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=30, octaves=4, persistence=0.5, lacunarity=2, start_freq=9)


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

        negative_space_noise = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        negative_space_noise = self.normalise(negative_space_noise, 0, 1)
        heightmap = heightmap + (negative_space_noise*0.2*inverted_image_entropy)

        perturbing_noise = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=200, octaves=1, persistence=0.5, lacunarity=2)

        heightmap = heightmap + perturbing_noise*0.3
        heightmap = self.normalise(heightmap, 0.26, temperate_seasonal_forest_max_height)
        heightmap *= self.spread_mask

        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.3, sparseness=7)
        return heightmap, placed_plants


    def subtropical_desert(self):
        subtropical_desert_max_height = self.parameters.get("subtropical_desert").get("max_height", 100) / 100
        subtropical_desert_max_height = self.global_max_height * subtropical_desert_max_height
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0.22, subtropical_desert_max_height)
        heightmap = noise_map * self.spread_mask
        placed_plants = []
        return heightmap, placed_plants

    def ocean(self):
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.07
        return noise_map * self.spread_mask, []


    def default(self):
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
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


