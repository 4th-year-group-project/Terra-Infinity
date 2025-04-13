import numpy as np
import matplotlib.pyplot as plt
import networkx as nx
from scipy import ndimage
from skimage import draw, morphology

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
        
        # Get node coordinates directly from centroids array
        node_x = [self.centroids[i][0] for i in range(len(self.centroids)) if i in self.G.nodes()]
        node_y = [self.centroids[i][1] for i in range(len(self.centroids)) if i in self.G.nodes()]
        
        # Plot nodes
        plt.scatter(node_x, node_y, c='blue', s=50, zorder=3)
        
        # Plot node labels
        for node in self.G.nodes():
            plt.annotate(str(node), self.centroids[node], xytext=(5, 5), 
                         textcoords='offset points')

        # Plot original tree edges if requested
        if plot_original:
            for parent, child in self.edges:
                plt.plot([self.centroids[parent][0], self.centroids[child][0]], 
                        [self.centroids[parent][1], self.centroids[child][1]], 
                        'k--', alpha=0.3, zorder=1)
        
        # Plot smoothed tree using Bezier curves
        for edge, points in self.control_points.items():
            bezier_points = self.get_bezier_points(points)
            plt.plot(bezier_points[:, 0], bezier_points[:, 1], 'g-', linewidth=2, zorder=2)
            
            # Plot control points if requested
            if plot_control_points:
                p0, p1, p2, p3 = points
                plt.plot([p0[0], p1[0]], [p0[1], p1[1]], 'r:', alpha=0.5)
                plt.plot([p2[0], p3[0]], [p2[1], p3[1]], 'r:', alpha=0.5)
                plt.scatter([p1[0], p2[0]], [p1[1], p2[1]], c='red', s=30, alpha=0.5)
        
        plt.title('Tree with Spline Smoothing')
        plt.grid(True, alpha=0.3)
        plt.axis('equal')
        plt.tight_layout()
        plt.show()

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

    def create_binary_mask(self, width, height, thickness=5, scale_factor=10, fillet_quality=1.5):
        """
        Create a binary mask of the tree with true fillet joins at branch junctions.
        
        Parameters:
        - width, height: Dimensions of the output mask in pixels
        - thickness: Thickness of the branches in the mask
        - scale_factor: Scale factor to convert from tree coordinates to pixel coordinates
        - fillet_quality: Controls the smoothness of fillets (higher values = smoother fillets)
        
        Returns:
        - Binary mask as a numpy array (0s and 1s)
        """
        # Make sure we have control points calculated
        if not self.control_points:
            self.smooth_tree()
        
        # Calculate the bounds of the tree to center it in the image
        x_coords = [self.centroids[i][0] for i in range(len(self.centroids)) if i in self.G.nodes()]
        y_coords = [self.centroids[i][1] for i in range(len(self.centroids)) if i in self.G.nodes()]
        min_x, max_x = min(x_coords), max(x_coords)
        min_y, max_y = min(y_coords), max(y_coords)
        
        # Calculate offsets to center the tree in the image
        x_offset = (width / scale_factor - (max_x - min_x)) / 2 - min_x
        y_offset = (height / scale_factor - (max_y - min_y)) / 2 - min_y
        
        # Create an empty mask
        mask = np.zeros((height, width), dtype=np.uint8)
        
        # First pass: Draw thin splines for the skeleton
        for edge, control_points in self.control_points.items():
            # Get points along the spline
            points = self.get_bezier_points(control_points, num_points=100)
            
            # Transform points to image coordinates
            points_transformed = []
            for p in points:
                x = int((p[0] + x_offset) * scale_factor)
                y = int((p[1] + y_offset) * scale_factor)
                # Ensure points are inside the image
                x = max(0, min(width - 1, x))
                y = max(0, min(height - 1, y))
                points_transformed.append((y, x))  # Note: y,x order for skimage
            
            # Draw thin line for this spline (skeleton)
            for i in range(len(points_transformed) - 1):
                rr, cc = draw.line(*points_transformed[i], *points_transformed[i+1])
                # Make sure indices are within bounds
                valid_idx = (rr >= 0) & (rr < height) & (cc >= 0) & (cc < width)
                mask[rr[valid_idx], cc[valid_idx]] = 1
        
        # Second pass: Dilate the skeleton to create branches with thickness
        mask = ndimage.binary_dilation(mask, iterations=thickness)
        
        # Find branch junctions (via morphological operations)
        # First, get the skeleton of the mask
        skeleton = morphology.skeletonize(mask)
        
        # Create an empty mask for filleted regions
        fillet_mask = np.zeros_like(mask)
        
        # Get branch points in the skeleton
        # A branch point is where a pixel has more than 2 neighbors
        rows, cols = np.where(skeleton)
        for r, c in zip(rows, cols):
            # Check 3x3 neighborhood
            if r > 0 and r < height-1 and c > 0 and c < width-1:
                # Count neighbors
                neighbors = skeleton[r-1:r+2, c-1:c+2].sum() - skeleton[r, c]
                if neighbors > 2:  # Branch point detected
                    # Mark this point for filleting
                    fillet_mask[r, c] = 1
        
        # Dilate the branch points to get regions where fillets should be applied
        fillet_regions = ndimage.binary_dilation(fillet_mask, iterations=int(thickness*1.5))
        
        # Apply distance transform to the inverted mask to get distance to background
        distance = ndimage.distance_transform_edt(mask)
        
        # Create a new mask for the final result
        final_mask = mask.copy()
        
        # Apply filleting in the branch regions
        for r, c in zip(*np.where(fillet_regions)):
            # If this is inside the mask and close to the boundary
            if mask[r, c] == 1:
                # Calculate distances to branch edges within neighborhood
                neighborhood_size = int(thickness * 1.5 * fillet_quality)
                r_min = max(0, r - neighborhood_size)
                r_max = min(height, r + neighborhood_size + 1)
                c_min = max(0, c - neighborhood_size)
                c_max = min(width, c + neighborhood_size + 1)
                
                # Get distance values in neighborhood
                dist_neighborhood = distance[r_min:r_max, c_min:c_max]
                
                # Filter out extreme values for smoother filleting
                # This creates nice concave fillets at branch joints
                if dist_neighborhood.size > 0:
                    threshold = thickness * 0.3  # Adjust this value for fillet size
                    if distance[r, c] < threshold:
                        final_mask[r, c] = 0  # Remove interior corners for filleting
        
        return final_mask
    
    def plot_binary_mask(self, mask, figsize=(10, 8)):
        """
        Plot the binary mask.
        
        Parameters:
        - mask: Binary mask as a numpy array
        - figsize: Figure size (width, height)
        """
        plt.figure(figsize=figsize)
        plt.imshow(mask, cmap='binary')
        plt.title('Tree Binary Mask with Filleted Joints')
        plt.axis('off')
        plt.tight_layout()
        plt.show()
        
    def save_binary_mask(self, mask, filename):
        """
        Save the binary mask to a file.
        
        Parameters:
        - mask: Binary mask as a numpy array
        - filename: Output filename
        """
        plt.figure(figsize=(10, 8))
        plt.imshow(mask, cmap='binary')
        plt.axis('off')
        plt.tight_layout()
        plt.savefig(filename, bbox_inches='tight', pad_inches=0)
        plt.close()
        
        # Also save as numpy array if needed
        np.save(filename.replace('.png', '.npy'), mask)
        print(f"Mask saved to {filename} and {filename.replace('.png', '.npy')}")


