import numpy as np
import matplotlib.pyplot as plt
from scipy import stats
from scipy.spatial import cKDTree
import multiprocessing

# Function to generate Poisson disc samples
def poisson_disc_sampling(width, height, radius, density=None, max_attempts=30):
    """
    Generate samples using Poisson disc sampling method.
    
    Parameters:
    width, height - dimensions of the sampling area
    radius - minimum distance between samples
    density - approximate number of points to generate (optional)
    max_attempts - maximum number of attempts to place a new point
    
    Returns:
    List of points as (x, y) coordinates
    """
    # Calculate cell size for acceleration grid
    cell_size = radius / np.sqrt(2)
    
    # Calculate grid dimensions
    grid_width = int(np.ceil(width / cell_size))
    grid_height = int(np.ceil(height / cell_size))
    
    # Initialize grid
    grid = [None] * (grid_width * grid_height)
    
    # List to store samples
    samples = []
    active_list = []
    
    # Helper function to get grid index
    def grid_index(x, y):
        return int(x / cell_size) + int(y / cell_size) * grid_width
    
    # Helper function to generate random point around a reference point
    def random_point_around(point, min_dist):
        radius_range = (min_dist, 2 * min_dist)
        
        # Choose random radius and angle
        random_radius = np.random.uniform(radius_range[0], radius_range[1])
        random_angle = np.random.uniform(0, 2 * np.pi)
        
        # Calculate new point
        new_x = point[0] + random_radius * np.cos(random_angle)
        new_y = point[1] + random_radius * np.sin(random_angle)
        
        return (new_x, new_y)
    
    # Helper function to check if point is valid
    def is_valid(point):
        # Check if point is in bounds
        if not (0 <= point[0] < width and 0 <= point[1] < height):
            return False
        
        # Get nearby cells
        cell_x = int(point[0] / cell_size)
        cell_y = int(point[1] / cell_size)
        
        # Check surrounding cells
        search_start_x = max(0, cell_x - 2)
        search_end_x = min(grid_width - 1, cell_x + 2)
        search_start_y = max(0, cell_y - 2)
        search_end_y = min(grid_height - 1, cell_y + 2)
        
        for search_y in range(search_start_y, search_end_y + 1):
            for search_x in range(search_start_x, search_end_x + 1):
                cell_idx = search_x + search_y * grid_width
                sample_idx = grid[cell_idx]
                
                if sample_idx is not None:
                    sample = samples[sample_idx]
                    distance = np.sqrt((point[0] - sample[0])**2 + (point[1] - sample[1])**2)
                    if distance < radius:
                        return False
        
        return True
    
    # Add first point
    first_point = (np.random.uniform(0, width), np.random.uniform(0, height))
    samples.append(first_point)
    grid_idx = grid_index(first_point[0], first_point[1])
    grid[grid_idx] = 0
    active_list.append(0)
    
    # Continue until desired density is reached or no active points remain
    target_count = density if density else float('inf')
    
    while active_list and (density is None or len(samples) < target_count):
        # Randomly select an active point
        active_idx = np.random.randint(0, len(active_list))
        active_point_idx = active_list[active_idx]
        active_point = samples[active_point_idx]
        
        # Try to find a valid new point around the active point
        found = False
        for _ in range(max_attempts):
            new_point = random_point_around(active_point, radius)
            if is_valid(new_point):
                samples.append(new_point)
                new_idx = len(samples) - 1
                grid[grid_index(new_point[0], new_point[1])] = new_idx
                active_list.append(new_idx)
                found = True
                break
        
        # If no valid point was found, remove the active point
        if not found:
            active_list.pop(active_idx)
    
    return np.array(samples)

# Set parameters
width, height = 100, 100
num_points = 500
radius = 3.0  # Minimum distance between points for Poisson disc

# 1. Generate uniform random points
np.random.seed(42)
uniform_points = np.random.uniform(0, [width, height], size=(num_points, 2))

# 2. Generate Poisson disc samples
np.random.seed(42)
poisson_points = poisson_disc_sampling(width, height, radius, density=1000)

# 3. Plot each distribution separately
plt.figure(figsize=(8, 8))
plt.scatter(uniform_points[:, 0], uniform_points[:, 1], s=10, color='blue', alpha=0.7)
plt.xlim(0, width)
plt.ylim(0, height)
plt.gca().set_aspect('equal', adjustable='box')
plt.tight_layout()
plt.axis('off')
plt.savefig('uniform_distribution.png', dpi=300)
plt.show()

plt.figure(figsize=(8, 8))
plt.scatter(poisson_points[:, 0], poisson_points[:, 1], s=10, color='red', alpha=0.7)
plt.xlim(0, width)
plt.ylim(0, height)
plt.gca().set_aspect('equal', adjustable='box')
plt.tight_layout()
plt.axis('off')
plt.savefig('poisson_distribution.png', dpi=300)
plt.show()

# Optional: Print some statistics about the distributions
print(f"Uniform distribution: {len(uniform_points)} points")
print(f"Poisson disc sampling: {len(poisson_points)} points")

# Analyze the minimum distances between points
def min_distance_analysis(points):
    tree = cKDTree(points)
    distances, _ = tree.query(points, k=2)
    min_distances = distances[:, 1]  # Second column has distance to nearest neighbor
    return {
        "mean": np.mean(min_distances),
        "min": np.min(min_distances),
        "max": np.max(min_distances)
    }

uniform_stats = min_distance_analysis(uniform_points)
poisson_stats = min_distance_analysis(poisson_points)

print("\nMinimum distance statistics:")
print(f"Uniform - Mean: {uniform_stats['mean']:.2f}, Min: {uniform_stats['min']:.2f}, Max: {uniform_stats['max']:.2f}")
print(f"Poisson - Mean: {poisson_stats['mean']:.2f}, Min: {poisson_stats['min']:.2f}, Max: {poisson_stats['max']:.2f}")