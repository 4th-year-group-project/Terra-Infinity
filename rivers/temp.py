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




    # for node1, spline_list in splines_by_node.items():
    #     if len(spline_list) > 1:
    #         spline1 = spline_list[0]
    #         spline2 = spline_list[1]
            
    #         rasterized1 = list(dict.fromkeys((int(x), int(y)) for x, y in spline1[1]))
    #         rasterized2 = list(dict.fromkeys((int(x), int(y)) for x, y in spline2[1]))
            
    #         for i in range(min(len(rasterized1), len(rasterized2))):
    #             dist_between = (rasterized1[i][0] - rasterized2[i][0])**2 + (rasterized1[i][1] - rasterized2[i][1])**2
    #             dist_between = np.sqrt(dist_between)
    #             if dist_between > 50:
    #                 interest_points.append(rasterized1[i])
    #                 interest_points.append(rasterized2[i])

    #                 p0 = rasterized1[i]
    #                 p2 = rasterized2[i]
    #                 mid_x = (p0[0] + p2[0]) // 2
    #                 mid_y = (p0[1] + p2[1]) // 2
    #                 # Shift outward from centerline (you could use vector normal, or something quick like this)
    #                 p1 = (mid_x, mid_y - 40)  # tweak this offset!

    #                 bridge_curve = bezier_curve(p0, p1, p2, num_points=15)

    #                 poly_points = rasterized1[:i+1] + bridge_curve + list(reversed(rasterized2[:i+1]))

    #                 fill_polygon_on_grid_fast(centerline_grid, poly_points)

    #                 max_width = max(width_grid[rasterized1[i][1], rasterized1[i][0]], width_grid[rasterized2[i][1], rasterized2[i][0]])

    #                 fill_polygon_on_grid_fast(width_grid, poly_points, value=max_width)

    #                 break

                # avg_point = np.array([(rasterized1[i][0] + rasterized2[i][0]) / 2, (rasterized1[i][1] + rasterized2[i][1]) / 2])
                # dist_from_1 = np.linalg.norm(rasterized1[i] - avg_point)
                # dist_from_2 = np.linalg.norm(rasterized2[i] - avg_point)

                # if dist_from_1 + dist_from_2 > 20:
                #     interest_points.append(avg_point)

    # # Connect splines that share the same node1
    # for node1, spline_list in splines_by_node.items():
    #     if len(spline_list) > 1:  # Only if there are multiple splines from this node
    #         # For each pair of splines from the same node1
    #         for i in range(len(spline_list)):
    #             for j in range(i+1, len(spline_list)):
    #                 edge1, points1 = spline_list[i]
    #                 edge2, points2 = spline_list[j]

    #                 p1 = get_point_at_percentage(points1, 0.5)  # 50% along the first spline
    #                 p2 = get_point_at_percentage(points2, 0.5)


    #                 avg_width = (width_grid[int(p1[1]), int(p1[0])] + width_grid[int(p2[1]), int(p2[0])]) / 2

    #                 p1, p2 = get_divergence_point(points1, points2, epsilon=avg_width*10)
    #                 interest_points.append(p2)
    #                 interest_points.append(p1)

    #                 avg_x = (p1[0] + p2[0]) / 2
    #                 avg_y = (p1[1] + p2[1]) / 2

    #                 # Find the vector from p1 to p2
    #                 dx = p2[0] - p1[0]
    #                 dy = p2[1] - p1[1]

    #                 # Get a perpendicular vector to the line connecting p1 and p2, scaled by a desired distance
    #                 perp_x, perp_y = get_perpendicular_vector(dx, dy, distance=avg_width)  # Adjust distance as needed

    #                 # Calculate the control point for the curve by pushing the average point in the perpendicular direction
    #                 control_x = avg_x + perp_x
    #                 control_y = avg_y + perp_y

    #                 # Generate the Bezier curve between p1, control_point, and p2
    #                 curve_points = quadratic_bezier_curve(p1, (control_x, control_y), p2, num_points=100)

    #                 # Draw the curve on the grid
    #                 for point in curve_points:
    #                     x, y = int(round(point[0])), int(round(point[1]))
    #                     if 0 <= x < width and 0 <= y < height:
    #                         centerline_grid[y, x] = 1
    #                         # For width, use average of the two points' widths
    #                         width_grid[y, x] = avg_width

