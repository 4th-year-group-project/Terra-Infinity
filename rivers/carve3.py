from collections import defaultdict
from generation import Noise, Display, tools
import matplotlib.pyplot as plt
import numpy as np
from scipy.spatial import ckdtree
import time
from matplotlib.path import Path
from skimage.draw import polygon
from scipy.spatial.distance import cdist
from .smooth2 import TreeSpline  
from scipy.ndimage import distance_transform_edt, gaussian_filter, binary_dilation
import cv2

def smooth_min(a, b, k):
    h = np.clip((b - a + k) / (2 * k), 0, 1)
    return a * h + b * (1 - h) - k * h * (1 - h)

def smooth_max(a, b, k):
    k = -k
    h = np.clip((b - a + k) / (2 * k), 0, 1)
    return a * h + b * (1 - h) - k * h * (1 - h)

width, height = 1024, 1024

noise_generator = Noise(seed=0, width=1024, height=1024, x_offset=0, y_offset=0)

centroids = np.array([
    [0, 0],         # Node 0 (not used)
    [512, -200],       # Node 1 (root)
    [768, 220],     # Node 2
    [256, 230],     # Node 3
    [896, 530],     # Node 4
    [630, 590],     # Node 5
    [128, 490],     # Node 6
    [960, 768],     # Node 7
    [789, 808],     # Node 8
    [192, 858],     # Node 9
    [30, 728],      # Node 10
    [512, 250]
])*2 + np.array([0, 500])

edges = [
    (1, 2), (1, 3),          # Root to first level
    (2, 4), (2, 5), (3, 6),   # First to second level
    (4, 7), (4, 8), (6, 9), (6, 10)  # Second to third level
]

strahler_numbers = {
    1: 3,
    2: 2,
    3: 2,
    4: 2,
    5: 1,
    6: 2,
    7: 1,
    8: 1,
    9: 1,
    10: 1,
    11: 1
} 


interest_points = []

def mask_splines(spline_points, strahler_numbers, min_x, min_y, max_x, max_y, river_width=1.5, scale_exponent=3, padding=200):
    # Add padding to the bounds
    padded_min_x = min_x - padding
    padded_min_y = min_y - padding
    padded_max_x = max_x + padding
    padded_max_y = max_y + padding
    
    # Calculate padded grid dimensions
    padded_width = padded_max_x - padded_min_x + 1
    padded_height = padded_max_y - padded_min_y + 1
    
    # Original dimensions for reference
    original_width = max_x - min_x + 1
    original_height = max_y - min_y + 1
    
    centerline_grid = np.zeros((padded_height, padded_width), dtype=int)
    width_grid = np.zeros((padded_height, padded_width), dtype=float)
    
    # First collect all splines that share the same node1
    splines_by_node = {}
    for edge, points in spline_points.items():
        node1, node2 = edge
        if node1 not in splines_by_node:
            splines_by_node[node1] = []
        splines_by_node[node1].append((edge, points))
    
    # Draw regular splines
    for edge, points in spline_points.items():
        node1, node2 = edge
        width1 = (strahler_numbers[node1]*river_width)**scale_exponent
        width2 = (strahler_numbers[node2]*river_width)**scale_exponent
        
        num_points = len(points)
        t_values = np.linspace(0, 1, num_points)
        widths = width1 + t_values * (width2 - width1)
        
        for i, point in enumerate(points):
            # Adjust coordinates to the padded grid boundaries
            x, y = int(point[0] - padded_min_x), int(point[1] - padded_min_y)
            if 0 <= x < padded_width and 0 <= y < padded_height:
                centerline_grid[y, x] = 1
                width_grid[y, x] = widths[i]

    distance_grid, indices = distance_transform_edt(centerline_grid == 0, return_indices=True)
    closest_widths = width_grid[indices[0], indices[1]]

    padded_mask = distance_grid <= closest_widths
    
    # Remove padding to get the original area mask
    # Calculate the slice indices for the original area within the padded grid
    
    return padded_mask, (padding, original_width, original_height)

