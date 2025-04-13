"""Generates the voronoi cells which overlaps a certain superchunk"""
import random

import matplotlib.pyplot as plt
import numpy as np
from scipy.spatial import Voronoi, voronoi_plot_2d
from scipy.stats import qmc


def get_polygons(points):
    """Gets the polygon points and edges of each polygon from the voronoi diagram of a set of points

    Parameters:
    points: List of points where voronoi diagram is to be constructed around

    Returns:
    region_polygons: List of edges of each polygon
    vor: Voronoi object
    shared_edges: Dictionary of shared edges between polygons
    polygon_points: List of points in each polygon
    """
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

def construct_points(chunk_coords, chunk_size, seed, biome_size):
    """Constructs a set of points for the voronoi diagram to be constructed around for a 7x7 grid of superchunks around the target superchunk

    Parameters:
    chunk_coords: Coordinates of the target superchunk
    chunk_size: Size of the superchunk
    seed: Seed value for the world

    Returns:
    points: List of points for the voronoi diagram
    """
    points= []
    max_size = 0.9
    min_size = 0.4
    normalised_size = (((biome_size) / 100) * (max_size - min_size)) + min_size
    max_size = 0.9
    min_size = 0.4
    normalised_size = (((biome_size) / 100) * (max_size - min_size)) + min_size
    for i in range(-3, 4):
        for j in range(-3, 4):

            #min_x = round((chunk_coords[0] + i * chunk_size) / chunk_size) * chunk_size
            #min_y = round((chunk_coords[1] + j * chunk_size) / chunk_size) * chunk_size

            min_x = chunk_coords[0] + (i * chunk_size)
            min_y = chunk_coords[1] + (j * chunk_size)

            chunk_seed = f"{seed:b}" + f"{min_x+(1<<32):b}" + f"{min_y+(1<<32):b}"

            rng = np.random.default_rng(list(chunk_seed))
            random.seed(chunk_seed)
            #centre_x = (chunk_coords[0] + i * chunk_size)
            #centre_y = (chunk_coords[1] + j * chunk_size)

            #min_x = centre_x - chunk_size / 2
            #max_x = centre_x + chunk_size / 2
            max_x = min_x + chunk_size
            #min_y = centre_y - chunk_size / 2
            #max_y = centre_y + chunk_size / 2
            max_y = min_y + chunk_size
            dist_from_edge = 200

            l_bounds = [min_x+dist_from_edge, min_y + dist_from_edge]
            u_bounds = [max_x-dist_from_edge, max_y-dist_from_edge]
            # engine = qmc.PoissonDisk(d=2, radius=0.65, seed=rng)
            engine = qmc.PoissonDisk(d=2, radius=normalised_size, seed=rng)
            # engine = qmc.PoissonDisk(d=2, radius=0.65, seed=rng)
            engine = qmc.PoissonDisk(d=2, radius=normalised_size, seed=rng)

            ind = engine.integers(l_bounds=l_bounds, u_bounds=u_bounds, n=10)
            for p in ind:
                x = p[0] + random.randint(-150, 150)
                x = max(min(x, max_x-1), min_x + 1)
                y = p[1] + random.randint(-150, 150)
                y = max(min(y, max_y -1), min_y + 1)
                points.append([x, y])

    return points

# def plot_chunks(vor):
#     voronoi_plot_2d(vor)
#     # plot 7x7 chunks of 1024x1024
#     plt.plot([0, 0, 1024, 1024, 0], [0, 1024, 1024, 0, 0], 'k-')
#     plt.plot([1024, 1024, 2048, 2048, 1024], [0, 1024, 1024, 0, 0], 'k-')

#     plt.plot([0, 0, -1024, -1024, 0], [0, 1024, 1024, 0, 0], 'k-')
#     plt.plot([-1024, -1024, 0, 0, -1024], [0, 1024, 1024, 0, 0], 'k-')
#     plt.plot([0, 0, 1024, 1024, 0], [1024, 2048, 2048, 1024, 1024], 'k-')
#     plt.plot([-1024, -1024, 0, 0, -1024], [1024, 2048, 2048, 1024, 1024], 'k-')
#     plt.plot([1024, 1024, 2048, 2048, 1024], [1024, 2048, 2048, 1024, 1024], 'k-')
#     plt.plot([-1024, -1024, 0, 0, -1024], [-1024, 0, 0, -1024, -1024], 'k-')
#     plt.plot([0, 0, 1024, 1024, 0], [-1024, 0, 0, -1024, -1024], 'k-')
#     plt.plot([1024, 1024, 2048, 2048, 1024], [-1024, 0, 0, -1024, -1024], 'k-')

