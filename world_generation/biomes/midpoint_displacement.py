from hashlib import sha256

import numpy as np


def stable_seed(v1, v2):
    """Generates a consistent seed for an edge regardless of vertex order.

    Args:
        v1: First vertex of the edge.
        v2: Second vertex of the edge.

    Returns:
        hash_value: A stable seed value for the edge. Integer.
    """
    v1_modified = (int(v1[0]), int(v1[1]))
    v2_modified = (int(v2[0]), int(v2[1]))
    key = tuple(sorted([tuple(v1_modified), tuple(v2_modified)]))
    hash_value = int(sha256(str(key).encode()).hexdigest(), 16) % (2**32)
    return hash_value

def consistent_normal(v1, v2):
    """Ensures the normal direction is consistent regardless of vertex order.

    Args:
        v1: First vertex of the edge.
        v2: Second vertex of the edge.

    Returns:
        normal: A unit normal vector perpendicular to the edge from v1 to v2.
    """
    v1, v2 = sorted([v1, v2], key=lambda v: tuple(v))

    line = v2 - v1
    line_unit = line / np.linalg.norm(line)
    normal = np.array([-line_unit[1], line_unit[0]])

    return normal

def cycle_array(polygon):
    """Cycles the array to start from the vertex with the minimum x-coordinate.

    Args:
        polygon: List of points defining the polygon.

    Returns:
        The polygon with the first vertex being the one with the minimum x-coordinate.
    """
    polygon = np.array(polygon)
    min_index = np.argmin([sublist[0] for sublist in polygon])
    return np.roll(polygon, -min_index, axis=0)

def midpoint_displacement(relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids, strength=0.5):
    """Applies midpoint displacement to polygons.

    Args:
        relevant_polygons_edges: List of edges for the polygons.
        relevant_polygons_points: List of points defining the polygons.
        shared_edges: List of shared edges between polygons.
        polygon_ids: List of polygon IDs.
        strength: Displacement strength.

    Returns:
        relevant_polygons_edges: Updated edges for the polygons.
        output_polygons: List of modified polygons after midpoint displacement.
        shared_edges: Updated list of shared edges.
        polygon_ids: List of polygon IDs.
    """
    output_polygons = []
    # Iterate through each polygon
    for polygon in relevant_polygons_points:
        polygon = cycle_array(polygon)
        new_vertices = []
        # Iterate through each vertex in the polygon
        for i in range(len(polygon)):
            new_vertices.append(polygon[i].tolist())
            # Get the next vertex
            next_index = (i + 1) % len(polygon)

            # Calculate the midpoint and apply displacement
            alpha = 0.5
            midpoint = (1 - alpha) * polygon[i] + alpha * polygon[next_index]

            line_length = np.linalg.norm(polygon[next_index] - polygon[i])
            displacement = min(line_length * strength, 100)

            normal = consistent_normal(polygon[i], polygon[next_index])
            edge_seed = stable_seed(polygon[i], polygon[next_index])
            np.random.seed(edge_seed)
            displacement_factor = np.random.uniform(-displacement, displacement)

            midpoint += displacement_factor * normal
            new_vertices.append(midpoint.tolist())

        output_polygons.append(np.array(new_vertices))

    return relevant_polygons_edges, output_polygons, shared_edges, polygon_ids
