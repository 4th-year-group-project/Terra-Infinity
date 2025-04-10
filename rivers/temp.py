# k = min(10, len(boundary_nodes))  
# sampled_nodes = random.sample(list(boundary_nodes), k)

# def bfs_limited(graph, start, ocean_nodes, caught, max_depth):
#     visited = set()
#     queue = deque([(start, 0)])  # (node, depth)

#     while queue:
#         node, depth = queue.popleft()

#         # Stop if max depth reached
#         if depth > max_depth:
#             break

#         for neighbor in graph.get(node, []):
#             if neighbor not in ocean_nodes and neighbor not in visited and neighbor not in caught:  # Only explore land
#                 visited.add(neighbor)
#                 caught.add(neighbor)  
#                 queue.append((neighbor, depth + 1))

#     return visited

# catchment_areas = {}
# caught = set()
# for node in sampled_nodes:
#     visited = bfs_limited(neighbors, node, ocean, caught, np.random.randint(2, 5))
#     catchment_areas[node] = visited
  
# colors = {node: (random.random(), random.random(), random.random()) for node in catchment_areas.keys()}

# t = 4
# catchment = catchment_areas[sampled_nodes[t]]
# catchment_polys = [polygons[node] for node in catchment if node in polygons]
# x_values = [point[0] for poly in catchment_polys for point in poly]
# y_values = [point[1] for poly in catchment_polys for point in poly]

# bounding_box = [min(x_values), min(y_values), max(x_values), max(y_values)]
# print(bounding_box)

# from shapely.geometry import Polygon, Point
# from shapely.ops import unary_union

# start = time.time()
# catchment_polygons = [Polygon(polygons[node]) for node in catchment if node in polygons] + [Polygon(polygons[sampled_nodes[t]])]
# catchment_union = unary_union(catchment_polygons)

# rows, cols = 30, 30
# dx = (bounding_box[2] - bounding_box[0]) / (cols - 1)
# dy = (bounding_box[3] - bounding_box[1]) / (rows - 1)

# catchment_points = []

# for i in range(rows):
#     for j in range(cols):
#         x = bounding_box[0] + j * dx
#         y = bounding_box[1] + i * dy
        
#         if i % 2 == 1:
#             x += 0.5 * dx  # Offset every other row
        
#         point = Point(x, y)
#         if catchment_union.contains(point):
#             catchment_points.append([x, y])

# catchment_points = np.array(catchment_points)
# jitter_strength = min(dx, dy) / 2

# jitter = np.random.uniform(-jitter_strength, jitter_strength, catchment_points.shape)
# catchment_points += jitter


# from scipy.spatial import Delaunay
# tri = Delaunay(catchment_points)

# edge_count = {}
# interior_simplices = tri.simplices
# triangle_centroids = np.array([
#     np.mean(catchment_points[simplex], axis=0) for simplex in interior_simplices
# ])

# triangle_neighbors = {i: set() for i in range(len(interior_simplices))}
# edges_to_triangles = {}

# for i, simplex in enumerate(interior_simplices):
#     edges = [
#         (simplex[0], simplex[1]),
#         (simplex[1], simplex[2]),
#         (simplex[2], simplex[0]),
#     ]
    
#     for edge in edges:
#         edge = tuple(sorted(edge))  
#         if edge in edges_to_triangles:
#             neighbor = edges_to_triangles[edge]  
#             triangle_neighbors[i].add(neighbor)  
#             triangle_neighbors[neighbor].add(i)
#         else:
#             edges_to_triangles[edge] = i  

# # exterior_triangles = [x for x in triangle_neighbors if len(triangle_neighbors[x]) < 3]
# # exterior_centroids = triangle_centroids[exterior_triangles]

# origin_polygon = Polygon(polygons[sampled_nodes[t]])
# exterior_triangles = [x for x in triangle_neighbors if origin_polygon.contains(Point(triangle_centroids[x]))]
# exterior_centroids = triangle_centroids[exterior_triangles]

# def bfs_water_flow(triangle_neighbors, exterior_triangles, exterior_colors):
#     visited = set()
#     flow_directions = {}
#     triangle_colors = {}  
    
#     queue = deque(exterior_triangles)
    
#     for tri, color in zip(exterior_triangles, exterior_colors):
#         triangle_colors[tri] = color
    
#     while queue:
#         current = queue.popleft()
        
#         if current not in visited:
#             visited.add(current)

#             current_color = triangle_colors[current]
            
#             for neighbor in triangle_neighbors[current]:
#                 if neighbor not in visited:
#                     flow_directions[neighbor] = current
#                     triangle_colors[neighbor] = current_color  
#                     queue.append(neighbor)

#     return flow_directions, triangle_colors

# cmap = plt.get_cmap("tab10", len(exterior_triangles))  
# exterior_colors = [cmap(i) for i in range(len(exterior_triangles))] 
# flow_directions, triangle_colors = bfs_water_flow(triangle_neighbors, exterior_triangles, exterior_colors)

# def reverse_tree(flow_directions):
#     regular_tree = {}
    
#     for neighbor, current in flow_directions.items():
#         if current not in regular_tree:
#             regular_tree[current] = []
#         regular_tree[current].append(neighbor)
    
#     return regular_tree

# flow_tree = reverse_tree(flow_directions)

# strahler_numbers = {}
# def compute_strahler_number(tree, node):
#     if node not in tree:
#         strahler_numbers[node] = 1
#         return 1 
    
#     children = tree[node]
#     child_strahlers = [compute_strahler_number(tree, child) for child in children]
#     max_strahler = max(child_strahlers)
#     count_max = child_strahlers.count(max_strahler)

#     if count_max > 1:
#         strahler_numbers[node] = max_strahler + 1
#         return max_strahler + 1
#     else:
#         strahler_numbers[node] = max_strahler
#         return max_strahler

# for tri_idx in exterior_triangles:
#     compute_strahler_number(flow_tree, tri_idx)

# print(time.time() - start)  

# for origin, catchment in catchment_areas.items():
#     for node in catchment:
#         if node in polygons:
#             poly = np.array(polygons[node])
#             plt.fill(*zip(*poly), color=colors[origin], alpha=0.9)

# for boundary in sampled_nodes:
#     if boundary in polygons:
#         center = np.mean(polygons[boundary], axis=0)
#         plt.plot(center[0], center[1], 'ro', markersize=5)


# rect_x = [bounding_box[0], bounding_box[2], bounding_box[2], bounding_box[0], bounding_box[0]]
# rect_y = [bounding_box[1], bounding_box[1], bounding_box[3], bounding_box[3], bounding_box[1]]
# plt.plot(rect_x, rect_y, 'r--', linewidth=2, label="Bounding Box")
# plt.legend()

#ax.plot(points[:, 0], points[:, 1], 'ro', markersize=2)  
# plt.plot(catchment_points[:, 0], catchment_points[:, 1], 'ro', markersize=1)


# for tri_idx, children in flow_tree.items():
#     centroid = triangle_centroids[tri_idx]
#     color = triangle_colors[tri_idx]  
    
#     for child in children:
#         child_centroid = triangle_centroids[child]
        
#         plt.plot(
#             [centroid[0], child_centroid[0]], 
#             [centroid[1], child_centroid[1]], 
#             "-", color="blue", alpha=1,
#             #linewidth = strahler_numbers[child]/2
#             linewidth = strahler_numbers[child]/2
#         )