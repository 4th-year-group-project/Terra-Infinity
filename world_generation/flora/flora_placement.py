import numba as nb
import numpy as np
from scipy.ndimage import sobel

from world_generation.generation import Noise


@nb.jit(fastmath=True)
def find_intersections(circle1_centre, circle2_centre, circle1_radius, circle2_radius):
    """Find the intersection points of two circles.

    Args:
        circle1_centre: Centre of the first circle (x, y).
        circle2_centre: Centre of the second circle (x, y).
        circle1_radius: Radius of the first circle.
        circle2_radius: Radius of the second circle.

    Returns:
        point1: First intersection point (x, y) or None if no intersection.
        point2: Second intersection point (x, y) or None if no intersection.
    """

    x, y = circle1_centre
    x1, y1 = circle2_centre
    dist_between_centres = np.sqrt((x - x1) ** 2 + (y - y1) ** 2)
    if (
        dist_between_centres > circle1_radius + circle2_radius
        or dist_between_centres < np.abs(circle1_radius - circle2_radius)
        or dist_between_centres == 0
        and circle1_radius == circle2_radius
    ):
        return None, None
    else:
        a = (circle1_radius**2 - circle2_radius**2 + dist_between_centres**2) / (2 * dist_between_centres)
        h = np.sqrt(circle1_radius**2 - a**2)
        x2 = x + a * (x1 - x) / dist_between_centres
        y2 = y + a * (y1 - y) / dist_between_centres
        x3 = x2 + h * (y1 - y) / dist_between_centres
        y3 = y2 - h * (x1 - x) / dist_between_centres
        x4 = x2 - h * (y1 - y) / dist_between_centres
        y4 = y2 + h * (x1 - x) / dist_between_centres

        return (x3, y3), (x4, y4)


# Old implementation for placing points using 2D Poisson disk sampling; however, this was too slow for practical use.
# def poisson(min_x, max_x, min_y, max_y, seed, chunk_size, radius, sparseness):
#     rng = np.random.default_rng(seed)
#     engine = qmc.PoissonDisk(d=2, radius=radius, seed=rng)
#     l_bounds = np.array([min_x, min_y])
#     u_bounds = np.array([max_x, max_y])
#     points = chunk_size*chunk_size*sparseness
#     ind = engine.integers(l_bounds=l_bounds, u_bounds=u_bounds, n=points)
#     return ind


@nb.jit(fastmath=True)
def packing(seed, min_x, max_x, min_y, max_y, chunk_size, sparseness=4):
    """Generate points based on the intersection of staggered concentric rings of two circles.

    Args:
        seed: Random seed for reproducibility.
        min_x: Minimum x-coordinate of the bounding box.
        max_x: Maximum x-coordinate of the bounding box.
        min_y: Minimum y-coordinate of the bounding box.
        max_y: Maximum y-coordinate of the bounding box.
        chunk_size: Size of the superchunk.
        sparseness: Sparseness factor for point generation.

    Returns:
        points: List of points generated within the bounding box.
    """
    np.random.seed(seed)
    points = []
    pointRadius = sparseness
    centre_A = (min_x - (chunk_size / 2) - 250, min_y - (chunk_size / 2) - 250)
    init_radius_A = 256
    centre_B = (min_x - (chunk_size / 2) - 250, max_y + (chunk_size / 2) + 250)
    init_radius_B = 256
    dist_between_rings = 2.3 * pointRadius
    for A_radius_step in range(0, 1024):
        radius_A = init_radius_A + A_radius_step * dist_between_rings
        for B_radius_step in range(0, 1024):
            radius_B = init_radius_B + B_radius_step * dist_between_rings

            val = np.random.uniform(0.15, 0.95)
            radius_Ap = radius_A + (0.0 if (B_radius_step % 3 == 0) else (val * dist_between_rings))
            radius_Bp = radius_B + (0.0 if (A_radius_step % 3 == 0) else (val * dist_between_rings))

            point1, point2 = find_intersections(centre_A, centre_B, radius_Ap, radius_Bp)

            if point1 is not None:
                x, y = point1
                if x >= min_x and x <= max_x and y >= min_y and y <= max_y:
                    points.append((x, y))
            if point2 is not None:
                x, y = point2
                if x >= min_x and x <= max_x and y >= min_y and y <= max_y:
                    points.append((x, y))

    return points


