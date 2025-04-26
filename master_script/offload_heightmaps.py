import time
from concurrent.futures import ThreadPoolExecutor

import cv2
import numpy as np
from numba import config, njit, prange, set_num_threads
from scipy.ndimage import distance_transform_edt, gaussian_filter

from coastline.geom import GeometryUtils
from master_script.biome_based_terrain_generator import BBTG
from real_rivers.riverize import riverize
from utils.voronoi_binary_mask import polygon_to_tight_binary_image


def map_to_contiguous_ids(biome_image):
    """Maps the old non-contiguous biome IDs to new contiguous IDs.

    The new IDs are:
    1: Boreal Forest Plains
    2: Boreal Forest Hills
    3: Boreal Forest Mountains
    4: Grassland Plains
    5: Grassland Hills
    6: Grassland Rocky Fields
    7: Grassland Terraced Fields
    8: Tundra Plains
    9: Tundra Blunt Mountains
    10: Tundra Pointy Peaks
    11: Savanna Plains
    12: Savanna Mountains
    13: Woodland Hills
    14: Tropical Rainforest Plains
    15: Tropical Rainforest Mountains
    16: Tropical Rainforest Volcanoes
    17: Tropical Rainforest Hills
    18: Temperate Rainforest Hills
    19: Temperate Rainforest Mountains
    20: Temperate Rainforest Swamp
    21: Temperate Seasonal Forest Hills (Autumnal)
    22: Temperate Seasonal Forest Mountains (Autumnal)
    23: Temperate Seasonal Forest Hills (Default)
    24: Temperate Seasonal Forest Mountains (Default)
    25: Desert Terraces
    26: Desert Dunes
    27: Desert Oasis
    28: Desert Ravines
    29: Desert Cracked
    30: Ocean Seabed
    31: Ocean Trenches
    32: Ocean Volcanic Islands
    33: Ocean Water Stacks

    Args:
        biome_image: A 2D numpy array representing the biome IDs for each pixel in the superchunk.
    
    Returns:
        A 2D numpy array with the same shape as the input, but with the biome IDs mapped to contiguous IDs.
    """

    mapping_dict = {
        1: 1,
        2: 2,
        3: 3,
        10: 4,
        11: 5,
        12: 6,
        13: 7,
        20: 8,
        21: 9,
        22: 10,
        30: 11,
        31: 12,
        40: 13,
        50: 14,
        51: 15,
        52: 16,
        53: 17,
        60: 18,
        61: 19,
        62: 20,
        70: 21,
        71: 22,
        72: 23,
        73: 24,
        80: 25,
        81: 26,
        82: 27,
        83: 28,
        84: 29,
        90: 30,
        91: 31,
        92: 32,
        93: 33,
    }
    mapper = np.vectorize(lambda x: mapping_dict.get(x, x))
    return mapper(biome_image)

def generate_terrain_in_cell(binary_mask, spread_mask, seed, biome_number, smallest_x, smallest_y, parameters):
    """Generates terrain in a polygon cell based on the associated biome number.

    Args:
        binary_mask: A binary mask representing the polygon cell.
        spread_mask: A mask representing the spread of polygon cell.
        seed: Seed value for terrain generation.
        biome_number: The biome number associated with the polygon cell.
        smallest_x: The smallest x-coordinate of the polygon in global space.
        smallest_y: The smallest y-coordinate of the polygon in global space.
        parameters: Parameters for terrain generation.

    Returns:
        heightmap: A 2D numpy array representing the generated heightmap.
        tree_points: A list of points where trees are placed.
    """
    bbtg = BBTG(binary_mask, spread_mask, seed, smallest_x, smallest_y, parameters)
    return bbtg.generate_terrain(biome_number)

