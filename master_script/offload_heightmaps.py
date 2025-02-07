from utils.voronoi_binary_mask import polygon_to_tight_binary_image
from Noise.generate import noise_in_mask
from cellular_automata.scaling_heightmap import main
import numpy as np
import cv2
import random
import matplotlib.pyplot as plt
from concurrent.futures import ProcessPoolExecutor


def process_polygon(polygon, terrain_type):
        binary_polygon, (min_x, min_y) = polygon_to_tight_binary_image(polygon)
        if terrain_type == 'CA':
            heightmap = main(931, binary_polygon)
            noise_to_add, sm  = noise_in_mask(binary_polygon, 931, 100, min_x, min_y, octaves=7) 
            heightmap = heightmap + (0.4 * noise_to_add)
        elif terrain_type == 'Noise':
            heightmap, sm = noise_in_mask(binary_polygon, 931, 100, min_x, min_y, octaves=8)
            
        temp_sm = np.zeros((4000, 4000))
        temp = np.zeros((4000, 4000))  # Image size adjusted to fit the coordinate range 
        temp_sm[min_y:min_y+binary_polygon.shape[0], min_x:min_x+binary_polygon.shape[1]] = sm
        temp[min_y:min_y+binary_polygon.shape[0], min_x:min_x+binary_polygon.shape[1]] = heightmap
        return (temp, temp_sm)

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
        big_sm = np.zeros((4000, 4000))
        with ProcessPoolExecutor() as executor: 
            results = executor.map(process_polygon, in_frame, terrain_types)
        print("multis done")
        i = 0
        for item in results:
            temp = item[0]
            sm = item[1]
            #get the corresponding terrain type
            terrain_type = terrain_types[i]
            if terrain_type == 'CA':
                reconstructed_image = combine_heightmaps(reconstructed_image, temp, sm, "CA")
            elif terrain_type == 'Noise':
                reconstructed_image = combine_heightmaps(reconstructed_image, temp, sm, "Noise")
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

def combine_heightmaps(old_heightmap, new_heightmap, new_sm, terrain_type, blend_radius=100):

    # plt.imshow(old_heightmap, cmap='gray')
    # plt.title("Old Heightmap")
    # plt.show()

    # plt.imshow(new_heightmap, cmap='gray')
    # plt.title("New Heightmap")
    # plt.show()

    if terrain_type == "CA":
        new_heightmap = new_heightmap * np.random.uniform(0.2, 0.5)
    elif terrain_type == "Noise":
        new_heightmap = new_heightmap * np.random.uniform(0.1, 0.25)
        new_heightmap = new_heightmap + np.random.uniform(0.04, 0.1)

    spread_mask = new_sm
    blending_factor = spread_mask
    # Normalize spread mask to [0,1] range within the blend radius
    blending_factor = np.clip(blending_factor / blend_radius, 0, 1)
    blending_factor = (np.cos(blending_factor * np.pi) + 1) / 2  # Smoothstep blend

    # plt.imshow(blending_factor, cmap='gray')
    # plt.title("Blending Factor")
    # plt.show()

    
    # Reverse blending logic to correctly transition between old and new
    blended_heightmap = old_heightmap * (blending_factor) + new_heightmap * (1 - blending_factor)
    
    return blended_heightmap
    

    # return old_heightmap + (new_heightmap * random.uniform(0.3, 0.6))
