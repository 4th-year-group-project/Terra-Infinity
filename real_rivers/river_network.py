from generation import Noise, tools
import numpy as np
from collections import defaultdict, deque
import heapq
from .tree_spline import TreeSpline  

def get_chunk(x, y):
    return (
        int(np.floor(x / 1023)),
        int(np.floor(y / 1023))
    )

def get_neighboring_chunks(cx, cy):
    return [
        (cx + dx, cy + dy)
        for dx in [-1, 0, 1]
        for dy in [-1, 0, 1]
    ]


def get_max_depth(neighbors, boundary_nodes, ocean_nodes):
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
    return np.sqrt(centroids[neighbor][0]**2 + centroids[neighbor][1]**2) 

def weighted_bfs_water_flow(
    neighbors, boundary_nodes, ocean_nodes, coastal_nodes,
    centroids, max_depth, super_duper_size
):
    visited = set()
    flow_directions = {}
    heap = []
    depth = {}

    # Initialize heap with boundary nodes and tie-breaker as node ID
    for node in boundary_nodes:
        heapq.heappush(heap, (0.0, node, node))  # (weight, tie-breaker, node)
        depth[node] = 0

    while heap:
        current_weight, _, current = heapq.heappop(heap)

        if current in visited:
            continue
        visited.add(current)

        for neighbor in sorted(neighbors[current]):  # ensure consistent order
            if (
                neighbor not in visited and
                neighbor not in ocean_nodes and
                not (current in coastal_nodes and neighbor in coastal_nodes)
            ):
                next_depth = depth[current] + 1
                alpha = 1

                weight_component = (
                    alpha * get_weight(neighbor, centroids) / (np.sqrt(2 * super_duper_size) * 1023)
                    - (1 - alpha) * next_depth / max_depth
                )
                total_weight = current_weight + weight_component

                # Only update if neighbor not already queued or has better depth
                #if neighbor not in depth or next_depth < depth[neighbor]:
                depth[neighbor] = next_depth
                heapq.heappush(heap, (total_weight, neighbor, neighbor))
                flow_directions[neighbor] = current

    return flow_directions, depth

def reverse_tree(flow_directions):
    regular_tree = {}
    
    for neighbor, current in flow_directions.items():
        if current not in regular_tree:
            regular_tree[current] = []
        regular_tree[current].append(neighbor)
    
    return regular_tree

def compute_strahler_number(tree, node, strahler_numbers):
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
        
    if len(children) > 3:
        strahler_numbers[node] += 1

    return strahler_numbers[node]

def identify_trees(flow_tree):
    visited = set()
    trees_with_edges = {}

    def dfs(node, current_tree_edges, root_node):
        visited.add(node)
        if node in flow_tree:
            for neighbor in flow_tree[node]:
                if neighbor not in visited:
                    current_tree_edges.append((node, neighbor))  # Add edge to the list
                    dfs(neighbor, current_tree_edges, root_node)

    # Identify each tree and collect edges
    for node in flow_tree:
        if node not in visited:
            current_tree_edges = []
            dfs(node, current_tree_edges, node)  # Pass the root node as the starting point
            trees_with_edges[node] = current_tree_edges  # Store edges for this root node
    
    return trees_with_edges



