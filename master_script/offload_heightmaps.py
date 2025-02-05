from utils.voronoi_binary_mask import polygon_to_tight_binary_image
from Noise.generate import noise_in_mask
from cellular_automata.scaling_heightmap import main
import numpy as np
import cv2
import random
import matplotlib.pyplot as plt
from concurrent.futures import ProcessPoolExecutor


def process_polygon(polygon, polygon_edges, terrain_type):
        binary_polygon, (min_x, min_y) = polygon_to_tight_binary_image(polygon)
        if terrain_type == 'CA':
            heightmap = main(931, binary_polygon)
            noise_to_add= noise_in_mask(binary_polygon, 931, 100, min_x, min_y, octaves=1, start_frequency=4)
            heightmap = noise_to_add
        elif terrain_type == 'Noise':
            heightmap= noise_in_mask(binary_polygon, 931, 100, min_x, min_y, octaves=4)
            

        temp = np.zeros((4000, 4000))  # Image size adjusted to fit the coordinate range 

        temp[min_y:min_y+binary_polygon.shape[0], min_x:min_x+binary_polygon.shape[1]] = heightmap
        return temp, polygon_edges

def terrain_voronoi(polygon_coords_edges, polygon_coords_points):
    in_frame = []
    in_frame_edges = []
    terrain_types = []
    for i, polygon in enumerate(polygon_coords_points):
        in_frame.append(polygon)
        in_frame_edges.append(polygon_coords_edges[i])
        terrain_types.append('CA' if i % 2 == 0 else 'Noise')
    
    def reconstruct_image(in_frame, terrain_types):
        reconstructed_image = np.zeros((4000, 4000))  
        with ProcessPoolExecutor() as executor: 
            results = executor.map(process_polygon, in_frame, in_frame_edges, terrain_types)
        print("multis done")

        results_arr = []
        for temp in results:
            results_arr.append(temp)


        for i, (temp, temp_edges) in enumerate(results_arr):
                reconstructed_image = blend_in(reconstructed_image, temp, temp_edges)


        
        return reconstructed_image

        

    reconstructed_image = reconstruct_image(in_frame, in_frame_edges, terrain_types)

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


def blend_in(old_heightmap, new_heightmap, new_heightmaps_edges):
    #for each edge make a blending gradient from new to old heightmap
    #then blend the two heightmaps together
    
     
    

def combine_heightmaps(old_heightmap, new_heightmap, old_sm, new_sm):
    new_regions = np.where(old_sm > 0)
    old_regions = np.where(new_sm > 0)

    #get the parts where they are BOTH non zero
    overlap = (old_sm > 0) & (new_sm > 0)

    # plt.imshow(old_sm)
    # plt.title("Old Spread Mask")
    # plt.show()
    # plt.imshow(new_sm)
    # plt.title("New Spread Mask")
    # plt.show()

    blending_region = np.zeros(old_heightmap.shape)
    blending_region[overlap] = new_sm[overlap]
    blending_region = (blending_region - np.min(blending_region)) / (np.max(blending_region) - np.min(blending_region))

    # plt.imshow(blending_region)
    # plt.title("Blending Region")
    # plt.show()

    combined_heightmap = old_heightmap.copy()

    # 1. Handle Overlapping Regions:
    if np.any(overlap):
        old_contribution = old_heightmap[overlap] * (1 - blending_region[overlap])
        new_contribution = new_heightmap[overlap] * blending_region[overlap]
        combined_heightmap[overlap] = old_contribution + new_contribution

    # 2. Handle Non-Overlapping Regions (Corrected):
    old_only = np.logical_and(old_sm > 0, ~overlap)  # Corrected: ~overlap
    new_only = np.logical_and(new_sm > 0, ~overlap)  # Corrected: ~overlap

    combined_heightmap[old_only] = old_heightmap[old_only]
    combined_heightmap[new_only] = new_heightmap[new_only]

    combined_sm = old_sm + new_sm
    return combined_heightmap, combined_sm

    

    # return old_heightmap + (new_heightmap * random.uniform(0.3, 0.6))
