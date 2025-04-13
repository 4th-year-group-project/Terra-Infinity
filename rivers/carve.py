from generation import Noise, Display, tools
import matplotlib.pyplot as plt
import numpy as np
from scipy.spatial import cKDTree
from scipy.ndimage import distance_transform_edt
import time

noise = Noise(seed=0, width=1024, height=1024, x_offset=0, y_offset=0)
heightmap = noise.fractal_simplex_noise(noise="open", scale=256, octaves=6, persistence=0.5, lacunarity=2.0)
heightmap = tools.normalize(heightmap, 0.2, 1)

def smooth_min(a, b, k):
    h = np.clip((b - a + k) / (2 * k), 0, 1)
    return a * h + b * (1 - h) - k * h * (1 - h)

def smooth_max(a, b, k):
    k = -k
    h = np.clip((b - a + k) / (2 * k), 0, 1)
    return a * h + b * (1 - h) - k * h * (1 - h)

y_center = 512
width = 10
falloff = 2

depth = 0.1

centroids = np.array([
    [0, 0],        
    [512, 0],      
    [768, 256],     
    [256, 256],     
    [896, 512],     
    [640, 512],     
    [128, 512],     
    [960, 768],     
    [832, 768],     
    [192, 768],     
    [64, 768],     
])

edges = [
    (1, 2), (1, 3),          
    (2, 4), (2, 5), (3, 6),   
    (4, 7), (4, 8), (6, 9), (6, 10)  
]

strahler_numbers = {
    1: 3,
    2: 2,
    3: 2,
    4: 2,
    5: 1,
    6: 2,
    7: 1,
    8: 1,
    9: 1,
    10: 1
} 

from .smooth import TreeSpline
treespline = TreeSpline(edges, centroids)
treespline.smooth_tree()
spline_points = treespline.get_spline_points(resolution=1000)

start = time.time() 
grid_size = (1024, 1024)  
centerline_grid = np.zeros(grid_size, dtype=int)
width_grid = np.zeros(grid_size, dtype=float)

# First pass: same as before
for edge, points in spline_points.items():
    node1, node2 = edge
    width1 = (strahler_numbers[node1])**2
    width2 = (strahler_numbers[node2])**2
    
    num_points = len(points)
    t_values = np.linspace(0, 1, num_points)
    widths = width1 + t_values * (width2 - width1)
    
    for i, point in enumerate(points):
        x, y = int(point[0]), int(point[1])
        if 0 <= x < grid_size[1] and 0 <= y < grid_size[0]:
            centerline_grid[y, x] = 1
            width_grid[y, x] = widths[i]

# Compute distance transform
distance_grid, indices = distance_transform_edt(centerline_grid == 0, return_indices=True)

# Get the width at each point's nearest centerline
closest_y = indices[0]
closest_x = indices[1]
closest_widths = width_grid[closest_y, closest_x]*3

# Create tree grid where distance is less than half the width
#norm_dist = tools.normalize(distance_grid)

mask = distance_grid <= closest_widths
print(np.max(distance_grid[mask]))
tree_grid = np.where(mask, True, False)

#distance_grid = distance_transform_edt(tree_grid == 0, return_indices=False)

# mask = grid[closest_y, closest_x] == 1
# pixel_widths[mask] = width_grid[closest_y[mask], closest_x[mask]]

# widths_grid = pixel_widths*distance_grid

river_mask = 1 - smooth_max(0, 1-(distance_grid/1000)**2, 2)
new_heightmap = heightmap * river_mask + depth * (1 - river_mask)

print(time.time()-start)

plt.imshow(tree_grid, cmap='gray')
plt.colorbar()
plt.show()

#new_heightmap = heightmap

# display = Display(new_heightmap, height_scale=250, colormap='terrain')
# display.display_heightmap()





# def v(distance_field, width, falloff, valley_smoothness, depth):
#     river_mask = 1 - smooth_max(0, 1-(distance_field/width)**falloff, valley_smoothness)
#     river = river_mask + depth * (1 - river_mask)
#     return river

# river1 = v(distance_field, 20, 2.5, 1, 0.15)
# river2 = v(distance_field, 100, 2, 1, 0.2)
# valley = smooth_min(river1, river2, 0.5)

# new_heightmap = heightmap * valley + depth * (1 - valley)   

# plt.imshow(valley + depth*(1-valley), cmap='gray')
# plt.plot(all_points[:, 0], all_points[:, 1], 'r.', markersize=1)
# plt.colorbar()
# plt.show()




# # rim = rim_steepness * np.minimum(norm_dist**2 -2-rim_width, 0)**2 + 0.1
# # valley_rim = smooth_min(valley, rim, rim_smoothness)

# # p = smooth_min(smooth_max(np.minimum(np.abs(norm_dist)-rim_width,1), valley_rim, 0.5), 1, 0.1)
# # new_heightmap = heightmap*p + depth*(1-p)

# #falloff_mask = smooth_min(falloff_mask, rim, 0.1)

# # plt.subplot(1, 2, 1)
# # plt.imshow(falloff_mask, cmap='gray')
# # plt.colorbar()

# # plt.subplot(1, 2, 2)
# # plt.imshow(rim, cmap='gray')
# # plt.colorbar()
# # plt.show()


# print(new_heightmap.min(), new_heightmap.max())

# display = Display(new_heightmap, 250, "terrain")
# display.display_heightmap()