import numpy as np
from scipy.spatial import cKDTree
from scipy.stats import qmc

from .parallel import (
    open_simplex_fractal_noise,
    simplex_fractal_noise,
    snoise_fractal_noise,
    uber_noise,
    warped_open_simplex_fractal_noise,
    warped_uber_noise,
    point_open_simplex_fractal_noise,
    batch_open_simplex_fractal_noise,
)
from .tools import *


class Noise:
    def __init__(self, seed, width=1024, height=1024, x_offset=0, y_offset=0):
        self.seed = seed
        self.width = width
        self.height = height
        self.x_offset = x_offset
        self.y_offset = y_offset

    def fractal_simplex_noise(self, noise="simplex", x_offset=0, y_offset=0,
                              scale=100, octaves=7, persistence=0.5, lacunarity=2.0, start_freq=1,
                              height=None, width=None, seed=None):
        height = self.height if height is None else height
        width = self.width if width is None else width
        x_offset = self.x_offset if x_offset is None else x_offset
        y_offset = self.y_offset if y_offset is None else y_offset
        seed = self.seed if seed is None else seed


        rng = np.random.RandomState(seed)
        perm = rng.permutation(256)
        if noise == "simplex":
            return simplex_fractal_noise(perm, width, height, scale, octaves, persistence, lacunarity, x_offset, y_offset, start_freq)
        elif noise == "open":
            return open_simplex_fractal_noise(perm, width, height, scale, octaves, persistence, lacunarity, x_offset, y_offset, start_freq)
        elif noise == "snoise":
            return snoise_fractal_noise(width, height, scale, octaves, persistence, lacunarity, x_offset, y_offset, start_freq)

    def point_simplex_noise(self, x, y, 
                              scale=100, octaves=7, persistence=0.5, lacunarity=2.0, start_freq=1, seed=None):
        x_offset = self.x_offset if self.x_offset is not None else x_offset
        y_offset = self.y_offset if self.y_offset is not None else y_offset
        seed = self.seed if seed is None else seed

        rng = np.random.RandomState(seed)
        perm = rng.permutation(256)
        return point_open_simplex_fractal_noise(perm, x, y, scale, octaves, persistence, lacunarity, start_freq)
    
    def batch_simplex_noise(self, points, x_offset=0, y_offset=0,
                            scale=100, octaves=7, persistence=0.5, lacunarity=2.0, start_freq=1, seed=None):
        x_offset = self.x_offset if self.x_offset is not None else x_offset
        y_offset = self.y_offset if self.y_offset is not None else y_offset
        seed = self.seed if seed is None else seed

        rng = np.random.RandomState(seed)
        perm = rng.permutation(256)
        return batch_open_simplex_fractal_noise(perm, points, scale, octaves, persistence, lacunarity, x_offset, y_offset, start_freq)


    def warped_simplex_noise(self, warp_x, warp_y, warp_strength=100,
                             x_offset=0, y_offset=0,
                             scale=100, octaves=7, persistence=0.5, lacunarity=2.0,
                             height=None, width=None, seed=None):
        height = self.height if height is None else height
        width = self.width if width is None else width
        x_offset = self.x_offset if x_offset is None else x_offset
        y_offset = self.y_offset if y_offset is None else y_offset
        seed = self.seed if seed is None else seed

        rng = np.random.RandomState(seed)
        perm = rng.permutation(256)
        return warped_open_simplex_fractal_noise(perm, width, height,
                                                 scale, octaves, persistence, lacunarity,
                                                 warp_x, warp_y, warp_strength,
                                                 x_offset, y_offset)

    def uber_noise(self, x_offset=0, y_offset=0,
                     scale=100, octaves=7,
                     sharpness=0, feature_amp=1, slope_erosion=0.5, altitude_erosion=0.5, ridge_erosion=0.5,
                     lacunarity=2.0, persistence=0.5,
                     warp_x=None, warp_y=None, warp_strength=100,
                     height=None, width=None, seed=None):

        height = self.height if height is None else height
        width = self.width if width is None else width
        x_offset = self.x_offset if x_offset is None else x_offset
        y_offset = self.y_offset if y_offset is None else y_offset
        seed = self.seed if seed is None else seed

        rng = np.random.RandomState(seed)
        perm = rng.permutation(256)

        if warp_x is not None and warp_y is not None:
            return warped_uber_noise(perm, width, height, scale, octaves,
                                     sharpness=sharpness, feature_amp=feature_amp,
                                     slope_erosion=slope_erosion, altitude_erosion=altitude_erosion, ridge_erosion=ridge_erosion,
                                     lacunarity=lacunarity, init_gain=persistence,
                                     warp_x=warp_x, warp_y=warp_y, warp_strength=warp_strength,
                                     x_offset=x_offset, y_offset=y_offset)
        else:
            return uber_noise(perm, width, height, scale, octaves,
                            sharpness=sharpness, feature_amp=feature_amp,
                            slope_erosion=slope_erosion, altitude_erosion=altitude_erosion, ridge_erosion=ridge_erosion,
                            lacunarity=lacunarity, init_gain=persistence,
                            x_offset=x_offset, y_offset=y_offset)


    def billow_noise(self, noise="simplex", x_offset=0, y_offset=0,
                     scale=100, octaves=7, persistence=0.5, lacunarity=2.0,
                     height=None, width=None, seed=None):
        billow = normalize(self.fractal_simplex_noise(noise=noise, x_offset=x_offset, y_offset=y_offset,
                                                            scale=scale, octaves=octaves, persistence=persistence, lacunarity=lacunarity,
                                                            height=height, width=width, seed=seed),
                                a=-1, b=1)
        return np.abs(billow)

    def ridged_noise(self, noise="simplex", x_offset=0, y_offset=0,
                     scale=100, octaves=7, persistence=0.5, lacunarity=2.0,
                     height=None, width=None, seed=None):
        return 1 - self.billow_noise(noise=noise, x_offset=x_offset, y_offset=y_offset,
                                     scale=scale, octaves=octaves, persistence=persistence, lacunarity=lacunarity,
                                     height=height, width=width, seed=seed)

    def worley_noise(self, density=50, k=1, p=2, distribution="uniform", radius=0.1,
                          height=None, width=None, seed=None):
        # https://stackoverflow.com/questions/65703414/how-can-i-make-a-worley-noise-algorithm-faster
        height = self.height if height is None else height
        width = self.width if width is None else width
        seed = self.seed if seed is None else seed

        rng = np.random.RandomState(seed)

        if distribution == "uniform":
            points = [[rng.randint(0, height), rng.randint(0, width)] for _ in range(density)]
        elif distribution == "poisson":
            poisson_disk = qmc.PoissonDisk(2, radius=radius, seed=rng)
            points = poisson_disk.random(n=density, workers=-1)*np.array([1024, 1024])

        coord = np.dstack(np.mgrid[0:height, 0:width])
        tree = cKDTree(points)
        distances = tree.query(coord, workers=-1, p=p, k=k)[0]
        return distances #[..., 0], [..., 1], ...

    def angular_noise(self, density=50, k=1, p=2, distribution="uniform", radius=0.1,
                          height=None, width=None, seed=None):
        height = self.height if height is None else height
        width = self.width if width is None else width
        seed = self.seed if seed is None else seed

        rng = np.random.RandomState(seed)

        # Generate feature points
        if distribution == "uniform":
            points = np.array([[rng.randint(0, height), rng.randint(0, width)] for _ in range(density)])
        elif distribution == "poisson":
            poisson_disk = qmc.PoissonDisk(2, radius=radius, seed=rng)
            points = poisson_disk.random(n=density) * np.array([height, width])

        # Create coordinate grid
        coord = np.dstack(np.mgrid[0:height, 0:width])

        # Create KDTree for fast nearest neighbor search
        tree = cKDTree(points)

        # Get nearest feature point distances & indices
        distances, indices = tree.query(coord, workers=-1, p=p, k=k)

        # Extract nearest feature point coordinates
        nearest_points = points[indices]

        # Compute angle map
        y_f, x_f = nearest_points[..., 0], nearest_points[..., 1]
        y, x = np.mgrid[0:height, 0:width]

        angles = np.arctan2(y_f - y, x_f - x)  # Compute angle in radians
        angles_degrees = np.degrees(angles)  # Convert to degrees if needed

        return distances, angles

    def phasor_noise(self, num_phasors=20, freq_range=(1, 10), amplitude=1.0, direction_bias=np.pi/2, anisotropy=0.2,
                              height=None, width=None, seed=None):
        height = self.height if height is None else height
        width = self.width if width is None else width
        seed = self.seed if seed is None else seed

        rng = np.random.RandomState(seed)

        x = np.linspace(-1, 1, width)
        y = np.linspace(-1, 1, height)
        X, Y = np.meshgrid(x, y)
        noise = np.zeros((height, width))

        for _ in range(num_phasors):
            frequency = rng.uniform(freq_range[0], freq_range[1])
            theta = rng.normal(direction_bias, np.pi * 0.1)
            phase = rng.uniform(0, 2 * np.pi)
            amplitude_i = rng.uniform(0, amplitude)

            kx = frequency * np.cos(theta)
            ky = frequency * np.sin(theta) * anisotropy

            noise += amplitude_i * np.cos(2 * np.pi * (kx * X + ky * Y) + phase)

        return noise / np.max(np.abs(noise))



