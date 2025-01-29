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
    polygon_points = []
    shared_edges = {}
    count = 0
    for i in range(len(regions)):
        if -1 in regions[i] or len(regions[i]) == 0:
            continue
        edges = []
        points = vertices[regions[i]]
        
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
        polygon_points.append(points)
    return region_polygons, vor, shared_edges, polygon_points

def construct_points(chunk_coords, chunk_size, seed):
    points= []
    rng = np.random.default_rng(seed)
    random.seed(seed)
    for i in range(-3, 4):
        for j in range(-3, 4):
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
            dist_from_edge = 250

            l_bounds = [min_x+dist_from_edge, min_y + dist_from_edge]
            u_bounds = [max_x-dist_from_edge, max_y-dist_from_edge]
            engine = qmc.PoissonDisk(d=2, radius=0.45, seed=rng)

            ind = engine.integers(l_bounds=l_bounds, u_bounds=u_bounds, n=10)
            for p in ind:
                x = p[0] + random.randint(-180, 180)
                x = max(min(x, max_x-1), min_x + 1)
                y = p[1] + random.randint(-180, 180)
                y = max(min(y, max_y -1), min_y + 1)
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

    # plt.show()

def create_voronoi(chunk_coords, seed):
    p = construct_points(chunk_coords, 1024, seed)
    region_polygons, vor, shared_edges, polygon_points = get_polygons(p)
    # plot_chunks(vor)

    return region_polygons, shared_edges, vor, polygon_points

def ccw(A,B,C):
    return (C[1]-A[1]) * (B[0]-A[0]) > (B[1]-A[1]) * (C[0]-A[0])

def intersect(A,B,C,D):
    return ccw(A,C,D) != ccw(B,C,D) and ccw(A,B,C) != ccw(A,B,D)

