import numpy as np 
from shapely.geometry import Polygon as ShapelyPolygon
import cv2
from skimage.draw import polygon2mask
from scipy.ndimage import distance_transform_edt

class Point(np.ndarray):
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
    def __init__(self, p1, p2, i):
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
        return hash((tuple(self.ordered_start), tuple(self.ordered_end)))

    def __eq__(self, other):
        if not isinstance(other, Segment):
            return NotImplemented
        return (self.ordered_start, self.ordered_end) == (other.ordered_start, other.ordered_end)
    
    def __lt__(self, other):
        if not isinstance(other, Segment):
            return NotImplemented
        return (self.start[0], self.start[1], self.index) < (other.start[0], other.start[1], other.index)
        
class Polygon:
    def __init__(self, vertices=None):
        if vertices == None:
            self.vertices = []
        else:
            self.vertices = vertices

        self.n = len(self.vertices)
        self.center = self.calculate_center()
        self.bounding_box = self.calculate_bounding_box()
        self.distance_triple = self.calculate_distances()
        
    def calculate_center(self):
        if not self.vertices:
            return Point([0.0, 0.0])
        
        x_coords = np.array([v[0] for v in self.vertices])
        y_coords = np.array([v[1] for v in self.vertices])
        center_x = np.mean(x_coords)
        center_y = np.mean(y_coords)
        return Point([center_x, center_y])
    
    def translate_to_origin(self):
        translation_vector = -self.center
        self.vertices = [Point(v + translation_vector) for v in self.vertices]
        self.center = Point([0.0, 0.0])

    def calculate_bounding_box(self):
        x_coords = [v[0] for v in self.vertices]
        y_coords = [v[1] for v in self.vertices]
        min_point = Point([min(x_coords), min(y_coords)])
        max_point = Point([max(x_coords), max(y_coords)])
        return (min_point, max_point)
    
    def calculate_distances(self):
        minimum, maximum, summation = np.inf, -np.inf, 0
        for i in range(self.n):
            v = self.vertices[i] - self.vertices[(i + 1) % self.n]
            distance = v[0]*v[0] + v[1]*v[1]    
            minimum = min(minimum, distance)
            maximum = max(maximum, distance)
            summation += distance
        return (np.sqrt(minimum), np.sqrt(maximum), summation)

    def calculate_average_distance(self):
        return self.distance_triple[2] / self.n

    def scale_from_center(self, factor):
        original_center = self.center
        self.translate(-original_center)
        self.scale(factor)
        self.translate(original_center)

    def scale(self, factor):
        self.vertices = [Point(v * factor) for v in self.vertices]
        min_point, max_point = self.bounding_box
        self.bounding_box = (Point(min_point * factor), Point(max_point * factor))
        self.distance_triple = (factor * self.distance_triple[0], factor * self.distance_triple[1], factor * self.distance_triple[2])

    def translate(self, translation_vector):
        self.vertices = [Point(v + translation_vector) for v in self.vertices]
        self.center = self.calculate_center()
        min_point, max_point = self.bounding_box
        self.bounding_box = (Point(min_point + translation_vector), Point(max_point + translation_vector))

    def to_raster(self, width, height):
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
    
    @staticmethod
    def mask_transform(mask, spread_rate=0.2):
        dist_transform = distance_transform_edt(mask)
        dist_transform = dist_transform ** spread_rate
        dist_transform = dist_transform / dist_transform.max()
        return dist_transform