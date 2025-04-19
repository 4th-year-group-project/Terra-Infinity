import warnings

import cv2
import matplotlib.pyplot as plt
import numpy as np
from scipy.ndimage import gaussian_filter
from skimage.filters.rank import entropy
from skimage.morphology import disk
from scipy.special import softmax

from cellular_automata.scaling_heightmap import ca_in_mask
from generation import Noise, tools
from biomes import sub_biomes

from flora.flora_placement import place_plants

warnings.filterwarnings("ignore")

class BBTG:
    """Biome Based Terrain Generator

    Generates terrain based on the biome number given.

    Numbering:  1: Boreal Forest Plains
                2: Boreal Forest Hills
                3: Boreal Forest Mountains
                10: Grassland Plains
                11: Grassland Hills
                12: Grassland Rocky Fields
                13: Grassland Terraced Fields
                20: Tundra Plains
                21: Tundra Blunt Mountains
                22: Tundra Pointy Peaks
                30: Savanna Plains
                31: Savanna Mountains
                40: Woodland Hills
                50: Tropical Rainforest Plains
                51: Tropical Rainforest Mountains
                52: Tropical Rainforest Volcanoes
                53: Tropical Rainforest Hills
                60: Temperate Rainforest Hills
                61: Temperate Rainforest Mountains
                62: Temperate Rainforest Swamp
                70: Temperate Seasonal Forest Hills (Autumnal)
                71: Temperate Seasonal Forest Mountains (Autumnal)
                72: Temperate Seasonal Forest Hills
                73: Temperate Seasonal Forest Mountains
                80: Desert Terraces
                81: Desert Dunes
                82: Desert Oasis
                83: Desert Ravines
                84: Desert Cracked
                90: Ocean Seabed
                91: Ocean Trenches
                92: Ocean Volcanic Islands
                93: Ocean Water Stacks
    """
    def __init__(self, binary_mask, spread_mask, seed, x_offset, y_offset, parameters):
        self.seed = seed
        self.binary_mask = binary_mask
        self.spread_mask = spread_mask
        self.x_offset = x_offset
        self.y_offset = y_offset

        combined_seed = hash((seed, x_offset, y_offset)) % (2**32 - 1)  # Ensure it's in range for RNG
        self.rng = np.random.default_rng(combined_seed)

        self.width = spread_mask.shape[1]
        self.height = spread_mask.shape[0]
        self.noise = Noise(seed=seed, width=self.width, height=self.height)
        self.parameters = parameters
        self.global_max_height = parameters.get("global_max_height", 100) / 100
        # lowest global max height corresponds to 0.25
        self.global_max_height = 0.75 * self.global_max_height + 0.25
        self.sub_biomes = sub_biomes.Sub_Biomes(seed, self.width, self.height, x_offset, y_offset)
        self.global_tree_density = parameters.get("global_tree_density", 50)
        self.global_tree_density =  (self.global_tree_density / 100) * (2 - -2) + -2
        self.global_ruggedness = parameters.get("global_ruggedness", 50) / 100


    def normalise(self, heightmap, low, high):
        return (heightmap - np.min(heightmap)) / (np.max(heightmap) - np.min(heightmap)) * (high - low) + low
        
    def get_sparseness(self, tree_density, low, high):
        sparseness = 100 - tree_density
        return (((sparseness) / 100) * (high - low)) + low

    
    def boreal_forest_plains(self):
        lowest_height = 0.22

        boreal_forest = self.parameters.get("boreal_forest").get("plains")

        # Map max_height from 0–100 to [lowest_height, self.global_max_height]
        max_height_pct = boreal_forest.get("max_height", 30)
        boreal_forest_plains_max_height = tools.map0100(max_height_pct, lowest_height, self.global_max_height)

        # Map evenness from 0–100 (inverted) to [0.5, 10]
        evenness_pct = boreal_forest.get("evenness", 50)
        evenness = tools.map0100(100 - evenness_pct, 0.5, 10)

        terrain_map = self.sub_biomes.flats(lowest_height, boreal_forest_plains_max_height, evenness, scale=800, persistence=0.42)
        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("boreal_forest").get("plains").get("tree_density", 50)

        if tree_density == 0:
            return heightmap, []
        sparseness = self.get_sparseness(tree_density, 7 - self.global_tree_density, 12)

        placed_plants =  place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.3, sparseness=sparseness, low=lowest_height, high=boreal_forest_plains_max_height)
    
        return heightmap, placed_plants
    
    def boreal_forest_hills(self):
        lowest_height = 0.22

        boreal_forest_hills = self.parameters.get("boreal_forest").get("hills")

        # Map max_height from 0–100 to [lowest_height, self.global_max_height]
        max_height_pct = boreal_forest_hills.get("max_height", 40)
        boreal_forest_hills_max_height = tools.map0100(max_height_pct, lowest_height, self.global_max_height)

        # Map bumpiness from 0–100 to [1, 5]
        bumpiness_pct = boreal_forest_hills.get("bumpiness", 50)
        bumpiness = tools.map0100(bumpiness_pct, 1, 5)

        terrain_map = self.sub_biomes.hills(lowest_height, boreal_forest_hills_max_height, bumpiness, scale=800)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("boreal_forest").get("hills").get("tree_density", 50)

        if tree_density == 0:
            return heightmap, []
        
        sparseness = self.get_sparseness(tree_density, 6 - self.global_tree_density, 11)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.3, sparseness=sparseness, low=lowest_height, high=boreal_forest_hills_max_height)
        return heightmap, placed_plants
    
    def boreal_forest_mountains(self):
        if self.rng.random() < 0.5:
            lowest_height = 0.22
            boreal_forest_mountains_max_height = self.parameters.get("boreal_forest").get("mountains").get("max_height", 70) / 100
            boreal_forest_mountains_max_height = (self.global_max_height - lowest_height) * boreal_forest_mountains_max_height + lowest_height
            boreal_forest_ruggedness = self.parameters.get("boreal_forest").get("mountains").get("ruggedness", 50)
            boreal_forest_ruggedness = boreal_forest_ruggedness * self.global_ruggedness
            num_iterations = 1 + int(boreal_forest_ruggedness / 8)
            terrain_map = self.sub_biomes.dla_mountains(lowest_height, boreal_forest_mountains_max_height, self.binary_mask, num_iterations, 1.5)
        else:
            lowest_height = 0.22
            boreal_forest_mountains_max_height = self.parameters.get("boreal_forest").get("mountains").get("max_height", 70) / 100
            boreal_forest_mountains_max_height = (self.global_max_height - lowest_height) * boreal_forest_mountains_max_height + lowest_height
            
            boreal_forest_ruggedness = self.parameters.get("boreal_forest").get("mountains").get("ruggedness", 50)
            boreal_forest_ruggedness = boreal_forest_ruggedness * self.global_ruggedness
            ruggedness = tools.map0100(boreal_forest_ruggedness, 0.45, 0.55)

            warp_noise = self.noise.fractal_simplex_noise(scale=512, octaves=4, persistence=0.5, lacunarity=1.9)
            terrain_map = self.sub_biomes.noise.uber_noise(scale=350, octaves=8, persistence=ruggedness, lacunarity=1.9,
                                 warp_x=warp_noise, warp_y=warp_noise*2, warp_strength=40,
                                 sharpness=-0.7, slope_erosion=0.8, altitude_erosion=0.2, ridge_erosion=0)
            terrain_map = tools.normalize(terrain_map, lowest_height, boreal_forest_mountains_max_height)
        
        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("boreal_forest").get("mountains").get("tree_density", 50)

        if tree_density == 0:
            return heightmap, []
        
        sparseness = self.get_sparseness(tree_density, 8 - self.global_tree_density, 13)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.3, sparseness=sparseness, low=lowest_height, high=boreal_forest_mountains_max_height)
        return heightmap, placed_plants
    
    def grassland_plains(self):
        lowest_height = 0.22

        grassland_plains = self.parameters.get("grassland").get("plains")

        # Map max_height from 0–100 to [lowest_height, self.global_max_height]
        max_height_pct = grassland_plains.get("max_height", 30)
        grassland_plains_max_height = tools.map0100(max_height_pct, lowest_height, self.global_max_height)

        # Map inverted evenness from 0–100 to [0.5, 10]
        evenness_pct = grassland_plains.get("evenness", 50)
        evenness = tools.map0100(100 - evenness_pct, 0.5, 10)

        terrain_map = self.sub_biomes.flats(lowest_height, grassland_plains_max_height, evenness, scale=1200)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("grassland").get("plains").get("tree_density", 50)

        if tree_density == 0:
            return heightmap, []
        
        sparseness = self.get_sparseness(tree_density, 20 - self.global_tree_density, 30)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.7, sparseness=sparseness, low=lowest_height, high=grassland_plains_max_height)
        return heightmap, placed_plants
    
    def grassland_hills(self):
        lowest_height = 0.22

        grassland_hills = self.parameters.get("grassland").get("hills")

        # Map max_height from 0–100 to [lowest_height, self.global_max_height]
        max_height_pct = grassland_hills.get("max_height", 40)
        grassland_hills_max_height = tools.map0100(max_height_pct, lowest_height, self.global_max_height)

        # Map bumpiness from 0–100 to [1, 3]
        bumpiness_pct = grassland_hills.get("bumpiness", 50)
        bumpiness = tools.map0100(bumpiness_pct, 1, 3)

        terrain_map = self.sub_biomes.hills(lowest_height, grassland_hills_max_height, bumpiness, lacunarity=1.8)
        
        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("grassland").get("hills").get("tree_density", 50)

        if tree_density == 0:
            return heightmap, []
        
        sparseness = self.get_sparseness(tree_density, 20 - self.global_tree_density, 30)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.7, sparseness=sparseness, low=lowest_height, high=grassland_hills_max_height)
        return heightmap, placed_plants
    
    def grassland_rocky_fields(self):
        lowest_height = 0.22

        grassland_rocky_fields = self.parameters.get("grassland").get("rocky_fields")

        # Map max_height from 0–100 to [lowest_height, self.global_max_height]
        max_height_pct = grassland_rocky_fields.get("max_height", 40)
        grassland_rocky_fields_max_height = tools.map0100(max_height_pct, lowest_height, self.global_max_height)

        # Map rockiness from 0–100 to [0.2, 0.8]
        rockiness_pct = grassland_rocky_fields.get("rockiness", 50)
        rockiness = tools.map0100(rockiness_pct, 0.2, 0.8)

        terrain_map = self.sub_biomes.rocky_field(lowest_height, grassland_rocky_fields_max_height, rockiness)
        heightmap = terrain_map * self.spread_mask

        tree_density = self.parameters.get("grassland").get("rocky_fields").get("tree_density", 50)
        if tree_density == 0:
            return heightmap, []
        
        sparseness = self.get_sparseness(tree_density, 20 - self.global_tree_density, 30)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.7, sparseness=sparseness, low=lowest_height, high=grassland_rocky_fields_max_height)

        return heightmap, placed_plants
    
    # doesnt blend well
    def grassland_terraced_fields(self):
        lowest_height = 0.1
        terraced_fields = self.parameters.get("grassland").get("terraced_fields")

        # Map max_height from 0–100 to [lowest_height, self.global_max_height]
        max_height_pct = terraced_fields.get("max_height", 40)
        grassland_terraced_fields_max_height = tools.map0100(max_height_pct, lowest_height, self.global_max_height)

        # Map number_of_terraces from 0–100 to [5, 20], then cast to int
        num_terraces_pct = terraced_fields.get("number_of_terraces", 50)
        num_terraces = int(tools.map0100(num_terraces_pct, 5, 20))

        # Map smoothness to steepness: 0–100 → [3, 10]
        smoothness_pct = terraced_fields.get("smoothness", 50)
        steepness = tools.map0100(smoothness_pct, 3, 10)

        # Map size from 0–100 to [250, 1000]
        size_pct = terraced_fields.get("size", 50)
        size = tools.map0100(size_pct, 250, 1000)

        terrain_map = self.sub_biomes.terraced_rice_fields(lowest_height, grassland_terraced_fields_max_height, num_terraces, steepness, size)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("grassland").get("terraced_fields").get("tree_density", 50)

        if tree_density == 0:
            return heightmap, []
        
        sparseness = self.get_sparseness(tree_density, 20 - self.global_tree_density, 30)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.7, sparseness=sparseness, low=lowest_height, high=grassland_terraced_fields_max_height)
        return heightmap, placed_plants

    def tundra_plains(self):
        lowest_height = 0.22

        tundra_plains = self.parameters.get("tundra").get("plains")

        # Map max_height from 0–100 to [lowest_height, self.global_max_height]
        max_height_pct = tundra_plains.get("max_height", 40)
        tundra_plains_max_height = tools.map0100(max_height_pct, lowest_height, self.global_max_height)

        # Map inverted evenness from 0–100 to [0.5, 10]
        evenness_pct = tundra_plains.get("evenness", 50)
        evenness = tools.map0100(100 - evenness_pct, 0.5, 10)

        terrain_map = self.sub_biomes.flats(lowest_height, tundra_plains_max_height, evenness, persistence=0.38)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("tundra").get("plains").get("tree_density", 50)

        if tree_density == 0:
            return heightmap, []
        
        sparseness = self.get_sparseness(tree_density, 25 - self.global_tree_density, 30)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.75, sparseness=sparseness, low=lowest_height, high=tundra_plains_max_height)
        return heightmap, placed_plants
    
    def tundra_blunt_mountains(self): 
        if self.rng.random() < 0.5:
            lowest_height = 0.22
            tundra_mountains_max_height = self.parameters.get("tundra").get("blunt_mountains").get("max_height", 100) / 100
            tundra_mountains_max_height = (self.global_max_height - lowest_height) * tundra_mountains_max_height + lowest_height
            tundra_mountains_ruggedness = self.parameters.get("tundra").get("blunt_mountains").get("ruggedness", 50)
            tundra_mountains_ruggedness = tundra_mountains_ruggedness * self.global_ruggedness
            num_iterations = 1 + int(tundra_mountains_ruggedness / 3)
            terrain_map = self.sub_biomes.dla_mountains(lowest_height, tundra_mountains_max_height, self.binary_mask, num_iterations, 1.9)
        else:
            lowest_height = 0.22
            tundra_mountains_max_height = self.parameters.get("tundra").get("blunt_mountains").get("max_height", 100) / 100
            tundra_mountains_max_height = (self.global_max_height - lowest_height) * tundra_mountains_max_height + lowest_height

            tundra_mountains_ruggedness = self.parameters.get("tundra").get("blunt_mountains").get("ruggedness", 50)
            tundra_mountains_ruggedness = tundra_mountains_ruggedness * self.global_ruggedness
            ruggedness = tools.map0100(tundra_mountains_ruggedness, 0.45, 0.52)
            terrain_map = self.sub_biomes.sheer_peaks(lowest_height, tundra_mountains_max_height, persistence=ruggedness, sheer_density=100, warp_strength=40)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("tundra").get("blunt_mountains").get("tree_density", 50)
        if tree_density == 0:
            return heightmap, []
        sparseness = self.get_sparseness(tree_density, 25 - self.global_tree_density, 30)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.75, sparseness=sparseness, low=lowest_height, high=tundra_mountains_max_height)
        return heightmap, placed_plants

    def tundra_pointy_mountains(self): 
        lowest_height = 0.22

        tundra_pointy_mountains = self.parameters.get("tundra").get("pointy_mountains")

        # Map max_height from 0–100 to [lowest_height, self.global_max_height]
        max_height_pct = tundra_pointy_mountains.get("max_height", 100)
        tundra_mountains_max_height = tools.map0100(max_height_pct, lowest_height, self.global_max_height)
        
        steepness_pct = tundra_pointy_mountains.get("steepness", 50)
        steepness = tools.map0100(steepness_pct, 2, 5)

        frequency_pct = tundra_pointy_mountains.get("frequency", 50)
        mountain_density = tools.map0100(100-frequency_pct, 150, 350)

        prominence = (-5e-6)*mountain_density**2 + (3.5e-3)*mountain_density + 0.0875
        terrain_map = self.sub_biomes.pointy_peaks(mountain_density=mountain_density, mountain_sharpness=steepness, mountain_prominence=prominence)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("tundra").get("pointy_mountains").get("tree_density", 50)
        if tree_density == 0:
            return heightmap, []
        sparseness = self.get_sparseness(tree_density, 25 - self.global_tree_density, 30)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.6, sparseness=sparseness, low=lowest_height, high=tundra_mountains_max_height)
        return heightmap, placed_plants
    
    def savanna_plains(self):
        lowest_height = 0.22
        savanna_plains_max_height = self.parameters.get("savanna").get("plains").get("max_height", 30) / 100
        savanna_plains_max_height = (self.global_max_height - lowest_height) * savanna_plains_max_height + lowest_height
        evenness = self.parameters.get("savanna").get("plains").get("evenness", 50)
        evenness = 100 - evenness
        evenness = (evenness / 100) * (10 - 0.5) + 0.5
        terrain_map = self.sub_biomes.flats(lowest_height, savanna_plains_max_height, evenness, lacunarity=2.1)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("savanna").get("plains").get("tree_density", 50)

        if tree_density == 0:
            return heightmap, []
        
        sparseness = self.get_sparseness(tree_density, 20 - self.global_tree_density, 25)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.65, sparseness=sparseness, low=lowest_height, high=savanna_plains_max_height)
        return heightmap, placed_plants

    def savanna_mountains(self):
        lowest_height = 0.22
        savanna_mountains_max_height = self.parameters.get("savanna").get("mountains").get("max_height", 50) / 100
        savanna_mountains_max_height = (self.global_max_height - lowest_height) * savanna_mountains_max_height + lowest_height
        savanna_mountains_ruggedness = self.parameters.get("savanna").get("mountains").get("ruggedness", 50)
        savanna_mountains_ruggedness = savanna_mountains_ruggedness * self.global_ruggedness
        num_iterations = 1 + int(savanna_mountains_ruggedness / 10)

        if self.rng.random() < 0.7:
            terrain_map = self.sub_biomes.dla_mountains(lowest_height, savanna_mountains_max_height, self.binary_mask, num_iterations, 1.3)
        else:
            terrain_map = self.sub_biomes.dla_mountains(lowest_height, savanna_mountains_max_height, self.binary_mask, num_iterations, 1, noise_scale=0.2, sigma=3)
            terrain_map = tools.high_smooth(terrain_map, 13, 0.5)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("savanna").get("mountains").get("tree_density", 50)
        tree_density = 100
        if tree_density == 0:
            return heightmap, []
        
        sparseness = self.get_sparseness(tree_density, 25 - self.global_tree_density, 30)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.65, sparseness=sparseness, low=lowest_height, high=savanna_mountains_max_height)
        return heightmap, placed_plants
    
    def woodland_hills(self):
        lowest_height = 0.22
        woodland_hills_max_height = self.parameters.get("woodland").get("hills").get("max_height", 40) / 100
        woodland_hills_max_height = (self.global_max_height - lowest_height) * woodland_hills_max_height + lowest_height
        bumpiness = self.parameters.get("woodland").get("hills").get("bumpiness", 50)
        bumpiness = (bumpiness / 100) * (5 - 1) + 1
        terrain_map = self.sub_biomes.hills(lowest_height, woodland_hills_max_height, bumpiness)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("woodland").get("hills").get("tree_density", 50)

        if tree_density == 0:
            return heightmap, []
        
        sparseness = self.get_sparseness(tree_density, 6 - self.global_tree_density, 10)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.5, sparseness=sparseness, low=lowest_height, high=woodland_hills_max_height)
        return heightmap, placed_plants
    
    def tropical_rainforest_plains(self):
        lowest_height = 0.22
        tropical_rainforest_flats_max_height = self.parameters.get("tropical_rainforest").get("plains").get("max_height", 40) / 100
        tropical_rainforest_flats_max_height = (self.global_max_height - lowest_height) * tropical_rainforest_flats_max_height + lowest_height
        evenness = self.parameters.get("tropical_rainforest").get("plains").get("evenness", 50)
        evenness = 100 - evenness
        evenness = (evenness / 100) * (10 - 1) + 1
        terrain_map = self.sub_biomes.flats(lowest_height, tropical_rainforest_flats_max_height, evenness, scale=900)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("tropical_rainforest").get("plains").get("tree_density", 50)
        if tree_density == 0:
            return heightmap, []
        sparseness = self.get_sparseness(tree_density, 5 - self.global_tree_density, 9)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.2, sparseness=sparseness, low=lowest_height, high=tropical_rainforest_flats_max_height)
        return heightmap, placed_plants

    def tropical_rainforest_mountains(self):
        if self.rng.random() < 0.5:
            lowest_height = 0.22
            tropical_rainforest_mountains_max_height = self.parameters.get("tropical_rainforest").get("mountains").get("max_height", 80) / 100
            tropical_rainforest_mountains_max_height = (self.global_max_height - lowest_height) * tropical_rainforest_mountains_max_height + lowest_height
            tropical_rainforest_mountains_ruggedness = self.parameters.get("tropical_rainforest").get("mountains").get("ruggedness", 50)
            tropical_rainforest_mountains_ruggedness = tropical_rainforest_mountains_ruggedness * self.global_ruggedness
            num_iterations = 1 + int(tropical_rainforest_mountains_ruggedness / 10)
            terrain_map = self.sub_biomes.dla_mountains(lowest_height, tropical_rainforest_mountains_max_height, self.binary_mask, num_iterations, 1)
        else:
            lowest_height = 0.22
            tropical_rainforest_mountains_max_height = self.parameters.get("tropical_rainforest").get("mountains").get("max_height", 80) / 100
            tropical_rainforest_mountains_max_height = (self.global_max_height - lowest_height) * tropical_rainforest_mountains_max_height + lowest_height

            tropical_rainforest_mountains_ruggedness = self.parameters.get("tropical_rainforest").get("mountains").get("ruggedness", 50)
            tropical_rainforest_mountains_ruggedness = tropical_rainforest_mountains_ruggedness * self.global_ruggedness
            ruggedness = tools.map0100(tropical_rainforest_mountains_ruggedness, 0.45, 0.52)

            warp_noise = self.noise.fractal_simplex_noise(scale=512, octaves=4, persistence=0.5, lacunarity=1.9)
            heightmap = self.noise.uber_noise(scale=450, octaves=8, persistence=ruggedness, lacunarity=1.9,
                                             warp_x=warp_noise, warp_y=warp_noise*2, warp_strength=30,
                                             sharpness=0.7, slope_erosion=0.8, altitude_erosion=0.3, ridge_erosion=0)
            terrain_map = tools.normalize(heightmap, lowest_height, tropical_rainforest_mountains_max_height)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("tropical_rainforest").get("mountains").get("tree_density", 50)
        if tree_density == 0:
            return heightmap, []
        sparseness = self.get_sparseness(tree_density, 5 - self.global_tree_density, 9)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.3, sparseness=sparseness, low=lowest_height, high=tropical_rainforest_mountains_max_height)
        return heightmap, placed_plants
    
    def tropical_rainforest_hills(self):
        lowest_height = 0.22
        tropical_rainforest_hills_max_height = self.parameters.get("tropical_rainforest").get("hills").get("max_height", 50) / 100
        tropical_rainforest_hills_max_height = (self.global_max_height - lowest_height) * tropical_rainforest_hills_max_height + lowest_height
        bumpiness = self.parameters.get("tropical_rainforest").get("hills").get("bumpiness", 50)
        bumpiness = (bumpiness / 100) * (6 - 2) + 2
        terrain_map = self.sub_biomes.hills(lowest_height, tropical_rainforest_hills_max_height, bumpiness, scale=900, persistence=0.46)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("tropical_rainforest").get("hills").get("tree_density", 50)
        if tree_density == 0:
            return heightmap, []
        sparseness = self.get_sparseness(tree_density, 5 - self.global_tree_density, 9)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.3, sparseness=sparseness, low=lowest_height, high=tropical_rainforest_hills_max_height)
        return heightmap, placed_plants
    
    def tropical_rainforest_volcanoes(self):
        lowest_height = 0.22
        tropical_volcanoes = self.parameters.get("tropical_rainforest").get("volcanoes")

        # Map max_height from 0–100 to [lowest_height, self.global_max_height]
        max_height_pct = tropical_volcanoes.get("max_height", 50)
        tropical_rainforest_volcanoes_max_height = tools.map0100(max_height_pct, lowest_height, self.global_max_height)

        density_pct = tropical_volcanoes.get("density", 50)
        density = tools.map0100(100-density_pct, 200, 450)
        
        thickness_pct = tropical_volcanoes.get("thickness", 50)
        thickness = tools.map0100(100-thickness_pct, 10, 20)

        c_pct = tropical_volcanoes.get("size", 50)
        c = tools.map0100(100-c_pct, 0.001, 0.05)

        terrain_map = self.sub_biomes.volcanoes(lowest_height, tropical_rainforest_volcanoes_max_height,
                                                volcano_density=density, tau=thickness, c=c, volcano_prominence=0.65)        

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("tropical_rainforest").get("volcanoes").get("tree_density", 50)
        if tree_density == 0:
            return heightmap, []
        sparseness = self.get_sparseness(tree_density, 10 - self.global_tree_density, 15)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.6, sparseness=sparseness, low=lowest_height, high=tropical_rainforest_volcanoes_max_height)
        return heightmap, placed_plants
    
    def temperate_rainforest_hills(self):
        lowest_height = 0.15
        temperate_rainforest_hills_max_height = self.parameters.get("temperate_rainforest").get("hills").get("max_height", 40) / 100
        temperate_rainforest_hills_max_height = (self.global_max_height - lowest_height) * temperate_rainforest_hills_max_height + lowest_height
        bumpiness = self.parameters.get("temperate_rainforest").get("hills").get("bumpiness", 50)   
        bumpiness = (bumpiness / 100) * (5 - 2) + 2
        terrain_map = self.sub_biomes.hills(lowest_height, temperate_rainforest_hills_max_height, bumpiness, scale=1100)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("temperate_rainforest").get("hills").get("tree_density", 50)   
        if tree_density == 0:
            return heightmap, []
        sparseness = self.get_sparseness(tree_density, 7 - self.global_tree_density, 13)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.4, sparseness=sparseness, low=lowest_height, high=temperate_rainforest_hills_max_height)
        return heightmap, placed_plants
    
    def temperate_rainforest_mountains(self):
        lowest_height = 0.22
        temperate_rainforest_mountains_max_height = self.parameters.get("temperate_rainforest").get("mountains").get("max_height", 80) / 100
        temperate_rainforest_mountains_max_height = (self.global_max_height - lowest_height) * temperate_rainforest_mountains_max_height + lowest_height
        temperate_rainforest_mountains_ruggedness = self.parameters.get("temperate_rainforest").get("mountains").get("ruggedness", 50)
        temperate_rainforest_mountains_ruggedness = temperate_rainforest_mountains_ruggedness * self.global_ruggedness
        num_iterations = 1 + int(temperate_rainforest_mountains_ruggedness / 7)
        terrain_map = self.sub_biomes.dla_mountains(lowest_height, temperate_rainforest_mountains_max_height, self.binary_mask, num_iterations, 1.6)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("temperate_rainforest").get("mountains").get("tree_density", 50)
        if tree_density == 0:
            return heightmap, []
        sparseness = self.get_sparseness(tree_density, 10 - self.global_tree_density, 15)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.5, sparseness=sparseness, low=lowest_height, high=temperate_rainforest_mountains_max_height)
        return heightmap, placed_plants
    
    def temperate_rainforest_swamp(self):
        lowest_height = 0.16
        temperate_rainforest_swamp_max_height = self.parameters.get("temperate_rainforest").get("swamp").get("max_height", 30) / 100
        temperate_rainforest_swamp_max_height = (self.global_max_height - lowest_height) * temperate_rainforest_swamp_max_height + lowest_height

        wetness = self.parameters.get("temperate_rainforest").get("swamp").get("wetness", 50)
        wetness = 1 + wetness/9

        terrain_map = self.sub_biomes.swamp(lowest_height, temperate_rainforest_swamp_max_height, wetness)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("temperate_rainforest").get("swamp").get("tree_density", 50)
        if tree_density == 0:
            return heightmap, []
        sparseness = self.get_sparseness(tree_density, 8 - self.global_tree_density, 13)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.4, sparseness=sparseness, low=lowest_height, high=temperate_rainforest_swamp_max_height)
        return heightmap, placed_plants

    def temperate_seasonal_forest_hills(self):
        lowest_height = 0.22
        temperate_seasonal_forest_hills_max_height = self.parameters.get("temperate_seasonal_forest").get("hills").get("max_height", 40) / 100
        temperate_seasonal_forest_hills_max_height = (self.global_max_height - lowest_height) * temperate_seasonal_forest_hills_max_height + lowest_height
        bumpiness = self.parameters.get("temperate_seasonal_forest").get("hills").get("bumpiness", 50)
        bumpiness = (bumpiness / 100) * (6 - 1) + 1
        terrain_map = self.sub_biomes.hills(lowest_height, temperate_seasonal_forest_hills_max_height, bumpiness)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("temperate_seasonal_forest").get("hills").get("tree_density", 50)
        if tree_density == 0:
            return heightmap, []
        sparseness = self.get_sparseness(tree_density, 5 - self.global_tree_density, 15)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.3, sparseness=sparseness, low=lowest_height, high=temperate_seasonal_forest_hills_max_height)
        return heightmap, placed_plants
    
    def temperate_seasonal_forest_mountains(self):
        lowest_height = 0.22
        temperate_seasonal_forest_mountains_max_height = self.parameters.get("temperate_seasonal_forest").get("mountains").get("max_height", 80) / 100
        temperate_seasonal_forest_mountains_max_height = (self.global_max_height - lowest_height) * temperate_seasonal_forest_mountains_max_height + lowest_height
        temperate_seasonal_forest_mountains_ruggedness = self.parameters.get("temperate_seasonal_forest").get("mountains").get("ruggedness", 50)
        temperate_seasonal_forest_mountains_ruggedness = temperate_seasonal_forest_mountains_ruggedness * self.global_ruggedness
        num_iterations = 1 + int(temperate_seasonal_forest_mountains_ruggedness / 10)
        terrain_map = self.sub_biomes.dla_mountains(lowest_height, temperate_seasonal_forest_mountains_max_height, self.binary_mask, num_iterations, 2)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("temperate_seasonal_forest").get("mountains").get("tree_density", 50)
        if tree_density == 0:
            return heightmap, []
        sparseness = self.get_sparseness(tree_density, 7 - self.global_tree_density, 15)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.4, sparseness=sparseness, low=lowest_height, high=temperate_seasonal_forest_mountains_max_height)
        return heightmap, placed_plants
    
    def desert_dunes(self):
        if self.rng.random() < 0.7:
            lowest_height = 0.3
            dunes = self.parameters.get("subtropical_desert").get("dunes")
            max_height_pct = dunes.get("max_height", 40)
            desert_dunes_max_height = tools.map0100(max_height_pct, lowest_height, 0.55)

            freq_pct = dunes.get("frequency", 50)
            desert_dunes_frequency = tools.map0100(freq_pct, 5, 15)

            waviness_pct = dunes.get("waviness", 50)
            desert_dunes_waviness = tools.map0100(waviness_pct, 50, 120)

            bumpiness_pct = dunes.get("bumpiness", 50)
            desert_dunes_bumpiness = tools.map0100(bumpiness_pct, 0.01, 0.1)

            terrain_map = self.sub_biomes.dunes(lowest_height, self.rng.normal(desert_dunes_max_height, 0.1),
                                                direction=self.rng.normal(np.pi/4, np.pi/4), 
                                                core_freq=desert_dunes_frequency, core_noise_strength=desert_dunes_waviness,
                                                phasor_amplitude=desert_dunes_bumpiness)
        else:
            lowest_height = 0.3
            dunes = self.parameters.get("subtropical_desert").get("dunes")
            max_height_pct = dunes.get("max_height", 40)
            desert_dunes_max_height = tools.map0100(max_height_pct, lowest_height, 0.7)

            freq_pct = dunes.get("frequency", 50)
            desert_dunes_frequency = tools.map0100(freq_pct, 256, 1024)

            bumpiness_pct = dunes.get("bumpiness", 50)
            desert_dunes_bumpiness = tools.map0100(bumpiness_pct, 0.001, 0.02)

            warp = self.noise.fractal_simplex_noise(scale=512, octaves=2, persistence=0.5, lacunarity=1.9)
            heightmap = self.noise.warped_simplex_noise(
                warp_x=warp, warp_y=warp*2, warp_strength=100,
                scale=desert_dunes_frequency, octaves=2, persistence=0.45, lacunarity=1.9,
            )
            phasor = self.noise.phasor_noise(num_phasors=20, freq_range=(50, 60), anisotropy=0.05, direction_bias=np.pi/3)
            terrain_map = tools.normalize(heightmap, lowest_height, desert_dunes_max_height) + tools.smooth_min(phasor, 0, 1)*desert_dunes_bumpiness

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("subtropical_desert").get("dunes").get("tree_density", 0)
        if tree_density == 0:
            return heightmap, []
        sparseness = self.get_sparseness(tree_density, 25 - self.global_tree_density, 30)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.65, sparseness=sparseness, low=lowest_height, high=desert_dunes_max_height)

        return heightmap, placed_plants
    
    # they work when not next to oceans or rivers (visually work i mean)
    def desert_terraces(self):
        lowest_height = 0.3
        mesas = self.parameters.get("subtropical_desert").get("mesas")
        max_height_pct = mesas.get("max_height", 40)
        desert_mesas_max_height = tools.map0100(max_height_pct, lowest_height, self.global_max_height)

        scale_pct = mesas.get("scale", 50) 
        desert_mesas_scale = tools.map0100(scale_pct, 128, 256)

        number_of_terraces_pct = mesas.get("number_of_terraces", 50)
        number_of_terraces = int(tools.map0100(number_of_terraces_pct, 5, 10))

        steepness_pct = mesas.get("steepness", 50)
        steepness = tools.map0100(steepness_pct, 3, 5)

        terrain_map = self.sub_biomes.mesa_terraces(lowest_height, desert_mesas_max_height,
                                                    num_terraces=number_of_terraces,
                                                    steepness=steepness, scale=desert_mesas_scale,
                                                    ground_flatness=5, peak_flatness=10 
                                                    )

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("subtropical_desert").get("mesas").get("tree_density", 0)
        if tree_density == 0:
            return heightmap, []
        sparseness = self.get_sparseness(tree_density, 25 - self.global_tree_density, 30)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.65, sparseness=sparseness, low=lowest_height, high=desert_mesas_max_height)
        
        return heightmap, placed_plants
    
    def desert_ravines(self):
        lowest_height = 0.22
        ravines = self.parameters.get("subtropical_desert").get("ravines")
        max_height_pct = ravines.get("max_height", 40)
        desert_ravines_max_height = tools.map0100(max_height_pct, lowest_height, self.global_max_height)

        ravine_scale = ravines.get("density", 50)
        desert_ravines_scale = tools.map0100(ravine_scale, 200, 500)
        
        ravine_width = ravines.get("ravine_width", 50)
        desert_ravines_width = tools.map0100(ravine_width, 0, 1)

        smoothness_pct = ravines.get("smoothness", 50)
        desert_ravines_smoothness = tools.map0100(smoothness_pct, 2, 10)

        steepness_pct = ravines.get("steepness", 50)  
        desert_ravines_steepness = tools.map0100(steepness_pct, 2,5 )

        terrain_map = self.sub_biomes.ravines(lowest_height, desert_ravines_max_height,
                                              scale=desert_ravines_scale, sharpness=desert_ravines_steepness,
                                              ravine_width=desert_ravines_width, low_flatness=desert_ravines_smoothness)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("subtropical_desert").get("ravines").get("tree_density", 0)
        if tree_density == 0:
            return heightmap, []
        sparseness = self.get_sparseness(tree_density, 25 - self.global_tree_density, 30)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.65, sparseness=sparseness, low=lowest_height, high=desert_ravines_max_height)
        return heightmap, placed_plants
    
    def desert_oasis(self):
        lowest_height = 0.05
        oasis = self.parameters.get("subtropical_desert").get("oasis")
        max_height_pct = oasis.get("max_height", 40)
        desert_oasis_max_height = tools.map0100(max_height_pct, lowest_height, 0.6)

        oasis_size_pct = oasis.get("size", 50)
        oasis_size = tools.map0100(oasis_size_pct, 100, 220)

        flatness_pct = oasis.get("flatness", 50)
        oasis_flatness = tools.map0100(flatness_pct, 0.005, 0.02)

        freq_pct = oasis.get("frequency", 50)
        oasis_frequency = tools.map0100(freq_pct, 20, 40)

        terrain_map = self.sub_biomes.oasis(lowest_height, desert_oasis_max_height,
                                            radial_freq=oasis_frequency, lake_size=self.rng.normal(oasis_size, 20),
                                            phasor_scale=oasis_flatness)

        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("subtropical_desert").get("oasis").get("tree_density", 0)
        if tree_density == 0:
            return heightmap, []
        sparseness = self.get_sparseness(tree_density, 25 - self.global_tree_density, 30)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.65, sparseness=sparseness, low=lowest_height, high=desert_oasis_max_height)
        return heightmap, placed_plants
    
    # add step
    def desert_cracked(self):
        if self.rng.random() < 0.7:

            lowest_height = 0.22
            desert_cracked = self.parameters.get("subtropical_desert").get("cracked")
            desert_cracked_max_height = desert_cracked.get("max_height", 40) 
            desert_cracked_max_height = tools.map0100(desert_cracked_max_height, lowest_height, 0.5)

            desert_size_pct = desert_cracked.get("size", 50)
            desert_cracked_size = int(tools.map0100(100-desert_size_pct, 100, 1000))

            desert_cracked_flatness_pct = desert_cracked.get("flatness", 50)
            desert_cracked_flatness = tools.map0100(100-desert_cracked_flatness_pct, 0.1, 0.5)

            terrain_map = self.sub_biomes.cracked_desert(lowest_height, desert_cracked_max_height,
                                                        density=desert_cracked_size, flatness=desert_cracked_flatness)
        else:
            lowest_height = 0.26
            desert_cracked = self.parameters.get("subtropical_desert").get("cracked")
            desert_cracked_max_height = desert_cracked.get("max_height", 40) 
            desert_cracked_max_height = tools.map0100(desert_cracked_max_height, lowest_height, 0.6)

            desert_size_pct = desert_cracked.get("size", 50)
            desert_cracked_size = int(tools.map0100(100-desert_size_pct, 40, 70))

            desert_cracked_flatness_pct = desert_cracked.get("flatness", 50)
            desert_cracked_flatness = tools.map0100(100-desert_cracked_flatness_pct, 0, 0.5)

            terrain_map = self.sub_biomes.step_desert(lowest_height, desert_cracked_max_height,
                                                        density=desert_cracked_size, beta=desert_cracked_flatness)


        heightmap = terrain_map * self.spread_mask
        tree_density = self.parameters.get("subtropical_desert").get("cracked").get("tree_density", 0)
        if tree_density == 0:
            return heightmap, []
        sparseness = self.get_sparseness(tree_density, 25 - self.global_tree_density, 30)
        placed_plants = place_plants(heightmap, self.spread_mask, self.seed, self.x_offset, self.y_offset, self.width, self.height, self.height, coverage=0.65, sparseness=sparseness, low=lowest_height, high=desert_cracked_max_height)
        return heightmap, placed_plants
    
    def ocean_seabed(self):
        lowest_height = 0
        ocean_seabed_max_height = self.parameters.get("ocean").get("flat_seabed").get("max_height", 50) / 100
        ocean_seabed_max_height = ocean_seabed_max_height * 0.2
        evenness = self.parameters.get("ocean").get("flat_seabed").get("evenness", 50)
        evenness = 100 - evenness
        evenness = (evenness / 100) * (10 - 0.5) + 0.5

        terrain_map = self.sub_biomes.flats(lowest_height, ocean_seabed_max_height, evenness)
        return terrain_map * self.spread_mask, []
    
    def ocean_trenches(self):
        lowest_height = 0
        ocean_trenches = self.parameters.get("ocean").get("trenches")
        ocean_trenches_max_height = ocean_trenches.get("max_height", 50) 
        ocean_trenches_max_height = tools.map0100(ocean_trenches_max_height, lowest_height, 0.2)

        trench_scale_pct = ocean_trenches.get("density", 50)
        trenches_scale = tools.map0100(trench_scale_pct, 60, 220)
        
        trench_width_pct = ocean_trenches.get("ravine_width", 50)
        trench_width = tools.map0100(trench_width_pct, 0, 1)

        smoothness_pct = ocean_trenches.get("smoothness", 50)
        trench_smoothness = tools.map0100(smoothness_pct, 4, 12)

        terrain_map = self.sub_biomes.ocean_trenches(lowest_height, ocean_trenches_max_height,
                                                        scale=trenches_scale, low_flatness=trench_smoothness,
                                                        trench_width=trench_width)
        
        heightmap = terrain_map * self.spread_mask
        return heightmap, []

    def ocean_volcanic_islands(self):
        lowest_height = 0
        ocean_volcanic_islands = self.parameters.get("ocean").get("volcanic_islands")
        ocean_volcanic_islands_max_height = ocean_volcanic_islands.get("max_height", 50)
        ocean_volcanic_islands_max_height = tools.map0100(ocean_volcanic_islands_max_height, lowest_height, 0.3)

        size_pct = ocean_volcanic_islands.get("size", 50)
        size = tools.map0100(size_pct, 0.2, 0.65)

        thickness_pct = ocean_volcanic_islands.get("thickness", 50) 
        thickness = tools.map0100(thickness_pct, 10, 20)

        density_pct = ocean_volcanic_islands.get("density", 50)
        density = tools.map0100(100-density_pct, 100, 200)

        terrain_map = self.sub_biomes.volcanoes(lowest_height, ocean_volcanic_islands_max_height, 
                                                volcano_density=density, tau=thickness, c=.01, volcano_prominence=size)
        
        return terrain_map * self.spread_mask, []
    
    def ocean_water_stacks(self):
        lowest_height = 0
        water_stack = self.parameters.get("ocean").get("water_stacks")
        ocean_water_stacks_max_height = water_stack.get("max_height", 50)
        ocean_water_stacks_max_height = tools.map0100(ocean_water_stacks_max_height, lowest_height, 0.5)

        stack_size_pct = water_stack.get("size", 50)
        stack_size = tools.map0100(stack_size_pct, 64, 128)

        terrain_map = self.sub_biomes.water_stacks(lowest_height, ocean_water_stacks_max_height, scale=stack_size)
        
        return terrain_map * self.spread_mask, []


    def default(self):
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, 0, 1)
        noise_map *= 0.1
        noise_map += 0.22
        return noise_map * self.spread_mask, []

    def generate_terrain(self, biome_number):
        match biome_number:
            case 1:
                return self.boreal_forest_plains()
            case 2:
                return self.boreal_forest_hills()
            case 3:
                return self.boreal_forest_mountains()
            case 10:
                return self.grassland_plains()
            case 11:
                return self.grassland_hills()
            case 12:
                return self.grassland_rocky_fields()
            case 13:
                return self.grassland_terraced_fields()
            case 20:
                return self.tundra_plains()
            case 21:
                return self.tundra_blunt_mountains()
            case 22:
                return self.tundra_pointy_mountains()
            case 30:
                return self.savanna_plains()
            case 31:
                return self.savanna_mountains()
            case 40:
                return self.woodland_hills()
            case 50:
                return self.tropical_rainforest_plains()
            case 51:
                return self.tropical_rainforest_mountains()
            case 52:
                return self.tropical_rainforest_volcanoes()
            case 53:
                return self.tropical_rainforest_hills()
            case 60:
                return self.temperate_rainforest_hills()
            case 61:
                return self.temperate_rainforest_mountains()
            case 62:
                return self.temperate_rainforest_swamp()
            case 70 | 72:
                return self.temperate_seasonal_forest_hills()
            case 71 | 73:
                return self.temperate_seasonal_forest_mountains()
            case 80:
                return self.desert_terraces()
            case 81:
                return self.desert_dunes()
            case 82:
                return self.desert_oasis()
            case 83:
                return self.desert_ravines()
            case 84:
                return self.desert_cracked()
            case 90:
                return self.ocean_seabed()
            case 91:
                return self.ocean_trenches()
            case 92:
                return self.ocean_volcanic_islands()
            case 93:
                return self.ocean_water_stacks()
            case _:
                return self.default()


