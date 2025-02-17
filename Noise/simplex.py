import numpy as np
from Noise.parallel import simplex_fractal_noise, open_simplex_fractal_noise, snoise_fractal_noise
import matplotlib.pyplot as plt

class SimplexNoise:
    def __init__(self, seed=42, width=512, height=512, scale=100.0, octaves=6, persistence=0.5, lacunarity=2.0):
        
        self.seed = seed
        self.width = width
        self.height = height   
        self.scale = scale
        self.ocataves = octaves
        self.persistence = persistence
        self.lacunarity = lacunarity

        # self.perm = self.grid_generate()

    # def grid_generate(self):
        
    #     return np.random.permutation(256)
    
    def fractal_noise(self, noise="simplex", x_offset=0, y_offset=0, reason=""):
        if noise == "simplex":
            return simplex_fractal_noise(self.perm, self.width, self.height, self.scale, self.ocataves, self.persistence, self.lacunarity)
        elif noise == "open":
            np.random.seed(10)
            rng = np.random.RandomState(10)  # Independent RNG instance
            perm = rng.permutation(256)
            # print("Offsets: ", x_offset, y_offset, perm[0], perm[100], self.width, self.height,  " | Reason: ", reason)
            return open_simplex_fractal_noise(perm, self.width, self.height, self.scale, self.ocataves, self.persistence, self.lacunarity, x_offset, y_offset)
        elif noise == "snoise":
            return snoise_fractal_noise(self.width, self.height, self.scale, self.ocataves, self.persistence, self.lacunarity)
        
# if __name__ == "__main__":
#     noise = SimplexNoise(seed=5, width=512, height=512, scale=100, octaves=8, persistence=0.5, lacunarity=2)
#     noise_map = noise.fractal_noise(noise="open", x_offset=49484, y_offset=480)
#     noise_map = (noise_map + 1) / 2
#     plt.imshow(noise_map)
#     plt.title('Noise Map')
#     plt.show()

#     noise = SimplexNoise(seed=5, width=522, height=522, scale=100, octaves=8, persistence=0.5, lacunarity=2)
#     noise_map = noise.fractal_noise(noise="open", x_offset=49484, y_offset=480)
#     noise_map = (noise_map + 1) / 2
#     plt.imshow(noise_map)
#     plt.title('Noise Map')
#     plt.show()
