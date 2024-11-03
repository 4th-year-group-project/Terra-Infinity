import numpy as np 
import random

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from functools import partial
from sortedcontainers import SortedList

from geom import *

seed = random.randint(0,1000)
print("Seed: " + str(seed))
random.seed(seed)

class Plot:
    def __init__(self):
        self.fig, self.ax = plt.subplots(figsize=(7, 7))
        self.ani_vertices = []
        self.ani_intersections = []

    def display(self, polygons, label=False):
        self.ax.clear()
        all_x = []
        all_y = []
        labeled_points = set()  # Set to track labeled points

        for polygon in polygons:
            vertices = polygon.vertices
            if len(vertices) > 2:
                x_coords = [v[0] for v in vertices]
                y_coords = [v[1] for v in vertices]

                # Close the polygon
                x_coords.append(x_coords[0])
                y_coords.append(y_coords[0])

                # Plot the polygon
                self.ax.plot(x_coords, y_coords, marker='o', markersize=2)
                self.ax.fill(x_coords, y_coords, alpha=0.3)

                # Extend all_x and all_y for axis limits
                all_x.extend(x_coords)
                all_y.extend(y_coords)

                # Add labels to each vertex, avoiding repeats
                if label:
                    idx = 0
                    for (x, y) in zip(x_coords[:-1], y_coords[:-1]):  # Exclude last point to avoid closing vertex
                        point = (x, y)
                        if point not in labeled_points:
                            idx += 1
                            labeled_points.add(point)  # Mark the point as labeled
                            self.ax.text(x, y, str(idx), fontsize=10, ha='right', va='bottom')  # Add label

        # Set limits based on all vertices
        self.ax.set_xlim(min(all_x) - 0.1, max(all_x) + 0.1)
        self.ax.set_ylim(min(all_y) - 0.1, max(all_y) + 0.1)
        self.ax.set_aspect('equal', adjustable='box')

    def animate(self, step):
        if step < len(self.ani_vertices):
            self.display([self.ani_vertices[step]])
        else:
            point, segs = self.ani_intersections[step - len(self.ani_vertices)]
            self.ax.plot(point[0], point[1], 'ro')

    def run_anim(self):
        for step in range(len(self.ani_vertices)):
            self.display([self.ani_vertices[step]])
            plt.pause(0.8)

        i = 0
        length = len(self.ani_intersections)
        while i < length // 2:
            point, seg1, seg2 = self.ani_intersections[i]
            self.ax.plot(point[0], point[1], 'ro', markersize=3)
            point, seg1, seg2 = self.ani_intersections[length-i-1]
            self.ax.plot(point[0], point[1], 'ro', markersize=3)
            i += 1
            #plt.pause(0.1/(100*length))

class GeometryUtils:
    @staticmethod
    def norm(vertex):
        mult = 1 / np.sqrt(vertex[0]**2 + vertex[1]**2)
        return mult * vertex

    @staticmethod
    def orientation(p, q, r):
        val = (q[1] - p[1]) * (r[0] - q[0]) - (q[0] - p[0]) * (r[1] - q[1])
        if val == 0:
            return 0  # Collinear
        elif val > 0:
            return 1  # Clockwise
        else:
            return 2  # Counterclockwise

    @staticmethod
    def on_segment(p, q, r):
        return (min(p[0], r[0]) < q[0] < max(p[0], r[0]) and
                min(p[1], r[1]) < q[1] < max(p[1], r[1]))
    
    @staticmethod
    def intersection(segment1, segment2):
        p1, q1 = segment1.start, segment1.end
        p2, q2 = segment2.start, segment2.end

        if p1 == p2 or p1 == q2 or q1 == p2 or q1 == q2:
            return False

        o1 = GeometryUtils.orientation(p1, q1, p2)
        o2 = GeometryUtils.orientation(p1, q1, q2)
        o3 = GeometryUtils.orientation(p2, q2, p1)
        o4 = GeometryUtils.orientation(p2, q2, q1)

        if o1 != o2 and o3 != o4:
            return True

        if o1 == 0 and GeometryUtils.on_segment(p1, p2, q1): return True
        if o2 == 0 and GeometryUtils.on_segment(p1, q2, q1): return True
        if o3 == 0 and GeometryUtils.on_segment(p2, p1, q2): return True
        if o4 == 0 and GeometryUtils.on_segment(p2, q1, q2): return True

        return False
    
    @staticmethod
    def intersection_point(p1, q1, p2, q2):
        r = q1 - p1 
        s = q2 - p2 

        r_cross_s = np.cross(r, s)
        if r_cross_s == 0:  # Lines are parallel
            return None
        
        t = np.cross(p2 - p1, s) / r_cross_s
        u = np.cross(p2 - p1, r) / r_cross_s

        if 0 <= t <= 1 and 0 <= u <= 1:
            intersection = p1 + t * r
            return intersection
        
        return None

    @staticmethod
    def bounding_box_check(seg1, seg2):
        min_x1, max_x1 = min(seg1.start[0], seg1.end[0]), max(seg1.start[0], seg1.end[0])
        min_y1, max_y1 = min(seg1.start[1], seg1.end[1]), max(seg1.start[1], seg1.end[1])
        min_x2, max_x2 = min(seg2.start[0], seg2.end[0]), max(seg2.start[0], seg2.end[0])
        min_y2, max_y2 = min(seg2.start[1], seg2.end[1]), max(seg2.start[1], seg2.end[1])

        return not (max_x1 < min_x2 or max_x2 < min_x1 or max_y1 < min_y2 or max_y2 < min_y1)

    @staticmethod
    def parameterize(p, v0, d):
        temp = p-v0
        return (temp @ d)/(d @ d)

