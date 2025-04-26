import math
from datetime import datetime

import cv2
import matplotlib.pyplot as plt
import numpy as np
from scipy.ndimage import convolve, distance_transform_edt, gaussian_filter, zoom

from cellular_automata.CA import Growth_And_Crowding_CA


def edge_boundary_distance_transform(binary_shape):
    """Compute the distance transform of the edge boundary of a binary shape.

    Used when we are finding new roots for more interesting shapes.

    Parameters:
    binary_shape (np.ndarray): A binary shape.

    Returns:
    distances (np.ndarray): The distance transform of the edge boundary of the shape.
    """
    padded_shape = np.pad(binary_shape, pad_width=1, mode="constant", constant_values=False)
    distances = distance_transform_edt(padded_shape)
    distances = distances[1:-1, 1:-1]
    return distances

def upscale_bilinear(image, scale_factor):
    """Upscale an image using bilinear interpolation.

    Parameters:
    image (np.ndarray): The input image.
    scale_factor (float): The factor by which to scale the image.

    Returns:
    upscaled_image (np.ndarray): The upscaled image.
    """
    image = image.astype(np.float32)
    height, width = image.shape[:2]
    new_width = int(width * scale_factor)
    new_height = int(height * scale_factor)
    upscaled_image = cv2.resize(image, (new_width, new_height), interpolation=cv2.INTER_LINEAR)
    return upscaled_image

def convolve_average(grid):
    """Convolve a grid with a 3x3 kernel that averages the values of the neighbors.

    Parameters:
    grid (np.ndarray): The input grid.

    Returns:
    neighbor_counts (np.ndarray): The result of the convolution.
    """
    kernel = np.array([[1, 1, 1],
                       [1, 0, 1],
                       [1, 1, 1]])
    neighbor_counts = convolve(grid, kernel, mode="reflect")
    return neighbor_counts / 8


def upscale_shape_with_full_adjacency(small_grid, cell_directions, scale_factor):
    """The 'clean upscale' of the DLA shape so far. Does DFS to get a tree - want no closed loops.

    Parameters:
    small_grid (np.ndarray): The input grid.
    scale_factor (int): The factor by which to scale the shape.

    Returns:
    large_grid (np.ndarray): The clean upscaled grid.
    """

    small_size = small_grid.shape[0]
    large_size = small_size * scale_factor
    large_grid = np.zeros((large_size, large_size), dtype=int)

    offset = scale_factor // 2

    def random_one_bit_index(number):

        one_indices = []
        index = 0

        while number > 0:
            if number & 1:
                one_indices.append(index)
            number >>= 1
            index += 1

        # random.seed(1)
        # return random.choice(one_indices) if one_indices else None
        return one_indices[-1] if one_indices else None
        # random.seed(1)
        # return random.choice(one_indices) if one_indices else None
        return one_indices[-1] if one_indices else None

    for x in range(small_size):
        for y in range(small_size):
            if small_grid[x, y] == 1:
                large_x = x * scale_factor + offset
                large_y = y * scale_factor + offset
                large_grid[large_x, large_y] = 1

                direction = cell_directions[x, y]

                chosen_direction = random_one_bit_index(direction)

                if chosen_direction == 0:
                    for i in range(scale_factor):
                        large_grid[large_x + i, large_y + i] = 1

                elif chosen_direction == 1:
                    for i in range(scale_factor):
                        large_grid[large_x + i, large_y] = 1

                elif chosen_direction == 2:
                    for i in range(scale_factor):
                        large_grid[large_x + i, large_y - i] = 1

                elif chosen_direction == 3:
                    for i in range(scale_factor):
                        large_grid[large_x, large_y + i] = 1

                elif chosen_direction == 4:
                    for i in range(scale_factor):
                        large_grid[large_x, large_y - i] = 1

                elif chosen_direction == 5:
                    for i in range(scale_factor):
                        large_grid[large_x - i, large_y + i] = 1

                elif chosen_direction == 6:
                    for i in range(scale_factor):
                        large_grid[large_x - i, large_y] = 1

                elif chosen_direction == 7:
                    for i in range(scale_factor):
                        large_grid[large_x - i, large_y - i] = 1

    return large_grid