#     plt.plot([2048, 2048, 3072, 3072, 2048], [0, 1024, 1024, 0, 0], 'k-')
#     plt.plot([3072, 3072, 4096, 4096, 3072], [0, 1024, 1024, 0, 0], 'k-')
#     plt.plot([2048, 2048, 3072, 3072, 2048], [1024, 2048, 2048, 1024, 1024], 'k-')
#     plt.plot([3072, 3072, 4096, 4096, 3072], [1024, 2048, 2048, 1024, 1024], 'k-')
#     plt.plot([2048, 2048, 3072, 3072, 2048], [-1024, 0, 0, -1024, -1024], 'k-')
#     plt.plot([3072, 3072, 4096, 4096, 3072], [-1024, 0, 0, -1024, -1024], 'k-')
#     plt.plot([2048, 2048, 3072, 3072, 2048], [-2048, -1024, -1024, -2048, -2048], 'k-')
#     plt.plot([3072, 3072, 4096, 4096, 3072], [-2048, -1024, -1024, -2048, -2048], 'k-')
#     plt.plot([2048, 2048, 3072, 3072, 2048], [-3072, -2048, -2048, -3072, -3072], 'k-')
#     plt.plot([3072, 3072, 4096, 4096, 3072], [-3072, -2048, -2048, -3072, -3072], 'k-')
#     plt.plot([1024, 1024, 2048, 2048, 1024], [-2048, -1024, -1024, -2048, -2048], 'k-')
#     plt.plot([0, 0, 1024, 1024, 0], [-2048, -1024, -1024, -2048, -2048], 'k-')
#     plt.plot([-1024, -1024, 0, 0, -1024], [-2048, -1024, -1024, -2048, -2048], 'k-')
#     plt.plot([-2048, -2048, -1024, -1024, -2048], [-2048, -1024, -1024, -2048, -2048], 'k-')
#     plt.plot([-2048, -2048, -3072, -3072, -2048], [-1024, 0, 0, -1024, -1024], 'k-')
#     plt.plot([-2048, -2048, -3072, -3072, -2048], [0, 1024, 1024, 0, 0], 'k-')
#     plt.plot([-2048, -2048, -3072, -3072, -2048], [1024, 2048, 2048, 1024, 1024], 'k-')
#     plt.plot([-2048, -2048, -3072, -3072, -2048], [-2048, -1024, -1024, -2048, -2048], 'k-')
#     plt.plot([-3072, -3072, -2048, -2048, -3072], [-2048, -1024, -1024, -2048, -2048], 'k-')
#     plt.plot([1024,1024, 2048, 2048, 1024], [-3072, -2048, -2048, -3072, -3072], 'k-')
#     plt.plot([0, 0, 1024, 1024, 0], [-3072, -2048, -2048, -3072, -3072], 'k-')
#     plt.plot([-1024, -1024, 0, 0, -1024], [-3072, -2048, -2048, -3072, -3072], 'k-')
#     plt.plot([-2048, -2048, -1024, -1024, -2048], [-3072, -2048, -2048, -3072, -3072], 'k-')
#     plt.plot([-3072, -3072, -2048, -2048, -3072], [-3072, -2048, -2048, -3072, -3072], 'k-')
#     plt.plot([-2048, -2048, -1024, -1024, -2048], [-1024, 0, 0, -1024, -1024], 'k-')
#     plt.plot([-2048, -2048, -1024, -1024, -2048], [0, 1024, 1024, 0, 0], 'k-')
#     plt.plot([-2048, -2048, -1024, -1024, -2048], [1024, 2048, 2048, 1024, 1024], 'k-')
#     plt.plot([-3072, -3072, -2048, -2048, -3072], [2048, 3072, 3072, 2048, 2048], 'k-')
#     plt.plot([-2048, -2048, -1024, -1024, -2048], [2048, 3072, 3072, 2048, 2048], 'k-')
#     plt.plot([-1024, -1024, 0, 0, -1024], [2048, 3072, 3072, 2048, 2048], 'k-')
#     plt.plot([0, 0, 1024, 1024, 0], [2048, 3072, 3072, 2048, 2048], 'k-')
#     plt.plot([1024, 1024, 2048, 2048, 1024], [2048, 3072, 3072, 2048, 2048], 'k-')
#     plt.plot([2048, 2048, 3072, 3072, 2048], [2048, 3072, 3072, 2048, 2048], 'k-')
#     plt.plot([3072, 3072, 4096, 4096, 3072], [2048, 3072, 3072, 2048, 2048], 'k-')
#     plt.plot([-3072, -3072, -2048, -2048, -3072], [3072, 4096, 4096, 3072, 3072], 'k-')
#     plt.plot([-2048, -2048, -1024, -1024, -2048], [3072, 4096, 4096, 3072, 3072], 'k-')
#     plt.plot([-1024, -1024, 0, 0, -1024], [3072, 4096, 4096, 3072, 3072], 'k-')
#     plt.plot([0, 0, 1024, 1024, 0], [3072, 4096, 4096, 3072, 3072], 'k-')
#     plt.plot([1024, 1024, 2048, 2048, 1024], [3072, 4096, 4096, 3072, 3072], 'k-')
#     plt.plot([2048, 2048, 3072, 3072, 2048], [3072, 4096, 4096, 3072, 3072], 'k-')
#     plt.plot([3072, 3072, 4096, 4096, 3072], [3072, 4096, 4096, 3072, 3072], 'k-')



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