def find_overlapping_polygons(region_polygons, shared_edges, chunk, polygon_points):
    min_x = chunk[0]
    max_x = chunk[0] + 1024
    min_y = chunk[1]
    max_y = chunk[1] + 1024
    overlapping_polygons = []
    overlapping_polygons_points = []
    unique_polygon_indices = set()
    edges = list(shared_edges.keys())
    for i in range(len(edges)):
        edge = edges[i]
        if (min_x <= edge[0][0] <= max_x and min_y <= edge[0][1] <= max_y) or (min_x <= edge[1][0] <= max_x and min_y <= edge[1][1] <= max_y):
            polygon_indices = shared_edges[edge]

            if polygon_indices[0] not in unique_polygon_indices:
                overlapping_polygons.extend([region_polygons[polygon_indices[0]]])
                overlapping_polygons_points.extend([polygon_points[polygon_indices[0]]])
                unique_polygon_indices.add(polygon_indices[0])
            if polygon_indices[1] not in unique_polygon_indices:
                overlapping_polygons.extend([region_polygons[polygon_indices[1]]])
                overlapping_polygons_points.extend([polygon_points[polygon_indices[1]]])
                unique_polygon_indices.add(polygon_indices[1])
        else:

            left_bound = min_x
            right_bound = max_x
            top_bound = max_y
            bottom_bound = min_y

            # check if line intersects with any of the chunk edges

            # check if line intersects with left edge

            if intersect(edge[0], edge[1], (left_bound, bottom_bound), (left_bound, top_bound)):
                polygon_indices = shared_edges[edge]
            
                if polygon_indices[0] not in unique_polygon_indices:
                    overlapping_polygons.extend([region_polygons[polygon_indices[0]]])
                    overlapping_polygons_points.extend([polygon_points[polygon_indices[0]]])
                    unique_polygon_indices.add(polygon_indices[0])
                if polygon_indices[1] not in unique_polygon_indices:
                    overlapping_polygons.extend([region_polygons[polygon_indices[1]]])
                    overlapping_polygons_points.extend([polygon_points[polygon_indices[1]]])
                    unique_polygon_indices.add(polygon_indices[1])

            elif intersect(edge[0], edge[1], (left_bound, top_bound), (right_bound, top_bound)):
                polygon_indices = shared_edges[edge]
            
                if polygon_indices[0] not in unique_polygon_indices:
                    overlapping_polygons.extend([region_polygons[polygon_indices[0]]])
                    overlapping_polygons_points.extend([polygon_points[polygon_indices[0]]])
                    unique_polygon_indices.add(polygon_indices[0])
                if polygon_indices[1] not in unique_polygon_indices:
                    overlapping_polygons.extend([region_polygons[polygon_indices[1]]])
                    overlapping_polygons_points.extend([polygon_points[polygon_indices[1]]])
                    unique_polygon_indices.add(polygon_indices[1])
            
            elif intersect(edge[0], edge[1], (right_bound, top_bound), (right_bound, bottom_bound)):
                polygon_indices = shared_edges[edge]
            
                if polygon_indices[0] not in unique_polygon_indices:
                    overlapping_polygons.extend([region_polygons[polygon_indices[0]]])
                    overlapping_polygons_points.extend([polygon_points[polygon_indices[0]]])
                    unique_polygon_indices.add(polygon_indices[0])
                if polygon_indices[1] not in unique_polygon_indices:
                    overlapping_polygons.extend([region_polygons[polygon_indices[1]]])
                    overlapping_polygons_points.extend([polygon_points[polygon_indices[1]]])
                    unique_polygon_indices.add(polygon_indices[1])
            
            elif intersect(edge[0], edge[1], (right_bound, bottom_bound), (left_bound, bottom_bound)):
                polygon_indices = shared_edges[edge]
            
                if polygon_indices[0] not in unique_polygon_indices:
                    overlapping_polygons.extend([region_polygons[polygon_indices[0]]])
                    overlapping_polygons_points.extend([polygon_points[polygon_indices[0]]])
                    unique_polygon_indices.add(polygon_indices[0])
                if polygon_indices[1] not in unique_polygon_indices:
                    overlapping_polygons.extend([region_polygons[polygon_indices[1]]])
                    overlapping_polygons_points.extend([polygon_points[polygon_indices[1]]])
                    unique_polygon_indices.add(polygon_indices[1])
    
    
    return overlapping_polygons, overlapping_polygons_points

def get_chunk_polygons(chunk_coords, seed):
    min_x = round(chunk_coords[0] / 1024) * 1024
    min_y = round(chunk_coords[1] / 1024) * 1024
    region_polygons, shared_edges, vor, polygon_points = create_voronoi((min_x, min_y), seed)

    overlapping_polygons, overlapping_polygon_points = find_overlapping_polygons(region_polygons, shared_edges, (0, 0), polygon_points)
    #print(len(overlapping_polygons))
    # voronoi_plot_2d(vor)
    # plt.plot([0, 0, 1024, 1024, 0], [0, 1024, 1024, 0, 0], 'k-')

    plt.figure()

    for region in overlapping_polygons:
        for i in range(len(region)):
            x1, y1 = region[i][0]
            x2, y2 = region[i][1]

            plt.plot([x1, x2], [y1, y2], 'r-')
    # print(len(overlapping_polygon_points))
    for points in overlapping_polygon_points:
        for point in points:
            x1, x2 = point
            plt.plot(x1, x2, 'bo')

    plt.gca().invert_yaxis()

    plt.show()

    return overlapping_polygons, overlapping_polygon_points

# polygons, poly_points = get_chunk_polygons((0, 0), 22)
# plt.plot([0, 0, 1024, 1024, 0], [0, 1024, 1024, 0, 0], 'k-')

# for region in polygons:
#     for i in range(len(region)):
#         x1, y1 = region[i][0]
#         x2, y2 = region[i][1]
#         plt.plot([x1, x2], [y1, y2], 'r-')

# plt.show()