def process_polygon(polygon, biome_number, coords, smallest_points, seed, parameters):
        """Generates terrain and tree placements for a given polygon.

        Args:
            polygon: The polygon to process.
            biome_number: The biome number associated with the polygon.
            coords: Coordinates of the target superchunk.
            smallest_points: The smallest x and y coordinates of the polygon in global space.
            seed: Seed value for terrain generation.
            parameters: Parameters for terrain generation.

        Returns:
            A tuple containing the generated heightmap, spread mask, and tree placements.
        """
        binary_polygon, (min_x, min_y) = polygon_to_tight_binary_image(polygon)
        smallest_x, smallest_y = smallest_points
        kernel_size = 25
        kernel = np.ones((kernel_size, kernel_size), np.uint8)
        expanded_mask = cv2.dilate(binary_polygon.astype(np.uint8), kernel, iterations=10)
        expanded_mask = cv2.dilate(binary_polygon.astype(np.uint8), kernel, iterations=10)
        spread_mask = GeometryUtils.mask_transform(expanded_mask, spread_rate=1)
        spread_mask_blurred = gaussian_filter(spread_mask, sigma=10)
        heightmap, tree_points = generate_terrain_in_cell(expanded_mask, 1 - np.exp(-12 * spread_mask), seed, biome_number, smallest_x, smallest_y, parameters)
        partial_reconstruction_spread_mask = np.zeros((4500, 4500))
        partial_reconstruction_spread_mask_blurred = np.zeros((4500, 4500))
        partial_reconstruction = np.zeros((4500, 4500))
        partial_reconstruction_spread_mask[min_y:min_y+binary_polygon.shape[0], min_x:min_x+binary_polygon.shape[1]] = spread_mask
        partial_reconstruction_spread_mask_blurred[min_y:min_y+binary_polygon.shape[0], min_x:min_x+binary_polygon.shape[1]] = spread_mask_blurred
        partial_reconstruction[min_y:min_y+binary_polygon.shape[0], min_x:min_x+binary_polygon.shape[1]] = heightmap
        tree_points = [(x + min_x, y + min_y) for x, y in tree_points]

        partial_tree = tree_points
        return (partial_reconstruction, partial_reconstruction_spread_mask_blurred, partial_tree)

