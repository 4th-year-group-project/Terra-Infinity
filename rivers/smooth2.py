import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from scipy.ndimage import gaussian_filter
import networkx as nx

class RiverTerrainCarver(object):
    def __init__(self, edges, centroids, strahler_numbers, heightmap, heightmap_extent=None):
        """
        Initialize a RiverTerrainCarver object.
        
        Parameters:
        - edges: List of tuples (parent, child) representing river network
        - centroids: Array of numpy arrays [x, y] for each node
        - strahler_numbers: Dictionary or array mapping node IDs to Strahler numbers
        - heightmap: 2D numpy array representing terrain elevation (0 to 1)
        - heightmap_extent: (xmin, xmax, ymin, ymax) coordinates for the heightmap
                           If None, will use the bounds of the river network
        """
        self.edges = edges
        self.centroids = centroids
        self.strahler_numbers = strahler_numbers
        self.heightmap = heightmap
        self.heightmap_shape = heightmap.shape
        
        # Set up the heightmap spatial reference
        if heightmap_extent is None:
            x_min = min(point[0] for point in centroids)
            x_max = max(point[0] for point in centroids)
            y_min = min(point[1] for point in centroids)
            y_max = max(point[1] for point in centroids)
            # Add a small margin
            margin = 0.1 * max(x_max - x_min, y_max - y_min)
            self.heightmap_extent = [x_min - margin, x_max + margin, y_min - margin, y_max + margin]
        else:
            self.heightmap_extent = heightmap_extent
            
        # Initialize the tree spline
        self.tree_spline = TreeSpline(edges, centroids)
        self.tree_spline.smooth_tree()
        
        # Store the original heightmap for comparison
        self.original_heightmap = heightmap.copy()
        
    def world_to_grid(self, x, y):
        """Convert world coordinates to grid indices."""
        x_min, x_max, y_min, y_max = self.heightmap_extent
        grid_x = int(((x - x_min) / (x_max - x_min)) * (self.heightmap_shape[1] - 1))
        grid_y = int(((y - y_min) / (y_max - y_min)) * (self.heightmap_shape[0] - 1))
        # Ensure within bounds
        grid_x = max(0, min(grid_x, self.heightmap_shape[1] - 1))
        grid_y = max(0, min(grid_y, self.heightmap_shape[0] - 1))
        return grid_x, grid_y
    
    def get_node_width(self, node):
        """Get river width based on Strahler number."""
        # Access strahler number based on its type
        if isinstance(self.strahler_numbers, dict):
            strahler = self.strahler_numbers.get(node, 1)
        else:  # Assume it's an array
            strahler = self.strahler_numbers[node]
            
        # Scale width with Strahler number (adjust as needed)
        # This makes higher order streams wider
        base_width = 2.0  # Minimum river width in grid cells
        return base_width * (1.5 ** (strahler - 1))
    
    def carve_river(self, river_depth=0.2, blend_factor=0.5, smooth_width=1.5):
        """
        Carve the river into the heightmap.
        
        Parameters:
        - river_depth: Target depth (0 to 1) for the river channel
        - blend_factor: Controls how abruptly the river banks slope (0=sharp, 1=smooth)
        - smooth_width: Smoothing width multiplier for the river channel
        """
        # Get spline points for each edge
        spline_points = self.tree_spline.get_spline_points(resolution=100)
        
        # Create a river mask
        river_mask = np.ones(self.heightmap.shape)
        
        # For each edge in the river network
        for edge, points in spline_points.items():
            parent, child = edge
            
            # Get river width at this segment (average of parent and child)
            parent_width = self.get_node_width(parent)
            child_width = self.get_node_width(child)
            
            # For each point along the spline
            for i, point in enumerate(points):
                # Interpolate width along the spline
                t = i / (len(points) - 1)
                width = parent_width * (1 - t) + child_width * t
                
                # Convert world coordinates to grid indices
                grid_x, grid_y = self.world_to_grid(point[0], point[1])
                
                # Create a circular mask at this point
                for dy in range(-int(width * smooth_width), int(width * smooth_width) + 1):
                    for dx in range(-int(width * smooth_width), int(width * smooth_width) + 1):
                        nx, ny = grid_x + dx, grid_y + dy
                        
                        # Check if within heightmap bounds
                        if 0 <= nx < self.heightmap_shape[1] and 0 <= ny < self.heightmap_shape[0]:
                            # Calculate distance from point to center of river
                            dist = np.sqrt(dx**2 + dy**2)
                            
                            # If within river width, carve fully
                            if dist <= width:
                                river_mask[ny, nx] = 0
                            # If within blend zone, apply smooth transition
                            elif dist <= width * (1 + blend_factor):
                                blend = (dist - width) / (width * blend_factor)
                                river_mask[ny, nx] = min(river_mask[ny, nx], blend)
        
        # Smooth the river mask
        river_mask = gaussian_filter(river_mask, sigma=0.5)
        
        # Apply the river mask to the heightmap
        # This interpolates between the original height and the target river depth
        self.heightmap = self.original_heightmap * river_mask + river_depth * (1 - river_mask)
        
        return self.heightmap
    
    def plot_results(self, figsize=(15, 10)):
        """Plot the original and carved heightmaps with the river network."""
        fig, axs = plt.subplots(1, 2, figsize=figsize)
        
        # Plot original heightmap
        im1 = axs[0].imshow(self.original_heightmap, cmap='terrain', 
                           extent=self.heightmap_extent, origin='lower')
        axs[0].set_title('Original Terrain')
        fig.colorbar(im1, ax=axs[0], shrink=0.6)
        
        # Plot the smoothed river network on original terrain
        self.tree_spline.plot_on_axis(axs[0], plot_original=False)
        
        # Plot carved heightmap
        im2 = axs[1].imshow(self.heightmap, cmap='terrain', 
                          extent=self.heightmap_extent, origin='lower')
        axs[1].set_title('Terrain with Carved River')
        fig.colorbar(im2, ax=axs[1], shrink=0.6)
        
        # Plot the smoothed river network on carved terrain
        self.tree_spline.plot_on_axis(axs[1], plot_original=False)
        
        plt.tight_layout()
        plt.show()

    def plot_3d(self, figsize=(12, 10)):
        """Create a 3D plot of the carved terrain."""
        fig = plt.figure(figsize=figsize)
        ax = fig.add_subplot(111, projection='3d')
        
        # Create coordinate matrices
        x_min, x_max, y_min, y_max = self.heightmap_extent
        x = np.linspace(x_min, x_max, self.heightmap_shape[1])
        y = np.linspace(y_min, y_max, self.heightmap_shape[0])
        X, Y = np.meshgrid(x, y)
        
        # Plot the surface
        surf = ax.plot_surface(X, Y, self.heightmap, cmap=cm.terrain, 
                              linewidth=0, antialiased=True, alpha=0.8)
        
        # Plot the river network
        for edge, points in self.tree_spline.get_spline_points().items():
            parent, child = edge
            xs = points[:, 0]
            ys = points[:, 1]
            
            # Get heights for each point
            zs = []
            for x_pt, y_pt in zip(xs, ys):
                grid_x, grid_y = self.world_to_grid(x_pt, y_pt)
                # Add small offset to make river visible
                zs.append(self.heightmap[grid_y, grid_x] + 0.01)
            
            ax.plot(xs, ys, zs, color='blue', linewidth=2)
        
        fig.colorbar(surf, shrink=0.5, aspect=5)
        ax.set_title('3D Terrain with Carved River')
        
        plt.tight_layout()
        plt.show()


