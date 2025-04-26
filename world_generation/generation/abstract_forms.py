import cv2
import numpy as np

from .simp_noise import Noise
from .tools import blend, domain_warp, high_smooth, low_smooth, normalize, sawtooth

width = 1024
height = 1024
noise = Noise(seed=42, width=1024, height=1024)

def volcanoe(density=7, radius=0.3, tau=20, c=0.01):
        worley = noise.worley_noise(density=density, k=1, p=2, distribution="poisson", radius=radius)
        sinusoidal_worley = ((worley.max() - worley)**tau)*np.sin(worley*c)
        return normalize(sinusoidal_worley, a=0, b=1)

def terrace(x, num_terraces=5, steepness=3):
        x = normalize(x, a=0, b=num_terraces)
        return normalize((np.round(x) + np.sign(x-np.round(x))*0.5*(np.abs(2*(x-np.round(x))))**steepness), 0, 1)

def dune(x, p, xm):
    s = np.where((x > 0) & (x < xm), 0.0, 1.0)
    ps1 = p * s + 1.0
    part1 = ps1 * 0.5
    part2 = 1 - np.cos((np.pi / ps1) * ((x - s) / (xm - s)))
    return (part1 * part2) - 1

def generate_dunes(frequency=10, noise_scale=5.0, noise_strength=200.0, rotation=-np.pi/4, amplitude=1.5, gap=100):
    # https://www.florisgroen.com/creating-sandy-desert-first-try/
    x = np.arange(width)
    y = np.arange(height)
    X, Y = np.meshgrid(x, y)
    noise_values = noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=width/noise_scale, octaves=1, persistence=0.5, lacunarity=2.0)
    shift = noise_strength * noise_values

    xb = X * np.cos(rotation) - Y * np.sin(rotation)
    xa = (frequency * xb + shift) % (width + gap)
    xa = np.clip(xa, 0, width)
    normalized_xa = xa / width

    heightmap = amplitude * dune(normalized_xa, 1, 0.72)

    return heightmap


### Volcanoes:
def volcanoes():
    sinusoidal_worley = volcanoe(density=5, radius=0.4, tau=18, c=0.01)
    simplex = normalize(noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=256, octaves=10, persistence=0.5, lacunarity=2.0))
    heightmap = blend(sinusoidal_worley, simplex, alpha=0.3)
    return heightmap

### Terraces:
def terraces():
    simplex = normalize(noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=512, octaves=10, persistence=0.42, lacunarity=2.0))
    heightmap = terrace(simplex, num_terraces=10, steepness=3)
    return heightmap

### Dunes:
def phasor_dunes():
    dunes1 = normalize(generate_dunes(frequency=3, noise_scale=2.0, noise_strength=200.0, rotation=-np.pi/4, amplitude=1, gap=3))
    dunes2 = normalize(generate_dunes(frequency=6, noise_scale=2.0, noise_strength=500.0, rotation=-np.pi/4, amplitude=1, gap=0.5))
    dunes3 = normalize(generate_dunes(frequency=20, noise_scale=8.0, noise_strength=100.0, rotation=-np.pi/4, amplitude=1, gap=1))
    heightmap = normalize(1.3*dunes1 + 0.2*dunes2 + 0.05*dunes3, 0, 0.5)

    phasor_noise = normalize(noise.phasor_noise(num_phasors=20, freq_range=(30, 40), amplitude=1.0, direction_bias=np.pi/4, anisotropy=1))
    phasor_noise = normalize(sawtooth(phasor_noise), 0, 0.007)
    heightmap = heightmap + phasor_noise
    return heightmap

### Mountains
def pointy_mountains():
    worley = (1-normalize(noise.worley_noise(density=50, k=1, p=2)))**2
    simplex = normalize(noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=512, octaves=10, persistence=0.5, lacunarity=2.0))
    heightmap = normalize(blend(simplex, worley, alpha=0.5), a=0, b=1)

    warpx = noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=128, octaves=2, persistence=0.5, lacunarity=2.0)
    warpy = noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=128, octaves=2, persistence=0.5, lacunarity=2.0)
    heightmap = domain_warp(heightmap, warpx, warpy, warp_strength=20)
    return heightmap

