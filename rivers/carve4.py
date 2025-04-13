from collections import defaultdict
from generation import Noise, Display, tools
import matplotlib.pyplot as plt
import numpy as np
from scipy.spatial import ckdtree
from scipy.spatial.distance import cdist
from .smooth2 import TreeSpline  
from scipy.ndimage import distance_transform_edt, gaussian_filter, binary_dilation
import cv2
from shapely.geometry import LineString
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Polygon

centroids = np.array([
    [0, 0],         # Node 0 (not used)
    [512, 0],       # Node 1 (root)
    [768, 220],     # Node 2
    [256, 230],     # Node 3
    [896, 530],     # Node 4
    [630, 590],     # Node 5
    [128, 490],     # Node 6
    [960, 768],     # Node 7
    [789, 808],     # Node 8
    [192, 858],     # Node 9
    [30, 728],      # Node 10
])

edges = [
    (1, 2), (1, 3),           # Root to first level
    (2, 4), (2, 5), (3, 6),   # First to second level
    (4, 7), (4, 8), (6, 9), (6, 10)  # Second to third level
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

tree = TreeSpline(edges, centroids)
tree.smooth_tree(curviness=0.5, meander=0.3)
#tree.plot_tree()
spline_dict = tree.get_spline_points(resolution=1000)

splines_by_first_node = defaultdict(list)

for (n1, n2), spline in spline_dict.items():
    splines_by_first_node[n1].append(spline[:20])

print(splines_by_first_node[1][0])
print("--------------")
print(splines_by_first_node[1][1])



import numpy as np
import matplotlib.pyplot as plt
from shapely.geometry import LineString
from matplotlib.patches import Polygon

# Define the first and second paths as LineStrings
path1 = LineString([(0, 0), (2, 2)])  # Example path 1
path2 = LineString([(2, 2), (4, 0)])  # Example path 2

# Function to compute cubic Bezier curve points
def cubic_bezier(t, p0, p1, p2, p3):
    p0, p1, p2, p3 = np.array(p0), np.array(p1), np.array(p2), np.array(p3)
    return (1 - t)**3 * p0 + 3 * (1 - t)**2 * t * p1 + 3 * (1 - t) * t**2 * p2 + t**3 * p3

# Define control points for the Bezier curve
start_point = path1.coords[0]  # End point of path1
end_point = path2.coords[-1]    # Start point of path2

# Choose some intermediate points to control the curve
control_point1 = (2, 3)  # A point above the line joining the end points
control_point2 = (3, 1)  # A point near the middle

# Generate the Bezier curve points
t = np.linspace(0, 1, 100)  # Parameter t from 0 to 1
curve_points = np.array([cubic_bezier(ti, start_point, control_point1, control_point2, end_point) for ti in t])

# Plot the paths and the Bezier curve
plt.plot(*path1.xy, label="Path 1", color='blue')
plt.plot(*path2.xy, label="Path 2", color='red')
plt.plot(curve_points[:, 0], curve_points[:, 1], label="Curved Edge", color='green')

# Create a closed polygon from the paths and the curve
path_coords = list(path1.coords) + list(curve_points) + list(path2.coords)
polygon = Polygon(path_coords, closed=True, color='cyan', alpha=0.5)

# Fill the shape and display the plot
plt.gca().add_patch(polygon)
plt.legend()
plt.title("Shape formed by two paths and a curved edge")
plt.show()
