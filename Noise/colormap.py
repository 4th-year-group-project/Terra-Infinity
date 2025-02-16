from matplotlib.colors import LinearSegmentedColormap
import numpy as np

class ColorMap:
    def __init__(self, ocean_threshold=-0.08, coast_threshold=0.01, beach_threshold=0.1, lowland_threshold=0.3, highland_threshold=0.4):
        self.ocean_threshold = ocean_threshold
        self.coast_threshold = coast_threshold
        self.beach_threshold = beach_threshold
        self.lowland_threshold = lowland_threshold
        self.highland_threshold = highland_threshold

        self.ocean_cmap = LinearSegmentedColormap.from_list("ocean", [(0, 0, 0.5), (0.25, 0.45, 0.96)])
        self.coast_cmap = LinearSegmentedColormap.from_list("coast", [(0.19, 0.6, 1), (0.8, 0.9, 1)])
        self.beach_cmap = LinearSegmentedColormap.from_list("beach", [(1, 1, 0.8), (1, 1, 0.5)])
        self.lowland_cmap = LinearSegmentedColormap.from_list("lowland", [(0.2, 0.7, 0.3), (0.0, 0.35, 0.0)])
        self.highland_cmap = LinearSegmentedColormap.from_list("highland", [(0.4, 0.3, 0.1), (0.6, 0.4, 0.2)])
        self.tip_cmap = LinearSegmentedColormap.from_list("highland", [(0.6, 0.4, 0.2), (1, 1, 1)])
        
    def positive_normalize(self):
        self.ocean_threshold = (self.ocean_threshold+1)/2
        self.coast_threshold = (self.coast_threshold+1)/2
        self.beach_threshold = (self.beach_threshold+1)/2
        self.lowland_threshold = (self.lowland_threshold+1)/2
        self.highland_threshold = (self.highland_threshold+1)/2

    def negative_normalize(self):
        self.ocean_threshold = 2*self.ocean_threshold-1
        self.coast_threshold = 2*self.coast_threshold-1
        self.beach_threshold = 2*self.beach_threshold-1
        self.lowland_threshold = 2*self.lowland_threshold-1
        self.highland_threshold = 2*self.highland_threshold-1

    def apply_terrain_colormap(self, noise_map):
        terrain_colored = np.zeros((*noise_map.shape, 3))
        ocean_mask = noise_map <= self.ocean_threshold
        terrain_colored[ocean_mask] = self.ocean_cmap((noise_map[ocean_mask] - noise_map[ocean_mask].min()) /
                                                    (self.ocean_threshold - noise_map[ocean_mask].min()))[:, :3]
        coast_mask = (noise_map > self.ocean_threshold) & (noise_map <= self.coast_threshold)
        terrain_colored[coast_mask] = self.coast_cmap((noise_map[coast_mask] - self.ocean_threshold) /
                                                    (self.coast_threshold - self.ocean_threshold))[:, :3]
        beach_mask = (noise_map > self.coast_threshold) & (noise_map <= self.beach_threshold)
        terrain_colored[beach_mask] = self.beach_cmap((noise_map[beach_mask] - self.coast_threshold) /
                                                    (self.beach_threshold - self.coast_threshold))[:, :3]
        lowland_mask = (noise_map > self.beach_threshold) & (noise_map <= self.lowland_threshold)
        terrain_colored[lowland_mask] = self.lowland_cmap((noise_map[lowland_mask] - self.coast_threshold) /
                                                    (self.lowland_threshold - self.coast_threshold))[:, :3]
        highland_mask = (noise_map > self.lowland_threshold) & (noise_map <= self.highland_threshold)
        terrain_colored[highland_mask] = self.highland_cmap((noise_map[highland_mask] - self.lowland_threshold) /
                                                    (self.highland_threshold - self.lowland_threshold))[:, :3]
        tip_mask = noise_map > self.highland_threshold
        if (terrain_colored[tip_mask].size != 0):
            terrain_colored[tip_mask] = self.tip_cmap((noise_map[tip_mask] - self.highland_threshold) /
                                                    (noise_map[tip_mask].max() - self.highland_threshold))[:, :3]
        return terrain_colored