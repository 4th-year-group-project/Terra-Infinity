import numpy as np
from noise.parallel import simplex_fractal_noise, open_simplex_fractal_noise, snoise_fractal_noise

class SimplexNoise:
    def __init__(self, seed=42, width=512, height=512, scale=100.0, octaves=6, persistence=0.5, lacunarity=2.0):
        np.random.seed(seed)

        self.seed = seed
        self.width = width
        self.height = height   
        self.scale = scale
        self.ocataves = octaves
        self.persistence = persistence
        self.lacunarity = lacunarity

        self.perm = self.grid_generate()

    def grid_generate(self):
        return np.random.permutation(256)
    
    def fractal_noise(self, noise="simplex"):
        if noise == "simplex":
            return simplex_fractal_noise(self.perm, self.width, self.height, self.scale, self.ocataves, self.persistence, self.lacunarity)
        elif noise == "open":
            return open_simplex_fractal_noise(self.perm, self.width, self.height, self.scale, self.ocataves, self.persistence, self.lacunarity)
        elif noise == "snoise":
            return snoise_fractal_noise(self.width, self.height, self.scale, self.ocataves, self.persistence, self.lacunarity)