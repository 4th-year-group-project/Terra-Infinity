"""This file contains various terrain generation functions that are referred to by the different sub-biomes."""

import numpy as np
from numba import jit
from scipy.ndimage import gaussian_filter, laplace
from scipy.stats import qmc

from world_generation.cellular_automata.scaling_heightmap import ca_in_mask
from world_generation.generation import Noise, high_smooth, low_smooth, normalize, sawtooth, tools


@jit(nopython=True)
def smooth_min_numba(a, b, k):
    h = min(max(((b - a + k) / (2 * k)), 0), 1)
    return a * h + b * (1 - h) - k * h * (1 - h)


@jit(nopython=True)
def smooth_max_numba(a, b, k):
    h = min(max(((a - b + k) / (2 * k)), 0), 1)
    return a * (1 - h) + b * h + k * h * (1 - h)


@jit(nopython=True)
def generate_crater_numba(
    center, radius, rim_width, rim_steepness, floor_height, smoothness, rim_smoothness, width, height
):
    """Generates a crater. Outside the class due to Numba.

    Args:
        center: Center of the crater (x, y).
        radius: Radius of the crater.
        rim_width: Width of the crater rim.
        rim_steepness: Steepness of the crater rim.
        floor_height: Height of the crater floor.
        smoothness: Smoothness of the crater shape.
        rim_smoothness: Smoothness of the crater rim.
        width: The width of the heightmap
        height: The height of the heightmap

    Returns:
        The crater heightmap.
    """
    crater_height = np.zeros((height, width))

    for y in range(height):
        for x in range(width):
            diff_x = x - center[0]
            diff_y = y - center[1]

            squared_dist = diff_x * diff_x + diff_y * diff_y
            dist_from_center = np.sqrt(squared_dist) / radius

            cavity = dist_from_center * dist_from_center - 1
            rimX = min(dist_from_center - 1 - rim_width, 0)
            rim = rim_steepness * rimX * rimX

            crater_shape = smooth_max_numba(cavity, floor_height, smoothness)
            crater_shape = smooth_min_numba(crater_shape, rim, rim_smoothness)

            crater_height[y, x] = crater_shape * radius

    return crater_height


