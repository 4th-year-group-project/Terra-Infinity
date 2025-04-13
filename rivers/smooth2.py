import numpy as np
import matplotlib.pyplot as plt
import networkx as nx

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
        
        # Parameters for river-like curves
        self.curviness = 0.5  # Higher values = curvier (was 1/3 ~= 0.33)
        self.meander = 0.25   # Perpendicular displacement factor
        
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
        Calculate control points for each edge to ensure G1 continuity 
        with river-like curviness.
        """
        for parent, child in self.edges:
            p0 = self.centroids[parent]
            p3 = self.centroids[child]
            
            # Get tangent directions at parent and child nodes
            t_parent = self.compute_tangent_at_node(parent)
            t_child = self.compute_tangent_at_node(child)
            
            # Calculate distance between nodes
            dist = np.linalg.norm(p3 - p0)
            
            # Create perpendicular vectors for meandering effect
            perp_parent = np.array([-t_parent[1], t_parent[0]])  # Rotate 90 degrees
            perp_child = np.array([-t_child[1], t_child[0]])     # Rotate 90 degrees
            
            # Control points with increased distance factor and perpendicular displacement
            # The sign of meander effect is opposite for parent and child to create an S-curve
            p1 = p0 + t_parent * (dist * self.curviness) + perp_parent * (dist * self.meander)
            p2 = p3 - t_child * (dist * self.curviness) - perp_child * (dist * self.meander)
            
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
    
    def smooth_tree(self, curviness=None, meander=None):
        """
        Calculate control points and prepare splines for the entire tree.
        
        Parameters:
        - curviness: Control point distance factor (higher = curvier)
        - meander: Perpendicular displacement factor for river-like curves
        """
        if curviness is not None:
            self.curviness = curviness
        if meander is not None:
            self.meander = meander
            
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
        
        plt.title(f'Tree with River-like Curves (Curviness: {self.curviness}, Meander: {self.meander})')
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
        
    def offset_curve(self, points, widths):
        """
        Generate left and right offset curves based on width at each point.
        """
        left, right = [], []
        for i in range(len(points)):
            if i == 0:
                tangent = points[i+1] - points[i]
            elif i == len(points)-1:
                tangent = points[i] - points[i-1]
            else:
                tangent = points[i+1] - points[i-1]
            
            tangent = tangent / np.linalg.norm(tangent)
            normal = np.array([-tangent[1], tangent[0]])
            w = widths[i]
            left.append(points[i] + normal * w)
            right.append(points[i] - normal * w)
        return np.array(left), np.array(right)


# Example usage
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
    
    # Smooth the tree with river-like curves
    # Try different values to adjust curviness
    tree_spline.smooth_tree(curviness=0.5, meander=0.25)
    
    # Plot with original lines and control points to see the effect
    tree_spline.plot_tree(plot_original=True, plot_control_points=True)
    
    # Plot without original lines and control points for a cleaner view
    tree_spline.plot_tree(plot_original=False, plot_control_points=False)
    
    # Get the spline points for further processing if needed
    spline_points = tree_spline.get_spline_points()

    edge = (1, 2)
    points = tree_spline.get_bezier_points(tree_spline.control_points[edge], 50)

    # Define a width profile (e.g., tapering or constant)
    widths = np.linspace(1.0, 0.2, len(points))  # taper from 1.0 to 0.2

    # Get left/right outlines
    left, right = tree_spline.offset_curve(points, widths)

    # Plot it
    plt.plot(points[:, 0], points[:, 1], 'g--', label='centerline')
    plt.plot(left[:, 0], left[:, 1], 'r-', label='left')
    plt.plot(right[:, 0], right[:, 1], 'b-', label='right')
    plt.fill(np.concatenate([left[:, 0], right[::-1, 0]]),
            np.concatenate([left[:, 1], right[::-1, 1]]),
            color='lightgreen', alpha=0.5, label='ribbon')
    plt.legend()
    plt.axis('equal')
    plt.show()

if __name__ == "__main__":
    example()