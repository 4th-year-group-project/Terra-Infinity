"""A wrapper class for the Gradient, Worley and Phasor noise generation functions."""

import numpy as np
from scipy.spatial import cKDTree
from scipy.stats import qmc

from .parallel import (
    batch_open_simplex_fractal_noise,
    open_simplex_fractal_noise,
    point_open_simplex_fractal_noise,
    simplex_fractal_noise,
    snoise_fractal_noise,
    uber_noise,
    warped_open_simplex_fractal_noise,
    warped_uber_noise,
)
from .tools import normalize


class Noise:
    """A class for generating various types of noise.

    Attributes:
        seed (int): Random seed for reproducibility.
        width (int): Width of the generated noise.
        height (int): Height of the generated noise.
        x_offset (float): X offset for the noise generation.
        y_offset (float): Y offset for the noise generation.
    """

    def __init__(self, seed, width=1024, height=1024, x_offset=0, y_offset=0):
        """Initializes the Noise class.

        Args:
            seed (int): Random seed for reproducibility.
            width (int): Width of the generated noise.
            height (int): Height of the generated noise.
            x_offset (float): X offset for the noise generation.
            y_offset (float): Y offset for the noise generation.

        """

        self.seed = seed
        self.width = width
        self.height = height
        self.x_offset = x_offset
        self.y_offset = y_offset

    def fractal_simplex_noise(
        self,
        noise="simplex",
        x_offset=0,
        y_offset=0,
        scale=100,
        octaves=7,
        persistence=0.5,
        lacunarity=2.0,
        start_freq=1,
        height=None,
        width=None,
        seed=None,
    ):
        """Generates fractal simplex noise.
        
        Args:
            noise (str): Type of noise to generate. Options are "simplex", "open", or "snoise".
            x_offset: X offset for noise generation (default is 0).
            y_offset: Y offset for noise generation (default is 0).
            scale: Scale factor for the noise.
            octaves: Number of octaves for fractal generation.
            persistence: Persistence factor for amplitude scaling.
            lacunarity: Lacunarity factor for frequency scaling.
            start_frequency: Starting frequency for the first octave (default is 1).
            width: Width of the noise map.
            height: Height of the noise map.
            seed (int): Random seed for reproducibility.
        
        Returns:
            np.ndarray: 2D array of noise values in the range [-1, 1] of size (height, width).
        """

        height = self.height if height is None else height
        width = self.width if width is None else width
        x_offset = self.x_offset if x_offset is None else x_offset
        y_offset = self.y_offset if y_offset is None else y_offset
        seed = self.seed if seed is None else seed

        rng = np.random.RandomState(seed)
        perm = rng.permutation(256)
        if noise == "simplex":
            return simplex_fractal_noise(
                perm, width, height, scale, octaves, persistence, lacunarity, x_offset, y_offset, start_freq
            )
        elif noise == "open":
            return open_simplex_fractal_noise(
                perm, width, height, scale, octaves, persistence, lacunarity, x_offset, y_offset, start_freq
            )
        elif noise == "snoise":
            return snoise_fractal_noise(
                width, height, scale, octaves, persistence, lacunarity, x_offset, y_offset, start_freq
            )

    def point_simplex_noise(
        self,
        x,
        y,
        x_offset=0,
        y_offset=0,
        scale=100,
        octaves=7,
        persistence=0.5,
        lacunarity=2.0,
        start_freq=1,
        seed=None,
    ):
        """Generates simplex noise for a single point.
        
        Args:
            x (float): X coordinate of the point.
            y (float): Y coordinate of the point.
            x_offset: X offset for noise generation (default is 0).
            y_offset: Y offset for noise generation (default is 0).
            scale: Scale factor for the noise.
            octaves: Number of octaves for fractal generation.
            persistence: Persistence factor for amplitude scaling.
            lacunarity: Lacunarity factor for frequency scaling.
            start_frequency: Starting frequency for the first octave (default is 1).
            seed (int): Random seed for reproducibility.

        Returns:
            float: Noise value at the specified point.
        """

        x_offset = self.x_offset if x_offset is None else x_offset
        y_offset = self.y_offset if y_offset is None else y_offset
        seed = self.seed if seed is None else seed

        rng = np.random.RandomState(seed)
        perm = rng.permutation(256)
        return point_open_simplex_fractal_noise(
            perm, x, y, scale, octaves, persistence, lacunarity, x_offset, y_offset, start_freq
        )

    def batch_simplex_noise(
        self,
        points,
        x_offset=0,
        y_offset=0,
        scale=100,
        octaves=7,
        persistence=0.5,
        lacunarity=2.0,
        start_freq=1,
        seed=None,
    ):
        """Generates simplex noise for a batch of points.
        
        Args:
            points (list): List of tuples containing (x, y) coordinates of the points.
            x_offset: X offset for noise generation (default is 0).
            y_offset: Y offset for noise generation (default is 0).
            scale: Scale factor for the noise.
            octaves: Number of octaves for fractal generation.
            persistence: Persistence factor for amplitude scaling.
            lacunarity: Lacunarity factor for frequency scaling.
            start_frequency: Starting frequency for the first octave (default is 1).
            seed (int): Random seed for reproducibility.

        Returns:
            np.ndarray: 2D array of noise values in the range [-1, 1] of size (len(points),).
        """

        x_offset = self.x_offset if x_offset is None else x_offset
        y_offset = self.y_offset if y_offset is None else y_offset
        seed = self.seed if seed is None else seed

        rng = np.random.RandomState(seed)
        perm = rng.permutation(256)
        return batch_open_simplex_fractal_noise(
            perm, points, scale, octaves, persistence, lacunarity, x_offset, y_offset, start_freq
        )

    def warped_simplex_noise(
        self,
        warp_x,
        warp_y,
        warp_strength=100,
        x_offset=0,
        y_offset=0,
        scale=100,
        octaves=7,
        persistence=0.5,
        lacunarity=2.0,
        height=None,
        width=None,
        seed=None,
    ):
        """Generates domain warped simplex noise.
        
        Args:
            warp_x (np.ndarray): 2D array for x-axis warping.
            warp_y (np.ndarray): 2D array for y-axis warping.
            warp_strength: Strength of the warping effect.
            x_offset: X offset for noise generation (default is 0).
            y_offset: Y offset for noise generation (default is 0).
            scale: Scale factor for the noise.
            octaves: Number of octaves for fractal generation.
            persistence: Persistence factor for amplitude scaling.
            lacunarity: Lacunarity factor for frequency scaling.
            height: Height of the noise map.
            width: Width of the noise map.
            seed (int): Random seed for reproducibility.

        Returns:
            np.ndarray: 2D array of noise values in the range [-1, 1] of size (height, width).
        """

        height = self.height if height is None else height
        width = self.width if width is None else width
        x_offset = self.x_offset if self.x_offset is not None else x_offset
        y_offset = self.y_offset if self.y_offset is not None else y_offset
        seed = self.seed if seed is None else seed

        rng = np.random.RandomState(seed)
        perm = rng.permutation(256)
        return warped_open_simplex_fractal_noise(
            perm,
            width,
            height,
            scale,
            octaves,
            persistence,
            lacunarity,
            warp_x,
            warp_y,
            warp_strength,
            x_offset,
            y_offset,
        )

    def uber_noise(
        self,
        x_offset=0,
        y_offset=0,
        scale=100,
        octaves=7,
        sharpness=0,
        feature_amp=1,
        slope_erosion=0.5,
        altitude_erosion=0.5,
        ridge_erosion=0.5,
        lacunarity=2.0,
        persistence=0.5,
        warp_x=None,
        warp_y=None,
        warp_strength=100,
        height=None,
        width=None,
        seed=None,
    ):
        """Generates Uber noise.
        
        Args:
            x_offset: X offset for noise generation (default is 0).
            y_offset: Y offset for noise generation (default is 0).
            scale: Scale factor for the noise.
            octaves: Number of octaves for fractal generation.
            sharpness: Sharpness factor for the noise.
            feature_amp: Feature amplitude for the noise.
            slope_erosion: Slope erosion factor for the noise.
            altitude_erosion: Altitude erosion factor for the noise.
            ridge_erosion: Ridge erosion factor for the noise.
            lacunarity: Lacunarity factor for frequency scaling.
            persistence: Persistence factor for amplitude scaling.
            warp_x (np.ndarray): 2D array for x-axis warping.
            warp_y (np.ndarray): 2D array for y-axis warping.
            warp_strength: Strength of the warping effect.
            height: Height of the noise map.
            width: Width of the noise map.
            seed (int): Random seed for reproducibility.
        """

        height = self.height if height is None else height
        width = self.width if width is None else width
        x_offset = self.x_offset if self.x_offset is not None else x_offset
        y_offset = self.y_offset if self.y_offset is not None else y_offset
        seed = self.seed if seed is None else seed

        rng = np.random.RandomState(seed)
        perm = rng.permutation(256)

        if warp_x is not None and warp_y is not None:
            return warped_uber_noise(
                perm,
                width,
                height,
                scale,
                octaves,
                sharpness=sharpness,
                feature_amp=feature_amp,
                slope_erosion=slope_erosion,
                altitude_erosion=altitude_erosion,
                ridge_erosion=ridge_erosion,
                lacunarity=lacunarity,
                init_gain=persistence,
                warp_x=warp_x,
                warp_y=warp_y,
                warp_strength=warp_strength,
                x_offset=x_offset,
                y_offset=y_offset,
            )
        else:
            return uber_noise(
                perm,
                width,
                height,
                scale,
                octaves,
                sharpness=sharpness,
                feature_amp=feature_amp,
                slope_erosion=slope_erosion,
                altitude_erosion=altitude_erosion,
                ridge_erosion=ridge_erosion,
                lacunarity=lacunarity,
                init_gain=persistence,
                x_offset=x_offset,
                y_offset=y_offset,
            )

    def billow_noise(
        self,
        noise="simplex",
        x_offset=0,
        y_offset=0,
        scale=100,
        octaves=7,
        persistence=0.5,
        lacunarity=2.0,
        height=None,
        width=None,
        seed=None,
    ):
        billow = normalize(
            self.fractal_simplex_noise(
                noise=noise,
                x_offset=x_offset,
                y_offset=y_offset,
                scale=scale,
                octaves=octaves,
                persistence=persistence,
                lacunarity=lacunarity,
                height=height,
                width=width,
                seed=seed,
            ),
            a=-1,
            b=1,
        )
        return np.abs(billow)

    def ridged_noise(
        self,
        noise="simplex",
        x_offset=0,
        y_offset=0,
        scale=100,
        octaves=7,
        persistence=0.5,
        lacunarity=2.0,
        height=None,
        width=None,
        seed=None,
    ):
        return 1 - self.billow_noise(
            noise=noise,
            x_offset=x_offset,
            y_offset=y_offset,
            scale=scale,
            octaves=octaves,
            persistence=persistence,
            lacunarity=lacunarity,
            height=height,
            width=width,
            seed=seed,
        )

    def worley_noise(
        self,
        density=50,
        k=1,
        p=2,
        distribution="uniform",
        radius=0.1,
        jitter=False,
        jitter_strength=0.1,
        i=0,
        ret_points=False,
        height=None,
        width=None,
        seed=None,
    ):
        """Generates Worley noise.
        
        Args:
            density (int): Number of points to generate.
            k (int): Number of nearest points to consider.
            p (int): Power parameter for distance calculation.
            distribution (str): Distribution of points. Options are "uniform" or "poisson".
            radius (float): Radius for Poisson disk sampling.
            jitter (bool): Whether to apply jitter to the points.
            jitter_strength (float): Strength of the jitter.
            i (int): Index of the nearest point to return.
            ret_points (bool): Whether to return the generated points.
            height (int): Height of the noise map.
            width (int): Width of the noise map.
            seed (int): Random seed for reproducibility.

        Returns:
            np.ndarray: 2D array of noise values in the range [0, 1] of size (height, width).
            points (np.ndarray): Generated points if ret_points is True.
        """

        # https://stackoverflow.com/questions/65703414/how-can-i-make-a-worley-noise-algorithm-faster
        height = self.height if height is None else height
        width = self.width if width is None else width
        seed = self.seed if seed is None else seed

        rng = np.random.RandomState(seed)
        if distribution == "uniform":
            points = [[rng.randint(0, height), rng.randint(0, width)] for _ in range(density)]
        elif distribution == "poisson":
            poisson_disk = qmc.PoissonDisk(2, radius=radius / max(width, height), seed=rng)
            points = poisson_disk.random(n=density, workers=-1)
            points = qmc.scale(points, l_bounds=[0, 0], u_bounds=[width, height])
            if jitter:
                jitter_strength = jitter_strength * radius
                jitter_points = rng.uniform(-jitter_strength, jitter_strength, points.shape)
                points += jitter_points

        coord = np.dstack(np.mgrid[0:height, 0:width])
        tree = cKDTree(points)

        distances = tree.query(coord, workers=-1, p=p, k=k)[i] if i >= 0 else tree.query(coord, workers=-1, p=p, k=k)

        if ret_points:
            return distances, points
        else:
            return distances  # [..., 0], [..., 1], ...

    def angular_noise(
        self, density=50, k=1, p=2, distribution="uniform", radius=0.1, height=None, width=None, seed=None
    ):
        height = self.height if height is None else height
        width = self.width if width is None else width
        seed = self.seed if seed is None else seed

        rng = np.random.RandomState(seed)

        if distribution == "uniform":
            points = np.array([[rng.randint(0, height), rng.randint(0, width)] for _ in range(density)])
        elif distribution == "poisson":
            poisson_disk = qmc.PoissonDisk(2, radius=radius, seed=rng)
            points = poisson_disk.random(n=density) * np.array([height, width])

        coord = np.dstack(np.mgrid[0:height, 0:width])

        tree = cKDTree(points)

        distances, indices = tree.query(coord, workers=-1, p=p, k=k)
        nearest_points = points[indices]

        y_f, x_f = nearest_points[..., 0], nearest_points[..., 1]
        y, x = np.mgrid[0:height, 0:width]

        angles = np.arctan2(y_f - y, x_f - x)

        return distances, angles

    def phasor_noise(
        self,
        num_phasors=20,
        freq_range=(1, 10),
        amplitude=1.0,
        direction_bias=np.pi / 2,
        anisotropy=0.2,
        height=None,
        width=None,
        seed=None,
    ):
        """Generates phasor noise.
        
        Args:
            num_phasors (int): Number of phasors to generate.
            freq_range (tuple): Frequency range for the phasors.
            amplitude (float): Amplitude of the phasors.
            direction_bias (float): Direction bias for the phasors.
            anisotropy (float): Anisotropy factor for the phasors.
            height (int): Height of the noise map.
            width (int): Width of the noise map.
            seed (int): Random seed for reproducibility.

        Returns:
            np.ndarray: 2D array of noise values in the range [-1, 1] of size (height, width).
        """

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
            theta = rng.normal(direction_bias, np.pi * anisotropy)
            phase = rng.uniform(0, 2 * np.pi)
            amplitude_i = rng.uniform(0, amplitude)

            kx = frequency * np.cos(theta)
            ky = frequency * np.sin(theta)

            phi = 2 * np.pi * (kx * X + ky * Y) + phase
            noise += amplitude_i * np.cos(phi)

        return noise / np.max(np.abs(noise))

    def spiral_phasor_noise(
        self,
        num_phasors=20,
        freq_range=(1, 10),
        amplitude=1.0,
        direction_bias=np.pi / 2,
        anisotropy=0.2,
        height=None,
        width=None,
        seed=None,
        spiral_strength=1.0,
    ):
        height = self.height if height is None else height
        width = self.width if width is None else width
        seed = self.seed if seed is None else seed

        rng = np.random.RandomState(seed)

        x = np.linspace(-1, 1, width)
        y = np.linspace(-1, 1, height)
        X, Y = np.meshgrid(x, y)

        R = np.sqrt(X**2 + Y**2)
        Theta = np.arctan2(Y, X)

        Theta += spiral_strength * R

        X_spiral = R * np.cos(Theta)
        Y_spiral = R * np.sin(Theta)

        noise = np.zeros((height, width))

        for _ in range(num_phasors):
            frequency = rng.uniform(freq_range[0], freq_range[1])
            theta = rng.normal(direction_bias, np.pi * anisotropy)
            phase = rng.uniform(0, 2 * np.pi)
            amplitude_i = rng.uniform(0, amplitude)

            kx = frequency * np.cos(theta)
            ky = frequency * np.sin(theta)

            noise += amplitude_i * np.cos(2 * np.pi * (kx * X_spiral + ky * Y_spiral) + phase)

        return noise / np.max(np.abs(noise))
