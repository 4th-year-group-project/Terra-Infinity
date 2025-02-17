from scipy.spatial import Voronoi
from perlin_noise import PerlinNoise
from Noise import SimplexNoise
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.path as mpath
import random
import cv2
from copy import deepcopy
import hashlib
from biomes.climate_map import pnpoly

def determine_landmass(polygon_edges, polygon_points, og_polygon_points, shared_edges, polygon_ids, coords, seed):

    polygon_points_copy = deepcopy(polygon_points)
    relevant_polygons_og_coord_space = []

    start_coords_x = coords[0] * 1024
    end_coords_x = (coords[0] + 1) * 1024
    start_coords_y = coords[1] * 1024
    end_coords_y = (coords[1] + 1) * 1024

    all_points = np.vstack(polygon_points)

    overall_min_x, overall_min_y = np.round(np.min(all_points, axis=0)).astype(int)
    overall_max_x, overall_max_y = np.round(np.max(all_points, axis=0)).astype(int) 

    x_offset = overall_min_x
    y_offset = overall_min_y

    noise_1 = SimplexNoise(seed=seed, width=int((abs(overall_min_x - overall_max_x))), height=int((abs(overall_min_y - overall_max_y))), scale=600.0, octaves=1, persistence=0.5, lacunarity=2.0)
    noise_2 = SimplexNoise(seed=seed, width=int((abs(overall_min_x - overall_max_x))), height=int((abs(overall_min_y - overall_max_y))), scale=600.0, octaves=3, persistence=0.5, lacunarity=2.0)

    t_noise_1 = noise_1.fractal_noise(noise="open", x_offset=int(x_offset), y_offset=int(y_offset), reason="land")
    t_noise_2 = noise_2.fractal_noise(noise="open", x_offset=int(x_offset), y_offset=int(y_offset), reason="land")

    t_noise = 0.4 * t_noise_1 + 0.6 * t_noise_2

    # t_noise_1 = t_noise_1.tolist()
    # t_noise_2 = t_noise_2.tolist()

    #combine them in a weighted way

    # t_noise = lambda x: t_noise_1[int(x[0] * 512)][int(x[1] * 512)] + 0.7 * t_noise_2[int(x[0] * 512)][int(x[1] * 512)]

    # xpix, ypix = 100, 100
    # map = np.zeros((xpix, ypix))
    # for i in range(xpix):
    #     for j in range(ypix):
    #         noise_val = t_noise([i/xpix, j/ypix])
    #         map[i][j] = noise_val

    map = t_noise

    # scale up the tempmap using interpolation
    # map = cv2.resize(map, (int(np.ceil(overall_max_x - overall_min_x)), int(np.ceil(overall_max_y - overall_min_y))), interpolation=cv2.INTER_LINEAR)

    threshold = -0.1
    pic = np.array(map)

    pic[pic > threshold] = 1
    pic[pic <= threshold] = 0
    binary_image = pic

    fig, ax = plt.subplots(1, 3, figsize=(15, 5))
    fig.tight_layout()

    relevant_polygon_ids = []
    water_polygon_ids = []

    relevant_polygons = []
    water_polygons = []

    for i in range(len(polygon_points)):

        polygon = og_polygon_points[i]
        polygon_id = polygon_ids[i]
        if is_polygon_covering_image(polygon, overall_min_x, overall_min_y, binary_image):
            relevant_polygon_ids.append(polygon_id)
        else:
            water_polygon_ids.append(polygon_id)

    for polygon in polygon_points:
        for i in range(len(polygon)):
            polygon[i] = (polygon[i][0] - overall_min_x, polygon[i][1] - overall_min_y)

    start_coords_x = start_coords_x - overall_min_x
    end_coords_x = end_coords_x - overall_min_x
    start_coords_y = start_coords_y - overall_min_y
    end_coords_y = end_coords_y - overall_min_y

    slice_parts = (start_coords_x, end_coords_x, start_coords_y, end_coords_y)

    fixed_x_min, fixed_x_max = 0, 4000  
    fixed_y_min, fixed_y_max = 0, 4000  

    for i in range(len(polygon_points)):
        polygon_id = polygon_ids[i]
        if polygon_id in relevant_polygon_ids:
            polygon = polygon_points[i]
            ax[0].fill(*zip(*polygon), color='green', edgecolor='black', alpha=0.5)
        else:
            polygon = polygon_points[i]
            ax[0].fill(*zip(*polygon), color='blue', edgecolor='black', alpha=0.5)

    # ax[0].invert_yaxis()
    # ax[0].set_xlim(fixed_x_min, fixed_x_max)
    # ax[0].set_ylim(fixed_y_max, fixed_y_min)  # Inverted Y

    # ax[1].imshow(binary_image, cmap='gray', extent=[fixed_x_min, fixed_x_max, fixed_y_max, fixed_y_min])
    # ax[1].set_title("Binary Image")
    # ax[1].invert_yaxis()
    # ax[1].set_xlim(fixed_x_min, fixed_x_max)
    # ax[1].set_ylim(fixed_y_max, fixed_y_min)

    # ax[2].imshow(map, cmap='gray', extent=[fixed_x_min, fixed_x_max, fixed_y_max, fixed_y_min])
    # ax[2].set_title("Perlin Noise Map")
    # ax[2].invert_yaxis()
    # ax[2].set_xlim(fixed_x_min, fixed_x_max)
    # ax[2].set_ylim(fixed_y_max, fixed_y_min)

    # plt.show(block=False)


    for i in range(len(polygon_points)):
        polygon_id = polygon_ids[i]
        if polygon_id in relevant_polygon_ids:
            polygon = polygon_points[i]
            relevant_polygons.append(polygon)
            relevant_polygons_og_coord_space.append(polygon_points_copy[i])
        else:
            polygon = polygon_points[i]
            water_polygons.append(polygon)

    return polygon_edges, relevant_polygons, slice_parts, relevant_polygons_og_coord_space, (overall_min_x, overall_min_y)


def is_polygon_covering_image(polygon, x_min, y_min, binary_image, threshold=0.5):
    for i in range(len(polygon)):
        polygon[i] = (polygon[i][0] - x_min, polygon[i][1] - y_min)

    x_points = [point[0] for point in polygon]
    y_points = [point[1] for point in polygon]     

    min_polygon_x = int(min(x_points))
    max_polygon_x = int(max(x_points))
    min_polygon_y = int(min(y_points))
    max_polygon_y = int(max(y_points))

    diff_x = max_polygon_x - min_polygon_x
    diff_y = max_polygon_y - min_polygon_y

    polygon_seed = "{0:b}".format(diff_x+(1<<32)) + "{0:b}".format(diff_y+(1<<32))
    hashed_polygon_seed = int(hashlib.sha256(polygon_seed.encode()).hexdigest(), 16) % (2**32)
    np.random.seed(hashed_polygon_seed)

    count = 0
    color_list = []
    while count < 20:
        point = (np.random.randint(int(min(x_points)), int(max(x_points))), np.random.randint(int(min(y_points)), int(max(y_points))))
        point = (np.random.randint(int(min(x_points)), int(max(x_points))), np.random.randint(int(min(y_points)), int(max(y_points))))
        if pnpoly(len(x_points), x_points, y_points, point[0], point[1]) == 1:
            color_list.append(binary_image[point[1], point[0]])
            count += 1

    coverage_fraction = np.sum(color_list) / len(color_list)
    return coverage_fraction > threshold




