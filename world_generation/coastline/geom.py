"""Helper functions for geometry operations."""

import cv2
import numpy as np
from scipy.ndimage import distance_transform_edt

class Point(np.ndarray):
    """A class representing a point in 2D space extending numpy.ndarray."""

    def __new__(cls, input_array):
        obj = np.asarray(input_array).view(cls)
        return obj

    def __hash__(self):
        return hash(self.tobytes())

    def __eq__(self, other):
        return np.array_equal(self, other)

    def __ne__(self, other):
        return not np.array_equal(self, other)

class Segment:
    """A class representing a line segment defined by two endpoints.
    
    Attributes:
        ordered_start (Point): The starting point of the segment.
        ordered_end (Point): The ending point of the segment.
        index (int): The index of the segment.
        flag (bool): A flag indicating if the segment is valid.
        start (Point): The starting point of the segment in a specific order.
        end (Point): The ending point of the segment in a specific order.

    """

    def __init__(self, p1, p2, i):
        """Initialize a Segment object with two points and an index."""

        self.index = i
        self.ordered_start = p1
        self.ordered_end = p2

        self.flag = True

        if (p1[0] < p2[0]) or (p1[0] == p2[0] and p1[1] < p2[1]):
            self.start = p1
            self.end = p2
        else:
            self.start = p2
            self.end = p1

    def __repr__(self):
        return f"Segment({self.ordered_start}, {self.ordered_end}, {self.index}, {self.flag})"

    def __hash__(self):
        """Return a hash value for the segment based on its ordered start and end points."""
        return hash((tuple(self.ordered_start), tuple(self.ordered_end)))

    def __eq__(self, other):
        """Check if two segments are equal based on their ordered start and end points."""
        if not isinstance(other, Segment):
            return NotImplemented
        return (self.ordered_start, self.ordered_end) == (other.ordered_start, other.ordered_end)

    def __lt__(self, other):
        """Compare two segments based on their starting points and index."""
        if not isinstance(other, Segment):
            return NotImplemented
        return (self.start[0], self.start[1], self.index) < (other.start[0], other.start[1], other.index)

class Polygon:
    """A class representing a polygon defined by its vertices.

    Attributes:
        vertices (list): A list of vertices (Point objects) defining the polygon.
        n (int): The number of vertices in the polygon.
        center (Point): The center point of the polygon.
        bounding_box (tuple): A tuple containing the minimum and maximum points of the bounding box.
        distance_triple (tuple): A tuple containing the minimum, maximum, and sum of distances between vertices.

    """

    def __init__(self, vertices=None):
        """Initialize a Polygon object with a list of vertices."""
        if vertices is None:
            self.vertices = []
        else:
            self.vertices = vertices

        self.n = len(self.vertices)
        self.center = self.calculate_center()
        self.bounding_box = self.calculate_bounding_box()
        self.distance_triple = self.calculate_distances()

    def calculate_center(self):
        """Calculate the center of the polygon.
        
        Returns:
            Point: The center point of the polygon.
            
        """
        if not self.vertices:
            return Point([0.0, 0.0])

        x_coords = np.array([v[0] for v in self.vertices])
        y_coords = np.array([v[1] for v in self.vertices])
        center_x = np.mean(x_coords)
        center_y = np.mean(y_coords)
        return Point([center_x, center_y])

    def translate_to_origin(self):
        """Translates the polygon to the origin."""
        translation_vector = -self.center
        self.vertices = [Point(v + translation_vector) for v in self.vertices]
        self.center = Point([0.0, 0.0])

    def calculate_bounding_box(self):
        """Calculate the bounding box of the polygon.

        Returns:
            tuple: A tuple containing the minimum and maximum points of the bounding box.
            
        """
        x_coords = [v[0] for v in self.vertices]
        y_coords = [v[1] for v in self.vertices]
        min_point = Point([min(x_coords), min(y_coords)])
        max_point = Point([max(x_coords), max(y_coords)])
        return (min_point, max_point)

    def calculate_distances(self):
        """Calculate the distances between vertices of the polygon.

        Returns:
            tuple: A tuple containing the minimum, maximum, and sum of distances between vertices.
            
        """
        minimum, maximum, summation = np.inf, -np.inf, 0
        for i in range(self.n):
            v = self.vertices[i] - self.vertices[(i + 1) % self.n]
            distance = v[0] * v[0] + v[1] * v[1]
            minimum = min(minimum, distance)
            maximum = max(maximum, distance)
            summation += distance
        return (np.sqrt(minimum), np.sqrt(maximum), summation)

    def calculate_average_distance(self):
        """Returns the average distance between vertices of the polygon."""
        return self.distance_triple[2] / self.n

    def scale_from_center(self, factor):
        """Scale the polygon from its current center."""
        original_center = self.center
        self.translate(-original_center)
        self.scale(factor)
        self.translate(original_center)

    def scale(self, factor):
        """Scale the polygon by a given factor from the origin."""
        self.vertices = [Point(v * factor) for v in self.vertices]
        min_point, max_point = self.bounding_box
        self.bounding_box = (Point(min_point * factor), Point(max_point * factor))
        self.distance_triple = (
            factor * self.distance_triple[0],
            factor * self.distance_triple[1],
            factor * self.distance_triple[2],
        )

    def translate(self, translation_vector):
        """Translate the polygon by a given vector."""
        self.vertices = [Point(v + translation_vector) for v in self.vertices]
        self.center = self.calculate_center()
        min_point, max_point = self.bounding_box
        self.bounding_box = (Point(min_point + translation_vector), Point(max_point + translation_vector))

    def to_raster(self, width, height):
        """Convert the polygon to a binary mask.

        Returns:
            A binary mask of the polygon.
        """
        min_point, max_point = self.bounding_box

        w = max_point[0] - min_point[0]
        h = max_point[1] - min_point[1]

        scale_x = width / w
        scale_y = height / h
        scale = min(scale_x, scale_y)

        offset_x = (width - w * scale) / 2
        offset_y = (height - h * scale) / 2

        raster_vertices = []
        for v in self.vertices:
            raster_x = (v[0] - min_point[0]) * scale + offset_x
            raster_y = (v[1] - min_point[1]) * scale + offset_y
            raster_y = height - raster_y
            raster_vertices.append([int(raster_x), int(raster_y)])

        raster_vertices = np.array(raster_vertices)

        mask = np.zeros((height, width), dtype=np.uint8)
        cv2.fillPoly(mask, [raster_vertices], 1)
        return mask

    def __repr__(self):
        return f"Polygon(center={self.center}, bounding_box={self.bounding_box}, |V|= {self.n})"

    def __iter__(self):
        return iter(self.vertices)

    def __len__(self):
        return self.n

    def __getitem__(self, index):
        return self.vertices[index]


