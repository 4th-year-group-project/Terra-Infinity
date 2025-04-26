from copy import deepcopy

import numpy as np

from world_generation.generation import Noise


def generate_landmass_heights(seed, centroids, scale=5000, sharpness=0):
    """Generate the simplex noise values for a set of points. Used with Voronoi centroids to help determine landmass and ocean cells.

    Args:
        seed: Random seed for noise generation.
        centroids: Centroids of the Voronoi cells.
        scale: Scale of the noise.
        sharpness: Sharpness of the noise.

    Returns:
        heights: A list of noise values for the centroids.
    """
    centroids_np = np.array(centroids)

    # Generate the simplex noise points
    noise = Noise(seed)
    heights = noise.batch_simplex_noise(
        centroids_np, octaves=3,
        scale=scale, x_offset=0, y_offset=0, persistence=0.5, lacunarity=2,
    )

    billow = 2*np.abs(heights)-1
    ridge = 2*(1-np.abs(heights))-1

    if sharpness > 0:
        heights = heights*(1-sharpness) + billow*sharpness
    else:
        heights = heights*(1+sharpness) + ridge*abs(sharpness)

    return heights

def determine_landmass(polygon_edges, polygon_points, shared_edges, polygon_ids, coords, seed, polygon_centers, parameters):
    """Assign landmass and ocean cells to Voronoi polygons.

    Polygons are assigned based on values of a global noise map, which is consistent across independent
    calls.

    Args:
        polygon_edges: List of edges for each Voronoi polygon, coordinates in global space.
        polygon_points: List of points for each Voronoi polygon, coordinates in global space.
        shared_edges: List of edges shared between Voronoi polygons.
        polygon_ids: List of unique IDs for each Voronoi polygon.
        coords: Chunk coordinates for the desired superchunk.
        seed: Random seed for consistency of random processes.
        polygon_centers: List of centroids for each Voronoi polygon.
        parameters: Dictionary of parameters from the UI.

    Returns:
        polygon_edges: List of edges for each Voronoi polygon, coordinates in global space.
        polygons_to_return: List of points for each Voronoi polygon, coordinates in local space.
        land_water_ids: List of 0s or 1s for each Voronoi polygon, indicating water or land respectively.
        slice_parts: Two points that define a square. This square tells you where the superchunk is in local space.
        polygons_to_return_og_coord_space: List of points for each Voronoi polygon, coordinates in global space.
        overall_min_x: Minimum x coordinate of the Voronoi polygons in global space.
        overall_min_y: Minimum y coordinate of the Voronoi polygons in global space.
    """

    ocean_coverage = parameters.get("ocean_coverage", 50)
    polygon_points_copy = deepcopy(polygon_points)

    # Find the minimum and maximum x and y of the superchunk in global space
    start_coords_x = coords[0] * 1023
    end_coords_x = (coords[0] + 1) * 1023
    start_coords_y = coords[1] * 1023
    end_coords_y = (coords[1] + 1) * 1023
    all_points = np.vstack(polygon_points)
    overall_min_x, overall_min_y = np.round(np.min(all_points, axis=0)).astype(int)

    # Find the heights in the noise map which will define landmass and ocean
    relevant_polygon_ids = []
    water_polygon_ids = []
    landmass_heights = generate_landmass_heights(seed, polygon_centers)

    # Threshold the heights to determine landmass and ocean
    for i in range(len(landmass_heights)):
        polygon_id = polygon_ids[i]
        if landmass_heights[i] < -0.1:
            water_polygon_ids.append(polygon_id)
        else:
            relevant_polygon_ids.append(polygon_id)

    for polygon in polygon_points:
        for i in range(len(polygon)):
            polygon[i] = (polygon[i][0] - overall_min_x, polygon[i][1] - overall_min_y)

    # Find the minimum and maximum x and y of the superchunk in local space
    start_coords_x = start_coords_x - overall_min_x
    end_coords_x = end_coords_x - overall_min_x
    start_coords_y = start_coords_y - overall_min_y
    end_coords_y = end_coords_y - overall_min_y

    slice_parts = (start_coords_x, end_coords_x, start_coords_y, end_coords_y)

    # Construct required data structures
    polygons_to_return = []
    polygons_to_return_og_coord_space = []
    land_water_ids = []

    # Iterate through the polygons and assign them as land or water
    for i in range(len(polygon_points)):
        polygons_to_return.append(polygon_points[i])
        polygons_to_return_og_coord_space.append(polygon_points_copy[i])
        if ((polygon_ids[i] in relevant_polygon_ids) or (ocean_coverage == 0)) and (ocean_coverage != 100):
            land_water_ids.append(1)
        else:
            land_water_ids.append(0)

    return polygon_edges, polygons_to_return, land_water_ids, slice_parts, polygons_to_return_og_coord_space, (overall_min_x, overall_min_y)


def find_polygon_centroid_value(polygon, x_min, y_min, binary_image):
    """Finds the value of the centroid of a polygon in a corresponding black/white image.

    Args:
        polygon: List of points defining the polygon.
        x_min: Minimum x coordinate of the polygon.
        y_min: Minimum y coordinate of the polygon.
        binary_image: Binary image to query with a position.

    Returns:
        The value of the centroid in the binary image.
    """
    # Adjust the polygon coordinates to be relative to the binary image
    for i in range(len(polygon)):
        polygon[i] = (polygon[i][0] - x_min, polygon[i][1] - y_min)

    x_points = [point[0] for point in polygon]
    y_points = [point[1] for point in polygon]

    min_polygon_x = int(np.floor(min(x_points)))
    max_polygon_x = int(np.ceil(max(x_points)))
    min_polygon_y = int(np.floor(min(y_points)))
    max_polygon_y = int(np.ceil(max(y_points)))

    # Find centroid
    centroid = ((min_polygon_x + max_polygon_x) // 2, (min_polygon_y + max_polygon_y) // 2)
    return binary_image[centroid[1], centroid[0]]