### Sheer Mountains
def sheer_mountains():
    worley1 = normalize(noise.worley_noise(seed=1, density=60, k=1, p=1), -1, 1)
    worley2 = normalize(noise.worley_noise(seed=2, density=60, k=1, p=1), -1, 1)

    heightmap = normalize(
        noise.uber_noise(width=1024, height=1024, scale=256, octaves=10, persistence=0.5, lacunarity=2.0,
                        sharpness=0.3, feature_amp=1, altitude_erosion=0.05, slope_erosion=0.9, ridge_erosion=0,
                        warp_x=worley1, warp_y=worley2, warp_strength=42),
    )
    return heightmap

### Worley Warped DLA
def worley_warped_dla():
    from cellular_automata.scaling_heightmap import ca_in_mask
    binary_polygon = np.ones((1024, 1024))
    binary_polygon[:, 0], binary_polygon[:, -1], binary_polygon[0, :], binary_polygon[-1, :] = 0,0,0,0
    ca_map = normalize(ca_in_mask(40, binary_polygon), 0, 1)

    # from skimage.filters.rank import entropy
    # from skimage.morphology import disk
    # from skimage.util import img_as_ubyte
    # entropy_map = normalize(entropy(img_as_ubyte(ca_map), disk(10)))
    # simplex = normalize(noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=512, octaves=10, persistence=0.5, lacunarity=2.0))
    # ca_map = ca_map + simplex*entropy_map*0.4
    # ca_map = normalize(ca_map, 0, 1)**2

    worley1 = normalize(noise.worley_noise(seed=1, density=100, k=1, p=2), -1, 1)
    worley2 = normalize(noise.worley_noise(seed=2, density=100, k=1, p=2), -1, 1)

    warped = domain_warp(ca_map, worley1, worley2, warp_strength=40)
    return warped

### Ocean Trenches and Ravines
def ocean_trenches():
    simplex = normalize(
            noise.uber_noise(width=1024, height=1024,
                            scale=128, octaves=10, persistence=0.5, lacunarity=2.0,
                            sharpness=0, feature_amp=1,
                            altitude_erosion=0, slope_erosion=0.9, ridge_erosion=0.2),
    -1, 1)

    heightmap = normalize(1-np.abs(simplex))**2
    heightmap = high_smooth(low_smooth(normalize(1-normalize(heightmap, 0, 1)), a=20, b=0.15), a=15, b=0.9)
    heightmap = normalize(heightmap, 0, 0.2)
    return heightmap

### Terraced plains (INCOMPLETE)
def terraced_plains():
    simplex = normalize(
        noise.fractal_simplex_noise(
                noise="simplex", x_offset=0, y_offset=0,
                scale=1024, octaves=2, persistence=0.4, lacunarity=1.5,
        ),
    )
    heightmap = normalize(terrace(simplex, num_terraces=10, steepness=3))

    add = normalize(
            noise.fractal_simplex_noise(
                    noise="simplex", x_offset=0, y_offset=0,
                    scale=512, octaves=5, persistence=0.5, lacunarity=2.0,
            ),
    )

    heightmap = normalize(
        0.5*heightmap + 0.1*add,
    0.2, 0.7)
    return heightmap

### Angular Worley (INCOMPLETE)
def angular_worley():
    dist_worley, angular_worley = noise.angular_noise(seed=10, density=10, k=1, p=2)
    from noise import pnoise2
    perlin_noise = np.array([[pnoise2(x / 50.0, y / 50.0, octaves=4) for x in range(width)] for y in range(height)])
    angular_worley = angular_worley + 0.5 * perlin_noise  # Adjust 0.3 for more/less perturbation

    dist_worley = normalize(dist_worley, 0.2, 1)
    angular_worley = (normalize(np.sin(10*angular_worley),0,1))

    angular_worley = np.sin(angular_worley + 0.2 * perlin_noise)
    return angular_worley

### Pillar Structure (INCOMPLETE)
def natural_pillars():
    from scipy.ndimage import distance_transform_edt

    mask = np.zeros((height, width), dtype=np.uint8)
    cv2.circle(mask, (512, 512), 120, 1, thickness=-1)
    edt_mask = normalize(distance_transform_edt(mask))**1.2

    simplex = normalize(
            noise.fractal_simplex_noise(
                    noise="simplex", x_offset=0, y_offset=0,
                    scale=1024, octaves=10, persistence=0.5, lacunarity=2,
            ),
    )
    heightmap = normalize(high_smooth(normalize(edt_mask*simplex), a=15, b=0.1))
    return heightmap