class RiverNetwork:
    def __init__(self, world_map):
        self.world_map = world_map
        self.flow_directions = {}
        self.flow_tree = {}
        self.strahler_numbers = {}
        self.sampled_trees = {}
        self.chunk_index = defaultdict(set)
        self.tree_params = {}

    def build(self, parameters, super_duper_chunk_size):
        seed = parameters["seed"]
        max_depth = get_max_depth(
            self.world_map.neighbors,
            self.world_map.boundary_nodes,
            self.world_map.ocean
        )

        self.flow_directions, self.depth = weighted_bfs_water_flow(
            self.world_map.neighbors,
            self.world_map.boundary_nodes,
            self.world_map.ocean,
            self.world_map.coastal,
            self.world_map.centroids,
            max_depth,
            super_duper_chunk_size
        )

        self.flow_tree = reverse_tree(self.flow_directions)

        for node in self.world_map.boundary_nodes:
            compute_strahler_number(self.flow_tree, node, self.strahler_numbers)
            self.strahler_numbers[node] += 1

        self.trees = identify_trees(self.flow_tree)

        freq_pct = parameters["river_frequency"]
        freq = tools.map0100(freq_pct, 0, 1)

        rng = np.random.default_rng(seed)
        self.sampled_trees = rng.choice(
            list(self.trees.keys()), 
            size=int(freq * len(self.trees)), 
            replace=False
        )

    def spline_trees(self, seed, default_curviness=0.5, default_meander=0.2, default_river_width=2, default_scale_exponent=2.1):
        import numpy as np  # ensure numpy is imported

        self.tree_splines = {}

        max_width = 0
        max_scale_exponent = 0

        for tree_id in self.sampled_trees:
            # Create a unique RNG for this tree using the seed and tree_id
            rng = np.random.default_rng(hash((tree_id, seed)) % (2**32 - 1))

            self.tree_params[tree_id] = {
                "curviness": np.clip(rng.normal(loc=default_curviness, scale=0.1), 0.3, 0.7),      
                "meander": np.clip(rng.normal(loc=default_meander, scale=0.1), 0.0, 0.7),        
                "river_width": np.clip(rng.normal(loc=default_river_width, scale=0.5), 0.5, 4.0),
                "scale_exponent": rng.uniform(1.9, 2.8)
            }

            max_width = max(max_width, self.tree_params[tree_id]["river_width"])
            max_scale_exponent = max(max_scale_exponent, self.tree_params[tree_id]["scale_exponent"])

            edges = self.trees[tree_id]
            ts = TreeSpline(edges, self.world_map.centroids)

            # Apply curviness and meander from the params
            ts.smooth_tree(
                curviness=self.tree_params[tree_id]["curviness"],
                meander=self.tree_params[tree_id]["meander"]
            )
            self.tree_splines[tree_id] = ts

        self.max_river_width = 2 * (max(self.strahler_numbers.values()) * max_width) ** max_scale_exponent

    def index_splines_by_chunk(self):
        for tree_id, ts in self.tree_splines.items():
            spline_points_dict = ts.get_spline_points()

            for edge, points in spline_points_dict.items():
                # Compute bounding box of the spline
                min_x, min_y = points.min(axis=0)
                max_x, max_y = points.max(axis=0)

                # Convert bounding box to chunk range
                chunk_min_x, chunk_min_y = get_chunk(min_x, min_y)
                chunk_max_x, chunk_max_y = get_chunk(max_x, max_y)

                for cx in range(chunk_min_x, chunk_max_x + 1):
                    for cy in range(chunk_min_y, chunk_max_y + 1):
                        self.chunk_index[(cx, cy)].add((tree_id, edge))

    def get_splines_near(self, cx, cy):
        #cx, cy = get_chunk(x, y)
        chunks_to_check = get_neighboring_chunks(cx, cy)

        spline_refs = set()
        for chunk in chunks_to_check:
            spline_refs.update(self.chunk_index.get(chunk, []))

        return spline_refs  # or fetch actual bezier points if needed
    
    def plot_world(self, points, vor=None):
        import matplotlib.pyplot as plt
        from scipy.spatial import voronoi_plot_2d

        fig, ax = plt.subplots(figsize=(6, 6))
        if vor is not None:
            voronoi_plot_2d(vor, ax=ax, show_vertices=False, line_colors='gray', point_size=0)

        for polygon in range(len(self.world_map.polygons)):
            if polygon in self.world_map.ocean:
                color = 'blue'
            elif polygon in self.world_map.coastal:
                color = 'yellow'
            else:  
                color = 'green'
            plt.fill(*zip(*self.world_map.polygons[polygon]), color=color, alpha=0.5)  

        for tree_spline in self.tree_splines.values():
            spline_points = tree_spline.get_spline_points()
            for (parent, child), spoints in spline_points.items():
                plt.plot(spoints[:, 0], spoints[:, 1], color='blue', alpha=1, linewidth=self.strahler_numbers[child])

        plt.xlim(points[:, 0].min(), points[:, 0].max())
        plt.ylim(points[:, 1].min(), points[:, 1].max())

        plt.plot(points[:, 0], points[:, 1], 'bo', markersize=1)

        x_min, x_max = ax.get_xlim()
        y_min, y_max = ax.get_ylim()

        xticks = np.arange(np.floor((x_min) / 1024) * 1024, x_max + 1024, 1024)
        yticks = np.arange(np.floor((y_min) / 1024) * 1024, y_max + 1024, 1024)

        ax.set_xticks(xticks)
        ax.set_yticks(yticks)

        ax.grid(True, which='major', color='black', linestyle='--', linewidth=1)

        ax.tick_params(left=False, bottom=False, labelleft=False, labelbottom=False)
        plt.show()