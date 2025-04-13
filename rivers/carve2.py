from generation import Noise, Display, tools
import matplotlib.pyplot as plt
import numpy as np
from scipy.spatial import ckdtree
from scipy.spatial.distance import cdist
from .smooth import TreeSpline  
from scipy.ndimage import distance_transform_edt, gaussian_filter

import cv2

noise_generator = Noise(seed=0, width=1024, height=1024, x_offset=0, y_offset=0)
heightmap = noise_generator.fractal_simplex_noise(noise="open", scale=256, octaves=8, persistence=0.5, lacunarity=2.0)
heightmap = tools.normalize(heightmap, 0.2, 1)

def smooth_min(a, b, k):
    h = np.clip((b - a + k) / (2 * k), 0, 1)
    return a * h + b * (1 - h) - k * h * (1 - h)

def smooth_max(a, b, k):
    k = -k
    h = np.clip((b - a + k) / (2 * k), 0, 1)
    return a * h + b * (1 - h) - k * h * (1 - h)

centroids = np.array([
    [0, 0],         # Node 0 (not used)
    [512, 0],       # Node 1 (root)
    [768, 256],     # Node 2
    [256, 256],     # Node 3
    [896, 512],     # Node 4
    [640, 512],     # Node 5
    [128, 512],     # Node 6
    [960, 768],     # Node 7
    [832, 768],     # Node 8
    [192, 768],     # Node 9
    [64, 768],      # Node 10
])

edges = [
    (1, 2), (1, 3),           # Root to first level
    (2, 4), (2, 5), (3, 6),   # First to second level
    (4, 7), (4, 8), (6, 9), (6, 10)  # Second to third level
]

strahler_numbers = {
    1: 3,
    2: 2,
    3: 2,
    4: 2,
    5: 1,
    6: 2,
    7: 1,
    8: 1,
    9: 1,
    10: 1
} 

tree = TreeSpline(edges, centroids)
tree.smooth_tree()
splines = tree.get_spline_points()

