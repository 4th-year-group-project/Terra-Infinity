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

    Parameters:
    coords: Chunk coordinates
    seed: Seed value for terrain generation

    Returns:
    superchunk_heightmap: Heightmap data for the superchunk
    reconstructed_image: Image of all polygons that overlapped the superchunk
    """
    start_time = time.time()
    strength_factors = [0.2, 0.3, 0.3, 0.4, 0.4]
    chunk_size = 1023

    relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids = get_chunk_polygons(coords, seed, chunk_size, parameters)
    og_polygon_points = deepcopy(relevant_polygons_points)

    for strength in strength_factors:
        relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids = midpoint_displacement(relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids, strength=strength)
    land_polygon_edges, polygon_points, polygon_ids, slice_parts, relevant_polygons_og_coord_space, offsets = determine_landmass(relevant_polygons_edges, relevant_polygons_points, og_polygon_points, shared_edges, polygon_ids, coords, seed, parameters)
    biomes, biome_image = determine_biomes(coords, land_polygon_edges, polygon_points, polygon_ids, offsets, seed, specified_biome=biome, chunk_size=chunk_size)

    superchunk_heightmap, reconstructed_image, biome_image = terrain_voronoi(land_polygon_edges, polygon_points, slice_parts, relevant_polygons_og_coord_space, biomes, coords, seed, biome_image, parameters)
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
    with open(f"master_script/dump/{seed}_{cx-200}_{cy-200}.bin", "wb") as f:
        f.write(packed_data)
    with open(f"master_script/dump/{seed}_{cx-200}_{cy-200}_biome.bin", "wb") as f:
        f.write(packed_data)

    if debug:
        header_size = struct.calcsize(header_format)
        unpacked_header = struct.unpack(header_format, packed_data[:header_size])
        unpacked_array = np.frombuffer(packed_data[header_size:header_size + len(heightmap_bytes)], dtype=np.uint16).reshape(1026, 1026)
        # unpacked_biome = np.frombuffer(packed_data[header_size + len(heightmap_bytes) + biome_size:], dtype=np.uint8).reshape(1026, 1026)
        # cv2.imwrite(f"master_script/imgs/{seed}_{cx-200}_{cy-200}_biome.png", unpacked_biome)
        cv2.imwrite(f"master_script/imgs/{seed}_{cx-200}_{cy-200}.png", unpacked_array)

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
