"""Experimental code for designing fractal coastline generation with midpoint displacement. """

import cv2
import matplotlib.pyplot as plt
import numpy as np
from sortedcontainers import SortedList

from world_generation.coastline.geom import GeometryUtils, Point, Polygon, Segment

class Plot:
    """Class to plot the polygons and their intersections.
    
    Attributes:
        fig: Matplotlib figure object.
        ax: Matplotlib axis object.
        ani_vertices: List to store vertices of polygons for animation.
        ani_intersections: List to store intersections for animation.

    """

    def __init__(self):
        """Initialize the Plot class."""
        self.fig, self.ax = plt.subplots(figsize=(7, 7))
        self.ani_vertices = []
        self.ani_intersections = []

    @staticmethod
    def display(ax, polygons, label=False):
        """Creates the plot of polygons and their intersections.
        
        Args:
            ax: Matplotlib axis object to plot on.
            polygons: List of Polygon objects to plot.
            label: Boolean indicating whether to label the points or not.
        
        """
        ax.clear()
        all_x = []
        all_y = []
        labeled_points = set()  

        for polygon in polygons:
            vertices = polygon.vertices
            if len(vertices) > 2:
                x_coords = [v[0] for v in vertices]
                y_coords = [v[1] for v in vertices]

                x_coords.append(x_coords[0])
                y_coords.append(y_coords[0])

                ax.plot(x_coords, y_coords, marker="o", markersize=2)
                ax.fill(x_coords, y_coords, alpha=0.3)

                all_x.extend(x_coords)
                all_y.extend(y_coords)

                if label:
                    idx = 0
                    for x, y in zip(
                        x_coords[:-1], y_coords[:-1], strict=False
                    ):  
                        point = (x, y)
                        if point not in labeled_points:
                            idx += 1
                            labeled_points.add(point)  
                            ax.text(x, y, str(idx), fontsize=10, ha="right", va="bottom")  

        ax.set_xlim(min(all_x) - 0.1, max(all_x) + 0.1)
        ax.set_ylim(min(all_y) - 0.1, max(all_y) + 0.1)
        ax.set_aspect("equal", adjustable="box")

    def display_iteration(self, iteration=None):
        """Displays the polygons at a specific iteration.

        Args:
            iteration: The iteration number to display. If None, displays the last iteration.

        """

        if iteration is None:
            iteration = len(self.ani_vertices) - 1
        self.display(self.ax, polygons=[self.ani_vertices[iteration]], label=False)
        plt.show()
        self.fig, self.ax = plt.subplots(figsize=(7, 7))

    def display_polygons(self, polygons=None):
        """Displays the polygons.
        
        Args:
            polygons: List of Polygon objects to display. If None, displays the last iteration.

        """
        if polygons is None:
            self.display_iteration()
        else:
            self.display(self.ax, polygons=polygons, label=False)
            plt.show()
            self.fig, self.ax = plt.subplots(figsize=(7, 7))

    # def animate(self, step):
    #     if step < len(self.ani_vertices):
    #         self.display_polygons([self.ani_vertices[step]])
    #     else:
    #         point, segs = self.ani_intersections[step - len(self.ani_vertices)]
    #         self.ax.plot(point[0], point[1], 'ro')

    # def run_anim(self):
    #     for step in range(len(self.ani_vertices)):
    #         self.display_polygons([self.ani_vertices[step]])
    #         plt.pause(0.8)

    #     i = 0
    #     length = len(self.ani_intersections)
    #     while i < length // 2:
    #         point, seg1, seg2 = self.ani_intersections[i]
    #         self.ax.plot(point[0], point[1], 'ro', markersize=3)
    #         point, seg1, seg2 = self.ani_intersections[length-i-1]
    #         self.ax.plot(point[0], point[1], 'ro', markersize=3)
    #         i += 1
    #         #plt.pause(0.1/(100*length))