### Spiral Structure (INCOMPLETE)
def spiral_structures():
    dist_worley, angular_worley = noise.angular_noise(seed=10, density=10, k=1, p=2, distribution="poisson", radius=0.3)
    spiral = normalize(np.sin(0.3*dist_worley + 1*angular_worley))
    return spiral

### Fields + Rocks (INCOMPLETE)
def rocky_field():
    """High Scale, Low Octave Noise + Low Scale, High Octave Noise extremas masked.

    EDT Mask
    """
    base_noise = noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=1024, octaves=2, persistence=0.5, lacunarity=2.0)
    base_noise = normalize(base_noise, 0, 1)

    rocky_noise = noise.fractal_simplex_noise(seed=100, noise="simplex", x_offset=0, y_offset=0, scale=512, octaves=10, persistence=0.5, lacunarity=2.2)
    rocky_noise = np.abs(normalize(rocky_noise, -1, 1))

    texture_noise = noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=128, octaves=4, persistence=0.5, lacunarity=2.0)
    texture_noise = normalize(texture_noise, 0, 1)

    rocky_noise[rocky_noise < 0.5] = 0.5
    rocky_noise = normalize(rocky_noise, 0, 1)
    heightmap = base_noise*0.4 + rocky_noise*0.5 + texture_noise*0.01
    heightmap = normalize(heightmap, 0, 0.3)

    return heightmap

# If you want to introduce variety inside of a a single biome,
# Use noise to vary something like scale

def billowed_hills():
    base_noise = noise.fractal_simplex_noise(x_offset=0, y_offset=0, scale=1048, octaves=2, persistence=0.5, lacunarity=2.0)
    base_noise = normalize(base_noise, 0, 1)

    dy, dx = np.gradient(base_noise)
    dy = normalize(dy, -1, 1)*5
    dx = normalize(dx, -1, 1)*5

    freq = 10
    x = np.linspace(0, freq * np.pi, width)
    y = np.linspace(0, freq * np.pi, height)
    X, Y = np.meshgrid(x, y)

    dir_angle = -np.pi/5
    dir_x = np.cos(dir_angle)
    dir_y = np.sin(dir_angle)

    X = X + dx
    Y = Y + dy

    billow_noise = np.abs(np.sin(X*dir_x + Y*dir_y))
    billow_noise = normalize(billow_noise, 0, 0.05)

    texture_noise = noise.fractal_simplex_noise(x_offset=0, y_offset=0, scale=128, octaves=4, persistence=0.5, lacunarity=2.0)
    texture_noise = normalize(texture_noise, 0, 0.01)

    heightmap = base_noise + billow_noise + texture_noise
    heightmap = normalize(heightmap, 0, 0.5)

# def beaches():
#     x, y = np.meshgrid(np.arange(width), np.arange(height))
#     angle = np.pi/4
#     target_x, target_y = width//2 + 1024*np.cos(angle), height//2 + 1024*np.sin(angle)
#     dx = x - target_x
#     dy = y - target_y
#     mag = dx**2 + dy**2
#     spread_mask = normalize((mag / mag.max())**0.5, 0, 1)

def cove():
     """"Flat masked region + low scale billow uber noise around it"""

def dla_canyons():
     """Rectangular mask + dla"""

# noise_map = noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=512, octaves=3, persistence=0.5, lacunarity=2.0)
# heightmap = np.abs(normalize(noise_map, -1, 1))
# heightmap = terrace(heightmap, num_terraces=3, steepness=3)
# heightmap = normalize(heightmap, 0.5, 1)

# noise_map2 = noise.fractal_simplex_noise(noise="simplex", x_offset=0, y_offset=0, scale=256, octaves=10, persistence=0.5, lacunarity=2.0)
# noise_map2 = normalize(noise_map2, 0, 1)
# heightmap = heightmap*noise_map2
# heightmap = normalize(heightmap, 0, 1)



# heightmap = 0.3*normalize(generate_dunes(frequency=3, noise_scale=2.0, noise_strength=200.0, rotation=-np.pi/4, amplitude=1, gap=3))

# display = Display(height_array=heightmap, height_scale=250, colormap="hot_desert")
# display.display_heightmap()
# # display.save_heightmap("billowy_hills.png")

