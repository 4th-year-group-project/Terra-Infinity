"""Diamond-Square Algorithm for Terrain Generation."""
import matplotlib.pyplot as plt
import numpy as np

def diamond_square(size, roughness=0.5, seed=None):
    """Generates a heightmap using the Diamond-Square algorithm.
    
    Args:
        size (int): Size of the heightmap (must be (2^n) + 1 for some integer n).
        roughness (float): Roughness factor for the terrain.
        seed (int, optional): Random seed for reproducibility.

    Returns:
        np.ndarray: Generated heightmap.
    """

    n = np.log2(size - 1)
    if not n.is_integer() or n < 1:
        raise ValueError("Size must be (2^n) + 1 for some integer n >= 1")

    if seed is not None:
        np.random.seed(seed)

    heightmap = np.zeros((size, size))

    heightmap[0, 0] = np.random.random()
    heightmap[0, size - 1] = np.random.random()
    heightmap[size - 1, 0] = np.random.random()
    heightmap[size - 1, size - 1] = np.random.random()

    step = size - 1
    while step > 1:
        half_step = step // 2

        
        for i in range(half_step, size, step):
            for j in range(half_step, size, step):
                
                avg = (
                    heightmap[i - half_step, j - half_step]  
                    + heightmap[i - half_step, j + half_step]  
                    + heightmap[i + half_step, j - half_step]  
                    + heightmap[i + half_step, j + half_step]
                ) / 4.0  

                
                displacement = np.random.uniform(-1.0, 1.0) * roughness * step / (size - 1)
                heightmap[i, j] = avg + displacement

        
        for i in range(0, size, half_step):
            for j in range((i + half_step) % step, size, step):
                
                count = 0
                avg = 0

                
                if i - half_step >= 0:
                    avg += heightmap[i - half_step, j]
                    count += 1

                
                if i + half_step < size:
                    avg += heightmap[i + half_step, j]
                    count += 1

                
                if j - half_step >= 0:
                    avg += heightmap[i, j - half_step]
                    count += 1

                
                if j + half_step < size:
                    avg += heightmap[i, j + half_step]
                    count += 1

                
                avg /= count

                
                displacement = np.random.uniform(-1.0, 1.0) * roughness * step / (size - 1)
                heightmap[i, j] = avg + displacement

        
        step //= 2

    return heightmap

if __name__ == "__main__":
    size = 513
    roughness = 0.5

    heightmap = diamond_square(size, roughness, seed=42)
    heightmap = (heightmap - np.min(heightmap)) / (np.max(heightmap) - np.min(heightmap))

    plt.imshow(heightmap, cmap="terrain", origin="lower")
    plt.axis("off")
    plt.colorbar()
    plt.savefig("heightmap_2d.png", bbox_inches="tight", dpi=300)

    from .display import Display

    display = Display(heightmap, height_scale=250, colormap="terrain")
    display.display_heightmap()
    display.save_heightmap("diamond_square.png")