def terrain_voronoi(polygon_coords_edges, polygon_coords_points, slice_parts, pp_copy, biomes, coords, seed, biome_image, parameters, river_network):
    """Generates terrain for a superchunk based on the polygons that overlap it based on the biome classifications of the polygons and given parameters.

    Args:
        polygon_coords_edges: List of edges for each polygon in the form of (start, end) coordinates.
        polygon_coords_points: List of all points for each polygon.
        slice_parts: Tuple of (start_coords_x, end_coords_x, start_coords_y, end_coords_y) which tell us where to retrieve the superchunk from the overlapping polygon terrains.
        pp_copy: List of polygon points in global space.
        biomes: List of biome numbers for each polygon.
        coords: Coordinates of the target superchunk.
        seed: World seed value for terrain generation.
        biome_image: Image where each pixel is a number representing a biome type.
        parameters: Parameters for terrain generation.
        river_network: River network data.

    Returns:
        superchunk: The generated heightmap for the superchunk.
        reconstructed_image: Image of all polygons that overlapped the superchunk.
        biome_image: Image where each pixel is a number representing a biome type.
        tree_placements: List of points where trees are placed.
    """
    padding = 370
    (start_coords_x, end_coords_x, start_coords_y, end_coords_y) = slice_parts
    smallest_points_list = []
    polygon_points = []
    coords_list = []
    biomes_list = []
    seed_list = []
    parameters_list = []

    start_coords_x_terrain = int(start_coords_x + padding//2)
    start_coords_y_terrain = int(start_coords_y + padding//2)
    end_coords_x_terrain = int(end_coords_x + padding//2)
    end_coords_y_terrain = int(end_coords_y + padding//2)


    for i, polygon in enumerate(polygon_coords_points):
        polygon_copy = pp_copy[i]
        smallest_x, smallest_y = np.round(np.min(polygon_copy, axis=0)).astype(int)
        smallest_points_list.append((smallest_x, smallest_y))
        polygon_points.append(polygon)
        biomes_list.append(biomes[i])
        coords_list.append(coords)
        seed_list.append(seed)
        parameters_list.append(parameters)

    def reconstruct_image(polygon_points, biomes_list):
        """Constructs the heightmap for each of the polygons based on the polygons and their biomes and combines them into a single heightmap.
        
        Args:
            polygon_points: List of polygon points in global space.
            biomes_list: List of biome numbers for each polygon.

        Returns:
            reconstructed_image: The combined heightmap for all polygons.
            tree_placements: List of points where trees are placed.
        """
        reconstructed_image = np.zeros((1226, 1226))
        reconstructed_spread_mask = np.zeros((1226, 1226))

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
                parameters_list
            )
            results = list(results)

        #Back to N - 2 threads for combining heightmaps
        set_num_threads(config.NUMBA_DEFAULT_NUM_THREADS - 4)
        tree_placements = []
        for item in results:
            # Each item in results contains the heightmap, spread mask, and tree placements for a polygon
            # Each heightmap is combined one after another, with each subsequent heightmap being blended with the previous combined one
            partial_reconstruction = item[0]
            partial_reconstruction_spread_mask_blurred = item[1]
            tree_points = item[2]

            partial_reconstruction_cropped = partial_reconstruction[start_coords_y_terrain-1-100:end_coords_y_terrain+2+100, start_coords_x_terrain-1-100:end_coords_x_terrain+2+100]
            partial_reconstruction_spread_mask_blurred_cropped = partial_reconstruction_spread_mask_blurred[start_coords_y_terrain-1-100:end_coords_y_terrain+2+100, start_coords_x_terrain-1-100:end_coords_x_terrain+2+100]
            reconstructed_image, reconstructed_spread_mask = combine_heightmaps(reconstructed_image, partial_reconstruction_cropped, reconstructed_spread_mask, partial_reconstruction_spread_mask_blurred_cropped)

            tree_placements.extend(tree_points)

        return reconstructed_image, tree_placements

    start = time.time()
    reconstructed_image, tree_placements = reconstruct_image(polygon_points, biomes_list)
    print("Reconstructing image: ", time.time() - start)

    start = time.time()
    reconstructed_image_with_rivers = riverize(reconstructed_image, coords, parameters, river_network)
    print("Riverizing image: ", time.time() - start)

    superchunk = reconstructed_image_with_rivers
    superchunk = (superchunk * 65535).astype(np.uint16)

    start = time.time()
    if tree_placements:

        # Remove trees outside boundary
        trees = [tree for tree in tree_placements if start_coords_x < tree[1] - 370 < end_coords_x + 1 and start_coords_y  < tree[0] - 370 < end_coords_y+ 1]

        # Remove trees that are too close to the edge or too close to the water level
        if len(trees) > 0:
            tree_x, tree_y = zip(*trees, strict=False)
            tree_x_int = np.array(tree_x, dtype=np.int32) - start_coords_y - 370
            tree_y_int = np.array(tree_y, dtype=np.int32) - start_coords_x - 370

            height_values = superchunk[tree_y_int, tree_x_int]
            valid_trees = height_values > (0.25 * 65535)
            tree_placements = list(zip(np.array(tree_x)[valid_trees] - start_coords_y - 370, np.array(tree_y)[valid_trees] - start_coords_x-370, strict=False))
        else:
            tree_placements = []
    else:
        tree_placements = []

    tree_placements = np.array(tree_placements)
    tree_placements = tree_placements.astype(np.float16)
    print("Tree placing: ", time.time() - start)

    start = time.time()
    biome_image = biome_image.astype(np.uint8)
    biome_image = biome_image[start_coords_y-1:end_coords_y+2, start_coords_x-1:end_coords_x+2]
    biome_image = cv2.dilate(biome_image, np.ones((3, 3), np.uint8), iterations=1)

    # Any pixels within the biome image which are classified as ocean but are above the water level are assigned the biome of the nearest land pixel
    land_mask = (biome_image < 90)
    ocean_above_0_2_mask = (biome_image >= 90) & (superchunk >= 0.18*65535)
    distance, indices = distance_transform_edt(~land_mask, return_indices=True)

    rows, cols = np.where(ocean_above_0_2_mask)
    nearest_land_rows = indices[0][rows, cols]
    nearest_land_cols = indices[1][rows, cols]

    biome_image[rows, cols] = biome_image[nearest_land_rows, nearest_land_cols]

    biome_image = map_to_contiguous_ids(biome_image)
    print("Biome image: ", time.time() - start)

    return superchunk, reconstructed_image, biome_image, tree_placements

@njit(fastmath=True, parallel=True, cache=True)
def combine_heightmaps(old_heightmap, new_heightmap, old_sm, new_sm_blurred):
    """Combines two heightmaps using a weighted average based on the spread masks for each heightmap.

    Args:
        old_heightmap: The original heightmap.
        new_heightmap: The new heightmap to blend with the original.
        old_sm: The spread mask for the original heightmap.
        new_sm_blurred: The blurred spread mask for the new heightmap.

    Returns:
        blended_heightmap: The blended heightmap.
        blended_sm: The combined spread mask.
    """
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

