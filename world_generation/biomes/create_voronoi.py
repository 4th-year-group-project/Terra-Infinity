"""Generates the voronoi cells which overlaps a certain superchunk"""

from scipy.spatial import Voronoi

from world_generation.utils.point_generation import construct_points2


def get_polygons(points):
    """Gets the polygon points and edges of each polygon from the voronoi diagram of a set of points

    Args:
        points: List of points where voronoi diagram is to be constructed around

    Returns:
        region_polygons: List of edges of each polygon
        vor: Voronoi object
        shared_edges: Dictionary of shared edges between polygons
        polygon_points: List of points in each polygon
        polygon_center_points: List of center points of each polygon
    """
    vor = Voronoi(points)
    vertices = vor.vertices
    regions = vor.regions
    region_to_point = {}
    for i, region_index in enumerate(vor.point_region):
        region_to_point[region_index] = vor.points[i]

    ridge_vertices = vor.ridge_vertices
    region_polygons = []
    polygon_points = []
    polygon_center_points = []
    shared_edges = {}
    count = 0

    # Iterate through each polygon region and store the edges and points and polygon centers
    for i in range(len(regions)):
        if -1 in regions[i] or len(regions[i]) == 0:
            continue
        edges = []
        points = vertices[regions[i]]
        polygon_center = region_to_point[i]

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
        polygon_center_points.append(polygon_center)

    return region_polygons, vor, shared_edges, polygon_points, polygon_center_points

def create_voronoi(chunk_coords, chunk_size, seed, biome_size):
    """Creates a voronoi diagram for a 7x7 grid of superchunks around the target superchunk

    Args:
        chunk_coords: Coordinates of the target superchunk
        chunk_size: Size of the superchunk
        seed: Seed value for the world
        biome_size: Size of the biome

    Returns:
        region_polygons: List of edges of each polygon
        shared_edges: Dictionary of shared edges between polygons
        vor: Voronoi object
        polygon_points: List of points in each polygon
        polygon_centers: List of center points of each polygon
    """
    p = construct_points2(chunk_coords, chunk_size, seed, radius=7, skew_factor=biome_size)
    region_polygons, vor, shared_edges, polygon_points, polygon_centers = get_polygons(p)

    return region_polygons, shared_edges, vor, polygon_points, polygon_centers

def line_intersect(A,B,C):
    """Helper function for determining if two lines intersect
    
    Args:
        A: Start point of first line
        B: End point of first line
        C: Point to check intersection with

    Returns:
        True if the lines intersect, False otherwise
    """
    return (C[1]-A[1]) * (B[0]-A[0]) > (B[1]-A[1]) * (C[0]-A[0])

def intersect(A,B,C,D):
    """Determines if two lines intersect

    Args:
        A: Start point of first line
        B: End point of first line
        C: Start point of second line
        D: End point of second line

    Returns:
        True if the lines intersect, False otherwise
    """
    return line_intersect(A,C,D) != line_intersect(B,C,D) and line_intersect(A,B,C) != line_intersect(A,B,D)