def create_voronoi(chunk_coords, chunk_size, seed, biome_size):
    """Creates a voronoi diagram for a 7x7 grid of superchunks around the target superchunk

    Parameters:
    chunk_coords: Coordinates of the target superchunk
    chunk_size: Size of the superchunk
    seed: Seed value for the world

    Returns:
    region_polygons: List of edges of each polygon
    shared_edges: Dictionary of shared edges between polygons
    vor: Voronoi object
    polygon_points: List of points in each polygon
    """

    p = construct_points(chunk_coords, chunk_size, seed, biome_size)
    region_polygons, vor, shared_edges, polygon_points = get_polygons(p)
    #plot_chunks(vor)

    return region_polygons, shared_edges, vor, polygon_points

def ccw(A,B,C):
    """Helper function for determining if two lines intersect"""
    return (C[1]-A[1]) * (B[0]-A[0]) > (B[1]-A[1]) * (C[0]-A[0])

def intersect(A,B,C,D):
    """Determines if two lines intersect

    Parameters:
    A: Start point of first line
    B: End point of first line
    C: Start point of second line
    D: End point of second line

    Returns:
    True if the lines intersect, False otherwise
    """
    return ccw(A,C,D) != ccw(B,C,D) and ccw(A,B,C) != ccw(A,B,D)

def find_overlapping_polygons(region_polygons, shared_edges, chunk, polygon_points, chunk_size):
    """Finds the polygons that overlap with a target superchunk

    Parameters:
    region_polygons: List of edges of each polygon
    shared_edges: Dictionary of shared edges between polygons
    chunk: Coordinates of the target superchunk
    polygon_points: List of points in each polygon
    chunk_size: Size of the superchunk

    Returns:
    overlapping_polygons: List of overlapping polygons
    overlapping_polygons_points: List of points in each overlapping polygon
    overlapping_polygon_indices: List of indices of the overlapping polygons
    """
    min_x = chunk[0] * chunk_size
    max_x = min_x + chunk_size
    min_y = chunk[1] * chunk_size
    max_y = min_y + chunk_size

    min_x -= 200
    max_x += 200
    min_y -= 200
    max_y += 200

    overlapping_polygons = []
    overlapping_polygons_points = []
    unique_polygon_indices = set()
    edges = list(shared_edges.keys())
    overlapping_polygon_indices = []
    for i in range(len(edges)):
        edge = edges[i]
        if (min_x <= edge[0][0] <= max_x and min_y <= edge[0][1] <= max_y) or (min_x <= edge[1][0] <= max_x and min_y <= edge[1][1] <= max_y):
            polygon_indices = shared_edges[edge]

            if polygon_indices[0] not in unique_polygon_indices:
                overlapping_polygons.extend([region_polygons[polygon_indices[0]]])
                overlapping_polygons_points.extend([polygon_points[polygon_indices[0]]])
                unique_polygon_indices.add(polygon_indices[0])
                overlapping_polygon_indices.append(polygon_indices[0])
            if polygon_indices[1] not in unique_polygon_indices:
                overlapping_polygons.extend([region_polygons[polygon_indices[1]]])
                overlapping_polygons_points.extend([polygon_points[polygon_indices[1]]])
                unique_polygon_indices.add(polygon_indices[1])
                overlapping_polygon_indices.append(polygon_indices[1])
        else:

            left_bound = min_x
            right_bound = max_x
            top_bound = max_y
            bottom_bound = min_y

            # check if line intersects with any of the chunk edges

            # check if line intersects with left edge

            if intersect(edge[0], edge[1], (left_bound, bottom_bound), (left_bound, top_bound)) or intersect(edge[0], edge[1], (left_bound, top_bound), (right_bound, top_bound)) or intersect(edge[0], edge[1], (right_bound, top_bound), (right_bound, bottom_bound)) or intersect(edge[0], edge[1], (right_bound, bottom_bound), (left_bound, bottom_bound)):
                polygon_indices = shared_edges[edge]

                if polygon_indices[0] not in unique_polygon_indices:
                    overlapping_polygons.extend([region_polygons[polygon_indices[0]]])
                    overlapping_polygons_points.extend([polygon_points[polygon_indices[0]]])
                    unique_polygon_indices.add(polygon_indices[0])
                    overlapping_polygon_indices.append(polygon_indices[0])
                if polygon_indices[1] not in unique_polygon_indices:
                    overlapping_polygons.extend([region_polygons[polygon_indices[1]]])
                    overlapping_polygons_points.extend([polygon_points[polygon_indices[1]]])
                    unique_polygon_indices.add(polygon_indices[1])
                    overlapping_polygon_indices.append(polygon_indices[1])


    return overlapping_polygons, overlapping_polygons_points, overlapping_polygon_indices

