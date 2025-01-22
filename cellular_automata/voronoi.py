import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial import Voronoi
import matplotlib.path as mpath
from perlin_noise import PerlinNoise
import cv2
import random
from PIL import Image, ImageDraw
from scaling_heightmap import main
from scipy.ndimage import zoom
from concurrent.futures import ThreadPoolExecutor

def biomes_voronoi(points):
    points = points / 18
    x_min, x_max = 0, 3078/18
    y_min, y_max = 0, 3078/18

    noise2 = PerlinNoise(octaves=10, seed=180)
    noise1 = PerlinNoise(octaves=4, seed=119)
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

def terrain_voronoi(points):
    x_min, x_max = 0, 3078
    y_min, y_max = 0, 3078
    vor = Voronoi(points)
    fig, ax = plt.subplots(1, 3, figsize=(15, 5))
    fig.tight_layout()
    ax[0].scatter(points[:, 0], points[:, 1], color='blue', label='Input Points', s=2)
    in_frame = []

    for region_idx, region in enumerate(vor.regions):
        
        if not region or not is_polygon_in_frame(region, vor.vertices, x_min, x_max, y_min, y_max):
            continue
        polygon = vor.vertices[region]
        print(polygon)
        in_frame.append(polygon)
        ax[0].fill(*zip(*polygon), color='white', edgecolor='black', alpha=0.5)
        centroid = np.mean(polygon, axis=0)
        ax[0].annotate(str(region_idx), (centroid[0], centroid[1]), fontsize=5, color='red')

    ax[0].set_xlim(x_min, x_max)
    ax[0].set_ylim(y_min, y_max)
    ax[0].set_title("Voronoi Diagram with Regions Based on Binary Image")
    ax[0].invert_yaxis()


    # region1 = vor.regions[18]
    # region2 = vor.regions[15]
    # polygon1 = vor.vertices[region1]
    # polygon2 = vor.vertices[region2]
    # ax[1].fill(*zip(*polygon), color='white', edgecolor='black', alpha=0.5)
    # ax[1].fill(*zip(*polygon2), color='white', edgecolor='black', alpha=0.5)
    # ax[1].invert_yaxis()
    
    # reconstructed_image = np.zeros((3078, 3078))
    # for polygon in in_frame:
    #     binary_polygon, (min_x, min_y) = polygon_to_tight_binary_image(polygon)
    #     heightmap = main(43, binary_polygon)
    #     temp = np.zeros((3078, 3078))

    #     temp[min_y:min_y+binary_polygon.shape[0], min_x:min_x+binary_polygon.shape[0]] = heightmap
    #     reconstructed_image += temp

    def process_polygon(polygon):
        binary_polygon, (min_x, min_y) = polygon_to_tight_binary_image(polygon)
        heightmap = main(43, binary_polygon)
        temp = np.zeros((3578, 3578))
        temp[min_y:min_y+binary_polygon.shape[0], min_x:min_x+binary_polygon.shape[0]] = heightmap
        return temp

    def reconstruct_image(in_frame):
        reconstructed_image = np.zeros((3578, 3578))


        with ThreadPoolExecutor() as executor:
            results = executor.map(process_polygon, in_frame)
            for temp in results:
                reconstructed_image += temp

        return reconstructed_image

    reconstructed_image = reconstruct_image(in_frame)
    ax[1].imshow(reconstructed_image, cmap='gray')

    ax[2].imshow(reconstructed_image[1026:(1026+1026), 1026:(1026+1026)], cmap='gray')

    #save the thing on ax[1] as an image, no axes or anything

    plt.figure(figsize=(3078/100, 3078/100), dpi=100) 
    plt.imshow(reconstructed_image[1026:(1026+1026), 1026:(1026+1026)], cmap='gray')
    plt.axis('off')
    plt.savefig('cellular_automata/imgs/terrain_voronoi.png', bbox_inches='tight', pad_inches=0)




    plt.show()

        
def polygon_to_tight_binary_image(polygon, padding=170):
    """
    Create a binary image for a polygon with the original bounding box and padding around it.
    
    Parameters:
    - polygon: Array of vertices defining the polygon.
    - padding: The amount of padding to add around the bounding box (default is 100).
    
    Returns:
    - binary_image_np: Binary image with polygon.
    - (min_x, min_y): Top-left corner of the polygon relative to the original image.
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
    min_x = max(0, min_x)
    min_y = max(0, min_y)
    width += 2 * padding
    height += 2 * padding

    # Ensure the bounding box is square (optional, based on your earlier code)
    side_length = max(width, height)

    # Create a blank binary image with the size of the new bounding box with padding
    binary_image = Image.new("1", (int(side_length), int(side_length)), 0)
    draw = ImageDraw.Draw(binary_image)

    # Convert polygon to tuples for drawing
    polygon_tuples = [tuple(coord - [min_x, min_y]) for coord in polygon]

    # Draw the polygon (fill with white)
    draw.polygon(polygon_tuples, outline=1, fill=1)

    # Convert to numpy array
    binary_image_np = np.array(binary_image, dtype=np.uint8)



    # Return the binary image and the top-left corner of the bounding box (original coordinates)
    return binary_image_np, (min_x, min_y)


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

np.random.seed(3)
points = np.random.rand(24, 2) * 3078
terrain_voronoi(points)

