import matplotlib.pyplot as plt
import numpy as np


def diamond_square(size, roughness=0.5, seed=None):
    """Implementation of the Diamond-Square algorithm for terrain generation.

    Args:
        size: Size of the grid. Must be (2^n) + 1
        roughness: Controls the roughness of the terrain (0.0 to 1.0)
        seed: Random seed for reproducibility

    Returns:
        A 2D numpy array representing the heightmap
    """
    # Check if size is valid (must be 2^n + 1)
    n = np.log2(size - 1)
    if not n.is_integer() or n < 1:
        raise ValueError("Size must be (2^n) + 1 for some integer n >= 1")

    # Set random seed if provided
    if seed is not None:
        np.random.seed(seed)

    # Initialize the heightmap with zeros
    heightmap = np.zeros((size, size))

    # Set the four corners to random values
    heightmap[0, 0] = np.random.random()
    heightmap[0, size - 1] = np.random.random()
    heightmap[size - 1, 0] = np.random.random()
    heightmap[size - 1, size - 1] = np.random.random()

    # Diamond-Square algorithm
    step = size - 1
    while step > 1:
        half_step = step // 2

        # Diamond step
        for i in range(half_step, size, step):
            for j in range(half_step, size, step):
                # Average the four corners
                avg = (
                    heightmap[i - half_step, j - half_step]  # top-left
                    + heightmap[i - half_step, j + half_step]  # top-right
                    + heightmap[i + half_step, j - half_step]  # bottom-left
                    + heightmap[i + half_step, j + half_step]
                ) / 4.0  # bottom-right

                # Add random displacement
                displacement = np.random.uniform(-1.0, 1.0) * roughness * step / (size - 1)
                heightmap[i, j] = avg + displacement

        # Square step
        for i in range(0, size, half_step):
            for j in range((i + half_step) % step, size, step):
                # Average the four adjacent points
                count = 0
                avg = 0

                # Check top
                if i - half_step >= 0:
                    avg += heightmap[i - half_step, j]
                    count += 1

                # Check bottom
                if i + half_step < size:
                    avg += heightmap[i + half_step, j]
                    count += 1

                # Check left
                if j - half_step >= 0:
                    avg += heightmap[i, j - half_step]
                    count += 1

                # Check right
                if j + half_step < size:
                    avg += heightmap[i, j + half_step]
                    count += 1

                # Calculate average
                avg /= count

                # Add random displacement
                displacement = np.random.uniform(-1.0, 1.0) * roughness * step / (size - 1)
                heightmap[i, j] = avg + displacement

        # Reduce the random displacement range for the next iteration
        step //= 2

    return heightmap


def normalize_heightmap(heightmap):
    """Normalize the heightmap to the range [0, 1]"""
    min_val = np.min(heightmap)
    max_val = np.max(heightmap)
    return (heightmap - min_val) / (max_val - min_val)


def visualize_heightmap(heightmap):
    """Visualize the heightmap in 2D and 3D"""

    # Create a figure with two subplots
    plt.imshow(heightmap, cmap="terrain", origin="lower")
    plt.axis("off")
    plt.colorbar()
    plt.savefig("heightmap_2d.png", bbox_inches="tight", dpi=300)

    from .display import Display

    display = Display(heightmap, height_scale=250, colormap="terrain")
    display.display_heightmap()
    display.save_heightmap("diamond_square.png")


# # Example usage
# if __name__ == "__main__":
#     # Generate a 129x129 heightmap (2^7 + 1)
#     size = 513
#     roughness = 0.5


#     # Generate the heightmap
#     heightmap = diamond_square(size, roughness, seed=42)

#     # Normalize the heightmap
#     heightmap = normalize_heightmap(heightmap)

#     # Visualize the result
#     visualize_heightmap(heightmap)
