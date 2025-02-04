from biomes.create_voronoi import get_chunk_polygons
from biomes.land_water_map import determine_landmass
# from biomes.climate_map import determine_biomes
# from cellular_automata.voronoi import terrain_voronoi
from cellular_automata.scaling_heightmap import main
from utils.voronoi_binary_mask import polygon_to_tight_binary_image
from Noise.generate import noise_in_mask
from concurrent.futures import ProcessPoolExecutor
import numpy as np
import cv2
import random
import matplotlib.pyplot as plt

def fetch_superchunk_data(coords, seed):
    relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids = get_chunk_polygons(coords, seed)

    land_polygon_edges, land_polygon_points = determine_landmass(relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids, seed)

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

    superchunk_heightmap, reconstructed_image = terrain_voronoi(land_polygon_edges, land_polygon_points)




    print(superchunk_heightmap.shape)
    plt.figure(figsize=(10, 5))
    plt.subplot(1, 2, 1)
    plt.imshow(superchunk_heightmap, cmap='gray')
    plt.axis('off')
    plt.subplot(1, 2, 2)
    plt.imshow(reconstructed_image, cmap='gray')
    plt.show()
    return superchunk_heightmap

def process_polygon(polygon, terrain_type):
        binary_polygon, (min_x, min_y) = polygon_to_tight_binary_image(polygon)
        if terrain_type == 'CA':
            heightmap = main(931, binary_polygon)
            heightmap = heightmap + noise_in_mask(binary_polygon, 931, 100, min_x, min_y, octaves=1, start_frequency=6)
        elif terrain_type == 'Noise':
            heightmap = noise_in_mask(binary_polygon, 931, 100, min_x, min_y)
            
        
        temp = np.zeros((4000, 4000))  # Image size adjusted to fit the coordinate range 
        temp[min_y:min_y+binary_polygon.shape[0], min_x:min_x+binary_polygon.shape[1]] = heightmap
        return temp

def terrain_voronoi(polygon_coords_edges, polygon_coords_points):
    in_frame = []
    terrain_types = []
    for i, polygon in enumerate(polygon_coords_points):
        in_frame.append(polygon)
        terrain_types.append('CA' if i % 2 == 0 else 'Noise')
    
    def reconstruct_image(in_frame, terrain_types):
        reconstructed_image = np.zeros((4000, 4000))  
        with ProcessPoolExecutor() as executor: 
            results = executor.map(process_polygon, in_frame, terrain_types)
        print("multis done")
        i = 0
        for temp in results:
            #get the corresponding terrain type
            terrain_type = terrain_types[i]
            if terrain_type == 'CA':
                reconstructed_image += (temp * random.uniform(0.4, 0.7))
            elif terrain_type == 'Noise':
                reconstructed_image += (temp * random.uniform(0.3, 0.6))
            i += 1
        
        return reconstructed_image

    reconstructed_image = reconstruct_image(in_frame, terrain_types)

    print(reconstructed_image[2000, 2000])


    reconstructed_image = (reconstructed_image - np.min(reconstructed_image)) / (np.max(reconstructed_image) - np.min(reconstructed_image)) 

    heightmap_scaled = (reconstructed_image * 65535).astype(np.uint16)  # Scale to 16-bit range
    cv2.imwrite("cellular_automata/terrain_voronoi_terrace.png", heightmap_scaled)
    
    # plt.figure(figsize=(4000/100, 4000/100), dpi=100)
    # plt.imshow(reconstructed_image, cmap='gray', vmin=np.min(reconstructed_image), vmax=np.max(reconstructed_image))
    # plt.axis('off')
    # plt.gca().invert_yaxis()
    # # plt.savefig('cellular_automata/terrain_voronoi_inverted.png', bbox_inches='tight', pad_inches=0)
    # plt.show()

    superchunk = reconstructed_image[(-1024 + (1524-(370//2)) + 1024 - 1):(-1024 + (1524-(370//2)) + 1024 + 1024 + 1), (-1024 + (1524-(370//2)) + 1024 - 1):(-1024 + (1524-(370//2)) + 1024 + 1024 + 1)]

    # print(superchunk.shape)
    # plt.imshow(reconstructed_image, cmap='gray', vmin=np.min(reconstructed_image), vmax=np.max(reconstructed_image))
    # plt.axis('off')
    # plt.show()

    return superchunk, reconstructed_image

if __name__ == "__main__":
    seed = 42
    coords = (0,0)
    fetch_superchunk_data(coords, seed)