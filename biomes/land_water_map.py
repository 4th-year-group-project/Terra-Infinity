from scipy.spatial import Voronoi
from perlin_noise import PerlinNoise
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.path as mpath
import random
import cv2

def determine_landmass(polygon_edges, polygon_points, seed):
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

    colors = ['green']

    relevant_polygons = []

    for polygon in polygon_points:
        if is_polygon_covering_image(polygon, binary_image):
            random_color = random.choice(colors)
            ax[0].fill(*zip(*polygon), color=random_color, edgecolor='black', alpha=0.5)
            relevant_polygons.append(polygon)
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
