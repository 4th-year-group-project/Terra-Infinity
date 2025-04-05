from generation import Noise

class Sub_Biomes:
    def __init__(self, seed, width, height):
        self.seed = seed
        self.noise = Noise(seed=seed, width=width, height=height)

    def flats(self):
        pass

    def dla_mountains(self):
        pass    

    def volcanoes(self):
        pass

#...