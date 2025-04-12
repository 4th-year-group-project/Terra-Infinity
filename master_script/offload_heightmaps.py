import os
import random
import time
from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor

import cv2
import matplotlib.pyplot as plt
import numpy as np
from numba import njit, prange, set_num_threads, config
from scipy.ndimage import distance_transform_edt, gaussian_filter

from coastline.geom import GeometryUtils
from master_script.biome_based_terrain_generator import BBTG
from utils.voronoi_binary_mask import polygon_to_tight_binary_image


def scale(biome_number):

    biome_scales = {
        10: 0.9,
        40: 0.8,
        70: 0.7,
        80: 0.6,
    }

    if biome_number in biome_scales:
        return biome_scales[biome_number]
    else:
        return 0.55


def generate_terrain_in_cell(binary_mask, spread_mask, seed, biome_number, smallest_x, smallest_y, parameters):
    bbtg = BBTG(binary_mask, spread_mask, seed, smallest_x, smallest_y, parameters)
    return bbtg.generate_terrain(biome_number)

def process_polygon(polygon, biome_number, coords, smallest_points, seed, parameters, canvas_size):
        binary_polygon, (min_x, min_y) = polygon_to_tight_binary_image(polygon, padding=250)
        smallest_x, smallest_y = smallest_points
        kernel_size = 25
        kernel = np.ones((kernel_size, kernel_size), np.uint8)
        expanded_mask = cv2.dilate(binary_polygon.astype(np.uint8), kernel, iterations=7)
        spread_mask = GeometryUtils.mask_transform(expanded_mask, spread_rate=1)
        spread_mask_blurred = gaussian_filter(spread_mask, sigma=10)
        heightmap = generate_terrain_in_cell(expanded_mask, 1 - np.exp(-12 * spread_mask), seed, biome_number, smallest_x, smallest_y, parameters)
        partial_reconstruction_spread_mask = np.zeros(canvas_size)
        partial_reconstruction_spread_mask_blurred = np.zeros(canvas_size)
        partial_reconstruction = np.zeros(canvas_size)
        partial_reconstruction_spread_mask[min_y:min_y+binary_polygon.shape[0], min_x:min_x+binary_polygon.shape[1]] = spread_mask
        partial_reconstruction_spread_mask_blurred[min_y:min_y+binary_polygon.shape[0], min_x:min_x+binary_polygon.shape[1]] = spread_mask_blurred
        partial_reconstruction[min_y:min_y+binary_polygon.shape[0], min_x:min_x+binary_polygon.shape[1]] = heightmap

        return (partial_reconstruction, partial_reconstruction_spread_mask_blurred)

