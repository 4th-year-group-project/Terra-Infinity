from scipy.spatial import Voronoi, voronoi_plot_2d

import numpy as np

import matplotlib.pyplot as plt


def get_polygons(points):
    vor = Voronoi(points)
    vertices = vor.vertices
    regions = vor.regions
    ridge_vertices = vor.ridge_vertices
    region_polygons = []
    for i in range(len(regions)):
        if -1 in regions[i]:
            continue
        edges = []
        for j in range(len(ridge_vertices)):
            if ridge_vertices[j][0] in regions[i] and ridge_vertices[j][1] in regions[i]:
                edges.append(vertices[ridge_vertices[j]])
        region_polygons.append(edges)
    return region_polygons, vor

points = [(np.random.randint(-50, 50), np.random.randint(-50, 50)) for i in range(10)]
region_polygons, vor = get_polygons(points)
        
# draw edges in region_polygons
for region in region_polygons:
    for i in range(len(region)):
        x1, y1 = region[i][0]
        x2, y2 = region[i][1]
        plt.plot([x1, x2], [y1, y2], 'k-')

plt.show()

