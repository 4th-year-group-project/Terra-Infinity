import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial import Voronoi
import matplotlib.path as mpath
from perlin_noise import PerlinNoise
import cv2
import random
from PIL import Image, ImageDraw
from cellular_automata.scaling_heightmap import main
from scipy.ndimage import zoom
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
from biomes.create_voronoi import get_chunk_polygons
import sys

def biomes_voronoi(points):
    points = points / 18
    x_min, x_max = 0, 3078/18
    y_min, y_max = 0, 3078/18

    noise2 = PerlinNoise(octaves=10, seed=110)
    noise1 = PerlinNoise(octaves=4, seed=114)
    noise = lambda x: noise1([x[0], x[1]]) + 0.7 * noise2([x[0], x[1]])
    xpix, ypix = int(3078/18), int(3078/18)
    pic = [[noise([i/xpix, j/ypix]) for j in range(xpix)] for i in range(ypix)]
    threshold = -0.02
    pic = np.array(pic)


    pic[pic > threshold] = 1
    pic[pic <= threshold] = 0
    binary_image = pic

    adjacency_dict = {}
    for i in range(len(points)):
        adjacency_dict[i] = set()

    vor = Voronoi(points)
    colors = ['green', 'yellow', 'darkolivegreen', 'orange']

    fig, ax = plt.subplots(1, 3, figsize=(15, 5))
    fig.tight_layout()
    ax[0].scatter(points[:, 0]*18, points[:, 1]*18, color='blue', label='Input Points', s=2)

    for region_idx, region in enumerate(vor.regions):
        if not region or not is_polygon_in_frame(region, vor.vertices, x_min, x_max, y_min, y_max):
            continue

        polygon = vor.vertices[region]
        if is_polygon_covering_image(polygon, binary_image):
            random_color = random.choice(colors)
            ax[0].fill(*zip(*polygon *18), color=random_color, edgecolor='black', alpha=0.5)
        else:
            ax[0].fill(*zip(*polygon *18), color='blue', edgecolor='black', alpha=0.5)

        ax[0].annotate(str(region_idx), (polygon[0][0]*18, polygon[0][1]*18), 
                    xytext=(3, 3), textcoords='offset points', fontsize=5)

    ax[0].set_xlim(0, 3078)
    ax[0].set_ylim(0, 3078)
    ax[0].set_title("Voronoi Diagram with Regions Based on Binary Image")
    ax[0].invert_yaxis()

    ax[1].imshow(binary_image, cmap='gray')


    pic_og = [[noise([i/xpix, j/ypix]) for j in range(xpix)] for i in range(ypix)]
    ax[2].imshow(pic_og, cmap='gray')
    plt.show()

def process_polygon(polygon):
        binary_polygon, (min_x, min_y) = polygon_to_tight_binary_image(polygon)
        heightmap = main(991, binary_polygon)  # Assume 'main' returns a heightmap
        temp = np.zeros((4000, 4000))  # Image size adjusted to fit the coordinate range 
        temp[min_y:min_y+binary_polygon.shape[0], min_x:min_x+binary_polygon.shape[1]] = heightmap
        return temp

