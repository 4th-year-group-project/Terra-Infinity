from scipy.spatial import Voronoi, voronoi_plot_2d

import numpy as np
import random
import matplotlib.pyplot as plt
from scipy.stats import qmc


def get_polygons(points):
    vor = Voronoi(points)
    vertices = vor.vertices
    regions = vor.regions
    
    ridge_vertices = vor.ridge_vertices
    region_polygons = []
    shared_edges = {}
    count = 0
    for i in range(len(regions)):
        if -1 in regions[i] or len(regions[i]) == 0:
            continue
        edges = []
        for j in range(len(ridge_vertices)):
            if ridge_vertices[j][0] in regions[i] and ridge_vertices[j][1] in regions[i]:
                edge = vertices[ridge_vertices[j]]
                edge_tuple = (tuple(edge[0]), tuple(edge[1]))
                edges.append(edge_tuple)
                if shared_edges.get(edge_tuple) is None:
                    shared_edges[edge_tuple] = []
                shared_edges[edge_tuple].append(count)
                
        count += 1
        region_polygons.append(edges)
    return region_polygons, vor, shared_edges

def construct_points(chunk_coords, chunk_size, seed, random_points=False):
    n_chunks = 49
    points_per_chunk = 10
    if not random_points:
        rng = np.random.default_rng(seed)
        # l_bound = -((chunk_size * 3) // 2)
        # u_bound = (chunk_size * 3) // 2
        l_bound = -(chunk_size * 3)
        u_bound = chunk_size * 4
        l_bounds = [l_bound, l_bound]
        u_bounds = [u_bound, u_bound]
        engine = qmc.PoissonDisk(d=2, radius=0.085, seed=rng)
        ind = engine.integers(l_bounds=l_bounds, u_bounds=u_bounds, n=n_chunks* points_per_chunk)
        points = []
        random.seed(seed)
        for p in ind:
            x = p[0] + random.randint(-500, 500)
            x = max(min(x, u_bound -1), l_bound + 1)
            y = p[1] + random.randint(-500, 500)
            y = max(min(y, u_bound -1), l_bound + 1)
            points.append([x, y])
        return points
    else:
        points= []
        for i in range(-3, 4):
            for j in range(-3, 4):
                # find center of chunk given coordinate in centre chunk
                min_x = round((chunk_coords[0] + i * chunk_size) / chunk_size) * chunk_size
                min_y = round((chunk_coords[1] + j * chunk_size) / chunk_size) * chunk_size
                #centre_x = (chunk_coords[0] + i * chunk_size)
                #centre_y = (chunk_coords[1] + j * chunk_size)

                #min_x = centre_x - chunk_size / 2
                #max_x = centre_x + chunk_size / 2
                max_x = min_x + chunk_size
                #min_y = centre_y - chunk_size / 2
                #max_y = centre_y + chunk_size / 2
                max_y = min_y + chunk_size

                # generate random points in chunk
                for k in range(2):
                    x = random.randint(min_x+1, max_x-1)
                    y = random.randint(min_y+1, max_y-1)
                    points.append([x, y])

        return points

