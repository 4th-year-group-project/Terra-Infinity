from dataclasses import dataclass

import numpy as np
from scipy.spatial import Voronoi

from world_generation.biomes.land_water_map import generate_landmass_heights


@dataclass
class WorldMap:
    polygons: list
    centroids: list
    neighbors: dict
    ocean: set
    boundary_nodes: set
    coastal: set


def build_world_map(seed, voronoi: Voronoi, min_x, max_x, min_y, max_y) -> WorldMap:
    polygons = []
    centroids = []
    index_map = {}
    ocean = set()

    for i, region_index in enumerate(voronoi.point_region):
        region = voronoi.regions[region_index]
        if not region or -1 in region:
            continue

        polygon = [voronoi.vertices[j] for j in region]
        center = voronoi.points[i]

        if not ((min_x < center[0] < max_x) and (min_y < center[1] < max_y)):
            continue

        polygons.append(polygon)
        centroids.append(voronoi.points[i])
        index_map[i] = len(polygons) - 1

    centroids_np = np.array(centroids)
    heights = generate_landmass_heights(seed, centroids_np)

    for i, h in enumerate(heights):
        if h < -0.1:
            ocean.add(i)

    neighbors = {i: set() for i in range(len(polygons))}

    for p1, p2 in voronoi.ridge_points:
        if p1 in index_map and p2 in index_map:
            i1 = index_map[p1]
            i2 = index_map[p2]
            neighbors[i1].add(i2)
            neighbors[i2].add(i1)

    boundary_nodes = set()
    coastal = set()

    for node in ocean:
        for neighbor in neighbors[node]:
            if neighbor not in ocean:
                boundary_nodes.add(node)
                coastal.add(neighbor)

    return WorldMap(
        polygons=polygons,
        centroids=centroids,
        neighbors=neighbors,
        ocean=ocean,
        boundary_nodes=boundary_nodes,
        coastal=coastal
    )
