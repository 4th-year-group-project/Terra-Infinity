from scipy.spatial import Voronoi
from perlin_noise import PerlinNoise
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.path as mpath
import random
import cv2

def determine_landmass(polygon_edges, polygon_points, shared_edges, polygon_ids, seed):
    nut = random.randint(0, 100)
    print(nut)
    t_noise1 = PerlinNoise(octaves=2, seed=seed)
    t_noise2 = PerlinNoise(octaves=4, seed=seed)

    t_noise = lambda x: t_noise1([x[0], x[1]]) + 0.7 * t_noise2([x[0], x[1]])

    x_points = [point[k][0] for point in polygon_points for k in range(len(point))]
    y_points = [point[k][1] for point in polygon_points for k in range(len(point))]

    overall_min_x = min(x_points)
    overall_max_x = max(x_points)
    overall_min_y = min(y_points)
    overall_max_y = max(y_points)

    xpix, ypix = 100, 100
    map = np.zeros((xpix, ypix))
    for i in range(xpix):
        for j in range(ypix):
            noise_val = t_noise([i/xpix, j/ypix])
            map[i][j] = noise_val


    # scale up the tempmap using interpolation
    map = cv2.resize(map, (int(np.ceil(overall_max_x - overall_min_x)), int(np.ceil(overall_max_y - overall_min_y))), interpolation=cv2.INTER_LINEAR)

    threshold = -0.04
    pic = np.array(map)

    pic[pic > threshold] = 1
    pic[pic <= threshold] = 0
    binary_image = pic

    # plt.imshow(binary_image, cmap='gray')
    # plt.show()

    fig, ax = plt.subplots(1, 3, figsize=(15, 5))
    fig.tight_layout()

    relevant_polygon_ids = []
    water_polygon_ids = []

    relevant_polygons = []
    water_polygons = []

    for i in range(len(polygon_points)):
        polygon = polygon_points[i]
        polygon_id = polygon_ids[i]
        if is_polygon_covering_image(polygon, binary_image):
            # ax[0].fill(*zip(*polygon), color=random_color, edgecolor='black', alpha=0.5)
            relevant_polygon_ids.append(polygon_id)
        else:
            water_polygon_ids.append(polygon_id)

    #here we iterate over the edges, find which ones are between a water and land block.
    for (p1, p2), value in shared_edges.items():

        if len(value) > 1 and value[0] in water_polygon_ids and value[1] in relevant_polygon_ids:
            new_edge = [p1, p2]
            polygon_to_update = polygon_points[polygon_ids.index(value[1])]
            print("===== =====================")
            print(polygon_ids.index(value[1]))
            print(polygon_ids)
            print(polygon_to_update)
            # print(polygon_to_update.shape)
            index_to_insert_at = np.where(polygon_to_update == p1)[0][0]
            old_edge = np.array(p1, p2)
            polygon_to_update = np.setdiff1d(polygon_to_update, old_edge)
            for i in range(len(new_edge)):
                np.insert(polygon_to_update, index_to_insert_at + i, new_edge[i])
            polygon_points[polygon_ids.index(value[1])] = polygon_to_update

    for i in len(polygon_points):
        polygon = polygon_points[i]
        if polygon in relevant_polygon_ids:
            ax[0].fill(*zip(*polygon), color='green', edgecolor='black', alpha=0.5)
        else:
            ax[0].fill(*zip(*polygon), color='blue', edgecolor='black', alpha=0.5)


    ax[0].invert_yaxis()

    ax[1].imshow(binary_image, cmap='gray')
    ax[1].set_title("Binary Image")
    #invert y axis
    ax[1].invert_yaxis()

    ax[2].imshow(map, cmap='gray')
    ax[2].set_title("Perlin Noise Map")
    ax[2].invert_yaxis()

    plt.show(block=False)

    for i in range(len(polygon_points)):
        polygon = polygon_points[i]
        if polygon in relevant_polygon_ids:
            relevant_polygons.append(polygon)
        else:
            water_polygons.append(polygon)

    return None, relevant_polygons


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