class Sub_Biomes:
    """A class to generate various types of terrain. These are referenced and parameterized in the biome_based_terrain_generator.py file.

    Attributes:
        seed: The seed for the noise generator.
        width: The width of the heightmap.
        height: The height of the heightmap.
        x_offset: The x offset for the noise generator.
        y_offset: The y offset for the noise generator.
        noise: An instance of the Noise class for generating noise.

    """

    def __init__(self, seed, width, height, x_offset, y_offset):
        """Initializes the Sub_Biomes class with the given parameters.

        Args:
            seed: The seed for the noise generator.
            width: The width of the heightmap.
            height: The height of the heightmap.
            x_offset: The x offset for the noise generator. Needed for consistency across independent superchunks.
            y_offset: The y offset for the noise generator. Needed for consistency across independent superchunks.

        """
        self.seed = seed
        self.width = width
        self.height = height
        self.x_offset = x_offset
        self.y_offset = y_offset
        self.noise = Noise(seed=seed, width=width, height=height, x_offset=x_offset, y_offset=y_offset)

    def normalise(self, heightmap, low, high):
        """Helper function to normalise a heightmap to a given range.

        Args:
            heightmap: The heightmap to normalise.
            low: The minimum value of the range.
            high: The maximum value of the range.

        Returns:
            The normalised heightmap.

        """
        return (heightmap - np.min(heightmap)) / (np.max(heightmap) - np.min(heightmap)) * (high - low) + low

    def flats(self, min_height, max_height, variation=1, scale=1024, persistence=0.4, lacunarity=2.0):
        """Generates a parameterized 'flats' terrain form.

        Args:
            min_height: The minimum height of the terrain.
            max_height: The maximum height of the terrain.
            variation: The variation of the terrain.
            scale: The scale of the noise.
            persistence: The persistence of the noise.
            lacunarity: The lacunarity of the noise.

        Returns:
            A heightmap representing the 'flats' terrain.
        """
        base_noise = self.noise.fractal_simplex_noise(
            seed=self.seed,
            width=self.width,
            height=self.height,
            x_offset=self.x_offset,
            y_offset=self.y_offset,
            scale=scale,
            octaves=3,
            persistence=persistence,
            lacunarity=lacunarity,
        )
        base_noise = self.normalise(base_noise, 0, 1)

        base_noise = base_noise**variation
        texture_noise = self.noise.fractal_simplex_noise(
            seed=self.seed,
            width=self.width,
            height=self.height,
            x_offset=self.x_offset,
            y_offset=self.y_offset,
            scale=1024,
            octaves=8,
            persistence=0.5,
            lacunarity=2.0,
        )
        texture_noise = self.normalise(texture_noise, 0, 1)
        add_noise = base_noise + texture_noise
        add_noise = self.normalise(add_noise, min_height, max_height)
        return add_noise

    def hills(self, min_height, max_height, variation=2, scale=1024, persistence=0.5, lacunarity=2.0):
        """Generates a parameterized 'hills' terrain form.

        Args:
            min_height: The minimum height of the terrain.
            max_height: The maximum height of the terrain.
            variation: The variation of the terrain.
            scale: The scale of the noise.
            persistence: The persistence of the noise.
            lacunarity: The lacunarity of the noise.

        Returns:
            A heightmap representing the 'hills' terrain.

        """
        base_noise = self.noise.billow_noise(
            seed=self.seed,
            width=self.width,
            height=self.height,
            x_offset=self.x_offset,
            y_offset=self.y_offset,
            scale=scale,
            octaves=variation,
            persistence=persistence,
            lacunarity=lacunarity,
        )
        texture_noise = self.noise.fractal_simplex_noise(
            seed=self.seed,
            width=self.width,
            height=self.height,
            x_offset=self.x_offset,
            y_offset=self.y_offset,
            scale=1024,
            octaves=6,
            persistence=0.5,
            lacunarity=2.0,
        )
        texture_noise = self.normalise(texture_noise, 0, 1)
        base_noise = self.normalise(base_noise, 0, 1)
        base_noise = tools.smooth(base_noise)
        add_noise = base_noise + texture_noise
        add_noise = self.normalise(add_noise, min_height, max_height)
        return add_noise

    def super_fake_entropy(self, heightmap):
        """A fast, crude appoximation of image entropy.

        Args:
            heightmap: The heightmap to calculate the entropy of.

        Returns:
            The entropy of the heightmap.
        """
        return np.abs(laplace(heightmap, mode="reflect"))

    def dla_mountains(self, min_height, max_height, binary_mask, ruggedness, exponent, noise_scale=0.3, sigma=3):
        """Generates a parameterized 'mountains' terrain form using CA DLA approximation.

        Args:
            min_height: The minimum height of the terrain.
            max_height: The maximum height of the terrain.
            binary_mask: A binary mask to define the area of the terrain.
            ruggedness: The ruggedness of the terrain.
            exponent: The exponent to control the steepness of the mountains.
            noise_scale: The scale of the noise.
            sigma: The standard deviation for Gaussian blur.

        Returns:
            A heightmap representing the 'mountains' terrain.
        """

        # Generate a heightmap using CA DLA approximation with the upscale-and-blur algorithm.
        heightmap = ca_in_mask(self.seed, binary_mask, ruggedness)

        # Estimate regions where the mountains are
        heightmap_to_entropy = self.normalise(heightmap, 0, 1)
        image_std = self.super_fake_entropy(heightmap_to_entropy)
        image_std = self.normalise(image_std, 0, 1)
        image_std = gaussian_filter(image_std, sigma=sigma)
        inverted_image_std = 1 - image_std

        # Make them less smooth by adding low amplitude high frequency noise
        noise_to_add = self.noise.fractal_simplex_noise(
            noise="open",
            x_offset=self.x_offset,
            y_offset=self.y_offset,
            scale=30,
            octaves=4,
            persistence=0.5,
            lacunarity=2,
            start_freq=9,
        )
        noise_to_add = self.normalise(noise_to_add, 0, 1)
        noise_overlay_scale = 0.2
        heightmap = heightmap + (noise_to_add * noise_overlay_scale * image_std)
        heightmap = self.normalise(heightmap, 0, 1)

        # Bring out peaks (parameterize this)
        heightmap = np.power(heightmap, exponent)

        # Add some noise where the mountains did not reach
        negative_space_noise = self.noise.fractal_simplex_noise(
            noise="open",
            x_offset=self.x_offset,
            y_offset=self.y_offset,
            scale=100,
            octaves=8,
            persistence=0.5,
            lacunarity=2,
        )
        negative_space_noise = self.normalise(negative_space_noise, 0, 1)
        inverted_image_std = self.normalise(inverted_image_std, 0, 1)
        heightmap = heightmap + (negative_space_noise * 0.05 * inverted_image_std)

        # Add some low frequency noise to the mountains for less peak height uniformity
        perturbing_noise = self.noise.fractal_simplex_noise(
            noise="open",
            x_offset=self.x_offset,
            y_offset=self.y_offset,
            scale=200,
            octaves=1,
            persistence=0.5,
            lacunarity=2,
        )

        heightmap = heightmap + perturbing_noise * noise_scale
        heightmap = self.normalise(heightmap, min_height, max_height)

        return heightmap

    def pointy_peaks(
        self,
        mountain_density=150,
        mountain_squareness=2,
        mountain_sharpness=2.5,
        jitter_strength=0.2,
        mountain_smoothness=256,
        mountain_pointyness=-0.5,
        mountain_prominence=0.55,
    ):
        """Generates a parameterized 'mountains' terrain form using a combination of worley noise and simplex noise.

        Args:
            mountain_density: The density of the mountains.
            mountain_squareness: The squareness of the mountains.
            mountain_sharpness: The sharpness of the mountains.
            jitter_strength: The strength of the jitter.
            mountain_smoothness: The smoothness of the mountains.
            mountain_pointyness: The pointyness of the mountains.
            mountain_prominence: The prominence of the mountains.

        Returns:
            A heightmap representing the 'mountains' terrain.

        """

        worley = (
            1
            - normalize(
                self.noise.worley_noise(
                    density=max(self.width, self.height),
                    k=1,
                    p=mountain_squareness,
                    distribution="poisson",
                    radius=mountain_density,
                    jitter=(jitter_strength > 0),
                    jitter_strength=jitter_strength,
                )
            )
        ) ** mountain_sharpness

        noise = normalize(
            self.noise.uber_noise(
                scale=mountain_smoothness,
                octaves=8,
                persistence=0.48,
                lacunarity=2.0,
                sharpness=mountain_pointyness,
                feature_amp=1,
                slope_erosion=0.5,
                altitude_erosion=0.08,
                ridge_erosion=0,
            )
        )
        alpha = mountain_prominence
        heightmap = worley * alpha + noise * (1 - alpha)
        heightmap = normalize(heightmap, a=0.2, b=1)
        return heightmap

    def sheer_peaks(
        self,
        min_height,
        max_height,
        sheer_density=50,
        sheer_squareness=1,
        scale=256 - 54,
        persistence=0.49,
        sharpness=0.3,
        altitude_erosion=0.2,
        slope_erosion=0.3,
        warp_strength=55,
        texture_amplitude=0.2,
    ):
        """Generates a parameterized 'mountains' terrain form using uber noise.

        Args:
            min_height: The minimum height of the terrain.
            max_height: The maximum height of the terrain.
            sheer_density: The density of the mountains.
            sheer_squareness: The squareness of the mountains.
            scale: The scale of the noise.
            persistence: The persistence of the noise.
            sharpness: The sharpness of the noise.
            altitude_erosion: The altitude erosion of the noise.
            slope_erosion: The slope erosion of the noise.
            warp_strength: The strength of the warp.
            texture_amplitude: The number of octaves for added texture noise.

        Returns:
            A heightmap representing the 'mountains' terrain.
        """

        worley_x = normalize(
            self.noise.worley_noise(seed=self.seed + 1, density=sheer_density, k=1, p=sheer_squareness), -1, 1
        )
        # worley_y = normalize(self.noise.worley_noise(seed=self.seed+2, density=sheer_density, k=1, p=sheer_squareness), -1, 1)
        worley_y = np.flipud(worley_x)

        heightmap = normalize(
            self.noise.uber_noise(
                scale=scale,
                octaves=8,
                persistence=persistence,
                lacunarity=2.0,
                sharpness=sharpness,
                feature_amp=1,
                altitude_erosion=altitude_erosion,
                slope_erosion=slope_erosion,
                ridge_erosion=0,
                warp_x=worley_x,
                warp_y=worley_y,
                warp_strength=warp_strength,
            ),
            min_height,
            max_height,
        )

        return heightmap

    def uber_flats(
        self,
        scale=128,
        persistence=0.5,
        lacunarity=1.9,
        sharpness=0.3,
        slope_erosion=0.3,
        altitude_erosion=0.2,
        ridge_erosion=0.4,
        warp_strength=15,
    ):
        """Generates a parameterized 'flats' terrain form using uber noise.

        Args:
            scale: The scale of the noise.
            persistence: The persistence of the noise.
            lacunarity: The lacunarity of the noise.
            sharpness: The sharpness of the noise.
            slope_erosion: The slope erosion of the noise.
            altitude_erosion: The altitude erosion of the noise.
            ridge_erosion: The ridge erosion of the noise.
            warp_strength: The strength of the warp.

        Returns:
            A heightmap representing the 'flats' terrain.
        """
        noise_x = normalize(
            self.noise.fractal_simplex_noise(scale=256, octaves=4, persistence=0.5, lacunarity=2.0), -1, 1
        )
        noise_y = normalize(
            self.noise.fractal_simplex_noise(scale=256, octaves=4, persistence=0.5, lacunarity=2.0), -1, 1
        )

        heightmap = normalize(
            self.noise.uber_noise(
                scale=scale,
                octaves=10,
                persistence=persistence,
                lacunarity=lacunarity,
                sharpness=sharpness,
                feature_amp=1,
                slope_erosion=slope_erosion,
                altitude_erosion=altitude_erosion,
                ridge_erosion=ridge_erosion,
                warp_x=noise_x,
                warp_y=noise_y,
                warp_strength=warp_strength,
            ),
            0,
            1,
        )

        scalemap = normalize(
            self.noise.fractal_simplex_noise(
                noise="open",
                scale=512,
                octaves=3,
                persistence=0.5,
                lacunarity=2.0,
            )
        )

        return normalize(heightmap * scalemap, 0.2, 0.6)

    def volcanoes(
        self,
        min_height,
        max_height,
        volcano_density=300,
        jitter=0.3,
        tau=20,
        c=0.01,
        volcano_prominence=0.7,
    ):
        """Generates a parameterized 'volcanoes' terrain form using a combination of worley noise and simplex noise.

        Args:
            min_height: The minimum height of the terrain.
            max_height: The maximum height of the terrain.
            volcano_density: The density of the volcanoes.
            jitter: The jitter strength.
            tau: The tau parameter for the volcanoes.
            c: The c parameter for the volcanoes.
            volcano_prominence: The prominence of the volcanoes.

        Returns:
            A heightmap representing the 'volcanoes' terrain.
        """
        worley = self.noise.worley_noise(
            density=max(self.width, self.height),
            k=1,
            p=2,
            distribution="poisson",
            radius=volcano_density,
            jitter=True,
            jitter_strength=jitter,
        )
        sinusoidal_worley = normalize(((worley.max() - worley) ** tau) * np.sin(worley * c))

        noise = normalize(
            self.noise.fractal_simplex_noise(noise="open", scale=256, octaves=10, persistence=0.5, lacunarity=2.0)
        )

        alpha = volcano_prominence
        heightmap = sinusoidal_worley * alpha + noise * (1 - alpha)
        return normalize(heightmap, min_height, max_height)

    def terrace(self, x, num_terraces, steepness, height_exponent=1):
        """Generates one terraced structure.

        Args:
            x: The input noise.
            num_terraces: The number of terraces.
            steepness: The steepness of the terraces.
            height_exponent: The exponent applied to the heightmap.

        Returns:
            A heightmap representing the terraced structure.
        """

        heightmap = normalize(x, a=0, b=(num_terraces) ** (1 / height_exponent))
        heightmap = heightmap**height_exponent
        return normalize(
            (
                np.round(heightmap)
                + np.sign(heightmap - np.round(heightmap))
                * 0.5
                * (np.abs(2 * (heightmap - np.round(heightmap)))) ** steepness
            ),
            0,
            1,
        )

    def mesa_terraces(
        self,
        min_height,
        max_height,
        scale=128,
        persistence=0.48,
        lacunarity=1.8,
        ground_flatness=5,
        height_exponent=1.1,
        num_terraces=5,
        steepness=3,
        peak_flatness=10,
    ):
        """Generates a parameterized 'mesa' terrain form using a combination of simplex noise and the terrace function.

        Args:
            min_height: The minimum height of the terrain.
            max_height: The maximum height of the terrain.
            scale: The scale of the noise.
            persistence: The persistence of the noise.
            lacunarity: The lacunarity of the noise.
            ground_flatness: The flatness of the ground.
            height_exponent: The exponent for the heightmap.
            num_terraces: The number of terraces in the terrain.
            steepness: The steepness of the terraces.
            peak_flatness: The flatness of the peaks.

        Returns:
            A heightmap representing the 'mesa' terrain.
        """
        noise = normalize(
            self.noise.fractal_simplex_noise(
                noise="open",
                scale=scale,
                octaves=8,
                persistence=persistence,
                lacunarity=lacunarity,
            ),
            0,
            1,
        )
        noise = low_smooth(noise, ground_flatness, b=0.5)

        heightmap = self.terrace(noise, num_terraces, steepness, height_exponent=height_exponent)
        heightmap = normalize(high_smooth(heightmap, a=peak_flatness, b=0.5), min_height, max_height)
        return heightmap

    def dune(self, x, p, xm):
        """Generates a dune-like structure.

        Args:
            x: The input noise.
            p: The parameter for the dune shape.
            xm: The maximum value for the dune shape.

        Returns:
            A heightmap representing the dune structure.
        """
        s = np.where((x > 0) & (x < xm), 0.0, 1.0)
        ps1 = p * s + 1.0
        part1 = ps1 * 0.5
        part2 = 1 - np.cos((np.pi / ps1) * ((x - s) / (xm - s)))
        return (part1 * part2) - 1

    def generate_dunes(
        self, frequency=10, noise_scale=5.0, noise_strength=200.0, rotation=-np.pi / 4, gap=100, steepness=0.72
    ):
        """Generates a dune-like structure using a combination of noise and the dune function.

        Args:
            frequency: The frequency of the noise.
            noise_scale: The scale of the noise.
            noise_strength: The strength of the noise.
            rotation: The rotation angle for the dune shape.
            gap: The gap between dunes.
            steepness: The steepness of the dunes.

        Returns:
            A heightmap representing the dune structure.
        """
        dim = max(self.width, self.height)
        X, Y = np.meshgrid(np.arange(self.width), np.arange(self.height))
        shift = noise_strength * self.noise.fractal_simplex_noise(
            noise="open", x_offset=0, y_offset=0, scale=dim / noise_scale, octaves=1, persistence=0.5, lacunarity=2.0
        )

        xb = X * np.cos(rotation) - Y * np.sin(rotation)
        xa = (frequency * xb + shift) % (dim + gap)
        xa = np.clip(xa, 0, dim)
        normalized_xa = xa / dim

        heightmap = self.dune(normalized_xa, 1, steepness)
        return heightmap

    def generate_radial_dunes(
        self, frequency=10, noise_scale=5.0, noise_strength=200.0, center=None, gap=100, steepness=0.72
    ):
        """Generates a radial, circular shaped dune-like structure using a combination of noise and the dune function.

        Args:
            frequency: The frequency of the noise.
            noise_scale: The scale of the noise.
            noise_strength: The strength of the noise.
            center: The center point for the radial dunes. If None, the center of the heightmap is used.
            gap: The gap between dunes.
            steepness: The steepness of the dunes.

        Returns:
            A heightmap representing the radial dune structure.
        """
        dim = max(self.width, self.height)
        X, Y = np.meshgrid(np.arange(self.width), np.arange(self.height))

        # Calculate the distance from each point (X, Y) to the center (cx, cy)
        cx, cy = self.width // 2, self.height // 2 if center is None else center
        distance = np.sqrt((X - cx) ** 2 + (Y - cy) ** 2)

        # Generate noise and shift
        shift = noise_strength * self.noise.fractal_simplex_noise(
            noise="open", x_offset=0, y_offset=0, scale=dim / noise_scale, octaves=1, persistence=0.5, lacunarity=2.0
        )

        # Apply the frequency and shift based on the radial distance
        xa = (frequency * distance + shift) % (dim + gap)
        xa = np.clip(xa, 0, dim)
        normalized_xa = xa / dim

        # Generate the heightmap using the dune function
        heightmap = self.dune(normalized_xa, 1, steepness)
        return heightmap

    def dunes(
        self,
        min_height,
        max_height,
        direction=np.pi / 4,
        core_freq=5,
        core_noise_scale=20,
        core_noise_strength=120,
        core_gap=1,
        core_steepness=0.72,
        second_freq=20,
        second_noise_scale=2,
        second_noise_strength=500,
        second_gap=0.5,
        second_amplitude=0.2,
        second_steepness=0.72,
        third_freq=20,
        third_noise_scale=8,
        third_noise_strength=100,
        third_gap=1,
        third_amplitude=0,
        third_steepness=0.72,
        phasors=20,
        freq_range=(45, 50),
        anisotropy=1,
        phasor_amplitude=0.07,
        saw=True,
    ):
        """Generates a parameterized 'dunes' terrain form using a combination of noise and the dune function.

        Args:
            min_height: The minimum height of the terrain.
            max_height: The maximum height of the terrain.
            direction: The direction of the dunes.
            core_freq: The frequency of the core dunes.
            core_noise_scale: The scale of the core noise.
            core_noise_strength: The strength of the core noise.
            core_gap: The gap between the core dunes.
            core_steepness: The steepness of the core dunes.
            second_freq: The frequency of the second set of dunes.
            second_noise_scale: The scale of the second noise.
            second_noise_strength: The strength of the second noise.
            second_gap: The gap between the second set of dunes.
            second_amplitude: The amplitude of the second set of dunes.
            second_steepness: The steepness of the second set of dunes.
            third_freq: The frequency of the third set of dunes.
            third_noise_scale: The scale of the third noise.
            third_noise_strength: The strength of the third noise.
            third_gap: The gap between the third set of dunes.
            third_amplitude: The amplitude of the third set of dunes.
            third_steepness: The steepness of the third set of dunes.
            phasors: Number of phasors to generate.
            freq_range: Frequency range for phasor noise generation.
            anisotropy: Anisotropy parameter for phasor noise generation.
            phasor_amplitude: Amplitude for phasor noise generation.
            saw: Whether to apply sawtooth smoothing.

        Returns:
            A heightmap representing the 'dunes' terrain.
        """
        dunes1 = normalize(
            self.generate_dunes(
                frequency=core_freq,
                noise_scale=core_noise_scale,
                noise_strength=core_noise_strength,
                rotation=direction,
                gap=core_gap,
                steepness=core_steepness,
            )
        )
        # Uncomment for multilayered dunes
        # dunes2 = normalize(self.generate_dunes(frequency=second_freq, noise_scale=second_noise_scale, noise_strength=second_noise_strength, rotation=direction, gap=second_gap, steepness=second_steepness))
        # dunes3 = normalize(self.generate_dunes(frequency=third_freq, noise_scale=third_noise_scale, noise_strength=third_noise_strength, rotation=direction, gap=third_gap, steepness=third_steepness))
        heightmap = normalize(
            # Uncomment for multilayered dunes
            dunes1  # + second_amplitude*dunes2 #+ third_amplitude*dunes3, 0, 1
        )

        phasor_noise = self.noise.phasor_noise(
            num_phasors=phasors, freq_range=freq_range, amplitude=1.0, direction_bias=-direction, anisotropy=0.1
        )
        if saw:
            phasor_noise = tools.smooth_min(phasor_noise, 0, 1)
        else:
            phasor_noise = normalize(phasor_noise)

        heightmap = heightmap + phasor_noise * phasor_amplitude
        return normalize(heightmap, min_height, max_height)

    def phasor_dunes(
        self,
        direction=-np.pi / 4,
        scale=512,
        octaves=3,
        persistence=0.4,
        lacunarity=1.4,
        phasors=10,
        freq_range=(30, 40),
        anisotropy=2,
        saw=True,
        phasor_amplitude=0.009,
    ):
        """Generates a parameterized 'dunes' terrain form using a combination of noise and phasor noise.

        Args:
            direction: The direction of the dunes.
            scale: The scale of the noise.
            octaves: The number of octaves for the noise.
            persistence: The persistence of the noise.
            lacunarity: The lacunarity of the noise.
            phasors: Number of phasors to generate.
            freq_range: Frequency range for phasor noise generation.
            anisotropy: Anisotropy parameter for phasor noise generation.
            saw: Whether to apply sawtooth smoothing.
            phasor_amplitude: Amplitude for phasor noise generation.

        Returns:
            A heightmap representing the 'dunes' terrain.
        """
        heightmap = 0.9 * normalize(
            self.noise.fractal_simplex_noise(
                noise="open",
                x_offset=0,
                y_offset=0,
                scale=scale,
                octaves=octaves,
                persistence=persistence,
                lacunarity=lacunarity,
            )
        )

        phasor_noise = self.noise.phasor_noise(
            num_phasors=phasors, freq_range=freq_range, amplitude=1.0, direction_bias=-direction, anisotropy=anisotropy
        )
        if saw:
            phasor_noise = sawtooth(phasor_noise)
        else:
            phasor_noise = normalize(phasor_noise)

        phasor_noise *= phasor_amplitude

        return heightmap + phasor_noise

    def ravines(
        self,
        min_height,
        max_height,
        scale=256,
        persistence=0.45,
        slope_erosion=0.5,
        sharpness=2,
        low_flatness=10,
        ravine_width=0.25,
    ):
        """Generates a parameterized 'ravines' terrain form using uber noise.

        Args:
            min_height: The minimum height of the terrain.
            max_height: The maximum height of the terrain.
            scale: The scale of the noise.
            persistence: The persistence of the noise.
            slope_erosion: The slope erosion of the noise.
            sharpness: The sharpness of the noise.
            low_flatness: The flatness of the low areas.
            ravine_width: The width of the ravines.

        Returns:
            A heightmap representing the 'ravines' terrain.
        """
        noise = normalize(
            self.noise.uber_noise(
                scale=scale,
                octaves=10,
                persistence=persistence,
                lacunarity=2.0,
                sharpness=0,
                feature_amp=1,
                slope_erosion=slope_erosion,
                altitude_erosion=0,
                ridge_erosion=0,
            ),
            -1,
            1,
        )
        heightmap = normalize(1 - np.abs(noise)) ** sharpness
        heightmap = 1 - normalize(heightmap, 0, 1)
        heightmap = low_smooth(heightmap, a=low_flatness, b=ravine_width)
        heightmap = normalize(heightmap, min_height, max_height)
        return heightmap

    def step_desert(self, min_height, max_height, density=50, beta=0.2):
        """Generates a parameterized 'step desert' terrain form using a combination of worley noise and simplex noise.

        Args:
            min_height: The minimum height of the terrain.
            max_height: The maximum height of the terrain.
            density: The density of the worley noise.
            beta: The blending factor for the simplex noise.

        Returns:
            A heightmap representing the 'step desert' terrain.
        """
        worley_idx, points = self.noise.worley_noise(
            density=max(self.width, self.height),
            k=1,
            p=2,
            distribution="poisson",
            radius=density,
            jitter=True,
            jitter_strength=0.3,
            i=1,
            ret_points=True,
        )
        random_numbers = np.random.rand(len(points))
        random_grid = random_numbers[worley_idx]
        simplex = self.noise.fractal_simplex_noise(scale=512, octaves=8, persistence=0.5, lacunarity=2.0)
        alpha = 0.7
        heightmap = normalize(simplex * alpha + random_grid * (1 - alpha), 0.2, 0.3)
        simplex2 = normalize(self.noise.fractal_simplex_noise(scale=1024, octaves=2, persistence=0.5, lacunarity=2.0))
        heightmap = simplex2 * beta + heightmap * (1 - beta)
        return normalize(heightmap, min_height, max_height)

    def cracked_desert(self, min_height, max_height, density=1000, crack_width=0.05, flatness=0.5):
        """Generates a parameterized 'cracked desert' terrain form using a combination of worley noise and simplex noise.

        Args:
            min_height: The minimum height of the terrain.
            max_height: The maximum height of the terrain.
            density: The density of the worley noise.
            crack_width: The width of the cracks in the desert.
            flatness: The flatness of the terrain.

        Returns:
            A heightmap representing the 'cracked desert' terrain.
        """

        worley = self.noise.worley_noise(seed=21, density=density, k=2, p=3, distribution="uniform")
        line_boundaries = normalize(np.abs(worley[..., 0] - worley[..., 1]))
        noise1 = normalize(self.noise.fractal_simplex_noise(scale=128, octaves=7, persistence=0.5, lacunarity=2.0))
        line_boundaries = normalize(line_boundaries + noise1 * 0.05)

        noise2 = normalize(self.noise.fractal_simplex_noise(scale=512, octaves=7, persistence=0.5, lacunarity=2.0))
        mask = line_boundaries < crack_width

        noise3 = normalize(self.noise.fractal_simplex_noise(scale=1024, octaves=2, persistence=0.5, lacunarity=2.0))

        return normalize(
            normalize(np.where(mask, line_boundaries, 0.1) * (noise2 + 0.1)) * 0.05 + noise3 * flatness,
            min_height,
            max_height,
        )

    def oasis(self, min_height, max_height, radial_freq=20, phasor_scale=0.01, lake_size=200):
        """Generates a parameterized 'oasis' terrain form using a combination of simplex noise and phasor noise.

        Args:
            min_height: The minimum height of the terrain.
            max_height: The maximum height of the terrain.
            radial_freq: The frequency of the radial noise.
            phasor_scale: The scale of the phasor noise.
            lake_size: The size of the lake in the oasis.

        Returns:
            A heightmap representing the 'oasis' terrain.
        """
        noise1 = normalize(
            self.noise.fractal_simplex_noise(seed=1, scale=512, octaves=2, persistence=0.4, lacunarity=1.8)
        )

        phasor = self.noise.spiral_phasor_noise(num_phasors=20, freq_range=(30, 40), amplitude=1.0, anisotropy=0.1)

        def gaussian_2d(x, y, mean_x, mean_y, sigma_x, sigma_y):
            return np.exp(-(((x - mean_x) ** 2 / (2 * sigma_x**2)) + ((y - mean_y) ** 2 / (2 * sigma_y**2))))

        X, Y = np.meshgrid(np.arange(self.width), np.arange(self.height))
        center_x, center_y = self.width // 2, self.height // 2
        heightmap = 1 - gaussian_2d(X, Y, center_x, center_y, lake_size, lake_size)

        sand = (normalize(phasor)) * phasor_scale

        heightmap += noise1 * 0.5 + sand
        heightmap = normalize(heightmap, min_height, max_height)

        return heightmap

    def ocean_trenches(self, min_height, max_height, scale=128, low_flatness=20, trench_width=0.5):
        """Generates a parameterized 'ocean trenches' terrain form using uber noise.

        Args:
            min_height: The minimum height of the terrain.
            max_height: The maximum height of the terrain.
            scale: The scale of the noise.
            low_flatness: The flatness of the low areas.
            trench_width: The width of the trenches.

        Returns:
            A heightmap representing the 'ocean trenches' terrain.
        """
        noise = normalize(
            self.noise.uber_noise(
                width=self.width,
                height=self.width,
                scale=scale,
                octaves=10,
                persistence=0.5,
                lacunarity=2.0,
                sharpness=0,
                feature_amp=1,
                altitude_erosion=0,
                slope_erosion=0.9,
                ridge_erosion=0,
            ),
            -1.5,
            1,
        )
        heightmap = normalize(1 - np.abs(noise)) ** 2
        heightmap = high_smooth(
            low_smooth(normalize(1 - normalize(heightmap, 0, 1)), a=low_flatness, b=trench_width), a=15, b=0.9
        )
        heightmap = normalize(heightmap, min_height, max_height)
        return heightmap

    def salt_flats(self):
        """Generates a 'salt flats' terrain form using a combination of worley noise and simplex noise. Currently not used.

        Returns:
            A heightmap representing the 'salt flats' terrain.
        """
        worley = self.noise.worley_noise(
            density=max(self.width, self.height),
            k=2,
            p=3,
            distribution="poisson",
            radius=50,
            jitter=True,
            jitter_strength=0.3,
        )
        line_boundaries = normalize(np.abs(worley[..., 0] - worley[..., 1]))
        worley_lines = (line_boundaries < 0.03).astype(float)

        noise = normalize(self.noise.fractal_simplex_noise(scale=512, octaves=2, persistence=0.4, lacunarity=2.0))
        phasor = normalize(self.noise.phasor_noise(num_phasors=20, freq_range=(80, 90), amplitude=1.0, anisotropy=2))
        heightmap = noise * 0.3 + phasor * 0.005 - worley_lines * 0.01
        return heightmap

    def rocky_field(self, min_height, max_height, rockiness=0.5, field_scale=400, rock_scale=200):
        """Generates a parameterized 'rocky field' terrain form using fractal simplex noise.

        Args:
            min_height: The minimum height of the terrain.
            max_height: The maximum height of the terrain.
            rockiness: The rockiness of the terrain.
            field_scale: The scale of the field noise.
            rock_scale: The scale of the rocky noise.

        Returns:
            A heightmap representing the 'rocky field' terrain.
        """
        noise = normalize(
            self.noise.fractal_simplex_noise(
                noise="open", scale=field_scale, octaves=8, persistence=0.35, lacunarity=2.2
            )
        )
        noise = normalize(noise, 0, 1)

        rocky_noise = self.noise.fractal_simplex_noise(
            seed=self.seed + 1, noise="open", scale=rock_scale, octaves=9, persistence=0.5, lacunarity=2.2
        )
        rocky_noise = np.abs(normalize(rocky_noise, -1, 1))

        thresh = rockiness
        rocky_noise[rocky_noise < thresh] = thresh
        rocky_noise = normalize(rocky_noise, 0, 1)
        heightmap = noise * 0.5 + rocky_noise * 0.5  # + texture_noise*0.2
        heightmap = normalize(heightmap, min_height, max_height)

        return heightmap

    def water_stacks(self, min_height, max_height, scale=64):
        """Generates a parameterized 'water stacks' terrain form using a combination of uber noise and fractal simplex noise.

        Args:
            min_height: The minimum height of the terrain.
            max_height: The maximum height of the terrain.
            scale: The scale of the noise.

        Returns:
            A heightmap representing the 'water stacks' terrain.
        """
        heightmap = self.noise.uber_noise(
            scale=scale,
            octaves=7,
            persistence=0.4,
            lacunarity=2.2,
            sharpness=0.8,
            feature_amp=1,
            slope_erosion=0.5,
            altitude_erosion=0.4,
            ridge_erosion=0,
        )
        heightmap = high_smooth(heightmap, a=10, b=0.8)
        heightmap = normalize(heightmap, 0, 1)
        heightmap = low_smooth(heightmap, a=10, b=0.5)
        scalemap = normalize(
            self.noise.fractal_simplex_noise(seed=self.seed + 1, scale=1024, octaves=3, persistence=0.5, lacunarity=2.0)
        )
        heightmap = heightmap * scalemap * 0.5
        heightmap = normalize(heightmap, min_height, max_height)
        return heightmap

    def generate_multiple_craters(self, radius=150, jitter_strength=0.2):
        """Generates multiple craters within a single heightmap. Currently unused.

        Args:
            radius: The radius of the craters.
            jitter_strength: The strength of the jitter applied to the crater centers.

        Returns:
            A heightmap representing the craters.

        """
        heightmap = np.zeros((self.width, self.height))

        rng = np.random.RandomState(self.seed)

        poisson_disk = qmc.PoissonDisk(2, radius=radius / max(self.width, self.height), seed=rng)
        points = poisson_disk.random(n=max(self.width, self.height), workers=-1)
        points = qmc.scale(points, l_bounds=[0, 0], u_bounds=[self.width, self.height])

        jitter_strength = jitter_strength * radius
        jitter_points = rng.uniform(-jitter_strength, jitter_strength, points.shape)
        points += jitter_points

        for point in points:
            center = point
            radius = int(np.random.beta(2, 5) * (110 - 10) + 10)
            rim_width = np.random.uniform(0.7, 1.2)
            rim_steepness = np.random.uniform(0.42, 0.45)
            floor_height = np.random.uniform(0.5, 0.7)
            smoothness = np.random.uniform(0.5, 1.5)
            rim_smoothness = np.random.uniform(0.4, 0.6)

            crater_height = generate_crater_numba(
                center,
                radius,
                rim_width,
                rim_steepness,
                floor_height,
                smoothness,
                rim_smoothness,
                self.width,
                self.height,
            )
            heightmap += crater_height

        return heightmap

    def craters(self):
        """Generates a 'craters' terrain form using a combination of simplex noise and the multiple craters. Currently unused.

        Returns:
            A heightmap representing the 'craters' terrain.
        """
        craters1 = normalize(self.generate_multiple_craters(radius=150, jitter_strength=0.2))

        noise1 = normalize(
            self.noise.fractal_simplex_noise(seed=1, scale=512, octaves=3, persistence=0.4, lacunarity=1.8)
        )

        noise2 = normalize(
            self.noise.fractal_simplex_noise(seed=2, scale=256, octaves=7, persistence=0.46, lacunarity=2.0)
        )

        heightmap = normalize(noise1 * 0.7 + 1 * craters1 + noise2 * 0.3)
        return heightmap

    def terraced_rice_fields(self, min_height, max_height, num_terraces=10, steepness=3, scale=600):
        """Generates a parameterized 'terraced rice fields' terrain form using a combination of simplex noise and the terrace function.

        Args:
            min_height: The minimum height of the terrain.
            max_height: The maximum height of the terrain.
            num_terraces: The number of terraces in the terrain.
            steepness: The steepness of the terraces.
            scale: The scale of the noise.

        Returns:
            A heightmap representing the 'terraced rice fields' terrain.
        """
        noise1 = self.noise.fractal_simplex_noise(scale=scale, octaves=2, persistence=0.5, lacunarity=2.0)

        terrace1 = self.terrace(noise1, num_terraces=3, steepness=1.5, height_exponent=1)
        terrace2 = self.terrace(terrace1, num_terraces=num_terraces, steepness=steepness, height_exponent=0.8)
        terrace2 = high_smooth(terrace2, a=10, b=0.7)

        noise2 = (self.noise.fractal_simplex_noise(scale=256, octaves=5, persistence=0.4, lacunarity=2.1) + 1) / 2
        heightmap = terrace2 + noise2 * 0.1

        return normalize(heightmap, min_height, max_height)

    def swamp(self, min_height, max_height, wetness):
        """Generates a parameterized 'swamp' terrain form using fractal simplex noise.

        Args:
            min_height: The minimum height of the terrain.
            max_height: The maximum height of the terrain.
            wetness: The wetness factor for the swamp.

        Returns:
            A heightmap representing the 'swamp' terrain.
        """
        noise_map = self.noise.fractal_simplex_noise(scale=128, octaves=8, persistence=0.5, lacunarity=2.0)
        noise_map = normalize(noise_map, min_height, max_height)
        noise_map = np.power(noise_map, wetness)
        if wetness != 1:
            noise_map = normalize(noise_map, min_height, max_height)
        else:
            noise_map = normalize(noise_map, 0.21, max_height)
        return noise_map
