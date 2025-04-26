import numpy as np


def generate_points(chunk_seed, n, chunk_size, distance_from_edge=200, radius=100):
    points = []

    max_attempts = 1000
    rng = np.random.default_rng(chunk_seed)
    for _ in range(max_attempts):
        cx = rng.integers(distance_from_edge, chunk_size - distance_from_edge)
        cy = rng.integers(distance_from_edge, chunk_size - distance_from_edge)

        if all(np.linalg.norm(np.array([cx, cy]) - np.array(p)) >= radius for p in points):
            points.append(np.array([cx, cy]))
            if len(points) >= n:
                break

    shift = np.array([chunk_size // 2, chunk_size // 2])
    points = np.array(points) - shift

    return points


def construct_points2(chunk_coords, chunk_size, seed, radius=7, skew_factor=0):
    points = []

    skew_factor = skew_factor / 20 - 2.5

    densities = [1, 2, 3, 4]
    weights = [x**skew_factor for x in range(1, len(densities) + 1)]
    weights /= np.sum(weights)

    for i in range(-radius // 2, radius // 2 + 1):
        for j in range(-radius // 2, radius // 2 + 1):
            x = chunk_coords[0] + (i * chunk_size)
            y = chunk_coords[1] + (j * chunk_size)
            chunk_seed = (seed + (x << 32) + (y << 64)) & ((1 << 128) - 1)

            rng = np.random.default_rng(chunk_seed)
            num_points = rng.choice(densities, p=weights)
            chunk_points = generate_points(chunk_seed, num_points, chunk_size, 100, 1024 / num_points)
            chunk_points[:, 0] += x
            chunk_points[:, 1] += y

            points.extend(chunk_points)

    return points
