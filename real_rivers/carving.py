import numpy as np
from scipy.ndimage import distance_transform_edt, gaussian_filter
import time

def mask_splines(splines, strahler_numbers, min_x, min_y, max_x, max_y, padding=200):
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
    
    for spline in splines:
        node1, node2 = spline["edge"]
        spline_points = spline["spline_points"]
        river_width = spline["river_width"]
        scale_exponent = spline["scale_exponent"]

        width1 = (strahler_numbers[node1]*river_width)**scale_exponent
        width2 = (strahler_numbers[node2]*river_width)**scale_exponent

        num_points = len(spline_points)
        t_values = np.linspace(0, 1, num_points)
        widths = width1 + t_values * (width2 - width1)

        for i, point in enumerate(spline_points):
            # Adjust coordinates to the padded grid boundaries
            x, y = int(point[0] - padded_min_x), int(point[1] - padded_min_y)
            if 0 <= x < padded_width and 0 <= y < padded_height:
                centerline_grid[y, x] = 1
                width_grid[y, x] = widths[i]

    distance_grid, indices = distance_transform_edt(centerline_grid == 0, return_indices=True)
    closest_widths = width_grid[indices[0], indices[1]]

    padded_mask = distance_grid <= closest_widths
    
    return padded_mask, (padding, original_width, original_height)


def carve_smooth_river_into_terrain(
    heightmap,
    river_mask,
    max_river_width,
    water_threshold=0.2,
    river_depth_factor=0.1,
    influence_distance=20,
    smoothing_sigma=15,
    river_sigma=10,
    depth_variation=0.5,
    river_noise=None,
    noise_strength=0.15
):
    distance_map = distance_transform_edt(1 - river_mask)
    smooth_distance = gaussian_filter(distance_map, sigma=smoothing_sigma)
    normalized_distance = smooth_distance / influence_distance
    falloff = 1.0 / (1.0 + np.exp(-normalized_distance + 2))

    river_width = distance_transform_edt(river_mask)
    normalized_width = river_width / max(max_river_width, 1.0)

    base_river_depth = water_threshold * river_depth_factor
    min_river_depth = base_river_depth
    max_river_depth = base_river_depth * (1 - depth_variation)
    river_depth_map = min_river_depth - (normalized_width**0.1 * (min_river_depth - max_river_depth))

    river_height_map = np.ones_like(heightmap) * water_threshold * 0.9
    river_height_map[river_mask == 1] = river_depth_map[river_mask == 1]
    river_height_map = gaussian_filter(river_height_map, sigma=river_sigma)

    if river_noise is not None:
        river_height_map += river_noise * noise_strength * (1 - river_depth_map) **2

    modified_heightmap = river_height_map * (1 - falloff) + heightmap * falloff
    return modified_heightmap

def remove_padding(array, padding, original_width, original_height):
    return array[padding:padding + original_height, padding:padding + original_width]