import time

import numpy as np
from scipy.ndimage import gaussian_filter
from tqdm import tqdm


### https://github.com/dandrino/terrain-erosion-3-ways
def simple_gradient(terrain):
    dy = 0.5 * (np.roll(terrain, 1, axis=0) - np.roll(terrain, -1, axis=0))
    dx = 0.5 * (np.roll(terrain, 1, axis=1) - np.roll(terrain, -1, axis=1))
    return np.stack([dx, dy], axis=-1)

def sample(terrain, gradient):
    shape = np.array(terrain.shape)
    y, x = np.indices(terrain.shape)
    coords = np.stack([x - gradient[..., 0], y - gradient[..., 1]])

    lower_coords = np.floor(coords).astype(int)
    upper_coords = lower_coords + 1

    coord_offsets = coords - lower_coords

    lower_coords %= shape[:, np.newaxis, np.newaxis]
    upper_coords %= shape[:, np.newaxis, np.newaxis]

    f00 = terrain[lower_coords[1], lower_coords[0]]  # Bottom-left
    f10 = terrain[lower_coords[1], upper_coords[0]]  # Bottom-right
    f01 = terrain[upper_coords[1], lower_coords[0]]  # Top-left
    f11 = terrain[upper_coords[1], upper_coords[0]]  # Top-right

    interp_x0 = f00 + (f10 - f00) * coord_offsets[0]  # Interpolate along x-axis (bottom)
    interp_x1 = f01 + (f11 - f01) * coord_offsets[0]  # Interpolate along x-axis (top)
    result = interp_x0 + (interp_x1 - interp_x0) * coord_offsets[1]  # Interpolate along y-axis
    return result

def displace(a, delta):
    wx_prev = np.maximum(-delta[..., 0], 0.0)
    wx_curr = np.maximum(1 - np.abs(delta[..., 0]), 0.0)
    wx_next = np.maximum(delta[..., 0], 0.0)
    wy_prev = np.maximum(-delta[..., 1], 0.0)
    wy_curr = np.maximum(1 - np.abs(delta[..., 1]), 0.0)
    wy_next = np.maximum(delta[..., 1], 0.0)

    result = np.zeros_like(a)
    result += np.roll(np.roll(wx_prev * wy_prev * a, -1, axis=0), -1, axis=1)
    result += np.roll(np.roll(wx_prev * wy_curr * a, 0, axis=0), -1, axis=1)
    result += np.roll(np.roll(wx_prev * wy_next * a, 1, axis=0), -1, axis=1)
    result += np.roll(np.roll(wx_curr * wy_prev * a, -1, axis=0), 0, axis=1)
    result += np.roll(np.roll(wx_curr * wy_curr * a, 0, axis=0), 0, axis=1)
    result += np.roll(np.roll(wx_curr * wy_next * a, 1, axis=0), 0, axis=1)
    result += np.roll(np.roll(wx_next * wy_prev * a, -1, axis=0), 1, axis=1)
    result += np.roll(np.roll(wx_next * wy_curr * a, 0, axis=0), 1, axis=1)
    result += np.roll(np.roll(wx_next * wy_next * a, 1, axis=0), 1, axis=1)

    return result

def displace_fast(a, delta):
    x = delta[..., 0]
    y = delta[..., 1]

    wx_prev = np.minimum(x, 0.0)
    wx_curr = np.maximum(1 - np.abs(x), 0.0)
    wx_next = np.maximum(x, 0.0)
    wy_prev = np.minimum(y, 0.0)
    wy_curr = np.maximum(1 - np.abs(y), 0.0)
    wy_next = np.maximum(y, 0.0)

    wy_prev_a = wy_prev * a
    wy_curr_a = wy_curr * a
    wy_next_a = wy_next * a


    result = np.zeros_like(a)
    result += np.roll(np.roll(wx_prev * wy_prev_a, -1, axis=0) + wx_prev * wy_curr_a + np.roll(wx_prev * wy_next_a, 1, axis=0), -1, axis=1)
    result += np.roll(wx_curr * wy_prev_a, -1, axis=0) + wx_curr * wy_curr_a + np.roll(wx_curr * wy_next_a, 1, axis=0)
    result += np.roll(np.roll(wx_next * wy_prev_a, -1, axis=0) + wx_next * wy_curr_a + np.roll(wx_next * wy_next_a, 1, axis=0), 1, axis=1)

    return result


def apply_slippage(terrain, repose_slope, cell_width):
    delta = simple_gradient(terrain) / cell_width
    smoothed = gaussian_filter(terrain, sigma=1.5)
    return np.where(np.linalg.norm(delta, axis=-1) > repose_slope, smoothed, terrain)

def erosion(terrain, seed=42, cell_width=1, iterations=100,
            rain_rate=0.0008, evaporation_rate=0.0005, dissolving_rate=0.25, deposition_rate=0.001,
            repose_slope=0.06, gravity=30, sediment_capacity_constant=30,
            verbose=False,
            ):

    times = {}
    times["gradient"] = 0
    times["capacity"] = 0
    times["deposition"] = 0
    times["slippage"] = 0
    times["displacement"] = 0

    rain_rate *= cell_width**2

    sediment = np.zeros_like(terrain)
    water = np.zeros_like(terrain)
    velocity = np.zeros_like(terrain)

    rng = np.random.RandomState(seed)

    if verbose:
        progress_bar = tqdm(range(iterations))
    else:
        progress_bar = range(iterations)

    for _ in progress_bar:

        start_time = time.time()
        water += rng.rand(*terrain.shape) * rain_rate
        gradient = simple_gradient(terrain)
        mag = np.linalg.norm(gradient, axis=-1)
        mag[mag < 1e-10] = 1e-10
        gradient /= mag[..., None]
        times["gradient"] += time.time() - start_time

        start_time = time.time()
        neighbor_height = sample(terrain, -gradient)
        height_delta = terrain - neighbor_height
        sediment_capacity = (
            (np.maximum(height_delta, 0.05)/ cell_width) *
            velocity * water * sediment_capacity_constant
        )
        times["capacity"] += time.time() - start_time

        deposited_sediment = np.select(
            [height_delta < 0, sediment > sediment_capacity],
            [np.minimum(height_delta, sediment), deposition_rate * (sediment - sediment_capacity)],
            dissolving_rate * (sediment - sediment_capacity),
        )

        deposited_sediment = np.maximum(-height_delta, deposited_sediment)
        sediment -= deposited_sediment
        terrain += deposited_sediment

        start_time = time.time()
        sediment = displace(sediment, gradient)
        water = displace(water, gradient)
        times["displacement"] += time.time() - start_time

        start_time = time.time()
        terrain = apply_slippage(terrain, repose_slope, cell_width)
        times["slippage"] += time.time() - start_time
        velocity = gravity*height_delta/cell_width
        water *= 1 - evaporation_rate

    if verbose:
        print(f"""
        Gradient: {times['gradient']/iterations}
        Capacity: {times['capacity']/iterations}
        Slippage: {times['slippage']/iterations}
        Displacement: {times['displacement']/iterations}
        """)

    return terrain

