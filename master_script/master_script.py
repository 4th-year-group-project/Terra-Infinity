from biomes.create_voronoi import get_chunk_polygons
from biomes.land_water_map import determine_landmass
from biomes.climate_map import determine_biomes
# from cellular_automata.voronoi import terrain_voronoi
from cellular_automata.scaling_heightmap import main
from Noise.generate import generate_noise
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

    

    land_polygon_edges, land_polygon_points, slice_parts, relevant_poltgons_og_coord_space, offsets = determine_landmass(relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids, coords,  seed)

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

    superchunk_heightmap, reconstructed_image = terrain_voronoi(land_polygon_edges, land_polygon_points, slice_parts, relevant_poltgons_og_coord_space, biomes, coords)




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
    seed = 3
    coords_1 = [18,26]
    sc_1, rc_1 = fetch_superchunk_data(coords_1, seed)

    coords_2 = [19,26]
    sc_2, rc_2 = fetch_superchunk_data(coords_2, seed)

    coords_3 = [20,26]
    sc_3, rc_3 = fetch_superchunk_data(coords_3, seed)

    coords_4 = [21,26]
    sc_4, rc_4 = fetch_superchunk_data(coords_4, seed)

    coords_5 = [18,25]
    sc_5, rc_5 = fetch_superchunk_data(coords_5, seed)

    coords_6 = [19,25]
    sc_6, rc_6 = fetch_superchunk_data(coords_6, seed)

    coords_7 = [20,25]
    sc_7, rc_7 = fetch_superchunk_data(coords_7, seed)

    coords_8 = [21,25]
    sc_8, rc_8 = fetch_superchunk_data(coords_8, seed)

    coords_9 = [18,24]
    sc_9, rc_9 = fetch_superchunk_data(coords_9, seed)

    coords_10 = [19,24]
    sc_10, rc_10 = fetch_superchunk_data(coords_10, seed)

    coords_11 = [20,24]
    sc_11, rc_11 = fetch_superchunk_data(coords_11, seed)

    coords_12 = [21,24]
    sc_12, rc_12 = fetch_superchunk_data(coords_12, seed)

    coords_13 = [18,23]
    sc_13, rc_13 = fetch_superchunk_data(coords_13, seed)

    coords_14 = [19,23]
    sc_14, rc_14 = fetch_superchunk_data(coords_14, seed)

    coords_15 = [20,23]
    sc_15, rc_15 = fetch_superchunk_data(coords_15, seed)

    


    print(sc_1.shape)
    print(sc_2.shape)
    # print(sc_3.shape)

    sc_1 = (sc_1 * 65535).astype(np.uint16)

    sc_2 = (sc_2 * 65535).astype(np.uint16)

    sc_3 = (sc_3 * 65535).astype(np.uint16)

    sc_4 = (sc_4 * 65535).astype(np.uint16)

    sc_5 = (sc_5 * 65535).astype(np.uint16)

    sc_6 = (sc_6 * 65535).astype(np.uint16)

    sc_7 = (sc_7 * 65535).astype(np.uint16)

    sc_8 = (sc_8 * 65535).astype(np.uint16)


    

    rc_1 = (rc_1 * 65535).astype(np.uint16)

    rc_2 = (rc_2 * 65535).astype(np.uint16)

    # rc_3 = (rc_3 * 65535).astype(np.uint16)

    # Stack the images horizontally
    combined_image_1 = np.hstack((sc_1, sc_2))
    combined_image_1 = np.hstack((combined_image_1, sc_3))
    combined_image_1 = np.hstack((combined_image_1, sc_4))

    combined_image_2 = np.hstack((sc_5, sc_6))
    combined_image_2 = np.hstack((combined_image_2, sc_7))
    combined_image_2 = np.hstack((combined_image_2, sc_8))

    combined_image = np.vstack((combined_image_1, combined_image_2))

    

    combined_big_image = np.hstack((rc_1, rc_2))
    # combined_big_image = np.hstack((combined_big_image, rc_3))
    cv2.imwrite("cellular_automata/terrain_voronoi_combined3.png", combined_image)
    # cv2.imwrite("cellular_automata/terrain_voronoi_combined_big3.png", combined_big_image)

    plt.figure(figsize=(10, 5))
    plt.subplot(1, 2, 1)
    plt.axis('off')
    plt.imshow(sc_1, cmap='gray')
    plt.subplot(1, 2, 2)
    plt.axis('off')
    plt.imshow(sc_2, cmap='gray')
    plt.show()