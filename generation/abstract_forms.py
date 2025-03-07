import numpy as np
from noise import snoise2
from .tools import Tools
from .simp_noise import Noise
from .display import Display

width = 1024
height = 1024
noise = Noise(seed=42, width=1024, height=1024)

def volcanoe(density=7, radius=0.3, tau=20, c=0.01):
        worley = noise.worley_noise(density=density, k=1, p=2, distribution="poisson", radius=radius)
        sinusoidal_worley = ((worley.max() - worley)**tau)*np.sin(worley*c)
        return Tools.normalize(sinusoidal_worley, a=0, b=1)

def terrace(x, num_terraces=5, steepness=3):
        x = Tools.normalize(x, a=0, b=num_terraces)
        return Tools.normalize((np.round(x) + np.sign(x-np.round(x))*0.5*(np.abs(2*(x-np.round(x))))**steepness), 0, 1)

def dune(x, p, xm):
    s = np.where((x > 0) & (x < xm), 0.0, 1.0)
    ps1 = p * s + 1.0
    part1 = ps1 * 0.5
    part2 = 1 - np.cos((np.pi / ps1) * ((x - s) / (xm - s)))
    return (part1 * part2) - 1

def generate_dunes(self, frequency=10, noise_scale=5.0, noise_strength=200.0, rotation=-np.pi/4, amplitude=1.5, gap=100):
    # https://www.florisgroen.com/creating-sandy-desert-first-try/
    x = np.arange(width)
    y = np.arange(height)
    X, Y = np.meshgrid(x, y)
    noise_values = np.vectorize(lambda x, y: snoise2(x, y))(noise_scale * X / self.width, noise_scale * Y / self.height)
    shift = noise_strength * noise_values  

    xb = X * np.cos(rotation) - Y * np.sin(rotation)
    xa = (frequency * xb + shift) % (width + gap)
    xa = np.clip(xa, 0, width)
    normalized_xa = xa / width

    heightmap = amplitude * self.dune(normalized_xa, 1, 0.72)
    
    return heightmap


### Volcanoes:
# sinusoidal_worley = volcanoe(density=5, radius=0.4, tau=18, c=0.01)
# simplex = Tools.normalize(noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=256, octaves=10, persistence=0.5, lacunarity=2.0))
# heightmap = Tools.blend(sinusoidal_worley, simplex, alpha=0.3)

### Terraces:
# simplex = Tools.normalize(noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=512, octaves=10, persistence=0.42, lacunarity=2.0))
# heightmap = terrace(simplex, num_terraces=10, steepness=3)

### Dunes:
# dunes1 = Tools.normalize(generate_dunes(frequency=3, noise_scale=2.0, noise_strength=200.0, rotation=-np.pi/4, amplitude=1, gap=3))
# dunes2 = Tools.normalize(generate_dunes(frequency=6, noise_scale=2.0, noise_strength=500.0, rotation=-np.pi/4, amplitude=1, gap=0.5))
# dunes3 = Tools.normalize(generate_dunes(frequency=20, noise_scale=8.0, noise_strength=100.0, rotation=-np.pi/4, amplitude=1, gap=1))
# heightmap = Tools.normalize(1.3*dunes1 + 0.2*dunes2 + 0.05*dunes3, 0, 0.5)

# phasor_noise = Tools.normalize(noise.phasor_noise(num_phasors=20, freq_range=(30, 40), amplitude=1.0, direction_bias=np.pi/4, anisotropy=1))
# phasor_noise = Tools.normalize(Tools.sawtooth(phasor_noise), 0, 0.007)
# heightmap = heightmap + phasor_noise

### Mountains
# worley = (1-Tools.normalize(noise.worley_noise(density=50, k=1, p=2)))**2
# simplex = Tools.normalize(noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=512, octaves=10, persistence=0.5, lacunarity=2.0))
# heightmap = Tools.normalize(Tools.blend(simplex, worley, alpha=0.5), a=0, b=1)

# warpx = noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=128, octaves=2, persistence=0.5, lacunarity=2.0)
# warpy = noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=128, octaves=2, persistence=0.5, lacunarity=2.0)
# heightmap = Tools.domain_warp(heightmap, warpx, warpy, warp_strength=20)

### Sheer Mountains
# worley1 = Tools.normalize(noise.worley_noise(seed=1, density=30, k=1, p=1), -1, 1)
# worley2 = Tools.normalize(noise.worley_noise(seed=2, density=30, k=1, p=1), -1, 1)

# heightmap = Tools.normalize(
#       noise.uber_noise(width=1024, height=1024, scale=256, octaves=10, persistence=0.5, lacunarity=2.0,
#                        sharpness=0.5, feature_amp=1, altitude_erosion=0.15, slope_erosion=0.9, ridge_erosion=0,
#                        warp_x=worley1, warp_y=worley2, warp_strength=70)
# )

### Worley Warped DLA
# from cellular_automata.scaling_heightmap import ca_in_mask
# binary_polygon = np.ones((1024, 1024))
# binary_polygon[:, 0], binary_polygon[:, -1], binary_polygon[0, :], binary_polygon[-1, :] = 0,0,0,0
# ca_map = Tools.normalize(ca_in_mask(40, binary_polygon), 0, 1)

# # from skimage.filters.rank import entropy
# # from skimage.morphology import disk
# # from skimage.util import img_as_ubyte
# # entropy_map = Tools.normalize(entropy(img_as_ubyte(ca_map), disk(10))) 
# # simplex = Tools.normalize(noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=512, octaves=10, persistence=0.5, lacunarity=2.0))
# # ca_map = ca_map + simplex*entropy_map*0.4
# # ca_map = Tools.normalize(ca_map, 0, 1)**2

# worley1 = Tools.normalize(noise.worley_noise(seed=1, density=100, k=1, p=2), -1, 1)
# worley2 = Tools.normalize(noise.worley_noise(seed=2, density=100, k=1, p=2), -1, 1)

# warped = Tools.domain_warp(ca_map, worley1, worley2, warp_strength=40)




# display = Display(height_array=warped, height_scale=250, colormap="winter")
# display.display_heightmap()
#display.save_heightmap("generation/hold/warped.jpg")