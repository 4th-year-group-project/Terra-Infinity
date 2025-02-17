from coastline.fractal_coastline import FractalCoastline
from coastline.geom import Point, Polygon, GeometryUtils
import numpy as np
from hashlib import sha256


def stable_seed(v1, v2):
    """Generates a consistent seed for an edge regardless of vertex order."""
    key = tuple(sorted([tuple(v1), tuple(v2)]))  
    hash_value = int(sha256(str(key).encode()).hexdigest(), 16) % (2**32)
    return hash_value

def consistent_normal(v1, v2):
    """Ensures the normal direction is consistent regardless of vertex order."""
    v1, v2 = sorted([v1, v2], key=lambda v: tuple(v))  

    line = v2 - v1
    line_unit = line / np.linalg.norm(line)
    normal = np.array([-line_unit[1], line_unit[0]]) 
    
    return normal  
    
def midpoint_displacement(relevant_polygons_edges, relevant_polygons_points, shared_edges, polygon_ids, strength=0.5):
    output_polygons = []
    for polygon in relevant_polygons_points:
        new_vertices = []
        for i in range(len(polygon)):
            new_vertices.append(polygon[i].tolist())
            next_index = (i + 1) % len(polygon)

            alpha = 0.5  
            midpoint = (1 - alpha) * polygon[i] + alpha * polygon[next_index]

            line_length = np.linalg.norm(polygon[next_index] - polygon[i])
            displacement = line_length * strength

            normal = consistent_normal(polygon[i], polygon[next_index])
            edge_seed = stable_seed(polygon[i], polygon[next_index])
            np.random.seed(edge_seed)  # Seed based on edge, not traversal order
            displacement_factor = np.random.uniform(-displacement, displacement)


            midpoint += displacement_factor * normal  
            new_vertices.append(midpoint.tolist())

        output_polygons.append(np.array(new_vertices))

    return relevant_polygons_edges, output_polygons, shared_edges, polygon_ids





# def midpoint_displace(self):
#         new_vertices = []

#         for i in range(self.n - 1):
#             new_vertices.append(self.vertices[i])
            
#             next_index = (i + 1) % self.n
            
#             alpha = (0.5 + np.random.uniform(-self.width, self.width))
#             midpoint = (1 - alpha) * self.vertices[i] + alpha * self.vertices[next_index]
            
#             line = (self.vertices[next_index] - self.vertices[i])
#             displace = np.random.uniform(-self.displacement, self.displacement)
            
#             midpoint += displace * GeometryUtils.norm(Point([-line[1], line[0]]))
#             new_vertices.append(midpoint)

#         self.vertices = new_vertices
#         self.n = len(self.vertices)
#         self.displacement *= self.roughness