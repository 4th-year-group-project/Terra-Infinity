from generation import Noise
import numpy as np
from collections import defaultdict, deque
import heapq
from .tree_spline import TreeSpline  

def get_chunk(x, y):
    return (
        int(np.floor(x / 1024)),
        int(np.floor(y / 1024))
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
                    alpha * get_weight(neighbor, centroids) / (np.sqrt(2 * super_duper_size) * 1024)
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

    def build(self, seed, super_duper_chunk_size):
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

        self.trees = identify_trees(self.flow_tree)

        rng = np.random.default_rng(seed)
        self.sampled_trees = rng.choice(
            list(self.trees.keys()), 
            size=int(1 * len(self.trees)), 
            replace=False
        )

    def spline_trees(self, default_curviness=0.5, default_meander=0.3, default_river_width=3, default_scale_exponent=2.1):
        self.tree_splines = {}

        max_width = 0
        max_scale_exponent = 0

        for tree_id in self.sampled_trees:
            # Assign unique (or default) values per tree
            self.tree_params[tree_id] = {
                "curviness": default_curviness,       
                "meander": default_meander,         
                "river_width": default_river_width,     
                "scale_exponent": default_scale_exponent  
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

        self.max_river_width = 2*(max(self.strahler_numbers.values()) * max_width)**max_scale_exponent

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

    def get_splines_near(self, x, y):
        cx, cy = get_chunk(x, y)
        chunks_to_check = get_neighboring_chunks(cx, cy)

        spline_refs = set()
        for chunk in chunks_to_check:
            spline_refs.update(self.chunk_index.get(chunk, []))

        return spline_refs  # or fetch actual bezier points if needed