def carve_smooth_river_into_terrain(
    heightmap, 
    river_mask, 
    water_threshold=0.2,
    river_depth_factor=0.1,    # Controls how deep the river is relative to water_threshold (smaller = deeper)
    influence_distance=20,     # How far the river's influence extends
    smoothing_sigma=10,        # Controls overall smoothness of the transition
    river_sigma=10,             # Controls smoothness within the river itself
    depth_variation=1,        # How much the depth varies between narrow and wide sections (0=uniform)
    river_noise=None,            # Noise factor for river depth variation
    noise_strength=0.15
):
    """
    Carve a smooth river into a terrain heightmap based on a binary river mask with customizable parameters.
    
    Parameters:
    - heightmap: 2D numpy array (1024x1024) with values between 0 and 1
    - river_mask: Binary mask where 1 represents the river network
    - water_threshold: Height value below which terrain is considered water
    - river_depth_factor: Controls how deep the river is (smaller = deeper)
    - influence_distance: How far the river's influence extends
    - smoothing_sigma: Controls overall smoothness of the transition
    - river_sigma: Controls smoothness within the river itself
    - depth_variation: How much the depth varies between narrow and wide sections
    """

    distance_map = distance_transform_edt(1 - river_mask)
    
    smooth_distance = gaussian_filter(distance_map, sigma=smoothing_sigma)
    
    normalized_distance = smooth_distance / influence_distance
    falloff = 1.0 / (1.0 + np.exp(-normalized_distance + 2))
    
    river_width = distance_transform_edt(river_mask)
    print(np.max(river_width))

    max_width = 120
    normalized_width = river_width / max(20, 1.0)
    
    base_river_depth = water_threshold * river_depth_factor
    min_river_depth = base_river_depth
    max_river_depth = base_river_depth * (1 - depth_variation)
    river_depth_map = min_river_depth - (normalized_width**0.1 * (min_river_depth - max_river_depth))
    
    river_height_map = np.ones_like(heightmap) * water_threshold * 0.9
   
    river_height_map[river_mask == 1] = river_depth_map[river_mask == 1]

    river_height_map = gaussian_filter(river_height_map, sigma=river_sigma)

    if river_noise is not None:
        river_height_map += river_noise * (1-river_depth_map) * noise_strength 

    modified_heightmap = river_height_map * (1 - falloff) + heightmap * falloff

    return modified_heightmap

def remove_padding(array, padding, original_width, original_height):
    y_start = padding
    y_end = y_start + original_height
    x_start = padding
    x_end = x_start + original_width
    return array[y_start:y_end, x_start:x_end]




tree = TreeSpline(edges, centroids)
tree.smooth_tree(curviness=0.5, meander=0.5)
#tree.plot_tree()
splines = tree.get_spline_points(resolution=1000)

river_mask00, padding_info = mask_splines(splines, strahler_numbers, min_x=0, min_y=0, max_x=1023, max_y=1023)
padding, original_width, original_height = padding_info
heightmap00 = 1*(noise_generator.fractal_simplex_noise(noise="open", width=1024+2*padding, height=1024+2*padding,
                                                          scale=256, octaves=6, persistence=0.4, lacunarity=2.0, x_offset=0-padding, y_offset=0-padding)+1)/2

river_noise00 = noise_generator.fractal_simplex_noise(seed=noise_generator.seed+1, noise="open", width=1024+2*padding, height=1024+2*padding,
                                                            scale=100, octaves=6, persistence=0.4, lacunarity=2.0, x_offset=0-padding, y_offset=0-padding)
river_noise00 = ((river_noise00+1)/2)**2
new_heightmap00 = carve_smooth_river_into_terrain(heightmap00, river_mask00, river_noise=river_noise00)
new_heightmap00 = remove_padding(new_heightmap00, padding, original_width, original_height)

river_mask01, padding_info = mask_splines(splines, strahler_numbers, min_x=0, min_y=0+1024, max_x=1023, max_y=1023+1024)
padding, original_width, original_height = padding_info
heightmap01 = 1*(noise_generator.fractal_simplex_noise(noise="open", width=1024+2*padding, height=1024+2*padding,
                                                            scale=256, octaves=6, persistence=0.4, lacunarity=2.0, x_offset=0-padding, y_offset=1024-padding)+1)/2
river_noise01 = noise_generator.fractal_simplex_noise(seed=noise_generator.seed+1, noise="open", width=1024+2*padding, height=1024+2*padding,
                                                            scale=100, octaves=6, persistence=0.4, lacunarity=2.0, x_offset=0-padding, y_offset=1024-padding)
river_noise01 = ((river_noise01+1)/2)**2
new_heightmap01 = carve_smooth_river_into_terrain(heightmap01, river_mask01, river_noise=river_noise01)
new_heightmap01 = remove_padding(new_heightmap01, padding, original_width, original_height)

river_mask10, padding_info = mask_splines(splines, strahler_numbers, min_x=0+1024, min_y=0, max_x=1023+1024, max_y=1023)
padding, original_width, original_height = padding_info
heightmap10 = 1*(noise_generator.fractal_simplex_noise(noise="open", width=1024+2*padding, height=1024+2*padding,
                                                            scale=256, octaves=6, persistence=0.4, lacunarity=2.0, x_offset=1024-padding, y_offset=0-padding)+1)/2