# Example usage with binary mask generation
def example():
    # Define node positions as array of numpy arrays
    centroids = np.array([
        [0, 0],      # Node 0 (not used)
        [0, 0],      # Node 1 (root)
        [1, 2],      # Node 2
        [-1, 2],     # Node 3
        [2, 4],      # Node 4
        [0, 4],      # Node 5
        [-2, 4],     # Node 6
        [3, 6],      # Node 7
        [1, 6],      # Node 8
        [-1, 6],     # Node 9
        [-3, 6]      # Node 10
    ])
    
    # Define the edges (parent, child)
    edges = [
        (1, 2), (1, 3),           # Root to first level
        (2, 4), (2, 5), (3, 6),   # First to second level
        (4, 7), (4, 8), (6, 9), (6, 10)  # Second to third level
    ]
    
    # Create the tree spline
    tree_spline = TreeSpline(edges, centroids)
    
    # Smooth the tree and plot
    tree_spline.smooth_tree()
    tree_spline.plot_tree(plot_original=True, plot_control_points=True)
    
    # Create binary mask (512x512 pixels) with proper fillets
    mask = tree_spline.create_binary_mask(width=512, height=512, thickness=3, 
                                          scale_factor=50, fillet_quality=100.0)
    
    # Plot the mask
    tree_spline.plot_binary_mask(mask)
    
    # Save the mask
    tree_spline.save_binary_mask(mask, "tree_mask_with_fillets.png")


if __name__ == "__main__":
    example()