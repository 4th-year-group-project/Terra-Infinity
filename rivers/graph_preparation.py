from collections import deque
import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial import Voronoi, voronoi_plot_2d
from scipy.stats import qmc
from generation import point_open_simplex_fractal_noise
import random
import time
import heapq
from numba import njit, prange
from generation import Noise

def generate_points(seed, n, chunk_size, distance_from_edge=200, radius=100):
    rng = np.random.default_rng(seed)
    points = []

    max_attempts = 1000

    for _ in range(max_attempts):
        cx = rng.integers(distance_from_edge, chunk_size-distance_from_edge)
        cy = rng.integers(distance_from_edge, chunk_size-distance_from_edge)

        if all(np.linalg.norm(np.array([cx, cy]) - np.array(p)) >= radius for p in points):
            points.append(np.array([cx, cy]))
            if len(points) >= n:
                break

    shift = np.array([chunk_size // 2, chunk_size // 2])
    points = np.array(points) - shift 

    return points 

def construct_points2(chunk_coords, chunk_size, seed, radius=50, skew_factor=0):
    points = []

    densities = [1, 2, 3, 4]
    weights = [x**skew_factor for x in range(1, len(densities) + 1)]
    weights /= np.sum(weights)

    for i in range(-radius//2, radius//2 + 1):
        for j in range(-radius//2, radius//2 + 1):
            x = chunk_coords[0] + (i * chunk_size)
            y = chunk_coords[1] + (j * chunk_size)
            chunk_seed = (seed + (x << 32) + (y << 64)) & ((1 << 128) - 1) 

            num_points = np.random.choice(densities, p=weights)
            chunk_points = generate_points(chunk_seed, num_points, chunk_size, 100, 1024/num_points)
            chunk_points[:, 0] += x
            chunk_points[:, 1] += y

            points.extend(chunk_points)

    return points

width = 1024  
height = 1024    
seed = 0

points = construct_points2([0,0], 1024, seed, 50, 0)
points = np.array(points)

min_x, max_x = points[:, 0].min(), points[:, 0].max()
min_y, max_y = points[:, 1].min(), points[:, 1].max()

vor = Voronoi(points)   

polygons = []
centroids = []
index_map = {}
ocean = set()
noise = Noise(seed, width, height, 0, 0)

start = time.time()
for i, region_index in enumerate(vor.point_region): 
    region = vor.regions[region_index]
    if not region or -1 in region:  
        continue
    polygon = [vor.vertices[j] for j in region]
    center = np.mean(polygon, axis=0)

    if not ((min_x < center[0] < max_x) and (min_y < center[1] < max_y)):
        continue

    polygons.append(polygon)
    centroids.append(center)

    index_map[i] = len(polygons) - 1

heights = noise.batch_simplex_noise(np.array(centroids), 0, 0, 200000, 1, 1, 0.5, 2)

for i in range(len(polygons)):
    if heights[i] < -0.1:  
        ocean.add(i)

neighbors = {i: set() for i in range(len(polygons))}

for p1, p2 in vor.ridge_points:
    if p1 in index_map and p2 in index_map:
        i1 = index_map[p1]
        i2 = index_map[p2]
        neighbors[i1].add(i2)
        neighbors[i2].add(i1)

boundary_nodes = set()
coastal = set()

for node in ocean:
    for neighbor in neighbors[node]:
        if neighbor not in ocean:
            boundary_nodes.add(node)
            coastal.add(neighbor)
print(time.time()-start)    

def get_max_depth(neighbors, boundary_nodes, ocean_nodes, coastal_nodes):
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


def get_weight(current, neighbor, heights):
    return (np.sqrt(centroids[neighbor][0]**2 + centroids[neighbor][1]**2))/36203

def weighted_bfs_water_flow(neighbors, boundary_nodes, ocean_nodes, coastal_nodes, heights):
    visited = set()
    flow_directions = {}
    heap = []
    depth = {}

    for node in boundary_nodes:
        heapq.heappush(heap, (0, node))
        depth[node] = 0

    while heap:
        current_weight, current = heapq.heappop(heap)

        if current in visited:
            continue
        visited.add(current)

        for neighbor in neighbors[current]:
            if neighbor not in visited and neighbor not in ocean_nodes and not (current in coastal_nodes and neighbor in coastal_nodes):
                depth[neighbor] = depth[current] + 1

                alpha=0.99
                weight = alpha*get_weight(current, neighbor, heights) - (1-alpha)*depth[neighbor]/(51)
                heapq.heappush(heap, (current_weight + weight, neighbor))
                if neighbor not in flow_directions:
                    flow_directions[neighbor] = current

    return flow_directions

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
        return max_strahler + 1
    else:
        strahler_numbers[node] = max_strahler
        return max_strahler

start = time.time()
max_depth = get_max_depth(neighbors, boundary_nodes, ocean, coastal)
#print("Max depth:", max_depth)
flow = weighted_bfs_water_flow(neighbors, boundary_nodes, ocean, coastal, heights)
flow_tree = reverse_tree(flow)
strahler_numbers = {}
for node in boundary_nodes:
    compute_strahler_number(flow_tree, node, strahler_numbers)
print(time.time()-start)

fig, ax = plt.subplots(figsize=(6, 6))
voronoi_plot_2d(vor, ax=ax, show_vertices=False, line_colors='gray', point_size=0)

for polygon in range(len(polygons)):
    if polygon in ocean:
        color = 'blue'
    elif polygon in coastal:
        color = 'yellow'
    else:  
        color = 'green'
    plt.fill(*zip(*polygons[polygon]), color=color, alpha=0.5)

plt.plot(points[:, 0], points[:, 1], 'ro', markersize=0.1)

plt.xlim(points[:, 0].min(), points[:, 0].max())
plt.ylim(points[:, 1].min(), points[:, 1].max())

for node1, node2 in flow.items():
    center1 = centroids[node1]
    center2 = centroids[node2]
    plt.plot([center1[0], center2[0]], [center1[1], center2[1]], color='blue', alpha=1, linewidth=strahler_numbers[node1]/1.5)

# for node in boundary_nodes:
#     center = centroids[node]
#     plt.plot(center[0], center[1], 'ro', markersize=5)

x_min, x_max = ax.get_xlim()
y_min, y_max = ax.get_ylim()

xticks = np.arange(np.floor((x_min - 512) / 1024) * 1024 + 512, x_max + 1024, 1024)
yticks = np.arange(np.floor((y_min - 512) / 1024) * 1024 + 512, y_max + 1024, 1024)

ax.set_xticks(xticks)
ax.set_yticks(yticks)

ax.grid(True, which='major', color='black', linestyle='--', linewidth=1)

ax.tick_params(left=False, bottom=False, labelleft=False, labelbottom=False)
plt.show()