def plot_chunks(vor):
    voronoi_plot_2d(vor)
    # plot 7x7 chunks of 1024x1024
    plt.plot([0, 0, 1024, 1024, 0], [0, 1024, 1024, 0, 0], 'k-')
    plt.plot([1024, 1024, 2048, 2048, 1024], [0, 1024, 1024, 0, 0], 'k-')
    
    plt.plot([0, 0, -1024, -1024, 0], [0, 1024, 1024, 0, 0], 'k-')
    plt.plot([-1024, -1024, 0, 0, -1024], [0, 1024, 1024, 0, 0], 'k-')
    plt.plot([0, 0, 1024, 1024, 0], [1024, 2048, 2048, 1024, 1024], 'k-')
    plt.plot([-1024, -1024, 0, 0, -1024], [1024, 2048, 2048, 1024, 1024], 'k-')
    plt.plot([1024, 1024, 2048, 2048, 1024], [1024, 2048, 2048, 1024, 1024], 'k-')
    plt.plot([-1024, -1024, 0, 0, -1024], [-1024, 0, 0, -1024, -1024], 'k-')
    plt.plot([0, 0, 1024, 1024, 0], [-1024, 0, 0, -1024, -1024], 'k-')
    plt.plot([1024, 1024, 2048, 2048, 1024], [-1024, 0, 0, -1024, -1024], 'k-')

    plt.plot([2048, 2048, 3072, 3072, 2048], [0, 1024, 1024, 0, 0], 'k-')
    plt.plot([3072, 3072, 4096, 4096, 3072], [0, 1024, 1024, 0, 0], 'k-')
    plt.plot([2048, 2048, 3072, 3072, 2048], [1024, 2048, 2048, 1024, 1024], 'k-')
    plt.plot([3072, 3072, 4096, 4096, 3072], [1024, 2048, 2048, 1024, 1024], 'k-')
    plt.plot([2048, 2048, 3072, 3072, 2048], [-1024, 0, 0, -1024, -1024], 'k-')
    plt.plot([3072, 3072, 4096, 4096, 3072], [-1024, 0, 0, -1024, -1024], 'k-')
    plt.plot([2048, 2048, 3072, 3072, 2048], [-2048, -1024, -1024, -2048, -2048], 'k-')
    plt.plot([3072, 3072, 4096, 4096, 3072], [-2048, -1024, -1024, -2048, -2048], 'k-')
    plt.plot([2048, 2048, 3072, 3072, 2048], [-3072, -2048, -2048, -3072, -3072], 'k-')
    plt.plot([3072, 3072, 4096, 4096, 3072], [-3072, -2048, -2048, -3072, -3072], 'k-')
    plt.plot([1024, 1024, 2048, 2048, 1024], [-2048, -1024, -1024, -2048, -2048], 'k-')
    plt.plot([0, 0, 1024, 1024, 0], [-2048, -1024, -1024, -2048, -2048], 'k-')
    plt.plot([-1024, -1024, 0, 0, -1024], [-2048, -1024, -1024, -2048, -2048], 'k-')
    plt.plot([-2048, -2048, -1024, -1024, -2048], [-2048, -1024, -1024, -2048, -2048], 'k-')
    plt.plot([-2048, -2048, -3072, -3072, -2048], [-1024, 0, 0, -1024, -1024], 'k-')
    plt.plot([-2048, -2048, -3072, -3072, -2048], [0, 1024, 1024, 0, 0], 'k-')
    plt.plot([-2048, -2048, -3072, -3072, -2048], [1024, 2048, 2048, 1024, 1024], 'k-')
    plt.plot([-2048, -2048, -3072, -3072, -2048], [-2048, -1024, -1024, -2048, -2048], 'k-')
    plt.plot([-3072, -3072, -2048, -2048, -3072], [-2048, -1024, -1024, -2048, -2048], 'k-')
    plt.plot([1024,1024, 2048, 2048, 1024], [-3072, -2048, -2048, -3072, -3072], 'k-')
    plt.plot([0, 0, 1024, 1024, 0], [-3072, -2048, -2048, -3072, -3072], 'k-')
    plt.plot([-1024, -1024, 0, 0, -1024], [-3072, -2048, -2048, -3072, -3072], 'k-')
    plt.plot([-2048, -2048, -1024, -1024, -2048], [-3072, -2048, -2048, -3072, -3072], 'k-')
    plt.plot([-3072, -3072, -2048, -2048, -3072], [-3072, -2048, -2048, -3072, -3072], 'k-')
    plt.plot([-2048, -2048, -1024, -1024, -2048], [-1024, 0, 0, -1024, -1024], 'k-')
    plt.plot([-2048, -2048, -1024, -1024, -2048], [0, 1024, 1024, 0, 0], 'k-')
    plt.plot([-2048, -2048, -1024, -1024, -2048], [1024, 2048, 2048, 1024, 1024], 'k-')
    plt.plot([-3072, -3072, -2048, -2048, -3072], [2048, 3072, 3072, 2048, 2048], 'k-')
    plt.plot([-2048, -2048, -1024, -1024, -2048], [2048, 3072, 3072, 2048, 2048], 'k-')
    plt.plot([-1024, -1024, 0, 0, -1024], [2048, 3072, 3072, 2048, 2048], 'k-')
    plt.plot([0, 0, 1024, 1024, 0], [2048, 3072, 3072, 2048, 2048], 'k-')
    plt.plot([1024, 1024, 2048, 2048, 1024], [2048, 3072, 3072, 2048, 2048], 'k-')
    plt.plot([2048, 2048, 3072, 3072, 2048], [2048, 3072, 3072, 2048, 2048], 'k-')
    plt.plot([3072, 3072, 4096, 4096, 3072], [2048, 3072, 3072, 2048, 2048], 'k-')
    plt.plot([-3072, -3072, -2048, -2048, -3072], [3072, 4096, 4096, 3072, 3072], 'k-')
    plt.plot([-2048, -2048, -1024, -1024, -2048], [3072, 4096, 4096, 3072, 3072], 'k-')
    plt.plot([-1024, -1024, 0, 0, -1024], [3072, 4096, 4096, 3072, 3072], 'k-')
    plt.plot([0, 0, 1024, 1024, 0], [3072, 4096, 4096, 3072, 3072], 'k-')
    plt.plot([1024, 1024, 2048, 2048, 1024], [3072, 4096, 4096, 3072, 3072], 'k-')
    plt.plot([2048, 2048, 3072, 3072, 2048], [3072, 4096, 4096, 3072, 3072], 'k-')
    plt.plot([3072, 3072, 4096, 4096, 3072], [3072, 4096, 4096, 3072, 3072], 'k-')



    #plt.plot([1024, 1024, 2048, 2048, 1024], [1024, 2048, 2048, 1024, 1024], 'k-')

    # plt.plot([-512, -512, 512, 512, -512], [-512, 512, 512, -512, -512], 'k-')
    # plt.plot([512, 1536, 1536, 512, 512], [-512, -512, 512, 512, -512], 'k-')
    # plt.plot([-512, -1536, -1536, -512, -512], [-512, -512, 512, 512, -512], 'k-')
    # plt.plot([512, 1536, 1536, 512, 512], [512, 512, 1536, 1536, 512], 'k-')
    # plt.plot([512, 512, -512, -512, 512], [512, 1536, 1536, 512, 512], 'k-')
    # plt.plot([-512, -512, -1536, -1536, -512], [512, 1536, 1536, 512, 512], 'k-')
    # plt.plot([-512, -1536, -1536, -512, -512], [-512, -512, -1536, -1536, -512], 'k-')
    # plt.plot([-512, -512, 512, 512, -512], [-512, -1536, -1536, -512, -512], 'k-')
    # plt.plot([512, 512, 1536, 1536, 512], [-512, -1536, -1536, -512, -512], 'k-')

    plt.show()