# def get_divergence_point(spline1, spline2, epsilon=1):
#     for p in np.linspace(0, 1, 100):
#         p1 = get_point_at_percentage(spline1, p)
#         p2 = get_point_at_percentage(spline2, p)
#         if np.linalg.norm(p1 - p2) > epsilon:
#             return p1, p2

# def get_perpendicular_vector(dx, dy, distance):
#     """Get a unit vector perpendicular to (dx, dy) and scale it by `distance`."""
#     length = np.sqrt(dx**2 + dy**2)  # Calculate the length of the vector
#     if length == 0:
#         return 0, 0  # Avoid division by zero if the points are the same
#     # Perpendicular vector
#     perp_x = -dy / length
#     perp_y = dx / length
#     # Scale the perpendicular vector by the desired distance
#     return perp_x * distance, perp_y * distance

# def get_cumulative_length(points):
#     lengths = [0]
#     for i in range(1, len(points)):
#         dx = points[i][0] - points[i-1][0]
#         dy = points[i][1] - points[i-1][1]
#         segment_length = np.sqrt(dx**2 + dy**2)
#         lengths.append(lengths[-1] + segment_length)
#     return lengths

# def get_point_at_length(points, target_length):
#     cumulative_lengths = get_cumulative_length(points)
#     total_length = cumulative_lengths[-1]
#     # Find the segment where the target length lies
#     for i in range(1, len(cumulative_lengths)):
#         if cumulative_lengths[i] >= target_length:
#             p1 = points[i-1]
#             p2 = points[i]
#             segment_length = cumulative_lengths[i] - cumulative_lengths[i-1]
#             interpolation_factor = (target_length - cumulative_lengths[i-1]) / segment_length
#             x = p1[0] + interpolation_factor * (p2[0] - p1[0])
#             y = p1[1] + interpolation_factor * (p2[1] - p2[1])
#             return (x, y)
#     return points[-1]  # Return last point if not found

# def get_point_at_percentage(points, percentage):
#     cumulative_lengths = get_cumulative_length(points)
#     total_length = cumulative_lengths[-1]  # Total length is the last value in cumulative lengths
#     target_length = total_length * percentage  # The target length for the percentage

#     # Find the segment where the target length lies
#     for i in range(1, len(cumulative_lengths)):
#         if cumulative_lengths[i] >= target_length:
#             # Interpolate between points[i-1] and points[i] to find the exact position
#             p1 = points[i-1]
#             p2 = points[i]
#             segment_length = cumulative_lengths[i] - cumulative_lengths[i-1]
#             interpolation_factor = (target_length - cumulative_lengths[i-1]) / segment_length
            
#             # Calculate the point at the target length
#             x = p1[0] + interpolation_factor * (p2[0] - p1[0])
#             y = p1[1] + interpolation_factor * (p2[1] - p1[1])
#             return np.array([x, y])

#     return points[-1]

# def quadratic_bezier_curve(p0, p1, p2, num_points=100):
#     """Generates points along a quadratic Bezier curve."""
#     curve_points = []
#     for t in np.linspace(0, 1, num_points):
#         x = (1 - t)**2 * p0[0] + 2 * (1 - t) * t * p1[0] + t**2 * p2[0]
#         y = (1 - t)**2 * p0[1] + 2 * (1 - t) * t * p1[1] + t**2 * p2[1]
#         curve_points.append((x, y))
#     return curve_points
    
# def bezier_curve(p0, p1, p2, num_points=20):
#     t = np.linspace(0, 1, num_points)
#     curve = [(int((1-t)**2*p0[0] + 2*(1-t)*t*p1[0] + t**2*p2[0]),
#               int((1-t)**2*p0[1] + 2*(1-t)*t*p1[1] + t**2*p2[1])) for t in t]
#     return list(dict.fromkeys(curve))

# def fill_polygon_on_grid_fast(grid, polygon_points, value=1):
#     poly_y, poly_x = zip(*polygon_points)
#     rr, cc = polygon(poly_y, poly_x, grid.shape)
#     grid[cc,rr] = value