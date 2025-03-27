import argparse
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


def fetch_superchunk_data(coords, seed, biome, **kwargs):
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

    relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids = get_chunk_polygons(coords, seed, chunk_size=chunk_size, **kwargs)
    og_polygon_points = deepcopy(relevant_polygons_points)

    for strength in strength_factors:
        relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids = midpoint_displacement(relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids, strength=strength)
    land_polygon_edges, polygon_points, polygon_ids, slice_parts, relevant_polygons_og_coord_space, offsets = determine_landmass(relevant_polygons_edges, relevant_polygons_points, og_polygon_points, shared_edges, polygon_ids, coords, seed, **kwargs)
    biomes, biome_image = determine_biomes(coords, land_polygon_edges, polygon_points, polygon_ids, offsets, seed, specified_biome=biome, chunk_size=chunk_size, **kwargs)

    superchunk_heightmap, reconstructed_image, biome_image = terrain_voronoi(land_polygon_edges, polygon_points, slice_parts, relevant_polygons_og_coord_space, biomes, coords, seed, biome_image)
    print(f"Overall Time taken: {time.time() - start_time}")
    return superchunk_heightmap, reconstructed_image, biome_image


def main(seed, cx, cy, biome, debug, **kwargs):
    vx = 1023
    vy = 1023
    num_v = vx * vy
    size = 16
    biome_size = 8

    heightmap, _, biome_data = fetch_superchunk_data([cx, cy], seed, biome, **kwargs)
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
        unpacked_array = np.frombuffer(packed_data[header_size:header_size + len(heightmap_bytes)], dtype=np.uint16).reshape(1023, 1023)
        # unpacked_biome = np.frombuffer(packed_data[header_size + len(heightmap_bytes) + biome_size:], dtype=np.uint8).reshape(1026, 1026)
        # cv2.imwrite(f"master_script/imgs/{seed}_{cx-200}_{cy-200}_biome.png", unpacked_biome)
        cv2.imwrite(f"master_script/imgs/{seed}_{cx-200}_{cy-200}.png", unpacked_array)

        print(f"Unpacked header: {unpacked_header}")
        print(f"Unpacked array shape: {unpacked_array.shape}")
        # print(f"Unpacked biome shape: {unpacked_biome.shape}")

    return heightmap

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Process heightmap data.")
    parser.add_argument("--seed", type=int, required=True, help="Seed value for terrain generation.")
    parser.add_argument("--cx", type=int, required=True, help="Chunk X coordinate.")
    parser.add_argument("--cy", type=int, required=True, help="Chunk Y coordinate.")
    parser.add_argument("--biome", type=int, required=False, help="Biome number.")
    parser.add_argument("--debug", action="store_true", help="Display debug information.")
    parser.add_argument("--biome_size", type=int, required=True, help="Biome size (0-100).")
    parser.add_argument("--ocean_coverage", type=int, required=False, help="Ocean coverage (0-100).")
    parser.add_argument("--land_water_scale", type=int, help="Land/Water Scale (0-100) (read the user manual).")
    parser.add_argument("--Warmth", type=int, help="Warmth (0-100).")
    parser.add_argument("--Wetness", type=int, help="Wetness (0-100).")
    

    args = parser.parse_args()

    args_dict = vars(args)

    parameter_kwargs = {k: v for k, v in args_dict.items() if k not in ["seed", "cx", "cy", "biome", "debug"]}

    main(args.seed, args.cx, args.cy, args.biome, args.debug, **parameter_kwargs)