class FractalCoastline:
    """Class to generate fractal coastlines using midpoint displacement.

    Attributes:
        seed: Random seed for reproducibility.
        shape: Polygon object representing the initial shape.
        displacement: Maximum displacement for midpoint displacement.
        width: Width of the coastline.
        roughness: Roughness factor for the fractal generation.
        display: Boolean indicating whether to display the plot or not.
        vertices: List of vertices of the generated coastline.
        n: Number of vertices in the coastline.
        bounding_box: Bounding box of the coastline.
        distance_triple: Distance triple for the coastline.
        map: Dictionary to store intersections between segments.
        intersection_points: Set to store intersection points.
        vertex_adjacency: Dictionary to store adjacency of vertices.
        polygons: SortedList to store generated polygons.

    """
    def __init__(
        self,
        seed=42,
        shape=Polygon([Point([0.5, -np.sqrt(3) / 4]), Point([0, np.sqrt(3) / 4]), Point([-0.5, -np.sqrt(3) / 4])]),
        displacement=1,
        width=0,
        roughness=0.4,
        display=False,
    ):
        """Initialize the FractalCoastline class.
        
        Args:
            seed: Random seed for reproducibility.
            shape: Polygon object representing the initial shape.
            displacement: Maximum displacement for midpoint displacement.
            width: Width of the coastline.
            roughness: Roughness factor for the fractal generation.
            display: Boolean indicating whether to display the plot or not.

        """

        np.random.seed(seed)

        self.vertices = shape.vertices[:]
        self.n = len(self.vertices)
        self.displacement = displacement
        self.width = width
        self.roughness = roughness

        self.bounding_box = shape.bounding_box
        self.distance_triple = shape.distance_triple

        if display:
            self.plot = Plot()
            self.plot.ani_vertices.append(Polygon(self.vertices))
        else:
            self.plot = None

    def midpoint_displace(self):
        """Performs midpoint displacement on the coastline vertices."""

        new_vertices = []

        for i in range(self.n - 1):
            new_vertices.append(self.vertices[i])

            next_index = (i + 1) % self.n

            alpha = 0.5 + np.random.uniform(-self.width, self.width)
            midpoint = (1 - alpha) * self.vertices[i] + alpha * self.vertices[next_index]

            line = self.vertices[next_index] - self.vertices[i]
            displace = np.random.uniform(-self.displacement, self.displacement)

            midpoint += displace * GeometryUtils.norm(Point([-line[1], line[0]]))
            new_vertices.append(midpoint)

        self.vertices = new_vertices
        self.n = len(self.vertices)
        self.displacement *= self.roughness

        if self.plot is not None:
            self.plot.ani_vertices.append(Polygon(self.vertices))

    def find_intersections(self):
        """Finds intersections between segments using the sweep line algorithm.
        
        Returns: 
            List of tuples containing intersection points and the segments involved.

        """

        events = []
        for segment in self.edges:
            events.append((segment.start, "start", segment))
            events.append((segment.end, "end", segment))

        events.sort(key=lambda event: (event[0][0], event[0][1]))
        active_segments = SortedList()
        intersections = []

        for event in events:
            _, event_type, segment = event
            if event_type == "start":
                for active_segment in active_segments:
                    if GeometryUtils.bounding_box_check(segment, active_segment) and GeometryUtils.intersection(
                        segment, active_segment
                    ):
                        middle = GeometryUtils.intersection_point(
                            segment.start, segment.end, active_segment.start, active_segment.end
                        )
                        if segment.index < active_segment.index:
                            intersections.append((middle, segment, active_segment))
                        else:
                            intersections.append((middle, active_segment, segment))
                active_segments.add(segment)
            else:
                active_segments.remove(segment)

        return intersections

    def store_intersections(self):
        """Stores intersections in a dictionary."""

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
        """Splits lines at intersection points.
        
        Returns:
            List of new edges after splitting.
        """

        new_edges = []
        for edge in self.edges:
            if edge in self.map:
                v0 = edge.ordered_start
                v1 = edge.ordered_end
                d = v1 - v0
                points = sorted(self.map[edge], key=lambda point: GeometryUtils.parameterize(point, v0, d))

                new_edges.append(Segment(v0, points[0], -1))
                for i in range(len(points) - 1):
                    new_edges.append(Segment(points[i], points[i + 1], -1))
                new_edges.append(Segment(points[len(points) - 1], v1, -1))
            else:
                new_edges.append(edge)

        return new_edges

    def inside_flag_setting(self):
        """Sets the inside flag for edges based on their intersection points."""
        
        inside = True
        for edge in self.edges:
            edge.flag = inside
            if edge.ordered_end in self.intersection_points:
                inside = not inside

            lst = self.vertex_adjacency.get(edge.ordered_start, [])
            lst.append(edge)
            self.vertex_adjacency[edge.ordered_start] = lst

    def polygon_traversal(self):
        """Traverses the edges to create polygons where insideness is True."""

        visited = set()
        v1 = None
        for edge in self.edges:
            if edge not in visited:
                current_edge = edge
                initial = current_edge.ordered_start
                polygon = []
                while initial != v1:
                    v0, v1, flag = current_edge.ordered_start, current_edge.ordered_end, current_edge.flag
                    polygon.append(v0)
                    visited.add(current_edge)
                    current_edge = [adj for adj in self.vertex_adjacency[v1] if adj.flag == flag][0]
                new_polygon = Polygon(polygon)
                self.polygons.add(new_polygon)

                min_point = Point([
                    min(self.bounding_box[0][0], new_polygon.bounding_box[0][0]),
                    min(self.bounding_box[0][1], new_polygon.bounding_box[0][1]),
                ])
                max_point = Point([
                    max(self.bounding_box[1][0], new_polygon.bounding_box[1][0]),
                    max(self.bounding_box[1][1], new_polygon.bounding_box[1][1]),
                ])
                self.bounding_box = (min_point, max_point)
                self.distance_triple = (
                    min(self.distance_triple[0], new_polygon.distance_triple[0]),
                    max(self.distance_triple[1], new_polygon.distance_triple[1]),
                    self.distance_triple[2] + new_polygon.distance_triple[2],
                )

    def fractal(self, iterations=8, displacement=None, width=None, roughness=None):
        """Generates the fractal coastline.

        Args:
            iterations: Number of iterations for the fractal generation.
            displacement: Maximum displacement for midpoint displacement.
            width: Width of the coastline.
            roughness: Roughness factor for the fractal generation.

        Returns:
            List of vertices of the generated coastline.    
            
        """

        if displacement is not None:
            self.displacement = displacement
        if width is not None:
            self.width = width
        if roughness is not None:
            self.roughness = roughness

        for _ in range(iterations):
            self.midpoint_displace()

        return self.vertices

    def polygonize(self):
        """Converts the coastline into polygons based on intersections.
        
        Returns:
            List of Polygon objects representing the generated polygons.
        """

        self.map = {}
        self.intersection_points = set()
        self.vertex_adjacency = {}
        self.polygons = SortedList(key=lambda polygon: -polygon.n)

        self.edges = [Segment(self.vertices[i], self.vertices[i + 1], i) for i in range(self.n - 1)]
        self.edges.append(Segment(self.vertices[self.n - 1], self.vertices[0], self.n))

        self.intersections = self.find_intersections()
        if self.plot is not None:
            self.plot.ani_intersections = self.intersections

        self.store_intersections()
        self.edges = self.line_splitting()
        self.inside_flag_setting()

        self.distance_triple = (np.inf, -np.inf, 0)
        self.polygon_traversal()
        self.distance_triple = (self.distance_triple[0], self.distance_triple[1], self.distance_triple[2] / self.n)

        return self.polygons

    def to_raster(self, width, height):
        """Converts the polygons to a raster mask.

        Args:
            width: Width of the raster mask.
            height: Height of the raster mask.

        Returns:
            mask: Binary raster mask of the polygons.

        """

        min_point, max_point = self.bounding_box
        w = max_point[0] - min_point[0]
        h = max_point[1] - min_point[1]

        scale_x = width / w
        scale_y = height / h
        scale = min(scale_x, scale_y)

        offset_x = (width - w * scale) / 2
        offset_y = (height - h * scale) / 2

        isles = []
        for polygon in self.polygons:
            raster_vertices = []
            for v in polygon.vertices:
                raster_x = (v[0] - min_point[0]) * scale + offset_x
                raster_y = (v[1] - min_point[1]) * scale + offset_y
                raster_y = height - raster_y
                raster_vertices.append([int(raster_x), int(raster_y)])
            raster_vertices = np.array(raster_vertices)
            isles.append(raster_vertices)

        mask = np.zeros((height, width), dtype=np.uint8)
        # cv2.fillPoly(mask, isles, 1)
        for isle in isles:
            cv2.fillPoly(mask, [isle], 1)
            # mask = cv2.bitwise_or(mask, individual_mask)

        return mask
