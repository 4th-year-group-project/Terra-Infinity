"""Code to generate a river network using a flow algorithm."""

import heapq
from collections import defaultdict, deque

import numpy as np

from world_generation.generation import tools

from .tree_spline import TreeSpline


def get_chunk(x, y):
    return (int(np.floor(x / 1023)), int(np.floor(y / 1023)))


def get_neighboring_chunks(cx, cy):
    return [(cx + dx, cy + dy) for dx in [-1, 0, 1] for dy in [-1, 0, 1]]


def get_max_depth(neighbors, boundary_nodes, ocean_nodes):
    """Calculate the maximum possible depth of the river network using BFS.
    
    Args:
        neighbors (dict): Dictionary of neighboring nodes.
        boundary_nodes (list): List of boundary nodes.
        ocean_nodes (set): Set of ocean nodes.
        
    Returns:
        int: Maximum depth of the river network.
        
    """
    visited = set()
    max_depth = 0

    for node in boundary_nodes:
        if node not in visited:
            queue = deque([(node, 0)])
            visited.add(node)

            while queue:
                current, depth = queue.popleft()
                max_depth = max(max_depth, depth)

                for neighbor in neighbors[current]:
                    if neighbor not in visited and neighbor not in ocean_nodes:
                        visited.add(neighbor)
                        queue.append((neighbor, depth + 1))

    return max_depth


def get_weight(neighbor, centroids):
    """Calculate the weight based on the distance from the centroid (drain)."""
    return np.sqrt(centroids[neighbor][0] ** 2 + centroids[neighbor][1] ** 2)


def weighted_bfs_water_flow(
    neighbors,
    boundary_nodes,
    ocean_nodes,
    coastal_nodes,
    centroids,
    max_depth,
    super_duper_size,
    depth_penalty_factor=0.01, 
):
    """Perform a weighted BFS to determine water flow directions and depth across the world.
    
    Args:
        neighbors (dict): Dictionary of neighboring nodes.
        boundary_nodes (list): List of boundary nodes.
        ocean_nodes (set): Set of ocean nodes.
        coastal_nodes (set): Set of coastal nodes.
        centroids (dict): Dictionary of centroids for each node.
        max_depth (int): Maximum depth of the river network.
        super_duper_size (int): Size parameter for the BFS.
        depth_penalty_factor (float): Factor to penalize depth in the BFS.

    Returns:
        tuple: A tuple containing:
            - flow_directions (dict): Dictionary mapping each node to its flow direction.
            - depth (dict): Dictionary mapping each node to its depth.
    """
    visited = set()
    flow_directions = {}
    heap = []
    depth = {}

    
    for node in boundary_nodes:
        heapq.heappush(heap, (0.0, node, node)) 
        depth[node] = 0

    while heap:
        current_weight, _, current = heapq.heappop(heap)

        if current in visited:
            continue
        visited.add(current)

        for neighbor in sorted(neighbors[current]):  
            if (
                neighbor not in visited
                and neighbor not in ocean_nodes
                and not (current in coastal_nodes and neighbor in coastal_nodes)
            ):
                next_depth = depth[current] + 1

                weight_component = get_weight(neighbor, centroids) / (np.sqrt(2 * super_duper_size) * 1023)

                depth_penalty = -depth_penalty_factor * next_depth

                total_weight = current_weight + weight_component + depth_penalty

                depth[neighbor] = next_depth
                heapq.heappush(heap, (total_weight, neighbor, neighbor))
                flow_directions[neighbor] = current

    return flow_directions, depth


def reverse_tree(flow_directions):
    """Reverse the flow directions to create a tree structure.

    Args:
        flow_directions (dict): Dictionary mapping each node to its flow direction.

    Returns:
        dict: A dictionary representing the reversed tree structure.
    """

    regular_tree = {}

    for neighbor, current in flow_directions.items():
        if current not in regular_tree:
            regular_tree[current] = []
        regular_tree[current].append(neighbor)

    return regular_tree