class TreeSpline:
    def __init__(self, edges, centroids):
        """
        Initialize a TreeSpline object.
        
        Parameters:
        - edges: List of tuples (parent, child) representing tree edges
        - centroids: Array of numpy arrays [x, y] for each node
                     Node IDs are assumed to be indices into this array
        """
        self.edges = edges
        self.centroids = centroids  # Array of [x, y] coordinates
        self.G = nx.DiGraph()
        
        # Build the graph from edges
        for parent, child in edges:
            self.G.add_edge(parent, child)
        
        # Validate that we have a tree
        if not nx.is_tree(self.G.to_undirected()):
            print("Warning: Input graph is not a tree!")
            
        # Find the root node (node with in-degree 0)
        self.root = [n for n, d in self.G.in_degree() if d == 0]
        if len(self.root) != 1:
            print(f"Warning: Found {len(self.root)} root nodes: {self.root}")
        self.root = self.root[0] if self.root else None
        
        # Calculate paths from root to each leaf
        self.paths = {}
        self.calculate_paths()
        
        # Store spline parameters
        self.splines = {}
        self.control_points = {}
        
    def calculate_paths(self):
        """Calculate paths from root to each leaf node."""
        leaves = [n for n, d in self.G.out_degree() if d == 0]
        for leaf in leaves:
            path = nx.shortest_path(self.G, self.root, leaf)
            self.paths[leaf] = path
    
    def compute_tangent_at_node(self, node):
        """
        Compute the tangent direction at a node based on incoming and outgoing edges.
        For G1 continuity, we average directions for branching nodes.
        """
        neighbors = list(self.G.predecessors(node)) + list(self.G.successors(node))
        
        if not neighbors:
            return np.array([0, 0])  # Isolated node
        
        tangent = np.array([0.0, 0.0])
        node_pos = self.centroids[node]
        
        for neighbor in neighbors:
            neighbor_pos = self.centroids[neighbor]
            direction = neighbor_pos - node_pos
            # Normalize the direction
            norm = np.linalg.norm(direction)
            if norm > 0:
                direction = direction / norm
                # For predecessors (incoming), reverse direction
                if neighbor in self.G.predecessors(node):
                    direction = -direction
                tangent += direction
                
        # Normalize the resulting tangent
        norm = np.linalg.norm(tangent)
        if norm > 0:
            tangent = tangent / norm
        
        return tangent
    
    def calculate_control_points(self):
        """
        Calculate control points for each edge to ensure G1 continuity.
        """
        for parent, child in self.edges:
            p0 = self.centroids[parent]
            p3 = self.centroids[child]
            
            # Get tangent directions at parent and child nodes
            t_parent = self.compute_tangent_at_node(parent)
            t_child = self.compute_tangent_at_node(child)
            
            # Calculate distance between nodes
            dist = np.linalg.norm(p3 - p0)
            
            # Control points: we place them along the tangent directions
            # Adjust the factor (1/3) to control the "curviness"
            p1 = p0 + t_parent * (dist / 3)
            p2 = p3 - t_child * (dist / 3)
            
            self.control_points[(parent, child)] = (p0, p1, p2, p3)
    
    def get_bezier_points(self, control_points, num_points=20):
        """
        Get points along a cubic Bezier curve defined by control points.
        """
        p0, p1, p2, p3 = control_points
        points = []
        
        for t in np.linspace(0, 1, num_points):
            # Cubic Bezier formula
            point = (1-t)**3 * p0 + 3*(1-t)**2*t * p1 + 3*(1-t)*t**2 * p2 + t**3 * p3
            points.append(point)
            
        return np.array(points)
    
    def smooth_tree(self):
        """
        Calculate control points and prepare splines for the entire tree.
        """
        self.calculate_control_points()
    
    def plot_tree(self, figsize=(10, 8), plot_original=True, plot_control_points=False):
        """
        Plot the original tree and the smoothed version.
        
        Parameters:
        - figsize: Figure size (width, height)
        - plot_original: Whether to plot the original tree edges
        - plot_control_points: Whether to plot control points
        """
        plt.figure(figsize=figsize)
        self.plot_on_axis(plt.gca(), plot_original, plot_control_points)
        plt.title('Tree with Spline Smoothing')
        plt.grid(True, alpha=0.3)
        plt.axis('equal')
        plt.tight_layout()
        plt.show()
        
    def plot_on_axis(self, ax, plot_original=True, plot_control_points=False):
        """
        Plot the tree on a given axis.
        
        Parameters:
        - ax: Matplotlib axis to plot on
        - plot_original: Whether to plot original tree edges
        - plot_control_points: Whether to plot control points
        """
        # Get node coordinates directly from centroids array
        node_x = [self.centroids[i][0] for i in range(len(self.centroids)) if i in self.G.nodes()]
        node_y = [self.centroids[i][1] for i in range(len(self.centroids)) if i in self.G.nodes()]
        
        # Plot nodes
        ax.scatter(node_x, node_y, c='blue', s=50, zorder=3)
        
        # Plot node labels
        for node in self.G.nodes():
            ax.annotate(str(node), self.centroids[node], xytext=(5, 5), 
                       textcoords='offset points')

        # Plot original tree edges if requested
        if plot_original:
            for parent, child in self.edges:
                ax.plot([self.centroids[parent][0], self.centroids[child][0]], 
                      [self.centroids[parent][1], self.centroids[child][1]], 
                      'k--', alpha=0.3, zorder=1)
        
        # Plot smoothed tree using Bezier curves
        for edge, points in self.control_points.items():
            bezier_points = self.get_bezier_points(points)
            ax.plot(bezier_points[:, 0], bezier_points[:, 1], 'b-', linewidth=2, zorder=2)
            
            # Plot control points if requested
            if plot_control_points:
                p0, p1, p2, p3 = points
                ax.plot([p0[0], p1[0]], [p0[1], p1[1]], 'r:', alpha=0.5)
                ax.plot([p2[0], p3[0]], [p2[1], p3[1]], 'r:', alpha=0.5)
                ax.scatter([p1[0], p2[0]], [p1[1], p2[1]], c='red', s=30, alpha=0.5)

    def get_spline_points(self, resolution=100):
        """
        Get all points along the splines for the entire tree.
        
        Parameters:
        - resolution: Number of points per edge
        
        Returns:
        - Dictionary mapping edge (parent, child) to array of points
        """
        result = {}
        for edge, control_points in self.control_points.items():
            result[edge] = self.get_bezier_points(control_points, resolution)
        return result