class Coastline:
    def __init__(self, shape=Polygon([Point([0.5,-np.sqrt(3)/4]), Point([0,np.sqrt(3)/4]),Point([-0.5,-np.sqrt(3)/4])]), 
                 iterations=8, displacement=1, width=0, roughness=0.4, display=False):
        
        self.vertices = shape.vertices[:]
        self.n = len(self.vertices)
        self.iterations=iterations
        self.displacement=displacement
        self.width=width
        self.roughness=roughness

        self.map = {}
        self.intersection_points = set([])
        self.vertex_adjacency = {}
        self.polygons = []

        if display:
            self.plot = Plot()
            self.plot.ani_vertices.append(Polygon(self.vertices))
        else:
            self.plot = None

    def midpoint_displace(self):
        new_vertices = []

        for i in range(self.n):
            new_vertices.append(self.vertices[i])
            
            next_index = (i + 1) % self.n
            
            alpha = (0.5 + random.uniform(-self.width, self.width))
            midpoint = (1 - alpha) * self.vertices[i] + alpha * self.vertices[next_index]
            
            line = (self.vertices[next_index] - self.vertices[i])
            displace = random.uniform(-self.displacement, self.displacement)
            
            midpoint += displace * GeometryUtils.norm(Point([-line[1], line[0]]))
            new_vertices.append(midpoint)

        self.vertices = new_vertices
        self.n = len(self.vertices)
        self.displacement *= self.roughness
        if self.plot!=None:
            self.plot.ani_vertices.append(Polygon(self.vertices))
    
    def find_intersections(self):
        events = []
        for segment in self.edges:
            events.append((segment.start, 'start', segment))
            events.append((segment.end, 'end', segment))

        events.sort(key=lambda event: (event[0][0], event[0][1]))
        active_segments = SortedList() 
        intersections = []

        for event in events:
            point, event_type, segment = event
            if event_type == 'start':
                for active_segment in active_segments:
                    if GeometryUtils.bounding_box_check(segment, active_segment) and GeometryUtils.intersection(segment, active_segment):
                        middle = GeometryUtils.intersection_point(segment.start, segment.end, active_segment.start, active_segment.end)
                        if segment.index < active_segment.index:
                            intersections.append((middle, segment, active_segment))
                        else:
                            intersections.append((middle, active_segment, segment))
                active_segments.add(segment)
            else: 
                active_segments.remove(segment)

        return intersections

    def store_intersections(self):
        for intersection in self.intersections:
            point, seg1, seg2 = intersection

            temp = self.map.get(seg1, [])
            temp.append(point)
            self.map[seg1] = temp

            temp = self.map.get(seg2, [])
            temp.append(point)
            self.map[seg2] = temp

            self.intersection_points.add(point)

    def line_splitting(self):
        new_edges = []
        for edge in self.edges:
            if edge in self.map:
                v0 = edge.ordered_start
                v1 = edge.ordered_end
                d = v1 - v0
                points = sorted(self.map[edge], key=lambda point: GeometryUtils.parameterize(point, v0, d))

                new_edges.append(Segment(v0, points[0], -1))
                for i in range(len(points)-1):    
                    new_edges.append(Segment(points[i], points[i+1], -1))
                new_edges.append(Segment(points[len(points)-1], v1, -1))
            else:
                new_edges.append(edge)

        return new_edges
    
    def inside_flag_setting(self):
        inside = True
        for edge in self.edges:
            edge.flag = inside
            if edge.ordered_end in self.intersection_points:
                inside = not inside

            lst = self.vertex_adjacency.get(edge.ordered_start, [])
            lst.append(edge)
            self.vertex_adjacency[edge.ordered_start] = lst

    def polygon_traversal(self):
        visited = set([])
        v1 = None
        for edge in self.edges:
            if edge not in visited:
                current_edge = edge
                initial = current_edge.ordered_start
                polygon = []
                while (initial != v1):
                    v0, v1, flag = current_edge.ordered_start, current_edge.ordered_end, current_edge.flag
                    polygon.append(v0)
                    visited.add(current_edge)
                    current_edge = [adj for adj in self.vertex_adjacency[v1] if adj.flag == flag][0]
                self.polygons.append(Polygon(polygon))

    def fractal(self):
        for i in range(self.iterations):
            self.midpoint_displace()

        self.edges = [Segment(self.vertices[i], self.vertices[i + 1], i) for i in range(self.n - 1)]
        self.edges.append(Segment(self.vertices[self.n - 1], self.vertices[0], self.n))

        self.intersections = self.find_intersections()
        if self.plot != None:
            self.plot.ani_intersections = self.intersections

        self.store_intersections()
        self.edges = self.line_splitting()
        self.inside_flag_setting()

        self.polygon_traversal()