def terrain_voronoi(points, seed):
    polygon_coords_edges, polygon_coords_points = get_chunk_polygons(points, seed)
    in_frame = []
    for polygon in polygon_coords_points:

        in_frame.append(polygon)
    
    def reconstruct_image(in_frame):
        reconstructed_image = np.zeros((4000, 4000))  
        i = 0
        with ProcessPoolExecutor() as executor:  # Use ProcessPoolExecutor for CPU-bound tasks
            results = executor.map(process_polygon, in_frame)
        print("multis done")
        for temp in results:
            reconstructed_image += (temp * random.uniform(0.6, 1))
            i += 1
        
        return reconstructed_image

    reconstructed_image = reconstruct_image(in_frame)

    #normalise to 0 to 255
    reconstructed_image = (reconstructed_image - np.min(reconstructed_image)) / (np.max(reconstructed_image) - np.min(reconstructed_image)) * 255
    
    plt.figure(figsize=(4000/100, 4000/100), dpi=100)
    plt.imshow(reconstructed_image, cmap='gray', vmin=np.min(reconstructed_image), vmax=np.max(reconstructed_image))
    plt.axis('off')
    plt.gca().invert_yaxis()
    plt.savefig('cellular_automata/terrain_voronoi_inverted.png', bbox_inches='tight', pad_inches=0)
    plt.show()

    superchunk = reconstructed_image[(-1024 + (1524-(370//2)) + 1024 - 1):(-1024 + (1524-(370//2)) + 1024 + 1024 + 1), (-1024 + (1524-(370//2)) + 1024 - 1):(-1024 + (1524-(370//2)) + 1024 + 1024 + 1)]

    print(superchunk.shape)
    plt.imshow(superchunk, cmap='gray', vmin=np.min(reconstructed_image), vmax=np.max(reconstructed_image))
    plt.axis('off')
    plt.show()

    return reconstructed_image



def polygon_to_tight_binary_image(polygon, padding=370, img_size=4000):
    """
    Create a binary image for a polygon with the original bounding box and padding around it.
    The polygon is mapped to a 4000x4000 grid, with its position adjusted based on the original coordinates.
    
    Parameters:
    - polygon: Array of vertices defining the polygon.
    - padding: The amount of padding to add around the bounding box (default is 170).
    - img_size: The size of the final image (default is 4000).
    
    Returns:
    - binary_image_np: Binary image with polygon.
    - (min_x, min_y): Top-left corner of the polygon relative to the final image.
    """
    # Get the bounding box of the polygon
    min_x, min_y = np.round(np.min(polygon, axis=0)).astype(int)
    max_x, max_y = np.round(np.max(polygon, axis=0)).astype(int)
    
    # Compute the width and height of the bounding box
    width = max_x - min_x
    height = max_y - min_y
    
    # Add padding to the bounding box
    min_x -= padding
    min_y -= padding
    width += padding
    height += padding
    
    # Ensure the bounding box is square (optional)
    side_length = max(width, height)
    
    # Find the minimum x and y values of the polygon and apply the offset
    offset_x = min_x + 1524
    offset_y = min_y + 1524

    uffset_x = -(min_x + padding/2)
    uffset_y = -(min_y + padding/2)
    
    # Create a blank binary image with the size of the new bounding box with padding
    binary_image = Image.new("1", (int(side_length), int(side_length)), 0)
    draw = ImageDraw.Draw(binary_image)
    
    # Adjust the polygon coordinates relative to the new (min_x, min_y)
    polygon_tuples = [(coord[0] + uffset_x, coord[1] + uffset_y) for coord in polygon]
    
    # print(f"Adjusted Polygon Tuples: {polygon_tuples}")
    
    # Draw the polygon (fill with white)
    draw.polygon(polygon_tuples, outline=1, fill=1)
    
    # Convert to numpy array
    binary_image_np = np.array(binary_image, dtype=np.uint8)
    
    plt.imshow(binary_image_np)
    plt.show()

    # Return the binary image and the top-left corner of the bounding box (relative to the 4000x4000 array)
    return binary_image_np, (offset_x, offset_y)


def is_polygon_in_frame(region, vertices, x_min, x_max, y_min, y_max):
    for vertex_index in region:
        if vertex_index == -1:
            return False
        x, y = vertices[vertex_index]
        if not (x_min <= x <= x_max and y_min <= y <= y_max):
            return False
    return True

def is_polygon_covering_image(polygon, binary_image, threshold=0.5):
    path = mpath.Path(polygon)
    y, x = np.indices(binary_image.shape)
    points = np.c_[x.ravel(), y.ravel()]
    inside_polygon = path.contains_points(points)
    

    white_points = binary_image.ravel() == 1
    covered_points = np.sum(inside_polygon & white_points)


    total_points_inside = np.sum(inside_polygon)

    if total_points_inside == 0:
        return False
    
    coverage_fraction = covered_points / total_points_inside
    return coverage_fraction > threshold

if __name__ == "__main__":
    np.random.seed(710)
    points = np.random.rand(100, 2) * 3078

    import time
    start = time.time()
    points = (0,0)
    terrain_voronoi(points, 11)
    end = time.time()
    print("Time:", end-start)