def compute_strahler_number(tree, node, strahler_numbers):
    """Compute the Strahler number for a given node in the tree.
    
    Args:
        tree (dict): Dictionary representing the tree structure.
        node: The node for which to compute the Strahler number.
        strahler_numbers (dict): Dictionary to store computed Strahler numbers.
        
    Returns:
        int: The computed Strahler number for the node."""

    if node not in tree:
        strahler_numbers[node] = 1
        return 1

    children = tree[node]
    child_strahlers = [compute_strahler_number(tree, child, strahler_numbers) for child in children]
    max_strahler = max(child_strahlers)
    count_max = child_strahlers.count(max_strahler)

    if count_max > 1:
        strahler_numbers[node] = max_strahler + 1
    else:
        strahler_numbers[node] = max_strahler

    # small adjustment for the number of children for big rivers
    if len(children) > 3:
        strahler_numbers[node] += 1

    return strahler_numbers[node]


def identify_trees(flow_tree):
    """Identify trees in the flow tree and collect edges.

    Args:
        flow_tree (dict): Dictionary representing the flow tree structure.
    
    Returns:
        dict: A dictionary where keys are root nodes and values are lists of edges in the tree.

    """

    visited = set()
    trees_with_edges = {}

    def dfs(node, current_tree_edges, root_node):
        visited.add(node)
        if node in flow_tree:
            for neighbor in flow_tree[node]:
                if neighbor not in visited:
                    current_tree_edges.append((node, neighbor))  
                    dfs(neighbor, current_tree_edges, root_node)

    for node in flow_tree:
        if node not in visited:
            current_tree_edges = []
            dfs(node, current_tree_edges, node) 
            trees_with_edges[node] = current_tree_edges 

    return trees_with_edges