def create_voronoi(chunk_coords, seed, random=False):
    p = construct_points(chunk_coords, 1024, seed, random)
    region_polygons, vor, shared_edges = get_polygons(p)


    plot_chunks(vor)

    return region_polygons, shared_edges, vor

def ccw(A,B,C):
    return (C[1]-A[1]) * (B[0]-A[0]) > (B[1]-A[1]) * (C[0]-A[0])


def intersect(A,B,C,D):
    return ccw(A,C,D) != ccw(B,C,D) and ccw(A,B,C) != ccw(A,B,D)

def find_overlapping_polygons(region_polygons, shared_edges, chunk):
    min_x = chunk[0]
    max_x = chunk[0] + 1024
    min_y = chunk[1]
    max_y = chunk[1] + 1024
    overlapping_polygons = []
    edges = list(shared_edges.keys())
    for i in range(len(edges)):
        edge = edges[i]
        if (min_x <= edge[0][0] <= max_x and min_y <= edge[0][1] <= max_y) or (min_x <= edge[1][0] <= max_x and min_y <= edge[1][1] <= max_y):
            polygons = [region_polygons[j] for j in shared_edges[edge]]
            overlapping_polygons.extend(polygons)
        else:

            left_bound = min_x
            right_bound = max_x
            top_bound = max_y
            bottom_bound = min_y

            # check if line intersects with any of the chunk edges

            # check if line intersects with left edge

            if intersect(edge[0], edge[1], (left_bound, bottom_bound), (left_bound, top_bound)):
                polygons = [region_polygons[j] for j in shared_edges[edge]]
                overlapping_polygons.extend(polygons)

            elif intersect(edge[0], edge[1], (left_bound, top_bound), (right_bound, top_bound)):
                polygons = [region_polygons[j] for j in shared_edges[edge]]
                overlapping_polygons.extend(polygons)
            
            elif intersect(edge[0], edge[1], (right_bound, top_bound), (right_bound, bottom_bound)):
                polygons = [region_polygons[j] for j in shared_edges[edge]]
                overlapping_polygons.extend(polygons)
            
            elif intersect(edge[0], edge[1], (right_bound, bottom_bound), (left_bound, bottom_bound)):
                polygons = [region_polygons[j] for j in shared_edges[edge]]
                overlapping_polygons.extend(polygons)
    return overlapping_polygons

def get_chunk_polygons(chunk_coords, seed, random=False):
    min_x = round(chunk_coords[0] / 1024) * 1024
    min_y = round(chunk_coords[1] / 1024) * 1024
    region_polygons, shared_edges, vor = create_voronoi((min_x, min_y), seed, random)

    overlapping_polygons = find_overlapping_polygons(region_polygons, shared_edges, (0, 0))
    voronoi_plot_2d(vor)
    plt.plot([0, 0, 1024, 1024, 0], [0, 1024, 1024, 0, 0], 'k-')

    for region in overlapping_polygons:
        for i in range(len(region)):
            x1, y1 = region[i][0]
            x2, y2 = region[i][1]

            plt.plot([x1, x2], [y1, y2], 'r-')
    plt.show()
    return overlapping_polygons

polygons = get_chunk_polygons((0, 0), 1, random=False)

for region in polygons:
    for i in range(len(region)):
        x1, y1 = region[i][0]
        x2, y2 = region[i][1]
        plt.plot([x1, x2], [y1, y2], 'r-')

plt.show()