def carve_river(heightmap, splines, strahler_numbers, river_width=15, fade_distance=30, river_depth=0.18, noise_factor=0.2):
    """
    Carve a natural river into a heightmap based on tree splines with variable width.
    
    Parameters:
    -----------
    heightmap : 2D numpy array (1024x1024)
        The terrain heightmap with values between 0 and 1
    splines : dict
        Dictionary of edges to lists of points making up the path
    strahler_numbers : dict
        Dictionary mapping node indices to Strahler numbers
    river_width : float
        Base width of the river channel
    fade_distance : float
        Additional distance for smooth transition from river to terrain
    river_depth : float
        Target height for the river (should be below 0.2 for underwater)
    noise_factor : float
        Amount of random variation in river width (0.0 = uniform, higher = more varied)
        
    Returns:
    --------
    2D numpy array (1024x1024)
        Modified heightmap with river carved into it
    """
    # Create a copy of the heightmap to modify
    new_heightmap = heightmap.copy()
    height, width = heightmap.shape
    
    # Create a binary mask for the river paths
    river_mask = np.zeros((height, width), dtype=bool)
    
    # Find the maximum Strahler number for normalization
    max_strahler = max(strahler_numbers.values())
    
    # Create a node-to-edges mapping to find connected edges
    node_to_edges = {}
    for edge in splines.keys():
        start_node, end_node = edge
        if start_node not in node_to_edges:
            node_to_edges[start_node] = []
        if end_node not in node_to_edges:
            node_to_edges[end_node] = []
        node_to_edges[start_node].append(edge)
        node_to_edges[end_node].append(edge)
    
    # Find leaf nodes (nodes with only one connection)
    leaf_nodes = [node for node, edges in node_to_edges.items() if len(edges) == 1]
    
    # Find root node (assume it's node 1 based on your data)
    root_node = 1
    
    # Calculate path distance from each node to the root
    node_distances = {}
    
    def calculate_path_length(node, edges_to_node):
        """Calculate the path length from a node to the root."""
        if node == root_node:
            return 0
        
        # Find the edge that connects this node to the parent
        parent_edge = None
        for edge in edges_to_node:
            start, end = edge
            if end == node:  # Assuming direction is from parent to child
                parent_edge = edge
                parent_node = start
                break
            elif start == node:  # Handle case where direction might be reversed
                parent_edge = edge
                parent_node = end
                break
        
        if parent_edge is None:
            return 0  # No path to root
        
        # Get the spline points for this edge
        points = np.array(splines[parent_edge])
        
        # Calculate the length of this edge
        edge_length = 0
        for i in range(len(points) - 1):
            edge_length += np.linalg.norm(points[i+1] - points[i])
        
        # Recursively calculate parent's distance to root
        if parent_node in node_distances:
            parent_distance = node_distances[parent_node]
        else:
            parent_distance = calculate_path_length(parent_node, node_to_edges[parent_node])
            node_distances[parent_node] = parent_distance
        
        # Return total distance
        return parent_distance + edge_length
    
    # Calculate distances from leaf nodes to root
    max_path_length = 0
    for leaf in leaf_nodes:
        path_length = calculate_path_length(leaf, node_to_edges[leaf])
        node_distances[leaf] = path_length
        max_path_length = max(max_path_length, path_length)
    
    # Draw the river paths on the mask with varying widths
    for edge, points in splines.items():
        start_node, end_node = edge
        
        # Get Strahler number for this edge
        strahler = strahler_numbers.get(end_node, 1)
        
        # Convert points to numpy array if not already
        points = np.array(points)
        
        # Calculate width profiles for the edge
        num_points = len(points)
        
        # Base width factor from Strahler number - main channels are wider
        strahler_factor = 0.3 + 0.7 * (strahler / max_strahler)
        
        # Determine if this is a leaf edge (end node is a leaf)
        is_leaf_edge = end_node in leaf_nodes
        
        # For leaf edges, taper width toward the end
        if is_leaf_edge:
            # Taper from base width to very thin at the end
            width_profile = np.linspace(strahler_factor, 0.15, num_points)
        else:
            # For non-leaf edges, use a slight taper based on distance from root
            start_dist = node_distances.get(start_node, 0) / max_path_length
            end_dist = node_distances.get(end_node, 0) / max_path_length
            
            # Slight tapering based on distance from root (farther = thinner)
            width_profile = np.linspace(
                strahler_factor * (1 - 0.1 * start_dist),
                strahler_factor * (1 - 0.1 * end_dist),
                num_points
            )
        
        # Draw the river segment with varying width
        for i in range(len(points) - 1):
            p1 = points[i].astype(int)
            p2 = points[i + 1].astype(int)
            
            # Calculate interpolated width at this segment
            segment_width = int(river_width * width_profile[i])
            
            # Draw thick line using modified Bresenham algorithm
            draw_thick_line(river_mask, p1[0], p1[1], p2[0], p2[1], segment_width)
    

    distance = distance_transform_edt(~river_mask)

    plt.imshow(river_mask, cmap='gray')
    plt.colorbar(label='River Mask')
    plt.show()
    
    river_noise = (noise_generator.fractal_simplex_noise(scale=256, octaves=7, persistence=0.5, lacunarity=2.0)+1)/2

    # Apply the distance to carve the river
    for y in range(height):
        for x in range(width):
            d = distance[y, x]
            
            if d < fade_distance:
                # Get normalized distance
                t = d / fade_distance
                smooth_t = t * t * (3 - 2 * t)

                # Sample noise at this location
                noise_val = river_noise[y, x]

                # Amount of noise influence: stronger near center, fades out
                noise_strength = (1 - smooth_t)**1  # steeper fade near edges
                max_noise_depth = 0.15  # Max depth variation (adjustable)

                depth_variation = (noise_val - 0.5) * 2 * max_noise_depth * noise_strength

                # Final river height with noise
                base_river_height = river_depth + depth_variation
                new_heightmap[y, x] = base_river_height + (heightmap[y, x] - base_river_height) * smooth_t
    
    return new_heightmap

def draw_thick_line(mask, x0, y0, x1, y1, thickness):
    """
    Draw a thick line on a binary mask using a simplified approach.
    """
    # Ensure coordinates are within bounds
    h, w = mask.shape
    x0, y0 = max(0, min(x0, w-1)), max(0, min(y0, h-1))
    x1, y1 = max(0, min(x1, w-1)), max(0, min(y1, h-1))
    
    # Draw line using Bresenham algorithm
    dx = abs(x1 - x0)
    dy = abs(y1 - y0)
    sx = 1 if x0 < x1 else -1
    sy = 1 if y0 < y1 else -1
    err = dx - dy
    
    radius = max(1, thickness // 2)  # Ensure minimum radius of 1
    
    while True:
        # Draw a "dot" of specified thickness at current position
        y_min = max(0, y0 - radius)
        y_max = min(h, y0 + radius + 1)
        x_min = max(0, x0 - radius)
        x_max = min(w, x0 + radius + 1)
        
        for y in range(y_min, y_max):
            for x in range(x_min, x_max):
                # Use circle equation to determine if point is within radius
                if (x - x0)**2 + (y - y0)**2 <= radius**2:
                    mask[y, x] = True
        
        if x0 == x1 and y0 == y1:
            break
        
        e2 = 2 * err
        if e2 > -dy:
            err -= dy
            x0 += sx
        if e2 < dx:
            err += dx
            y0 += sy


import time
start = time.time()
new_heightmap = carve_river(heightmap, splines, strahler_numbers, river_width=10, fade_distance=100, river_depth=0.18, noise_factor=1)
print(time.time()-start)    

print(np.min(new_heightmap), np.max(new_heightmap))
display = Display(height_array=new_heightmap, height_scale=250, colormap="terrain")
display.display_heightmap()