"""Example Usage:

python3 -m master_script.master_script params = "{\
    \"seed\": 123,\
    \"cx\": 100,\
    \"cy\": 100,\
    \"biome\": null,\
    \"debug\": true,\
    \"biome_size\": 30,\
    \"ocean_coverage\": 50,\
    \"land_water_scale\": 20,\
    \"temperate_rainforest\": {\
        \"max_height\": 30\
    },\
    \"boreal_forest\": {\
        \"max_height\": 40\
    },\
    \"grassland\": {\
        \"max_height\": 40\
    },\
    \"tundra\": {\
        \"max_height\": 50\
    },\
    \"savanna\": {\
        \"max_height\": 25\
    },\
    \"woodland\": {\
        \"max_height\": 40\
    },\
    \"tropical_rainforest\": {\
        \"max_height\": 35\
    },\
    \"temperate_seasonal_forest\": {\
        \"max_height\": 90\
    },\
    \"subtropical_desert\": {\
        \"max_height\": 30\
    }\
}"

"""

import argparse
import json
import random
import struct
import sys
import time

# from cellular_automata.voronoi import terrain_voronoi
from concurrent.futures import ProcessPoolExecutor
from copy import deepcopy

import cv2
import matplotlib.pyplot as plt
import numpy as np
from scipy.ndimage import distance_transform_edt

from biomes.climate_map import determine_biomes
from biomes.create_voronoi import get_chunk_polygons
from biomes.land_water_map import determine_landmass
from biomes.midpoint_displacement import midpoint_displacement
from master_script.offload_heightmaps import terrain_voronoi


def fetch_superchunk_data(coords, seed, biome, parameters):
    """Fetches the heightmap data for a superchunk.

    Some terms used: 
        - Global space: The coordinate system with superchunk (0,0) at (0,0)
        - Local space: The coordinate system with the smallest x and y values of the set of polygons that overlap the target superchunk at x = 0 and y = 0.
                       Basically the set of polygons we care about translated so they sit nicely up against the x and y axis, where the coordinate
                       (smallest x, smallest y) in global space is (0, 0) in local space.

    Parameters:
    coords: Chunk coordinates
    seed: Seed value for terrain generation

    Returns:
    superchunk_heightmap: Heightmap data for the superchunk
    reconstructed_image: Image of all polygons that overlapped the superchunk
    biome_image: Image where each pixel is a number representing a biome type
    """
    start_time = time.time()
    strength_factors = [0.2, 0.3, 0.3, 0.4, 0.4]
    chunk_size = 1023

    #This gets information about all polygons that overlap the superchunk region. Outputs:
    # polygon_edges_global_space: List of edges for each polygon, in the form of (start, end) coordinates (currently not used)
    # polygon_points_global_space: List of all points for each polygon
    # shared_edges: List of edges and polygons that share each of them (currently not used)
    # polygon_ids: List of unique IDs for each polygon
    polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids = get_chunk_polygons(coords, seed, chunk_size, parameters)

    #Iteratively apply midpoint displacement to the polygons, strength factors are arbitrarily chosen.
    for strength in strength_factors:
        polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids = midpoint_displacement(polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, strength=strength)
    
    #This assigns a land or water ID to each polygon, and determines the local space coordinates for each polygon. Local space is required when we interact with a noise map when determining land/water and biomes. Outputs:
    # polygon_edges_global_space: List of edges for each polygon, in the form of (start, end) coordinates (currently not used)
    # polygon_points_local_space: List of all points for each polygon, in local space
    # land_water_ids: List of land/water IDs for each polygon (0 for water, 1 for land)
    # slice_parts: Tuple of (start_coords_x, end_coords_x, start_coords_y, end_coords_y) which tell you how "far away" the actual superchunk we want is from the origin in local space.
    # polygon_points_global_space: List of all points for each polygon, in global space
    # offsets: (smallest_x, smallest_y) in global space - needed for knowing where the biome noise map should start w.r.t global space
    polygon_edges_global_space, polygon_points_local_space, land_water_ids, slice_parts, polygon_points_global_space, offsets = determine_landmass(polygon_edges_global_space, polygon_points_global_space, shared_edges, polygon_ids, coords, seed, parameters)

    #This determines the biome for each polygon, and generates an image where each pixel is a number representing a biome type. Outputs:
    # biomes: List of biome IDs for each polygon
    # biome_image: Image where each pixel is a number representing a biome type
    biomes, biome_image = determine_biomes(coords, polygon_edges_global_space, polygon_points_local_space, land_water_ids, offsets, seed, parameters, specified_biome=biome, chunk_size=chunk_size)

    #This generates the heightmap for the superchunk, and returns the heightmap, an image of all polygons that overlapped the superchunk, and the biome image.
    # superchunk_heightmap: Heightmap data for the superchunk
    # reconstructed_image: Image of all polygons that overlapped the superchunk (its big)
    # biome_image: Image where each pixel is a number representing a biome type
    superchunk_heightmap, reconstructed_image, biome_image = terrain_voronoi(polygon_edges_global_space, polygon_points_local_space, slice_parts, polygon_points_global_space, biomes, coords, seed, biome_image, parameters)

    print(f"Overall Time taken: {time.time() - start_time}")
    return superchunk_heightmap, reconstructed_image, biome_image


