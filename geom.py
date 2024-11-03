import numpy as np 
import random

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
        distances = []  
        for i in range(self.n):
            v = self.vertices[i]
            w = self.vertices[(i + 1) % self.n]
            distance = np.linalg.norm(v - w) 
            distances.append(distance) 
        distances = np.array(distances)
        return (np.min(distances), np.mean(distances), np.max(distances))

    def scale_from_center(self, factor):
        original_center = self.center
        self.translate(-original_center)
        self.scale(factor)
        self.translate(original_center)

    def scale(self, factor):
        self.vertices = [Point(v * factor) for v in self.vertices]
        min_point, max_point = self.bounding_box
        self.bounding_box = (Point(min_point * factor), Point(max_point * factor))

    def translate(self, translation_vector):
        self.vertices = [Point(v + translation_vector) for v in self.vertices]
        self.center = self.calculate_center()
        min_point, max_point = self.bounding_box
        self.bounding_box = (Point(min_point + translation_vector), Point(max_point + translation_vector))

    def __repr__(self):
        return f"Polygon(center={self.center}, bounding_box={self.bounding_box})"