# Example usage
def example():
    # Define node positions as array of numpy arrays
    centroids = np.array([
        [0, 0],      # Node 0 (not used)
        [50, 50],    # Node 1 (root)
        [60, 70],    # Node 2
        [40, 70],    # Node 3
        [70, 90],    # Node 4
        [50, 90],    # Node 5
        [30, 90],    # Node 6
        [80, 110],   # Node 7
        [60, 110],   # Node 8
        [40, 110],   # Node 9
        [20, 110]    # Node 10
    ])
    
    # Define the edges (parent, child)
    edges = [
        (1, 2), (1, 3),           # Root to first level
        (2, 4), (2, 5), (3, 6),   # First to second level
        (4, 7), (4, 8), (6, 9), (6, 10)  # Second to third level
    ]
    
    # Define Strahler numbers for each node
    strahler_numbers = {
        1: 3,  # Root
        2: 2, 3: 2,  # First level
        4: 2, 5: 1, 6: 2,  # Second level
        7: 1, 8: 1, 9: 1, 10: 1  # Third level (leaves)
    }
    
    # Create a sample heightmap (100x100 grid)
    # This creates a terrain with hills and valleys
    size = 200
    x = np.linspace(0, 1, size)
    y = np.linspace(0, 1, size)
    X, Y = np.meshgrid(x, y)
    
    # Create some terrain features
    heightmap = 0.5 + 0.2 * np.sin(10 * X) * np.cos(10 * Y)
    heightmap = heightmap + 0.3 * gaussian_filter(np.random.random((size, size)), sigma=10)
    heightmap = heightmap / heightmap.max()  # Normalize to [0, 1]
    
    # Define heightmap spatial reference
    heightmap_extent = [0, 100, 0, 150]
    
    # Create the river carver
    river_carver = RiverTerrainCarver(edges, centroids, strahler_numbers, 
                                     heightmap, heightmap_extent)
    
    # Carve the river into the terrain
    river_carver.carve_river(river_depth=0.2, blend_factor=1.0, smooth_width=2.0)
    
    # Plot the results
    river_carver.plot_results()
    
    # Create a 3D visualization
    river_carver.plot_3d()


if __name__ == "__main__":
    example()