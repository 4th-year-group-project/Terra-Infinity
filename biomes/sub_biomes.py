from generation import Noise, Display, normalize, domain_warp, low_smooth, high_smooth, sawtooth
import numpy as np
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
        self.width = width
        self.height = height
        self.x_offset = x_offset
        self.y_offset = y_offset
        self.noise = Noise(seed=seed, width=width, height=height, x_offset=x_offset, y_offset=y_offset)

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

    def pointy_peaks(self, 
                            mountain_density=150, 
                            mountain_squareness=2, 
                            mountain_sharpness=2.5, 
                            jitter_strength=0.2, 
                            mountain_smoothness=256, 
                            mountain_pointyness=-0.5,
                            mountain_prominence=0.55, 
                    ):
        worley = (1-normalize(self.noise.worley_noise(density=max(self.width,self.height), 
                                                      k=1, p=mountain_squareness, 
                                                      distribution="poisson", radius=mountain_density,
                                                      jitter=(True if jitter_strength>0 else False), 
                                                      jitter_strength=jitter_strength,
                                                      )))**mountain_sharpness
        noise = normalize(self.noise.uber_noise(scale=mountain_smoothness, octaves=10, persistence=0.48, lacunarity=2.0,
                                        sharpness=mountain_pointyness, feature_amp=1, slope_erosion=0.5, altitude_erosion=0.05, ridge_erosion=0))
        alpha=mountain_prominence
        heightmap = worley * alpha + noise * (1-alpha)
        heightmap = normalize(heightmap, a=0.2, b=1)
        return heightmap
    
    def sheer_peaks(self,
                           sheer_density=50,
                           sheer_squareness=1.3,
                           scale=256-54, 
                           persistence=0.45,
                           sharpness=0.3, 
                           altitude_erosion=0.3, 
                           slope_erosion=0.3,
                           warp_strength=40, 
                           texture_amplitude=0.2
                    ):

            worley_x = normalize(self.noise.worley_noise(seed=self.seed+1, density=sheer_density, k=1, p=sheer_squareness), -1, 1)
            worley_y = normalize(self.noise.worley_noise(seed=self.seed+2, density=sheer_density, k=1, p=sheer_squareness), -1, 1)

            heightmap = normalize(
                self.noise.uber_noise(scale=scale, octaves=5, persistence=persistence, lacunarity=2.0,
                                sharpness=sharpness, feature_amp=1, altitude_erosion=altitude_erosion, slope_erosion=slope_erosion, ridge_erosion=0.1,
                                warp_x=worley_x, warp_y=worley_y, warp_strength=warp_strength),
            )
            texture_noise = normalize(
                 self.noise.fractal_simplex_noise(
                    noise="open", scale=128, octaves=7, persistence=0.48, lacunarity=2.0,
                 )
            )
            
            texture_noise = low_smooth(texture_noise,a=5,b=0.5)*texture_amplitude
            return normalize(heightmap+texture_noise)

    def uber_flats(self, 
                          scale=128, 
                          persistence=0.5, 
                          lacunarity=1.9, 
                          sharpness=0.3, 
                          slope_erosion=0.3, 
                          altitude_erosion=0.2, 
                          ridge_erosion=0.4,
                          warp_strength=15
                    ):
        noise_x = normalize(self.noise.fractal_simplex_noise(scale=256, octaves=4, persistence=0.5, lacunarity=2.0), -1, 1)
        noise_y = normalize(self.noise.fractal_simplex_noise(scale=256, octaves=4, persistence=0.5, lacunarity=2.0), -1, 1)

        heightmap = normalize(
            self.noise.uber_noise(scale=scale, octaves=10, persistence=persistence, lacunarity=lacunarity,
                                  sharpness=sharpness, feature_amp=1, slope_erosion=slope_erosion, altitude_erosion=altitude_erosion, ridge_erosion=ridge_erosion,
                                  warp_x=noise_x, warp_y=noise_y, warp_strength=warp_strength),
        0.2, 0.6)

        return heightmap

    def volcanoes(self,
                  volcano_density=300,
                  jitter=0.3,
                  tau=20,
                  c=0.01,
                  volcano_prominence=0.5,
                  ):
        worley = self.noise.worley_noise(density=max(self.width, self.height), k=1, p=2, 
                                         distribution="poisson", radius=volcano_density, 
                                         jitter=True, jitter_strength=jitter)
        sinusoidal_worley = normalize(((worley.max() - worley)**tau)*np.sin(worley*c))
        noise = normalize(self.noise.fractal_simplex_noise(
            noise="open", scale=256, octaves=10, persistence=0.5, lacunarity=2.0,
        ))

        noise = normalize(
            self.noise.uber_noise(scale=256, octaves=10, persistence=0.5, lacunarity=2.0,
                                sharpness=0.5, feature_amp=1, slope_erosion=0.8, altitude_erosion=0.1, ridge_erosion=0
            )
        )

        alpha = volcano_prominence
        heightmap = sinusoidal_worley * alpha + noise * (1-alpha)
        return normalize(heightmap, a=0.2, b=1) 

    def mesa_terraces(self, 
                      scale=128, 
                      persistence=0.48, 
                      lacunarity=1.8, 
                      ground_flatness=5, 
                      height_exponent=1.1, 
                      num_terraces=5, 
                      steepness=3, 
                      peak_flatness=10
                ):
        noise= normalize(
                self.noise.fractal_simplex_noise(
                    noise="open", scale=scale, octaves=8, persistence=persistence, lacunarity=lacunarity,
                ),
            0,1)
        noise = low_smooth(noise, ground_flatness, b=0.5)

        heightmap = normalize(noise, a=0, b=(num_terraces)**(1/height_exponent))
        heightmap=heightmap**height_exponent
        heightmap = normalize((np.round(heightmap) + np.sign(heightmap-np.round(heightmap))*0.5*(np.abs(2*(heightmap-np.round(heightmap))))**steepness), 0, 1)
        heightmap = normalize(high_smooth(heightmap, a=peak_flatness, b=0.5), 0.2, 0.5)
        return heightmap

    def dune(self, x, p, xm):
        s = np.where((x > 0) & (x < xm), 0.0, 1.0)
        ps1 = p * s + 1.0
        part1 = ps1 * 0.5
        part2 = 1 - np.cos((np.pi / ps1) * ((x - s) / (xm - s)))
        return (part1 * part2) - 1
    
    def generate_dunes(self, frequency=10, noise_scale=5.0, noise_strength=200.0, rotation=-np.pi/4, gap=100, steepness=0.72):
        dim = max(self.width, self.height)
        X, Y = np.meshgrid(np.arange(self.width), np.arange(self.height))
        shift = noise_strength * self.noise.fractal_simplex_noise(noise="open", x_offset=0, y_offset=0, scale=dim/noise_scale, octaves=1, persistence=0.5, lacunarity=2.0)

        xb = X * np.cos(rotation) - Y * np.sin(rotation)
        xa = (frequency * xb + shift) % (dim + gap)
        xa = np.clip(xa, 0, dim )
        normalized_xa = xa / dim

        heightmap = self.dune(normalized_xa, 1, steepness)
        return heightmap
    
    def generate_radial_dunes(self, frequency=10, noise_scale=5.0, noise_strength=200.0, center=None, gap=100, steepness=0.72):
        dim = max(self.width, self.height)
        X, Y = np.meshgrid(np.arange(self.width), np.arange(self.height))

        # Calculate the distance from each point (X, Y) to the center (cx, cy)
        cx, cy = self.width // 2, self.height // 2 if center == None else center
        distance = np.sqrt((X - cx)**2 + (Y - cy)**2)
        
        # Generate noise and shift
        shift = noise_strength * self.noise.fractal_simplex_noise(noise="open", x_offset=0, y_offset=0, scale=dim/noise_scale, octaves=1, persistence=0.5, lacunarity=2.0)
        
        # Apply the frequency and shift based on the radial distance
        xa = (frequency * distance + shift) % (dim + gap)
        xa = np.clip(xa, 0, dim)
        normalized_xa = xa / dim

        # Generate the heightmap using the dune function
        heightmap = self.dune(normalized_xa, 1, steepness)
        return heightmap
    
    def dunes(self,
              direction=np.pi/4,
              core_freq=3, core_noise_scale=2, core_noise_strength=200, core_gap=3, core_steepness=0.72,
              second_freq=10, second_noise_scale=2, second_noise_strength=500, second_gap=0.5, second_amplitude=0.1, second_steepness=0.72,
              third_freq=20, third_noise_scale=8, third_noise_strength=100, third_gap=1, third_amplitude=0, third_steepness=0.72,
              phasors=20, freq_range=(30, 40), anisotropy=1, phasor_amplitude=0.007, saw=False):
        dunes1 = normalize(self.generate_dunes(frequency=core_freq, noise_scale=core_noise_scale, noise_strength=core_noise_strength, rotation=direction, gap=core_gap, steepness=core_steepness))
        dunes2 = normalize(self.generate_dunes(frequency=second_freq, noise_scale=second_noise_scale, noise_strength=second_noise_strength, rotation=direction, gap=second_gap, steepness=second_steepness))
        dunes3 = normalize(self.generate_dunes(frequency=third_freq, noise_scale=third_noise_scale, noise_strength=third_noise_strength, rotation=direction, gap=third_gap, steepness=third_steepness))
        heightmap = normalize(
            dunes1 + second_amplitude*dunes2 + third_amplitude*dunes3, 0, 0.5
        )

        phasor_noise = self.noise.phasor_noise(num_phasors=phasors, freq_range=freq_range, amplitude=1.0, direction_bias=-direction, anisotropy=anisotropy)
        if saw:
            phasor_noise = sawtooth(phasor_noise)
        else:
            phasor_noise = normalize(phasor_noise)

        heightmap = heightmap + phasor_noise*phasor_amplitude
        return heightmap

    def phasor_dunes(self,
                     direction=-np.pi/4,
                     scale=512, octaves=3, persistence=0.4, lacunarity=1.4,
                     phasors=10, freq_range=(30, 40), anisotropy=2,
                     saw=True, phasor_amplitude=0.009
                     ):
        heightmap = 0.9*normalize(
            self.noise.fractal_simplex_noise(
                noise="open", x_offset=0, y_offset=0, scale=scale, octaves=octaves, persistence=persistence, lacunarity=lacunarity
            )
        )
       
        phasor_noise = self.noise.phasor_noise(num_phasors=phasors, freq_range=freq_range, amplitude=1.0, direction_bias=-direction, anisotropy=anisotropy)
        if saw:
            phasor_noise = sawtooth(phasor_noise)
        else:
            phasor_noise = normalize(phasor_noise)
            
        phasor_noise *= phasor_amplitude

        return heightmap+phasor_noise

    def ravines(self, 
                scale=256,
                persistence=0.45,
                slope_erosion=0.5,
                sharpness=2,
                low_flatness=10,
                ravine_width=0.25   
            ):
        noise = normalize(
            self.noise.uber_noise(scale=scale, octaves=10, persistence=persistence, lacunarity=2.0,
                             sharpness=0, feature_amp=1, slope_erosion=slope_erosion, altitude_erosion=0, ridge_erosion=0.2),                
        -1, 1)
        heightmap = normalize(1-np.abs(noise))**sharpness
        heightmap = 1-normalize(heightmap, 0, 1)
        heightmap = low_smooth(heightmap, a=low_flatness, b=ravine_width)

        return heightmap
    
    def step_desert(self): #parameterize
        worley_idx, points = self.noise.worley_noise(density=max(self.width, self.height), k=1, p=2, distribution="poisson", radius=50, jitter=True, jitter_strength=0.3, i=1, ret_points=True)
        random_numbers = np.random.rand(len(points))
        random_grid = random_numbers[worley_idx]
        simplex = self.noise.fractal_simplex_noise(scale=512, octaves=8, persistence=0.5, lacunarity=2.0)
        alpha=0.7
        heightmap = normalize(simplex*alpha + random_grid*(1-alpha), 0.2, 0.3)
        simplex2 = normalize(self.noise.fractal_simplex_noise(scale=1024, octaves=2, persistence=0.5, lacunarity=2.0))
        heightmap = simplex2*0.2 + heightmap*0.8
        return heightmap
    
    def cracked_desert(self): #parameterize
        worley = self.noise.worley_noise(density=max(self.width, self.height), k=2, p=3, distribution="poisson", radius=50, jitter=True, jitter_strength=0.3)
        line_boundaries = normalize(np.abs(worley[..., 0] - worley[..., 1]))
        noise1 = normalize(self.noise.fractal_simplex_noise(scale=128, octaves=7, persistence=0.5, lacunarity=2.0))
        line_boundaries = normalize(line_boundaries + noise1*0.05)
        
        noise2 = normalize(self.noise.fractal_simplex_noise(scale=512, octaves=7, persistence=0.5, lacunarity=2.0))
        mask = line_boundaries < 0.05

        noise3 = normalize(self.noise.fractal_simplex_noise(scale=1024, octaves=2, persistence=0.5, lacunarity=2.0))

        return normalize(np.where(mask, line_boundaries, 0.1)*(noise2+0.1))*0.05 + noise3*0.2

    def oasis(self):
        noise1 = normalize(self.noise.fractal_simplex_noise(seed=1, scale=512, octaves=2, persistence=0.4, lacunarity=1.8))

        phasor = sub.noise.phasor_noise(
            num_phasors=20, freq_range=(30, 40), amplitude=1.0, anisotropy=2
        )

        def gaussian_2d(x, y, mean_x, mean_y, sigma_x, sigma_y):
            return np.exp(-(((x - mean_x)**2 / (2 * sigma_x**2)) + ((y - mean_y)**2 / (2 * sigma_y**2))))

        X, Y = np.meshgrid(np.arange(1024), np.arange(1024))
        heightmap = 1-gaussian_2d(X, Y, 512, 512, 200, 200)

        dunes = normalize(self.generate_radial_dunes(frequency=20, noise_scale=10.0, noise_strength=250.0, gap=50, steepness=0.3))

        heightmap += noise1*0.5 + dunes*0.01 + normalize(phasor)*0.002
        return heightmap

    def ocean_trenches(self):
        noise = normalize(
            self.noise.uber_noise(width=1024, height=1024,
                            scale=128, octaves=10, persistence=0.5, lacunarity=2.0,
                            sharpness=0, feature_amp=1,
                            altitude_erosion=0, slope_erosion=0.9, ridge_erosion=0.2),
        -1, 1)
        heightmap = normalize(1-np.abs(noise))**2
        heightmap = high_smooth(low_smooth(normalize(1-normalize(heightmap, 0, 1)), a=20, b=0.15), a=15, b=0.9)
        heightmap = normalize(heightmap, 0, 0.2)
        return heightmap

    def salt_flats(self):
        worley = self.noise.worley_noise(density=max(self.width, self.height), k=2, p=3, distribution="poisson", radius=50, jitter=True, jitter_strength=0.3)
        line_boundaries = normalize(np.abs(worley[..., 0] - worley[..., 1]))
        worley_lines = (line_boundaries < 0.03).astype(float)

        noise = normalize(self.noise.fractal_simplex_noise(scale=512, octaves=2, persistence=0.4, lacunarity=2.0))
        phasor = normalize(self.noise.phasor_noise(num_phasors=20, freq_range=(80, 90), amplitude=1.0, anisotropy=2))
        heightmap = noise*0.3 + phasor*0.005 - worley_lines*0.01
        return heightmap

    def rocky_field(self):
        noise = normalize(self.noise.fractal_simplex_noise(noise="open", scale=512, octaves=3, persistence=0.5, lacunarity=2.0))
        noise = normalize(noise, 0, 1)

        rocky_noise = self.noise.fractal_simplex_noise(seed=self.seed+1, noise="open", scale=512, octaves=10, persistence=0.5, lacunarity=2.2)
        rocky_noise = np.abs(normalize(rocky_noise, -1, 1))

        texture_noise = self.noise.fractal_simplex_noise(seed=self.seed+2, noise="open", x_offset=0, y_offset=0, scale=128, octaves=4, persistence=0.5, lacunarity=2.0)
        texture_noise = normalize(texture_noise, 0, 1)

        rocky_noise[rocky_noise < 0.5] = 0.5
        rocky_noise = normalize(rocky_noise, 0, 1)
        heightmap = noise*0.5 + rocky_noise*0.5 + texture_noise*0.05
        heightmap = normalize(heightmap, 0, 0.4)

        return heightmap

    def glacial_fjords(self):
        pass

    def karst_landscapes(self):
        pass

    def terraced_rice_fields(self):
        pass

    def badlands(self):
        pass

    def craters(self):
        pass

    def tabletop_mountains(self):
        pass

    def beaches(self):
        pass

    def geothermal_fields(self):
        pass

    def bog(self):
        pass


sub = Sub_Biomes(seed=43, width=1024, height=1024, x_offset=0, y_offset=0)

heightmap = sub.rocky_field()

display = Display(heightmap, height_scale=250, colormap="rocky_field")
display.display_heightmap()