@nb.jit(fastmath=True)
def get_vegetation_map(spread_mask, sobel_h, sobel_v, heightmap, seed, noise_map, width, height):
    """Generate a vegetation map based on the heightmap and noise map which determines where plants can be placed.

    Args:
        spread_mask: The spread mask of the heightmap.
        sobel_h: Sobel filter applied to the heightmap in the horizontal direction.
        sobel_v: Sobel filter applied to the heightmap in the vertical direction.
        heightmap: The heightmap of the terrain.
        seed: Random seed for reproducibility.
        noise_map: The noise map generated from the heightmap.
        width: Width of the heightmap
        height: Height of the heightmap

    Returns:
        vegetation_map: The vegetation map generated from the heightmap and noise map which determines where plants can be placed.
    """
    magnitude = np.sqrt(sobel_h**2 + sobel_v**2)
    magnitude *= 255.0 / np.max(magnitude)
    vegetation_map = np.zeros((height, width))
    np.random.seed(seed)
    for y in range(height):
        for x in range(width):
            noise = noise_map[y, x]
            if heightmap[y, x] > 0.2 and heightmap[y, x] < noise and spread_mask[y, x] > noise + 0.2:
                vegetation_map[y, x] = noise

    return vegetation_map


def apply_sobel(heightmap, spread_mask, spread, seed, x_offset, y_offset, high=1, low=0):
    """Apply Sobel filter to the heightmap and generate a noise map for vegetation placement.

    Args:
        heightmap: The heightmap of the terrain.
        spread_mask: The spread mask of the heightmap.
        spread: Determines the persistence of the noise map.
        seed: Random seed for reproducibility.
        x_offset: X offset for the noise generation.
        y_offset: Y offset for the noise generation.
        high: Upper bound for noise values to consider for vegetation placement.
        low: Lower bound for noise values to consider for vegetation placement.

    Returns:
        vegetation_map: The vegetation map generated from the heightmap and noise map which determines where plants can be placed.
    """
    sobel_h = sobel(heightmap, 0)
    sobel_v = sobel(heightmap, 1)

    width = heightmap.shape[1]
    height = heightmap.shape[0]
    noise = Noise(seed=seed, width=width, height=height)

    noise_map = noise.fractal_simplex_noise(
        seed=seed,
        noise="open",
        x_offset=int(x_offset),
        y_offset=int(y_offset),
        scale=100,
        octaves=5,
        persistence=spread,
        lacunarity=2,
    )

    noise_map = (noise_map + 1) / 2
    noise_map = noise_map * (1 - low) + low

    return get_vegetation_map(spread_mask, sobel_h, sobel_v, heightmap, seed, noise_map, width, height)


def place_plants(
    heightmap,
    spread_mask,
    seed,
    x_offset,
    y_offset,
    width=1024,
    height=1024,
    size=1024,
    spread=0.05,
    sparseness=5,
    coverage=0.6,
    high=1,
    low=0,
):
    """Place plants on the terrain based on the heightmap

    Args:
        heightmap: The heightmap of the terrain.
        spread_mask: The spread mask of the heightmap.
        seed: Random seed for reproducibility.
        x_offset: X offset for the noise generation.
        y_offset: Y offset for the noise generation.
        width: Width of the heightmap
        height: Height of the heightmap
        size: Size of the superchunk
        spread: Spread factor for plant placement.
        sparseness: Sparseness factor for plant placement.
        coverage: Coverage threshold for plant placement.
        high: Upper bound for noise values to consider for plant placement.
        low: Lower bound for noise values to consider for plant placement.

    Returns:
        points: List of points where plants are placed.
    """
    np.random.seed(seed)
    points = packing(seed, 0, width, 0, height, size, sparseness)
    mask = apply_sobel(heightmap, spread_mask, spread, seed, x_offset, y_offset, high, low)

    points = [(x, y) for x, y in points if mask[int(y), int(x)] > coverage]

    return points
