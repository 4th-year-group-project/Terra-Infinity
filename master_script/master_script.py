from biomes.create_voronoi import get_chunk_polygons
from biomes.land_water_map import determine_landmass
from biomes.climate_map import determine_biomes
# from cellular_automata.voronoi import terrain_voronoi
from cellular_automata.scaling_heightmap import main
from Noise.generate import generate_noise
from concurrent.futures import ProcessPoolExecutor
import numpy as np
import cv2
import struct
import random
import matplotlib.pyplot as plt
from scipy.ndimage import distance_transform_edt
from master_script.offload_heightmaps import terrain_voronoi
from copy import deepcopy
from biomes.midpoint_displacement import midpoint_displacement
import argparse
import sys

def fetch_superchunk_data(coords, seed):
    relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids = get_chunk_polygons(coords, seed)
    og_polygon_points = deepcopy(relevant_polygons_points)
    relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids = midpoint_displacement(relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids, strength=0.2)
    relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids = midpoint_displacement(relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids, strength=0.3)
    relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids = midpoint_displacement(relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids, strength=0.3)
    relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids = midpoint_displacement(relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids, strength=0.4)
    relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids = midpoint_displacement(relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids, strength=0.4)
    land_polygon_edges, land_polygon_points, slice_parts, relevant_polygons_og_coord_space, offsets = determine_landmass(relevant_polygons_edges, relevant_polygons_points, og_polygon_points, shared_edges, polygon_ids, coords, seed)
    biomes = determine_biomes(coords, land_polygon_edges, land_polygon_points, [1 for i in range(len(land_polygon_edges))], offsets, seed)
    superchunk_heightmap, reconstructed_image = terrain_voronoi(land_polygon_edges, land_polygon_points, slice_parts, relevant_polygons_og_coord_space, biomes, coords, seed)

    return superchunk_heightmap, reconstructed_image


def main(seed, cx, cy):
    vx = 1026
    vy = 1026
    num_v = vx * vy
    size = 16

    heightmap, _ = fetch_superchunk_data([cx, cy], seed)
    heightmap = heightmap.astype(np.uint16)  # Ensure it's uint16
    heightmap_bytes = heightmap.tobytes()

    plt.figure()
    plt.imshow(heightmap, cmap='gray')

    plt.show()


    header_format = 'liiiiiiI'
    header = struct.pack(header_format, seed, cx, cy, num_v, vx, vy, size, len(heightmap_bytes))
    packed_data = header + heightmap_bytes
    with open("output.bin", "wb") as f:
        f.write(packed_data)

    print(f"Packed size: {len(packed_data)} bytes")

    header_size = struct.calcsize(header_format)
    unpacked_header = struct.unpack(header_format, packed_data[:header_size])
    unpacked_array = np.frombuffer(packed_data[header_size:], dtype=np.uint16).reshape(1026, 1026)
    

    plt.figure()
    plt.imshow(unpacked_array, cmap='gray')
    plt.title(f"Heightmap (Seed: {seed}, CX: {cx}, CY: {cy})")
    plt.show()

    print(f"Unpacked header: {unpacked_header}")
    print(f"Unpacked array shape: {unpacked_array.shape}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Process heightmap data.")
    parser.add_argument("--seed", type=int, required=True, help="Seed value for terrain generation.")
    parser.add_argument("--cx", type=int, required=True, help="Chunk X coordinate.")
    parser.add_argument("--cy", type=int, required=True, help="Chunk Y coordinate.")

    args = parser.parse_args()
    main(args.seed, args.cx, args.cy)