def terrain_voronoi(polygon_coords_edges, polygon_coords_points, slice_parts, pp_copy, biomes, coords, seed, biome_image, parameters):
    padding = 250
    (start_coords_x, end_coords_x, start_coords_y, end_coords_y) = slice_parts
    smallest_points_list = []
    polygon_points = []
    coords_list = []
    biomes_list = []
    seed_list = []
    parameters_list = []
    max_x_and_y_given_max_of_width_and_height = []
    
    start_time = time.time()
    for i, polygon in enumerate(polygon_coords_points):
        polygon_copy = pp_copy[i]
        polygon_local = polygon_coords_points[i]
        smallest_x, smallest_y = np.round(np.min(polygon_copy, axis=0)).astype(int)
        smallest_points_list.append((smallest_x, smallest_y))
        polygon_points.append(polygon)
        biomes_list.append(biomes[i])
        coords_list.append(coords)
        seed_list.append(seed)
        parameters_list.append(parameters)
        #foe every polygon do the following:
        #1. find the max of its height or its width
        #2. whichever is larger, add that to it's smalest x and smallest y
        #3. out of all tehse values across all polygons, find the largest x and largest y.
        #4. add padding to the largest x and largest y

        max_x = np.max(polygon_local[:, 0])
        max_y = np.max(polygon_local[:, 1])
        min_x = np.min(polygon_local[:, 0])
        min_y = np.min(polygon_local[:, 1])
        width = max_x - min_x
        height = max_y - min_y
        max_of_height_or_width = max(width, height)
        furthest_point_for_this_polygon_x = int(np.ceil(min_x + max_of_height_or_width + padding))
        furthest_point_for_this_polygon_y = int(np.ceil(min_y + max_of_height_or_width + padding))
        max_x_and_y_given_max_of_width_and_height.append((furthest_point_for_this_polygon_x, furthest_point_for_this_polygon_y))

    print("Time taken for processing polygons: ", time.time() - start_time)
    furthest_x = max(max_x_and_y_given_max_of_width_and_height, key=lambda x: x[0])[0]
    furthest_y = max(max_x_and_y_given_max_of_width_and_height, key=lambda x: x[1])[1]
    canvas_size = (furthest_y, furthest_x)

    canvas_size_list = [canvas_size] * len(polygon_points)
    print("Canvas size: ", canvas_size)


    def reconstruct_image(polygon_points, biomes_list):
        reconstructed_image = np.zeros(canvas_size)
        reconstructed_spread_mask = np.zeros(canvas_size)

        #Set num. Numba threads to 1 so that ThreadPoolExecutor's threads don't go on to spawn more threads
        set_num_threads(1)

        #Number of threads working on generating terrain cells
        max_workers = config.NUMBA_DEFAULT_NUM_THREADS - 4
        with ThreadPoolExecutor(max_workers=max_workers) as executor:
            results = executor.map(
                process_polygon,
                polygon_points,
                biomes_list,
                coords_list,
                smallest_points_list,
                seed_list,
                parameters_list,
                canvas_size_list,
            )
            results = list(results)
        # results = []
        # for poly, biome, coord, small_pts, seed_l in zip(polygon_points, biomes_list, coords_list, smallest_points_list, seed_list):
        #     start_time = time.time()
        #     results.append(process_polygon(poly, biome, coord, small_pts, seed_l))
        #     print("Time taken for processing polygon: ", time.time() - start_time)

        s2 = time.time()

        #Back to N - 2 threads for combining heightmaps
        set_num_threads(config.NUMBA_DEFAULT_NUM_THREADS - 4)
        for item in results:
            partial_reconstruction = item[0]
            partial_reconstruction_spread_mask_blurred = item[1]
            reconstructed_image, reconstructed_spread_mask = combine_heightmaps(reconstructed_image, partial_reconstruction, reconstructed_spread_mask, partial_reconstruction_spread_mask_blurred)
        s3 = time.time()
        print(f"Time taken for combining heightmaps: {s3 - s2}")
        return reconstructed_image

    

    reconstructed_image = reconstruct_image(polygon_points, biomes_list)
    reconstructed_image = (reconstructed_image * 65535).astype(np.uint16)

    start_coords_x_terrain = int(start_coords_x + padding//2)
    start_coords_y_terrain = int(start_coords_y + padding//2)
    end_coords_x_terrain = int(end_coords_x + padding//2)
    end_coords_y_terrain = int(end_coords_y + padding//2)
    superchunk = reconstructed_image[start_coords_y_terrain-1:end_coords_y_terrain+2, start_coords_x_terrain-1:end_coords_x_terrain+2]

    biome_image = biome_image / 10
    biome_image = biome_image.astype(np.uint8)
    biome_image = biome_image[start_coords_y-1:end_coords_y+2, start_coords_x-1:end_coords_x+2]
    biome_image = cv2.dilate(biome_image, np.ones((3, 3), np.uint8), iterations=1)

    return superchunk, reconstructed_image, biome_image

@njit(fastmath=True, parallel=True, cache=True)
def combine_heightmaps(old_heightmap, new_heightmap, old_sm, new_sm_blurred):
    # Pre-allocate output arrays
    height, width = old_heightmap.shape
    blended_heightmap = np.empty_like(old_heightmap)
    blended_sm = np.empty_like(old_sm)

    for i in prange(height):
        for j in range(width):
            old_sm_val = old_sm[i, j]
            new_sm_val = new_sm_blurred[i, j]

            sum_mask = old_sm_val + new_sm_val

            inv_sum = 1.0 / (sum_mask if sum_mask > 1e-5 else 1e-5)

            norm_old = old_sm_val * inv_sum
            norm_new = new_sm_val * inv_sum

            blended_heightmap[i, j] = (old_heightmap[i, j] * norm_old +
                                      new_heightmap[i, j] * norm_new)

            blended_sm[i, j] = sum_mask if sum_mask < 1.0 else 1.0

    return blended_heightmap, blended_sm