river_noise10 = noise_generator.fractal_simplex_noise(seed=noise_generator.seed+1, noise="open", width=1024+2*padding, height=1024+2*padding,
                                                            scale=100, octaves=6, persistence=0.4, lacunarity=2.0, x_offset=1024-padding, y_offset=0-padding)
river_noise10 = ((river_noise10+1)/2)**2
new_heightmap10 = carve_smooth_river_into_terrain(heightmap10, river_mask10, river_noise=river_noise10)
new_heightmap10 = remove_padding(new_heightmap10, padding, original_width, original_height)

river_mask11, padding_info = mask_splines(splines, strahler_numbers, min_x=0+1024, min_y=0+1024, max_x=1023+1024, max_y=1023+1024)
padding, original_width, original_height = padding_info
heightmap11 = 1*(noise_generator.fractal_simplex_noise(noise="open", width=1024+2*padding, height=1024+2*padding,
                                                            scale=256, octaves=6, persistence=0.4, lacunarity=2.0, x_offset=1024-padding, y_offset=1024-padding)+1)/2
river_noise11 = noise_generator.fractal_simplex_noise(seed=noise_generator.seed+1, noise="open", width=1024+2*padding, height=1024+2*padding,
                                                            scale=100, octaves=6, persistence=0.4, lacunarity=2.0, x_offset=1024-padding, y_offset=1024-padding)
river_noise11 = ((river_noise11+1)/2)**2
new_heightmap11 = carve_smooth_river_into_terrain(heightmap11, river_mask11, river_noise=river_noise11)
new_heightmap11 = remove_padding(new_heightmap11, padding, original_width, original_height)


# river_mask01 = mask_splines(splines, strahler_numbers, river_width=1.5, scale_exponent=2, min_x=0, min_y=0+1024, max_x=1023, max_y=1023+1024)
# heightmap01 = 0.8*(noise_generator.fractal_simplex_noise(noise="open", scale=256, octaves=6, persistence=0.4, lacunarity=2.0, x_offset=0, y_offset=1024)+1)/2
# new_heightmap01 = carve_smooth_river_into_terrain(heightmap01, river_mask01, influence_distance=20)

# river_mask10 = mask_splines(splines, strahler_numbers, river_width=1.5, scale_exponent=2, min_x=0+1024, min_y=0, max_x=1023+1024, max_y=1023)
# heightmap10 = 0.8*(noise_generator.fractal_simplex_noise(noise="open", scale=256, octaves=6, persistence=0.4, lacunarity=2.0, x_offset=1024, y_offset=0)+1)/2
# new_heightmap10 = carve_smooth_river_into_terrain(heightmap10, river_mask10, influence_distance=20)

# river_mask11 = mask_splines(splines, strahler_numbers, river_width=1.5, scale_exponent=2, min_x=0+1024, min_y=0+1024, max_x=1023+1024, max_y=1023+1024)
# heightmap11 = 0.8*(noise_generator.fractal_simplex_noise(noise="open", scale=256, octaves=6, persistence=0.4, lacunarity=2.0, x_offset=1024, y_offset=1024)+1)/2
# new_heightmap11 = carve_smooth_river_into_terrain(heightmap11, river_mask11, influence_distance=20)

river_mask00 = remove_padding(river_mask00, padding, original_width, original_height)
river_mask01 = remove_padding(river_mask01, padding, original_width, original_height)
river_mask10 = remove_padding(river_mask10, padding, original_width, original_height)
river_mask11 = remove_padding(river_mask11, padding, original_width, original_height)

top = np.hstack((river_mask00, river_mask10))
bottom = np.hstack((river_mask01, river_mask11))
full_mask = np.vstack((top, bottom))

# heightmap_top = np.hstack((heightmap00, heightmap10))
# heightmap_bottom = np.hstack((heightmap01, heightmap11))
# full_heightmap = np.vstack((heightmap_top, heightmap_bottom))

new_heightmap_top = np.hstack((new_heightmap00, new_heightmap10))
new_heightmap_bottom = np.hstack((new_heightmap01, new_heightmap11))
new_full_heightmap = np.vstack((new_heightmap_top, new_heightmap_bottom))

# # Plot it
# plt.figure(figsize=(10, 10))
# plt.imshow(full_mask, cmap='gray')
# plt.title('Combined River Mask (2048x2048)')
# plt.axis('off')
# chunk_size = 1024
# plt.axhline(y=chunk_size - 0.5, color='red', linewidth=1)  # Horizontal line between rows
# plt.axvline(x=chunk_size - 0.5, color='red', linewidth=1)  # Vertical line between columns

# plt.show()

# new_heightmap = carve_smooth_river_into_terrain(heightmap1, river_mask, influence_distance=20)

print(np.min(new_full_heightmap), np.max(new_full_heightmap))

display = Display(new_full_heightmap, 250, "lush_plains")
display.display_heightmap()