def get_chunk_polygons(chunk_coords, seed, chunk_size, parameters):
    """Generates a voronoi diagram that spans 7x7 superchunks around the target superchunk and finds the polygons that overlap with the target superchunk

    Parameters:
    chunk_coords: Coordinates of the target superchunk
    seed: Seed value for the world
    chunk_size: Size of the superchunk

    Returns:
    overlapping_polygons: List of polygons which overlap the target superchunk
    overlapping_polygon_points: List of points in each overlapping polygon
    shared_edges: Dictionary of shared edges between polygons
    polygon_indices: List of indices of the overlapping polygons
    """
    biome_size = parameters.get("biome_size", 50)
    min_x = chunk_coords[0] * (chunk_size)
    min_y = chunk_coords[1] * (chunk_size)
    region_polygons, shared_edges, vor, polygon_points = create_voronoi((min_x, min_y), chunk_size, seed, biome_size)
    overlapping_polygons, overlapping_polygon_points, polygon_indices = find_overlapping_polygons(region_polygons, shared_edges, chunk_coords, polygon_points, chunk_size)

    #voronoi_plot_2d(vor)
    #plt.plot([0, 0, 1024, 1024, 0], [0, 1024, 1024, 0, 0], 'k-')

    # plt.figure()

    # for region in overlapping_polygons:
    #     for i in range(len(region)):
    #         x1, y1 = region[i][0]
    #         x2, y2 = region[i][1]
    # for region in overlapping_polygons:
    #     for i in range(len(region)):
    #         x1, y1 = region[i][0]
    #         x2, y2 = region[i][1]

            # flip x axis
            # y1 = chunk_size - y1
            # y2 = chunk_size - y2
            # y1 = chunk_size - y1
            # y2 = chunk_size - y2
            # plt.plot([x1, x2], [y1, y2], 'r-')
    # print(len(overlapping_polygon_points))
    # for points in overlapping_polygon_points:
    #     for point in points:
    #         x, y = point
    #         y = chunk_size - y
    # for points in overlapping_polygon_points:
    #     for point in points:
    #         x, y = point
    #         y = chunk_size - y
            # plt.plot(x, y, 'bo')

    # plt.gca().invert_yaxis()

    # plt.show(block=False)

    return overlapping_polygons, overlapping_polygon_points, shared_edges, polygon_indices

# polygons, poly_points, _, pp = get_chunk_polygons((0, 0), 35, biome_size=50)
# polygons, poly_points, _, pp = get_chunk_polygons((0, 0), 35, biome_size=50)


# for region in polygons:
#     for i in range(len(region)):
#         x1, y1 = region[i][0]
#         x2, y2 = region[i][1]
#         plt.plot([x1, x2], [y1, y2], 'r-')
# plt.plot([-200, -200, 1223, 1223, -200], [-200, 1223, 1223, -200, -200], 'k-')
# plt.plot([-200, -200, 1223, 1223, -200], [-200, 1223, 1223, -200, -200], 'k-')

# plt.show()
