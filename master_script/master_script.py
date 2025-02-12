from biomes.create_voronoi import get_chunk_polygons
from biomes.land_water_map import determine_landmass
from biomes.climate_map import determine_biomes
# from cellular_automata.voronoi import terrain_voronoi
from cellular_automata.scaling_heightmap import main
from Noise.generate import noise_in_mask
from concurrent.futures import ProcessPoolExecutor
import numpy as np
import cv2
import random
import matplotlib.pyplot as plt
from scipy.ndimage import distance_transform_edt
from master_script.offload_heightmaps import terrain_voronoi
from copy import deepcopy

def fetch_superchunk_data(coords, seed):
    relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids = get_chunk_polygons(coords, seed)
    # print(relevant_polygons_edges)

    relevant_polygon_points_cp = deepcopy(relevant_polygons_points)

    land_polygon_edges, land_polygon_points, slice_parts, pp_copy, offsets = determine_landmass(relevant_polygons_edges, relevant_polygon_points_cp, shared_edges, polygon_ids, coords,  seed)

    biomes = determine_biomes(coords, land_polygon_edges, land_polygon_points, [1 for i in range(len(land_polygon_edges))], offsets, seed)

    # Next step: Biome assignment
    # biomes = determine_biome(relevant_polygons_edges, relevant_polygons_points, landmass_classifications, seed)
    #            |
    #            |                    
    #            v
    # Next step: Heightmap generation

    #Based on biome information & seed & such, choose methods to generate heightmap
    #Right now just CA heightmap

    # e.g.
    # voronoi_id, gen_method = determine_heightmap_method(relevant_polygons_edges, relevant_polygons_points, seed)

    superchunk_heightmap, reconstructed_image = terrain_voronoi(land_polygon_edges, land_polygon_points, slice_parts, pp_copy, biomes, coords)




    # print(superchunk_heightmap.shape)
    # plt.figure(figsize=(10, 5))
    # plt.subplot(1, 2, 1)
    # plt.imshow(superchunk_heightmap, cmap='gray')
    # plt.axis('off')
    # plt.subplot(1, 2, 2)
    # plt.imshow(reconstructed_image, cmap='gray')
    # plt.show()
    return superchunk_heightmap, reconstructed_image


if __name__ == "__main__":
    seed = 366
    coords_1 = [48,0]
    sc_1, rc_1 = fetch_superchunk_data(coords_1, seed)

    coords_2 = [49,0]
    sc_2, rc_2 = fetch_superchunk_data(coords_2, seed)

    print(sc_1.shape)
    print(sc_2.shape)

    sc_1 = (sc_1 * 65535).astype(np.uint16)


    sc_2 = (sc_2 * 65535).astype(np.uint16)

    rc_1 = (rc_1 * 65535).astype(np.uint16)

    rc_2 = (rc_2 * 65535).astype(np.uint16)

    # Stack the images horizontally
    combined_image = np.hstack((sc_1, sc_2))
    combined_big_image = np.hstack((rc_1, rc_2))
    cv2.imwrite("cellular_automata/terrain_voronoi_combined3.png", combined_image)
    cv2.imwrite("cellular_automata/terrain_voronoi_combined_big3.png", combined_big_image)

    plt.figure(figsize=(10, 5))
    plt.subplot(1, 2, 1)
    plt.axis('off')
    plt.imshow(sc_1, cmap='gray')
    plt.subplot(1, 2, 2)
    plt.axis('off')
    plt.imshow(sc_2, cmap='gray')
    plt.show()