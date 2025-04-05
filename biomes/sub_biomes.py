from generation import Noise
from scipy.ndimage import gaussian_filter
import numpy as np
from skimage.filters.rank import entropy
from skimage.morphology import disk
from cellular_automata.scaling_heightmap import ca_in_mask

class Sub_Biomes:
    def __init__(self, seed, width, height, x_offset, y_offset):
        self.seed = seed
        self.noise = Noise(seed=seed, width=width, height=height)
        self.x_offset = x_offset
        self.y_offset = y_offset

    def normalise(self, heightmap, low, high):
        return (heightmap - np.min(heightmap)) / (np.max(heightmap) - np.min(heightmap)) * (high - low) + low

    def flats(self):
        pass

    def dla_mountains(self, min_height, max_height, binary_mask):
        #Generate base DLA mountains
        heightmap = ca_in_mask(self.seed, binary_mask, iterations=10)
        heightmap_to_entropy = self.normalise(heightmap, 0, 1)
        image_entropy = entropy(heightmap_to_entropy, disk(5))
        image_entropy = self.normalise(image_entropy, 0, 1)
        inverted_image_entropy = 1 - image_entropy
        inverted_image_entropy = gaussian_filter(inverted_image_entropy, sigma=6)

        #Make them less smooth by adding low amplitude high frequency noise
        noise_to_add = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=30, octaves=4, persistence=0.5, lacunarity=2, start_freq=9)
        noise_to_add = self.normalise(noise_to_add, 0, 1)
        noise_overlay_scale = 0.028
        heightmap = heightmap + (noise_to_add*noise_overlay_scale*image_entropy)
        heightmap = self.normalise(heightmap, 0, 1)
        
        #Bring out peaks (parameterize this)
        heightmap = heightmap**2

        #Add some noise where the mountains did not reach
        negative_space_noise = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=100, octaves=8, persistence=0.5, lacunarity=2)
        negative_space_noise = self.normalise(negative_space_noise, 0, 1)
        heightmap = heightmap + (negative_space_noise*0.2*inverted_image_entropy)

        #Add some low frequency noise to the mountains for less peak height uniformity
        perturbing_noise = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                    scale=200, octaves=1, persistence=0.5, lacunarity=2)

        heightmap = heightmap + perturbing_noise*0.3
        heightmap = self.normalise(heightmap, min_height, max_height)

        return heightmap

    def volcanoes(self):
        pass

    def swamp(self, min_height, max_height):
        noise_map = self.noise.fractal_simplex_noise(noise="open", x_offset=self.x_offset, y_offset=self.y_offset,
                                                     scale=100, octaves=8, persistence=0.5, lacunarity=2)
        noise_map = self.normalise(noise_map, min_height, max_height)
        return noise_map


#...