def find_overlapping_polygons(region_polygons, shared_edges, chunk, polygon_points, chunk_size, polygon_centers):
    """Finds the polygons that overlap with a target superchunk

    Args:
        region_polygons: List of edges of each polygon
        shared_edges: Dictionary of shared edges between polygons
        chunk: Coordinates of the target superchunk
        polygon_points: List of points in each polygon
        chunk_size: Size of the superchunk
        polygon_centers: List of center points of each polygon

    Returns:
        overlapping_polygons: List of overlapping polygons
        overlapping_polygons_points: List of points in each overlapping polygon
        overlapping_polygon_indices: List of indices of the overlapping polygons
        overlapping_polygon_center_points: List of center points of the overlapping polygons
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
    overlapping_polygon_center_points = []

    # Check if the edges of the polygons are within the bounds of the superchunk and if so, add them to the list of overlapping polygons
    for i in range(len(edges)):
        edge = edges[i]
        # Check if the edge is within the bounds of the superchunk
        if (min_x <= edge[0][0] <= max_x and min_y <= edge[0][1] <= max_y) or (min_x <= edge[1][0] <= max_x and min_y <= edge[1][1] <= max_y):
            polygon_indices = shared_edges[edge]

            if polygon_indices[0] not in unique_polygon_indices:
                overlapping_polygons.extend([region_polygons[polygon_indices[0]]])
                overlapping_polygons_points.extend([polygon_points[polygon_indices[0]]])
                overlapping_polygon_center_points.extend([polygon_centers[polygon_indices[0]]])
                unique_polygon_indices.add(polygon_indices[0])
                overlapping_polygon_indices.append(polygon_indices[0])
            if polygon_indices[1] not in unique_polygon_indices:
                overlapping_polygons.extend([region_polygons[polygon_indices[1]]])
                overlapping_polygons_points.extend([polygon_points[polygon_indices[1]]])
                overlapping_polygon_center_points.extend([polygon_centers[polygon_indices[1]]])
                unique_polygon_indices.add(polygon_indices[1])
                overlapping_polygon_indices.append(polygon_indices[1])

        # Otherwise, check if line intersects with any of the chunk edges
        else:

            left_bound = min_x
            right_bound = max_x
            top_bound = max_y
            bottom_bound = min_y

            # Check if line intersects with left edge, bottom edge, right edge, or top edge of the chunk
            if intersect(edge[0], edge[1], (left_bound, bottom_bound), (left_bound, top_bound)) or intersect(edge[0], edge[1], (left_bound, top_bound), (right_bound, top_bound)) or intersect(edge[0], edge[1], (right_bound, top_bound), (right_bound, bottom_bound)) or intersect(edge[0], edge[1], (right_bound, bottom_bound), (left_bound, bottom_bound)):
                polygon_indices = shared_edges[edge]

                if polygon_indices[0] not in unique_polygon_indices:
                    overlapping_polygons.extend([region_polygons[polygon_indices[0]]])
                    overlapping_polygons_points.extend([polygon_points[polygon_indices[0]]])
                    overlapping_polygon_center_points.extend([polygon_centers[polygon_indices[0]]])
                    unique_polygon_indices.add(polygon_indices[0])
                    overlapping_polygon_indices.append(polygon_indices[0])
                if polygon_indices[1] not in unique_polygon_indices:
                    overlapping_polygons.extend([region_polygons[polygon_indices[1]]])
                    overlapping_polygons_points.extend([polygon_points[polygon_indices[1]]])
                    overlapping_polygon_center_points.extend([polygon_centers[polygon_indices[1]]])
                    unique_polygon_indices.add(polygon_indices[1])
                    overlapping_polygon_indices.append(polygon_indices[1])


    return overlapping_polygons, overlapping_polygons_points, overlapping_polygon_indices, overlapping_polygon_center_points

def get_chunk_polygons(chunk_coords, seed, chunk_size, parameters):
    """Generates a voronoi diagram that spans 7x7 superchunks around the target superchunk and finds the polygons that overlap with the target superchunk

    Args:
        chunk_coords: Coordinates of the target superchunk
        seed: Seed value for the world
        chunk_size: Size of the superchunk
        parameters: Parameters for the polygon generation e.g. polygon size

    Returns:
        overlapping_polygons: List of polygons which overlap the target superchunk
        overlapping_polygon_points: List of points in each overlapping polygon
        shared_edges: Dictionary of shared edges between polygons
        polygon_indices: List of indices of the overlapping polygons
        overlapping_polygon_centers: List of center points of the overlapping polygons
    """
    biome_size = parameters.get("biome_size", 50)
    min_x = chunk_coords[0] * (chunk_size)
    min_y = chunk_coords[1] * (chunk_size)
    region_polygons, shared_edges, vor, polygon_points, polygon_centers = create_voronoi((min_x, min_y), chunk_size, seed, biome_size)
    overlapping_polygons, overlapping_polygon_points, polygon_indices, overlapping_polygon_centers = find_overlapping_polygons(region_polygons, shared_edges, chunk_coords, polygon_points, chunk_size, polygon_centers)

    return overlapping_polygons, overlapping_polygon_points, shared_edges, polygon_indices, overlapping_polygon_centers