def generate_masks(mask):
    """Generate the downscaled masks for the DLA shape.

    Parameters:
    path (str): The path to the mask file. In future, this should be replaced by the biinary mask itself.
    """
    true_size = mask.shape[0]
    base_ca_size = math.floor(true_size / 126)
    scaley_size = base_ca_size * 126
    mask = zoom(mask, (scaley_size/true_size), order=0)
    mask = np.where(mask > 0, True, False)

    downscaled_shape_1 = zoom(mask, 1/3, order=0)
    downscaled_shape_2 = zoom(downscaled_shape_1, 1/6, order=0)
    downscaled_shape_3 = zoom(downscaled_shape_2, 1/7, order=0)

    downscaled_masks = {
        0: downscaled_shape_3,
        1: downscaled_shape_2,
        2: downscaled_shape_1,
        3: mask,
    }
    return downscaled_masks

def generate_close_points(downscaled_shape, threshold):
    """Generate the starting points for the DLA shape by determining a set of points an appropriate distance from the edge of the mask.

    Parameters:
    downscaled_shape (np.ndarray): The downscaled mask.
    threshold (float): The threshold for determining the distance from the edge.

    Returns:
    close_points (np.ndarray): The set of starting points.
    """
    distances = edge_boundary_distance_transform(downscaled_shape)
    max_distance = distances.max()
    half_max_distance = max_distance / threshold
    close_points = np.where(distances >= half_max_distance, 1, 0)
    return close_points

