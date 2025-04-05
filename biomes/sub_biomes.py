from generation import Noise
from scipy.ndimage import gaussian_filter
import numpy as np
from skimage.filters.rank import entropy
from skimage.morphology import disk
from cellular_automata.scaling_heightmap import ca_in_mask
import numpy as np
from generation import Display
from generation import tools
class Sub_Biomes:
    def __init__(self, seed, width, height, x_offset, y_offset):
        self.seed = seed
        self.noise = Noise(seed=seed, width=width, height=height)
        self.x_offset = x_offset
        self.y_offset = y_offset
        self.width = width
        self.height = height

    def normalise(self, heightmap, low, high):
        return (heightmap - np.min(heightmap)) / (np.max(heightmap) - np.min(heightmap)) * (high - low) + low

    def flats(self, min_height, max_height, variation=1):
        base_noise = self.noise.fractal_simplex_noise(seed=self.seed, width=self.width, height=self.height, x_offset=self.x_offset, y_offset=self.y_offset, scale=1024, octaves=3, persistence=0.4, lacunarity=2.0)
        #base_noise = self.noise.warped_simplex_noise(seed=self.seed, width=self.width, height=self.height, x_offset=self.x_offset, y_offset=self.y_offset, scale=512, octaves=8, persistence=0.45, lacunarity=2.0, warp_x=0, warp_y=0)
        #base_noise = self.noise.billow_noise(seed=self.seed, width=self.width, height=self.height, x_offset=self.x_offset, y_offset=self.y_offset, scale=512, octaves=8, persistence=0.45, lacunarity=2.0)
        base_noise = self.normalise(base_noise, 0, 1)

        base_noise = base_noise**variation
        texture_noise = self.noise.fractal_simplex_noise(seed=self.seed, width=self.width, height=self.height, x_offset=self.x_offset, y_offset=self.y_offset, scale=1024, octaves=8, persistence=0.5, lacunarity=2.0)
        texture_noise = self.normalise(texture_noise, 0, 1)
        add_noise = base_noise + texture_noise
        #base_noise =(base_noise - np.min(base_noise)) / (np.max(base_noise) - np.min(base_noise)) * (0.5 - 0.3) + 0.3
    # base_noise = normalize(base_noise, 0, 1)
        add_noise = self.normalise(add_noise, min_height, max_height)
        return add_noise

    # def hills(self, x_offset, y_offset, min_height, max_height):
    #     base_noise = noise.fractal_simplex_noise(seed=seed, width=width, height=height, x_offset=self.x_offset, y_offset=self.y_offset, scale=1024, octaves=2, persistence=0.45, lacunarity=2.0)
    #     base_noise = normalize(base_noise, 0.2, 1)
    #     hills = noise.fractal_simplex_noise(seed=seed, width=width, height=height, x_offset=self.x_offset, y_offset=self.y_offset, scale=1024, octaves=8, persistence=0.5, lacunarity=2.0)
    #     hills = normalize(hills, 0, 1)
    #     noise_map = base_noise + hills*0.3
    #     noise_map = normalize(noise_map, 0, 1)

    def hills(self, min_height, max_height, ruggedness=2):
        base_noise = self.noise.billow_noise(seed=self.seed, width=self.width, height=self.height, x_offset=self.x_offset, y_offset=self.y_offset, scale=1024, octaves=ruggedness, persistence=0.5, lacunarity=2.0)
        texture_noise = self.noise.fractal_simplex_noise(seed=self.seed, width=self.width, height=self.height, x_offset=self.x_offset, y_offset=self.y_offset, scale=1024, octaves=8, persistence=0.5, lacunarity=2.0)
        texture_noise = self.normalise(texture_noise, 0, 1)
        base_noise = self.normalise(base_noise, 0, 1)
        base_noise = tools.smooth(base_noise)
        add_noise = base_noise + texture_noise
        add_noise = self.normalise(add_noise, min_height, max_height)
        return add_noise


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

#...