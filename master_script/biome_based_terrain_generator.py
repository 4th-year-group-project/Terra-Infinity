import warnings

import cv2
import matplotlib.pyplot as plt
import numpy as np
from scipy.ndimage import gaussian_filter
from skimage.filters.rank import entropy
from skimage.morphology import disk
from scipy.special import softmax

from cellular_automata.scaling_heightmap import ca_in_mask
from generation import Noise
from biomes import sub_biomes

warnings.filterwarnings("ignore")

class BBTG:
    """Biome Based Terrain Generator

    Generates terrain based on the biome number given.

    Numbering:  0: Boreal Forest Plains
                1: Boreal Forest Hills
                2: Boreal Forest Mountains
                10: Grassland Plains
                11: Grassland Hills
                12: Grassland Rocky Fields
                13: Grassland Terraced Fields
                20: Tundra Plains
                21: Tundra Mountains
                30: Savanna Plains
                31: Savanna Mountains
                40: Woodland Hills
                50: Tropical Rainforest Flats
                51: Tropical Rainforest Mountains
                52: Tropical Rainforest Volcanoes
                54: Tropical Rainforest Hills
                60: Temperate Rainforest Hills
                61: Temperate Rainforest Mountains
                62: Temperate Rainforest Swamp
                70: Temperate Seasonal Forest Hills
                71: Temperate Seasonal Forest Mountains
                80: Desert Terraces
                81: Desert Dunes
                82: Desert Oasis
                83: Desert Ravines
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
        self.width = spread_mask.shape[1]
        self.height = spread_mask.shape[0]
        self.noise = Noise(seed=seed, width=self.width, height=self.height)
        self.parameters = parameters
        self.global_max_height = parameters.get("global_max_height", 100) / 100
        # lowest global max height corresponds to 0.25
        self.global_max_height = 0.75 * self.global_max_height + 0.25
        self.sub_biomes = sub_biomes.Sub_Biomes(seed, self.width, self.height, x_offset, y_offset)

    def normalise(self, heightmap, low, high):
        return (heightmap - np.min(heightmap)) / (np.max(heightmap) - np.min(heightmap)) * (high - low) + low

    
    def boreal_forest_plains(self):
        lowest_height = 0.22
        boreal_forest_plains_max_height = self.parameters.get("boreal_forest").get("plains").get("max_height", 30) / 100
        boreal_forest_plains_max_height = (self.global_max_height - lowest_height) * boreal_forest_plains_max_height + lowest_height
        terrain_map = sub_biomes.flats(lowest_height, boreal_forest_plains_max_height, 1)
        return terrain_map * self.spread_mask
    
    def boreal_forest_hills(self):
        lowest_height = 0.22
        boreal_forest_hills_max_height = self.parameters.get("boreal_forest").get("hills").get("max_height", 40) / 100
        boreal_forest_hills_max_height = (self.global_max_height - lowest_height) * boreal_forest_hills_max_height + lowest_height
        terrain_map = sub_biomes.hills(lowest_height, boreal_forest_hills_max_height, 2)
        return terrain_map * self.spread_mask
    
    def boreal_forest_mountains(self):
        lowest_height = 0.22
        boreal_forest_mountains_max_height = self.parameters.get("boreal_forest").get("dla").get("max_height", 70) / 100
        boreal_forest_mountains_max_height = (self.global_max_height - lowest_height) * boreal_forest_mountains_max_height + lowest_height
        terrain_map = sub_biomes.dla_mountains(lowest_height, boreal_forest_mountains_max_height, self.binary_mask)
        return terrain_map * self.spread_mask
    
    def grassland_plains(self):
        lowest_height = 0.22
        grassland_plains_max_height = self.parameters.get("grassland").get("plains").get("max_height", 30) / 100
        grassland_plains_max_height = (self.global_max_height - lowest_height) * grassland_plains_max_height + lowest_height
        terrain_map = sub_biomes.flats(lowest_height, grassland_plains_max_height, 1)
        return terrain_map * self.spread_mask
    
    def grassland_hills(self):
        lowest_height = 0.22
        grassland_hills_max_height = self.parameters.get("grassland").get("hills").get("max_height", 40) / 100
        grassland_hills_max_height = (self.global_max_height - lowest_height) * grassland_hills_max_height + lowest_height
        terrain_map = sub_biomes.hills(lowest_height, grassland_hills_max_height, 2)
        return terrain_map * self.spread_mask
    
    def grassland_rocky_fields(self):
        lowest_height = 0.22
        grassland_rocky_fields_max_height = self.parameters.get("grassland").get("rocky_fields").get("max_height", 40) / 100
        grassland_rocky_fields_max_height = (self.global_max_height - lowest_height) * grassland_rocky_fields_max_height + lowest_height
        terrain_map = sub_biomes.rocky_field(lowest_height, grassland_rocky_fields_max_height)
        return terrain_map * self.spread_mask
    
    def grassland_terraced_fields(self):
        lowest_height = 0.22
        grassland_terraced_fields_max_height = self.parameters.get("grassland").get("terraced_fields").get("max_height", 40) / 100
        grassland_terraced_fields_max_height = (self.global_max_height - lowest_height) * grassland_terraced_fields_max_height + lowest_height
        terrain_map = sub_biomes.terraced_rice_fields(lowest_height, grassland_terraced_fields_max_height)
        return terrain_map * self.spread_mask
    
    def tundra_plains(self):
        lowest_height = 0.22
        tundra_plains_max_height = self.parameters.get("tundra").get("plains").get("max_height", 40) / 100
        tundra_plains_max_height = (self.global_max_height - lowest_height) * tundra_plains_max_height + lowest_height
        terrain_map = sub_biomes.flats(lowest_height, tundra_plains_max_height, 1)
        return terrain_map * self.spread_mask
    
    def tundra_mountains(self): #random choice between mountain type
        lowest_height = 0.22
        tundra_mountains_max_height = self.parameters.get("tundra").get("mountains").get("max_height", 100) / 100
        tundra_mountains_max_height = (self.global_max_height - lowest_height) * tundra_mountains_max_height + lowest_height
        terrain_map = sub_biomes.dla_mountains(lowest_height, tundra_mountains_max_height, self.binary_mask)
        return terrain_map * self.spread_mask
    
    def savanna_plains(self):
        lowest_height = 0.22
        savanna_plains_max_height = self.parameters.get("savanna").get("plains").get("max_height", 30) / 100
        savanna_plains_max_height = (self.global_max_height - lowest_height) * savanna_plains_max_height + lowest_height
        terrain_map = sub_biomes.flats(lowest_height, savanna_plains_max_height, 1)
        return terrain_map * self.spread_mask
    
    def savanna_mountains(self):
        lowest_height = 0.22
        savanna_mountains_max_height = self.parameters.get("savanna").get("mountains").get("max_height", 50) / 100
        savanna_mountains_max_height = (self.global_max_height - lowest_height) * savanna_mountains_max_height + lowest_height
        terrain_map = sub_biomes.dla_mountains(lowest_height, savanna_mountains_max_height, self.binary_mask)
        return terrain_map * self.spread_mask
    
    def woodland_hills(self):
        lowest_height = 0.22
        woodland_hills_max_height = self.parameters.get("woodland").get("hills").get("max_height", 40) / 100
        woodland_hills_max_height = (self.global_max_height - lowest_height) * woodland_hills_max_height + lowest_height
        terrain_map = sub_biomes.hills(lowest_height, woodland_hills_max_height, 2)
        return terrain_map * self.spread_mask
    
    def tropical_rainforest_flats(self):
        lowest_height = 0.22
        tropical_rainforest_flats_max_height = self.parameters.get("tropical_rainforest").get("flats").get("max_height", 40) / 100
        tropical_rainforest_flats_max_height = (self.global_max_height - lowest_height) * tropical_rainforest_flats_max_height + lowest_height
        terrain_map = sub_biomes.flats(lowest_height, tropical_rainforest_flats_max_height, 1)
        return terrain_map * self.spread_mask
    
    def tropical_rainforest_mountains(self):
        lowest_height = 0.22
        tropical_rainforest_mountains_max_height = self.parameters.get("tropical_rainforest").get("mountains").get("max_height", 80) / 100
        tropical_rainforest_mountains_max_height = (self.global_max_height - lowest_height) * tropical_rainforest_mountains_max_height + lowest_height
        terrain_map = sub_biomes.dla_mountains(lowest_height, tropical_rainforest_mountains_max_height, self.binary_mask)
        return terrain_map * self.spread_mask
    
    def tropical_rainforest_hills(self):
        lowest_height = 0.22
        tropical_rainforest_hills_max_height = self.parameters.get("tropical_rainforest").get("hills").get("max_height", 50) / 100
        tropical_rainforest_hills_max_height = (self.global_max_height - lowest_height) * tropical_rainforest_hills_max_height + lowest_height
        terrain_map = sub_biomes.hills(lowest_height, tropical_rainforest_hills_max_height, 2)
        return terrain_map * self.spread_mask
    
    def tropical_rainforest_volcanoes(self):
        lowest_height = 0.22
        tropical_rainforest_volcanoes_max_height = self.parameters.get("tropical_rainforest").get("volcanoes").get("max_height", 60) / 100
        tropical_rainforest_volcanoes_max_height = (self.global_max_height - lowest_height) * tropical_rainforest_volcanoes_max_height + lowest_height
        terrain_map = sub_biomes.dla_mountains(lowest_height, tropical_rainforest_volcanoes_max_height, self.binary_mask)
        return terrain_map * self.spread_mask
    
    def temperate_rainforest_hills(self):
        lowest_height = 0.22
        temperate_rainforest_hills_max_height = self.parameters.get("temperate_rainforest").get("hills").get("max_height", 40) / 100
        temperate_rainforest_hills_max_height = (self.global_max_height - lowest_height) * temperate_rainforest_hills_max_height + lowest_height
        terrain_map = sub_biomes.hills(lowest_height, temperate_rainforest_hills_max_height, 2)
        return terrain_map * self.spread_mask
    
    def temperate_rainforest_mountains(self):
        lowest_height = 0.22
        temperate_rainforest_mountains_max_height = self.parameters.get("temperate_rainforest").get("mountains").get("max_height", 80) / 100
        temperate_rainforest_mountains_max_height = (self.global_max_height - lowest_height) * temperate_rainforest_mountains_max_height + lowest_height
        terrain_map = sub_biomes.dla_mountains(lowest_height, temperate_rainforest_mountains_max_height, self.binary_mask)
        return terrain_map * self.spread_mask
    
    def temperate_rainforest_swamp(self):
        lowest_height = 0.17
        temperate_rainforest_swamp_max_height = self.parameters.get("temperate_rainforest").get("swamp").get("max_height", 30) / 100
        temperate_rainforest_swamp_max_height = (self.global_max_height - lowest_height) * temperate_rainforest_swamp_max_height + lowest_height
        terrain_map = sub_biomes.hills(0.22, temperate_rainforest_swamp_max_height, 2)
        return terrain_map * self.spread_mask
    
    def temperate_seasonal_forest_hills(self):
        lowest_height = 0.22
        temperate_seasonal_forest_hills_max_height = self.parameters.get("temperate_seasonal_forest").get("hills").get("max_height", 40) / 100
        temperate_seasonal_forest_hills_max_height = (self.global_max_height - lowest_height) * temperate_seasonal_forest_hills_max_height + lowest_height
        terrain_map = sub_biomes.hills(lowest_height, temperate_seasonal_forest_hills_max_height, 2)
        return terrain_map * self.spread_mask
    
    def temperate_seasonal_forest_mountains(self):
        lowest_height = 0.22
        temperate_seasonal_forest_mountains_max_height = self.parameters.get("temperate_seasonal_forest").get("mountains").get("max_height", 80) / 100
        temperate_seasonal_forest_mountains_max_height = (self.global_max_height - lowest_height) * temperate_seasonal_forest_mountains_max_height + lowest_height
        terrain_map = sub_biomes.dla_mountains(lowest_height, temperate_seasonal_forest_mountains_max_height, self.binary_mask)
        return terrain_map * self.spread_mask
    
    def desert_dunes(self):
        lowest_height = 0.22
        desert_dunes_max_height = self.parameters.get("subtropical_desert").get("dunes").get("max_height", 30) / 100
        desert_dunes_max_height = (self.global_max_height - lowest_height) * desert_dunes_max_height + lowest_height
        terrain_map = sub_biomes.dunes(lowest_height, desert_dunes_max_height)
        return terrain_map * self.spread_mask
    
    def desert_terraces(self):
        lowest_height = 0.22
        desert_mesas_max_height = self.parameters.get("subtropical_desert").get("mesas").get("max_height", 40) / 100
        desert_mesas_max_height = (self.global_max_height - lowest_height) * desert_mesas_max_height + lowest_height
        terrain_map = sub_biomes.mesa_terraces(lowest_height, desert_mesas_max_height)
        return terrain_map * self.spread_mask
    
    def desert_ravines(self):
        lowest_height = 0.22
        desert_ravines_max_height = self.parameters.get("subtropical_desert").get("ravines").get("max_height", 40) / 100
        desert_ravines_max_height = (self.global_max_height - lowest_height) * desert_ravines_max_height + lowest_height
        terrain_map = sub_biomes.ravines(lowest_height, desert_ravines_max_height)
        return terrain_map * self.spread_mask
    
    def desert_oasis(self):
        lowest_height = 0.15
        desert_oasis_max_height = self.parameters.get("subtropical_desert").get("oasis").get("max_height", 30) / 100
        desert_oasis_max_height = (self.global_max_height - lowest_height) * desert_oasis_max_height + lowest_height
        terrain_map = sub_biomes.oasis(lowest_height, desert_oasis_max_height)
        return terrain_map * self.spread_mask
    
    def ocean_seabed(self):
        lowest_height = 0
        ocean_seabed_max_height = self.parameters.get("ocean").get("seabed").get("max_height", 50) / 100
        ocean_seabed_max_height = ocean_seabed_max_height * 0.2
        terrain_map = sub_biomes.flats(lowest_height, ocean_seabed_max_height)
        return terrain_map * self.spread_mask
    
    def ocean_trenches(self):
        terrain_map = sub_biomes.trenches()
        return terrain_map * self.spread_mask
    
    def ocean_volcanic_islands(self):
        lowest_height = 0
        ocean_volcanic_islands_max_height = self.parameters.get("ocean").get("volcanic_islands").get("max_height", 20) / 100
        ocean_volcanic_islands_max_height = (self.global_max_height - 0.2) * ocean_volcanic_islands_max_height + 0.2
        terrain_map = sub_biomes.volcanoes(lowest_height, ocean_volcanic_islands_max_height, self.binary_mask)
        return terrain_map * self.spread_mask
    
    def ocean_water_stacks(self):
        lowest_height = 0
        ocean_water_stacks_max_height = self.parameters.get("ocean").get("water_stacks").get("max_height", 20) / 100
        ocean_water_stacks_max_height = (self.global_max_height - 0.2) * ocean_water_stacks_max_height + 0.2
        terrain_map = sub_biomes.water_stacks(lowest_height, ocean_water_stacks_max_height)
        return terrain_map * self.spread_mask

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
            case 0:
                return self.boreal_forest_plains()
            case 1:
                return self.boreal_forest_hills()
            case 2:
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
                return self.tundra_mountains()
            case 30:
                return self.savanna_plains()
            case 31:
                return self.savanna_mountains()
            case 40:
                return self.woodland_hills()
            case 50:
                return self.tropical_rainforest_flats()
            case 51:
                return self.tropical_rainforest_mountains()
            case 52:
                return self.tropical_rainforest_volcanoes()
            case 54:
                return self.tropical_rainforest_hills()
            case 60:
                return self.temperate_rainforest_hills()
            case 61:
                return self.temperate_rainforest_mountains()
            case 62:
                return self.temperate_rainforest_swamp()
            case 70:
                return self.temperate_seasonal_forest_hills()
            case 71:
                return self.temperate_seasonal_forest_mountains()
            case 80:
                return self.desert_terraces()
            case 81:
                return self.desert_dunes()
            case 82:
                return self.desert_oasis()
            case 83:
                return self.desert_ravines()
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