class GeometryUtils:
    """A class containing static helper methods for various geometry operations."""

    @staticmethod
    def norm(vertex):
        """Normalize a 2D vector."""
        mult = 1 / np.sqrt(vertex[0] ** 2 + vertex[1] ** 2)
        return mult * vertex

    @staticmethod
    def norm2(vertex):
        """Normalize a 2D vector to unit length."""
        mult = 1 / np.sqrt(vertex[0] ** 2 + vertex[1] ** 2)
        return mult * vertex[0], mult * vertex[1]

    @staticmethod
    def orientation(p, q, r):
        """Find the orientation of the ordered triplet (p, q, r).

        Args:
            p (Point): First point
            q (Point): Second point.
            r (Point): Third point.

        Returns:
            0 if collinear, 1 if clockwise, 2 if counterclockwise.
        """ 
        val = (q[1] - p[1]) * (r[0] - q[0]) - (q[0] - p[0]) * (r[1] - q[1])
        if val == 0:
            return 0  # Collinear
        elif val > 0:
            return 1  # Clockwise
        else:
            return 2  # Counterclockwise

    @staticmethod
    def on_segment(p, q, r):
        """Check if point q lies on line segment 'pr'.
        
        Args:
            p (Point): Start point of the segment.
            q (Point): Point to check.
            r (Point): End point of the segment.

        Returns:
            bool: True if q lies on segment 'pr', False otherwise.
    
        """
        return min(p[0], r[0]) < q[0] < max(p[0], r[0]) and min(p[1], r[1]) < q[1] < max(p[1], r[1])

    @staticmethod
    def intersection(segment1, segment2):
        """Check if two segments intersect.
        
        Args:
            segment1 (Segment): First segment.
            segment2 (Segment): Second segment.

        Returns:
            bool: True if the segments intersect, False otherwise.

        """
        p1, q1 = segment1.start, segment1.end
        p2, q2 = segment2.start, segment2.end

        if p1 in (p2, q2) or q1 in (p2, q2):
            return False

        o1 = GeometryUtils.orientation(p1, q1, p2)
        o2 = GeometryUtils.orientation(p1, q1, q2)
        o3 = GeometryUtils.orientation(p2, q2, p1)
        o4 = GeometryUtils.orientation(p2, q2, q1)

        if o1 != o2 and o3 != o4:
            return True

        if o1 == 0 and GeometryUtils.on_segment(p1, p2, q1):
            return True
        if o2 == 0 and GeometryUtils.on_segment(p1, q2, q1):
            return True
        if o3 == 0 and GeometryUtils.on_segment(p2, p1, q2):
            return True
        return o4 == 0 and GeometryUtils.on_segment(p2, q1, q2)

    @staticmethod
    def intersection_point(p1, q1, p2, q2):
        """Calculate the intersection point of two line segments.   
        
        Args:
            p1 (Point): Start point of the first segment.
            q1 (Point): End point of the first segment.
            p2 (Point): Start point of the second segment.
            q2 (Point): End point of the second segment.

        Returns:
            Point: The intersection point if it exists, None otherwise.

        """

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
        """Check if the bounding boxes of two segments intersect.

        Args:
            seg1 (Segment): First segment.
            seg2 (Segment): Second segment.

        Returns:
            bool: True if the bounding boxes intersect, False otherwise.

        """

        min_x1, max_x1 = min(seg1.start[0], seg1.end[0]), max(seg1.start[0], seg1.end[0])
        min_y1, max_y1 = min(seg1.start[1], seg1.end[1]), max(seg1.start[1], seg1.end[1])
        min_x2, max_x2 = min(seg2.start[0], seg2.end[0]), max(seg2.start[0], seg2.end[0])
        min_y2, max_y2 = min(seg2.start[1], seg2.end[1]), max(seg2.start[1], seg2.end[1])

        return not (max_x1 < min_x2 or max_x2 < min_x1 or max_y1 < min_y2 or max_y2 < min_y1)

    @staticmethod
    def parameterize(p, v0, d):
        """Parameterize a point p on a line defined by point v0 and direction d."""
        temp = p - v0
        return (temp @ d) / (d @ d)

    @staticmethod
    def mask_transform(mask, spread_rate=0.2):
        """Apply a distance transform to a binary mask.
        
        Args:
            mask (np.ndarray): A binary mask.
            spread_rate (float): The rate at which to spread the distance transform.

        Returns:
            Numpy array of the distance transformed mask.
        """
        dist_transform = distance_transform_edt(mask)
        dist_transform = dist_transform**spread_rate
        dist_transform = dist_transform / dist_transform.max()
        return dist_transform