def find_new_roots(mask, shape_grids):
    """Find new roots for the DLA shape after 1 upscale by selecting deterministically
    some points an appropriate distance from the edge.

    Parameters:
    mask (np.ndarray): The mask.
    seed (int): Seed parameter (kept for compatibility, not used in this version).
    shape_grids (list): The list of shape grids (used for visualization).

    Returns:
    close_points (np.ndarray): The set of starting points.
    """
    distances = edge_boundary_distance_transform(mask)
    max_distance = distances.max()
    half_max_distance = max_distance / 5

    close_points = np.where(distances >= half_max_distance, 1, 0)

    one_indices = np.argwhere(close_points == 1)

    num_points_to_select = 10
    step = max(1, len(one_indices) // num_points_to_select)

    reduced_close_points = np.zeros_like(close_points)

    for i in range(0, len(one_indices), step):
        point = one_indices[i]
        reduced_close_points[point[0], point[1]] = 1

        if np.count_nonzero(reduced_close_points) >= num_points_to_select:
            break

    close_points = reduced_close_points
    shape_grids.append(close_points)
    return close_points, shape_grids


def ca_in_mask(seed, binary_mask, iterations=25):
    """The main function to generate the heightmap. It firstly generates the masks to be used at each scale.

    Then, it uses the CA to progressively fill each larger mask with the DLA shape, weighting and blurring the
    result at each step and combining it with the previous blurred result.

    Parameters:
    seed (int): The seed for the random number generator.
    binary_mask (np.ndarray): The binary mask to be used.

    Returns:
    final_heightmap (np.ndarray): The final heightmap.
    """
    save_path = "cellular_automata/heightmap.png"
    show_images = False
    save = False
    close_points_threshold = 1.4
    ca_size = math.floor(binary_mask.shape[0] / 126)
    scale_factors = {
        0: 7,
        1: 6,
        2: 3,
        3: 3,
    }

    downscaled_masks = generate_masks(binary_mask)
    close_points = generate_close_points(downscaled_masks.get(0), close_points_threshold)

    shape_grids = []

    ca = Growth_And_Crowding_CA(size=ca_size,
                            growth_threshold=2659,
                            initial_food=100,
                            food_algorithm="Diffuse",
                            eat_value=15,
                            steps_between_growth=2,
                            delta = 0.92,
                            initial_life_grid=close_points,
                            food_mask=downscaled_masks.get(0),
                            seed=seed,
                            )

    while ca.time < 4:
        ca.step()
    life_grid = ca.life_grid
    direction_grid = ca.direction_grid
    to_blur = life_grid
    shape_grids.append(to_blur)
    shape_grids.append(direction_grid)

    blurry_large = upscale_bilinear(to_blur, scale_factors.get(0))
    blurry_large = gaussian_filter(blurry_large, sigma=4)
    blurry_large *=1.4
    shape_grids.append(blurry_large)

    for i in range (1,3):
        large_grid = upscale_shape_with_full_adjacency(life_grid, direction_grid, scale_factors.get(i-1))
        shape_grids.append(large_grid)
        ca_size = ca_size * scale_factors.get(i-1)
        mask = downscaled_masks.get(i)
        shape_grids.append(mask)
        # if i == 1:
        #     close_points, shape_grids = find_new_roots(mask, shape_grids)
        #     initial_life_grid = np.logical_or(large_grid, close_points).astype(int)
        # else:
        initial_life_grid = large_grid
        shape_grids.append(mask)
        # if i == 1:
        #     close_points, shape_grids = find_new_roots(mask, shape_grids)
        #     initial_life_grid = np.logical_or(large_grid, close_points).astype(int)
        # else:
        initial_life_grid = large_grid

        ca = Growth_And_Crowding_CA(
            size=ca_size,
            growth_threshold=2650,
            initial_food=100,
            food_algorithm="Diffuse",
            eat_value=15,
            steps_between_growth=2,
            delta = 0.92,
            initial_life_grid = initial_life_grid,
            food_mask=mask,
            seed=seed,
        )

        if i == 1:
            while ca.time < 15:
                ca.step()
        else:
            while ca.time < iterations:
                ca.step()


        life_grid = ca.life_grid
        direction_grid = ca.direction_grid
        shape_grids.append(downscaled_masks.get(i))
        shape_grids.append(life_grid)
        to_blur = blurry_large + (0.7 * scale_factors.get(i)/10 * life_grid)
        blurry_large = upscale_bilinear(to_blur, scale_factors.get(i))
        blurry_large = gaussian_filter(blurry_large, 3+i*1.5)
        shape_grids.append(blurry_large)


    large_grid = upscale_shape_with_full_adjacency(life_grid, direction_grid, scale_factors.get(i))
    shape_grids.append(large_grid)
    to_blur = blurry_large + 0.15 * large_grid
    shape_grids.append(to_blur)
    blurred = gaussian_filter(to_blur, sigma=2)
    blurred += 0.011 * large_grid
    true_size = binary_mask.shape[0]
    base_ca_size = math.floor(true_size / 126)
    scaley_size = base_ca_size * 126
    final_heightmap = zoom(blurred, (true_size/scaley_size), order=1)
    shape_grids.append(blurred)

    if save:
        plt.figure(figsize=(1024/100, 1024/100), dpi=100)
        plt.imshow(shape_grids[-1], cmap="grey")
        plt.axis("off")
        plt.gca().set_position([0, 0, 1, 1])
        plt.savefig(save_path, bbox_inches="tight", pad_inches=0, dpi=100)
        plt.close()

    if show_images:
        fig, axes = plt.subplots(2, len(shape_grids)//2, figsize=(15, 10))
        axes = axes.flatten()
        for ax, grid in zip(axes, shape_grids, strict=False):
            ax.imshow(grid, cmap="grey")
            ax.set_title(f"Grid at size {grid.shape[0]}")
            ax.axis("off")
        plt.tight_layout()
        #make a new uuid for the image name
        plt.savefig(f"cellular_automata/{datetime.now().strftime('%Y-%m-%d_%H-%M-%S')}.png")
        #make a new uuid for the image name
        plt.savefig(f"cellular_automata/{datetime.now().strftime('%Y-%m-%d_%H-%M-%S')}.png")

    return final_heightmap