class RiverNetwork:
    """Class to represent and manage the river network.
    
    Attributes:
        world_map (WorldMap): The world map object containing the terrain and features.
        flow_directions (dict): Dictionary mapping each node to its flow direction.
        flow_tree (dict): Dictionary representing the tree structure of the river network.
        strahler_numbers (dict): Dictionary mapping each node to its Strahler number.
        sampled_trees (set): Set of sampled trees for river generation.
        chunk_index (defaultdict): Dictionary to index splines by chunks.
        tree_params (dict): Dictionary to store parameters for each tree.
        tree_splines (dict): Dictionary to store the generated tree splines.
    """
    def __init__(self, world_map):
        """Initialize the RiverNetwork with a world map."""
        self.world_map = world_map
        self.flow_directions = {}
        self.flow_tree = {}
        self.strahler_numbers = {}
        self.sampled_trees = {}
        self.chunk_index = defaultdict(set)
        self.tree_params = {}

    def build(self, parameters, super_duper_chunk_size):
        """Build the river network using a weighted BFS algorithm, and then define rivers with parameters.
        
        Args: 
            parameters (dict): Dictionary of parameters for river generation.
            super_duper_chunk_size (int): Size parameter for the BFS.
        
        """
        seed = parameters.get("seed", 0)
        max_depth = get_max_depth(self.world_map.neighbors, self.world_map.boundary_nodes, self.world_map.ocean)

        self.flow_directions, self.depth = weighted_bfs_water_flow(
            self.world_map.neighbors,
            self.world_map.boundary_nodes,
            self.world_map.ocean,
            self.world_map.coastal,
            self.world_map.centroids,
            max_depth,
            super_duper_chunk_size,
        )

        self.flow_tree = reverse_tree(self.flow_directions)

        for node in self.world_map.boundary_nodes:
            compute_strahler_number(self.flow_tree, node, self.strahler_numbers)
            self.strahler_numbers[node] += 1

        self.trees = identify_trees(self.flow_tree)

        self.trees = {root: edges for root, edges in self.trees.items() if len(edges) >= 3}

        freq_pct = parameters["river_frequency"]
        freq = tools.map0100(freq_pct, 0, 1)

        rng = np.random.default_rng(seed)
        self.sampled_trees = rng.choice(list(self.trees.keys()), size=int(freq * len(self.trees)), replace=False)

    def spline_trees(
        self, seed, default_curviness=0.5, default_meander=0.2, default_river_width=2, default_scale_exponent=2.1
    ):
        """Create splines for the sampled trees using the parameters defined.
        
        Args:
            seed (int): Random seed for reproducibility.
            default_curviness (float): Default curviness for the splines.
            default_meander (float): Default meander for the splines.
            default_river_width (float): Default river width for the splines.
            default_scale_exponent (float): Default scale exponent for the splines.
        """

        self.tree_splines = {}

        max_width = 0
        max_scale_exponent = 0

        for tree_id in self.sampled_trees:
            rng = np.random.default_rng(hash((tree_id, seed)) % (2**32 - 1))

            self.tree_params[tree_id] = {
                "curviness": np.clip(rng.normal(loc=default_curviness, scale=0.1), 0.3, 0.7),
                "meander": np.clip(rng.normal(loc=default_meander, scale=0.1), 0.0, 0.7),
                "river_width": np.clip(rng.normal(loc=default_river_width, scale=0.5), 0.5, 4.0),
                "scale_exponent": rng.uniform(1.9, 2.8),
            }

            max_width = max(max_width, self.tree_params[tree_id]["river_width"])
            max_scale_exponent = max(max_scale_exponent, self.tree_params[tree_id]["scale_exponent"])

            edges = self.trees[tree_id]
            ts = TreeSpline(edges, self.world_map.centroids)

            ts.smooth_tree(
                curviness=self.tree_params[tree_id]["curviness"], meander=self.tree_params[tree_id]["meander"]
            )
            self.tree_splines[tree_id] = ts

        self.max_river_width = 2 * (max(self.strahler_numbers.values()) * max_width) ** max_scale_exponent

    def index_splines_by_chunk(self):
        """Index the splines by chunk for efficient retrieval."""

        for tree_id, ts in self.tree_splines.items():
            spline_points_dict = ts.get_spline_points()

            for edge, points in spline_points_dict.items():
                min_x, min_y = points.min(axis=0)
                max_x, max_y = points.max(axis=0)

                chunk_min_x, chunk_min_y = get_chunk(min_x, min_y)
                chunk_max_x, chunk_max_y = get_chunk(max_x, max_y)

                for cx in range(chunk_min_x, chunk_max_x + 1):
                    for cy in range(chunk_min_y, chunk_max_y + 1):
                        self.chunk_index[(cx, cy)].add((tree_id, edge))

    def get_splines_near(self, cx, cy):
        """Get the splines near a given chunk."""
        chunks_to_check = get_neighboring_chunks(cx, cy)

        spline_refs = set()
        for chunk in chunks_to_check:
            spline_refs.update(self.chunk_index.get(chunk, []))

        return spline_refs 

    def plot_world(self, points, vor=None):
        """Plot the world map and river network."""

        import matplotlib.pyplot as plt
        from scipy.spatial import voronoi_plot_2d

        fig, ax = plt.subplots(figsize=(6, 6))
        if vor is not None:
            voronoi_plot_2d(vor, ax=ax, show_vertices=False, line_colors="gray", point_size=0)

        for polygon in range(len(self.world_map.polygons)):
            if polygon in self.world_map.ocean:
                color = "blue"
            elif polygon in self.world_map.coastal:
                color = "yellow"
            else:
                color = "green"
            plt.fill(*zip(*self.world_map.polygons[polygon], strict=False), color=color, alpha=0.5)

        for tree_spline in self.tree_splines.values():
            spline_points = tree_spline.get_spline_points()
            for (_parent, child), spoints in spline_points.items():
                plt.plot(spoints[:, 0], spoints[:, 1], color="blue", alpha=1, linewidth=self.strahler_numbers[child])

        plt.xlim(points[:, 0].min(), points[:, 0].max())
        plt.ylim(points[:, 1].min(), points[:, 1].max())

        plt.plot(points[:, 0], points[:, 1], "bo", markersize=1)

        x_min, x_max = ax.get_xlim()
        y_min, y_max = ax.get_ylim()

        xticks = np.arange(np.floor((x_min) / 1024) * 1024, x_max + 1024, 1024)
        yticks = np.arange(np.floor((y_min) / 1024) * 1024, y_max + 1024, 1024)

        ax.set_xticks(xticks)
        ax.set_yticks(yticks)

        ax.grid(True, which="major", color="black", linestyle="--", linewidth=1)

        ax.tick_params(left=False, bottom=False, labelleft=False, labelbottom=False)
        plt.show()
