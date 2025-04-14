from utils.point_generation import construct_points2
from .voronoi_map import build_world_map
from .river_network import RiverNetwork
from .carving import mask_splines, carve_smooth_river_into_terrain, remove_padding
from .tree_spline import TreeSpline
from generation import Noise, Display
from scipy.spatial import Voronoi
import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial import voronoi_plot_2d
import time


seed = 0
super_duper_chunk_size = 7

start = time.time()
points = construct_points2([0, 1023], 1023, seed, super_duper_chunk_size, 0)
points = np.array(points)
min_x, max_x = points[:, 0].min(), points[:, 0].max()
min_y, max_y = points[:, 1].min(), points[:, 1].max()
print("Point generation:", time.time() - start)

start = time.time()
vor = Voronoi(points)
world_map = build_world_map(seed, vor, min_x, max_x, min_y, max_y)
print("World generation:", time.time() - start)

start = time.time()
river_network = RiverNetwork(world_map)
river_network.build(seed, super_duper_chunk_size)
river_network.spline_trees()
print("River network generation:", time.time() - start)

start = time.time()
river_network.index_splines_by_chunk()
print("Indexing splines by chunk:", time.time() - start)

# -----------

x,y = 10500, 4500

spline_refs = river_network.get_splines_near(x,y)

nearby_edges = set()
if len(spline_refs) > 0:
    for tree_id, edge in spline_refs:
        nearby_edges.add(edge)

    splines = []
    for tree_id, edge in spline_refs:
        tree_params = river_network.tree_params[tree_id]
        ts = river_network.tree_splines[tree_id]
        t = {"river_width": tree_params["river_width"],
            "scale_exponent": tree_params["scale_exponent"],
            "edge": edge}
        t["spline_points"] = ts.get_bezier_points(ts.control_points[edge], num_points=1000)
        splines.append(t)


    def get_chunk(x, y):
        return (
            int(np.floor(x / 1024)),
            int(np.floor(y / 1024))
        )

    start = time.time()
    cx, cy = get_chunk(x, y)
    min_x = cx * 1024
    min_y = cy * 1024
    max_x = min_x + 1023
    max_y = min_y + 1023

    river_mask, padding_info = mask_splines(
        splines,
        river_network.strahler_numbers,
        min_x, min_y, max_x, max_y, padding=100
    )
    padding, original_width, original_height = padding_info
    print("Masking splines:", time.time() - start)

    noise_generator = Noise(seed)
    heightmap = noise_generator.fractal_simplex_noise(noise="open", height=1024+2*padding, width=1024+2*padding,
                                                    x_offset=min_x-padding, y_offset=min_y-padding,
                                                    scale=256, octaves=8, lacunarity=2, persistence=0.5)
    heightmap = (heightmap+1)/2   

    max_width = river_network.max_river_width
    start = time.time()
    river_noise = noise_generator.fractal_simplex_noise(seed=noise_generator.seed+1, noise="open", height=1024+2*padding, width=1024+2*padding,
                                                    x_offset=min_x-padding, y_offset=min_y-padding,
                                                    scale=200, octaves=7, lacunarity=2, persistence=0.5)
    river_noise = np.abs(river_noise) 

    new_heightmap = carve_smooth_river_into_terrain(heightmap, river_mask, max_width, river_noise=river_noise, noise_strength=0.3)
    print("Carving river into terrain:", time.time() - start)

    display = Display(new_heightmap, 250, "cliffs")
    display.display_heightmap()

    new_heightmap = remove_padding(new_heightmap, padding, original_width, original_height)
    

    


fig, ax = plt.subplots(figsize=(6, 6))
voronoi_plot_2d(vor, ax=ax, show_vertices=False, line_colors='gray', point_size=0)

for polygon in range(len(world_map.polygons)):
    if polygon in world_map.ocean:
        color = 'blue'
    elif polygon in world_map.coastal:
        color = 'yellow'
    else:  
        color = 'green'
    plt.fill(*zip(*world_map.polygons[polygon]), color=color, alpha=0.5)

plt.plot(x, y, 'go', markersize=5)  

for tree_spline in river_network.tree_splines.values():
    spline_points = tree_spline.get_spline_points()
    for (parent, child), spoints in spline_points.items():
        if (parent, child) in nearby_edges:
            plt.plot(spoints[:, 0], spoints[:, 1], color='red', alpha=1, linewidth=river_network.strahler_numbers[child])
        else:
            plt.plot(spoints[:, 0], spoints[:, 1], color='blue', alpha=1, linewidth=river_network.strahler_numbers[child])

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