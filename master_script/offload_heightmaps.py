from utils.voronoi_binary_mask import polygon_to_tight_binary_image
from Noise.generate import noise_in_mask
from cellular_automata.scaling_heightmap import main
import numpy as np
import cv2
import random
import matplotlib.pyplot as plt
from concurrent.futures import ProcessPoolExecutor


def process_polygon(polygon, terrain_type, coords, smallest_points, seed):
        binary_polygon, (min_x, min_y) = polygon_to_tight_binary_image(polygon)
        smallest_x, smallest_y = smallest_points
        
        if terrain_type == 'CA':
            ca_scale = 0.7
            noise_overlay_scale = 0.2
            heightmap = main(1, binary_polygon)
            noise_to_add, sm  = noise_in_mask(binary_polygon, seed, 100, (smallest_x), (smallest_y), octaves=8) 
            heightmap *= ca_scale
            heightmap = heightmap + (noise_to_add*noise_overlay_scale)

        elif terrain_type == 'Noise':
            heightmap, sm = noise_in_mask(binary_polygon, seed, 100, (smallest_x), (smallest_y), octaves=8)

        temp_sm = np.zeros((4000, 4000))
        temp = np.zeros((4000, 4000))  # Image size adjusted to fit the coordinate range 
        temp_sm[min_y:min_y+binary_polygon.shape[0], min_x:min_x+binary_polygon.shape[1]] = sm
        temp[min_y:min_y+binary_polygon.shape[0], min_x:min_x+binary_polygon.shape[1]] = heightmap
        return (temp, temp_sm)

def terrain_voronoi(polygon_coords_edges, polygon_coords_points, slice_parts, pp_copy, biomes, coords, seed):
    range_normalization_factor = 3500
    padding = 370
    (start_coords_x, end_coords_x, start_coords_y, end_coords_y) = slice_parts
    smallest_points_list = []
    polygon_points = []
    coords_list = []
    terrain_types = []
    seed_list = []

    for i, polygon in enumerate(polygon_coords_points):
        polygon_copy = pp_copy[i]
        smallest_x, smallest_y = np.round(np.min(polygon_copy, axis=0)).astype(int)
        smallest_points_list.append((smallest_x, smallest_y))
        polygon_points.append(polygon)
        terrain_types.append('CA' if biomes[i] <= 70 else 'Noise')
        coords_list.append(coords)
        seed_list.append(seed)
    
    def reconstruct_image(polygon_points, terrain_types):
        reconstructed_image = np.zeros((4000, 4000))  
        with ProcessPoolExecutor() as executor: 
            results = executor.map(process_polygon, polygon_points, terrain_types, coords_list, smallest_points_list, seed_list)
        i = 0
        for item in results:
            temp = item[0]
            sm = item[1]
            terrain_type = terrain_types[i]
            if terrain_type == 'CA':
                reconstructed_image = combine_heightmaps(reconstructed_image, temp, sm, "CA")
            elif terrain_type == 'Noise':
                reconstructed_image = combine_heightmaps(reconstructed_image, temp, sm, "Noise")
            i += 1
        
        return reconstructed_image

    reconstructed_image = reconstruct_image(polygon_points, terrain_types)
    reconstructed_image *= range_normalization_factor
    reconstructed_image = (reconstructed_image * 65535).astype(np.uint16) 

    start_coords_x = int(start_coords_x + padding//2)
    start_coords_y = int(start_coords_y + padding//2)
    end_coords_x = int(end_coords_x + padding//2)
    end_coords_y = int(end_coords_y + padding//2)
    superchunk = reconstructed_image[start_coords_y-1:end_coords_y+1, start_coords_x-1:end_coords_x+1]

    return superchunk, reconstructed_image

def combine_heightmaps(old_heightmap, new_heightmap, new_sm, terrain_type, blend_radius=100):
    if terrain_type == "CA":
        new_heightmap = new_heightmap * 0.5

    spread_mask = new_sm
    blending_factor = spread_mask
    blending_factor = np.clip(blending_factor / blend_radius, 0, 1)
    blending_factor = (np.cos(blending_factor * np.pi) + 1) / 2  
    blended_heightmap = old_heightmap * (blending_factor) + new_heightmap * (1 - blending_factor)
    
    return blended_heightmap