def main(parameters):
    seed = parameters["seed"]
    cx = parameters["cx"]
    cy = parameters["cy"]
    biome = parameters.get("biome", None)
    debug = parameters.get("debug", False)
    vx = 1026
    vy = 1026
    num_v = vx * vy
    size = 16
    biome_size = 8

    heightmap, _, biome_data = fetch_superchunk_data([cx, cy], seed, biome, parameters)
    heightmap = heightmap.astype(np.uint16)  # Ensure it's uint16
    biome_data = biome_data.astype(np.uint8)

    heightmap_bytes = heightmap.tobytes()
    biome_bytes = biome_data.tobytes()

    header_format = "liiiiiiIiI"
    header = struct.pack(header_format, seed, cx, cy, num_v, vx, vy, size, len(heightmap_bytes), biome_size, len(biome_bytes))
    packed_data = header + heightmap_bytes + biome_bytes
    with open(f"master_script/dump/{seed}_{cx}_{cy}.bin", "wb") as f:
        f.write(packed_data)
    # with open(f"master_script/dump/{seed}_{cx}_{cy}_biome.bin", "wb") as f:
    #     f.write(packed_data)

    if debug:
        header_size = struct.calcsize(header_format)
        unpacked_header = struct.unpack(header_format, packed_data[:header_size])
        unpacked_array = np.frombuffer(packed_data[header_size:header_size + len(heightmap_bytes)], dtype=np.uint16).reshape(1026, 1026)
        # unpacked_biome = np.frombuffer(packed_data[header_size + len(heightmap_bytes) + biome_size:], dtype=np.uint8).reshape(1026, 1026)
        # cv2.imwrite(f"master_script/imgs/{seed}_{cx-200}_{cy-200}_biome.png", unpacked_biome)
        cv2.imwrite(f"master_script/imgs/{seed}_{cx}_{cy}.png", unpacked_array)

        print(f"Unpacked header: {unpacked_header}")
        print(f"Unpacked array shape: {unpacked_array.shape}")
        # print(f"Unpacked biome shape: {unpacked_biome.shape}")

    return heightmap


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Process heightmap data.")
    parser.add_argument("--parameters", type=str, required=True, help="JSON string with all parameters.")

    args = parser.parse_args()

    try:
        parameters = json.loads(args.parameters)
    except json.JSONDecodeError:
        raise ValueError("Invalid JSON format. Ensure the JSON string is correctly formatted.")

    required_keys = {"seed", "cx", "cy"}
    missing_keys = required_keys - parameters.keys()

    if missing_keys:
        raise ValueError(f"Missing required parameters: {', '.join(missing_keys)}")

    main(parameters)
