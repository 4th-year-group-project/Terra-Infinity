from math import atan2

import numpy as np
from params.leaf_types import blossom, leaves


class Leaf:
    def __init__(self, pos, direction, right):
        self.pos = pos
        self.direction = direction
        self.right = right

    def get_shape(self, leaf_type, g_scale, scale, scale_x):
        # Removed u_v so no suport for triangles and rectangles

        if leaf_type < 0:  # blossom
            if leaf_type < -3:  # out of range
                leaf_type = -1
            shape = blossom(abs(leaf_type + 1))

        else:  # leaf
            if leaf_type < 1 or leaf_type > 10:  # is out of range or explicitly default
                leaf_type = 8
            shape = leaves(leaf_type - 1)

        verts = np.array(shape[0])
        faces = shape[1]

        for vert in verts:
            vert *= scale * g_scale
            vert[0] *= scale_x

        return verts, faces

    def to_track_quat(self, vector, track_axis="Z", up_axis="Y"):
        """Create a quaternion that rotates the track_axis to align with the given vector, with up_axis as a secondary reference."""
        vector = np.array(vector, dtype=float)
        vector = vector / np.linalg.norm(vector)  # normalize

        # Define axis indices
        axes = {"X": 0, "Y": 1, "Z": 2}
        track_idx = axes[track_axis]
        up_idx = axes[up_axis]

        # Create a rotation matrix
        rot_matrix = np.identity(3)

        # Set the track axis column to be the normalized vector
        rot_matrix[:, track_idx] = vector

        # Make sure the up axis is perpendicular to the track axis
        # First, create a perpendicular vector to the track axis
        if abs(vector[0]) < abs(vector[1]):
            temp = np.array([1.0, 0.0, 0.0])
        else:
            temp = np.array([0.0, 1.0, 0.0])

        temp = temp - vector * np.dot(temp, vector)
        temp = temp / np.linalg.norm(temp)

        # Set the up axis column
        rot_matrix[:, up_idx] = temp

        # The remaining axis is the cross product
        third_idx = 3 - track_idx - up_idx
        rot_matrix[:, third_idx] = np.cross(rot_matrix[:, track_idx], rot_matrix[:, up_idx])

        # Convert rotation matrix to quaternion
        return self.matrix_to_quaternion(rot_matrix)

    def matrix_to_quaternion(self, matrix):
        """Convert a rotation matrix to a quaternion"""
        trace = matrix[0, 0] + matrix[1, 1] + matrix[2, 2]

        if trace > 0:
            s = 0.5 / np.sqrt(trace + 1.0)
            w = 0.25 / s
            x = (matrix[2, 1] - matrix[1, 2]) * s
            y = (matrix[0, 2] - matrix[2, 0]) * s
            z = (matrix[1, 0] - matrix[0, 1]) * s
        elif matrix[0, 0] > matrix[1, 1] and matrix[0, 0] > matrix[2, 2]:
            s = 2.0 * np.sqrt(1.0 + matrix[0, 0] - matrix[1, 1] - matrix[2, 2])
            w = (matrix[2, 1] - matrix[1, 2]) / s
            x = 0.25 * s
            y = (matrix[0, 1] + matrix[1, 0]) / s
            z = (matrix[0, 2] + matrix[2, 0]) / s
        elif matrix[1, 1] > matrix[2, 2]:
            s = 2.0 * np.sqrt(1.0 + matrix[1, 1] - matrix[0, 0] - matrix[2, 2])
            w = (matrix[0, 2] - matrix[2, 0]) / s
            x = (matrix[0, 1] + matrix[1, 0]) / s
            y = 0.25 * s
            z = (matrix[1, 2] + matrix[2, 1]) / s
        else:
            s = 2.0 * np.sqrt(1.0 + matrix[2, 2] - matrix[0, 0] - matrix[1, 1])
            w = (matrix[1, 0] - matrix[0, 1]) / s
            x = (matrix[0, 2] + matrix[2, 0]) / s
            y = (matrix[1, 2] + matrix[2, 1]) / s
            z = 0.25 * s

        return np.array([w, x, y, z])

    def quaternion_multiply(self, q1, q2):
        """Multiply two quaternions"""
        w1, x1, y1, z1 = q1
        w2, x2, y2, z2 = q2

        w = w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2
        x = w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2
        y = w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2
        z = w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2

        return np.array([w, x, y, z])

    def quaternion_inverse(self, q):
        """Return the inverse of a quaternion"""
        w, x, y, z = q
        norm_sq = w * w + x * x + y * y + z * z
        return np.array([w, -x, -y, -z]) / norm_sq

    def vector_rotated(self, vector, quaternion):
        """Rotate a vector by a quaternion"""
        q = quaternion
        v = np.array([0, vector[0], vector[1], vector[2]])
        q_inv = self.quaternion_inverse(q)

        # Apply rotation: q * v * q^-1
        result = self.quaternion_multiply(q, self.quaternion_multiply(v, q_inv))

        return result[1:4]  # Extract the vector part

    def vector_angle(self, v1, v2):
        """Calculate the angle between two vectors"""
        v1_norm = v1 / np.linalg.norm(v1)
        v2_norm = v2 / np.linalg.norm(v2)

        dot_product = np.clip(np.dot(v1_norm, v2_norm), -1.0, 1.0)
        return np.arccos(dot_product)

    def axis_angle_to_quaternion(self, axis, angle):
        """Create a quaternion from an axis and angle"""
        axis = np.array(axis, dtype=float)
        axis = axis / np.linalg.norm(axis)

        half_angle = angle / 2.0
        sin_half = np.sin(half_angle)

        w = np.cos(half_angle)
        x = axis[0] * sin_half
        y = axis[1] * sin_half
        z = axis[2] * sin_half

        return np.array([w, x, y, z])

    def vector_declination(self, vector):
        """Calculate the declination (polar angle) of a vector. Declination is the angle between the vector and the positive Z axis"""
        vector_norm = vector / np.linalg.norm(vector)
        # Declination is the angle between the vector and the positive Z axis
        return np.arccos(np.clip(vector_norm[2], -1.0, 1.0))

    def get_mesh(self, bend, base_shape, index=0):
        trf = self.to_track_quat(self.direction, "Z", "Y")

        # Rotate the right vector by the inverse of the transformation
        right_t = self.vector_rotated(self.right, self.quaternion_inverse(trf))

        # Calculate the spin angle
        spin_ang = np.pi - self.vector_angle(right_t, np.array([1, 0, 0]))

        # Create a quaternion for the spin
        spin_ang_quat = self.axis_angle_to_quaternion(np.array([0, 0, 1]), spin_ang)

        # calculate bend transform if needed
        if bend > 0:
            bend_trf_1, bend_trf_2 = self.calc_bend_trf(bend)
        else:
            bend_trf_1 = None

        vertices = []
        for vertex in base_shape[0]:
            # rotate to correct direction
            n_vertex = vertex.copy()

            n_vertex = self.vector_rotated(n_vertex, spin_ang_quat)
            n_vertex = self.vector_rotated(n_vertex, trf)

            # apply bend if needed
            if bend > 0:
                self.vector_rotated(n_vertex, bend_trf_1)
                # self.vector_rotated(n_vertex, bend_trf_2)

            # move to right position
            n_vertex += self.pos

            # add to vertex array
            vertices.append(n_vertex)

        # set face to refer to vertices at correct offset in big vertex list
        index *= len(vertices)

        faces = [[elem + index for elem in face] for face in base_shape[1]]

        return vertices, faces

    def calc_bend_trf(self, bend):
        """Calculate the transformations required to 'bend' the leaf out/up from WP using numpy instead of Blender's mathutils"""
        # Calculate normal vector (cross product of direction and right)
        normal = np.cross(self.direction, self.right)

        # Calculate angle for first bend transformation
        theta_pos = atan2(self.pos[1], self.pos[0])  # y, x
        theta_bend = theta_pos - atan2(normal[1], normal[0])  # y, x

        # Create first bend quaternion (rotation around Z axis)
        bend_trf_1 = self.axis_angle_to_quaternion(np.array([0, 0, 1]), theta_bend * bend)

        # Rotate direction and right vectors by the first transformation
        self.direction = self.vector_rotated(self.direction, bend_trf_1)
        self.right = self.vector_rotated(self.right, bend_trf_1)

        # Recalculate normal after rotation
        normal = np.cross(self.direction, self.right)

        # Calculate angle for second bend transformation
        phi_bend = self.vector_declination(normal)
        if phi_bend > np.pi / 2:
            phi_bend = phi_bend - np.pi

        # Create second bend quaternion (rotation around right vector)
        bend_trf_2 = self.axis_angle_to_quaternion(self.right, phi_bend * bend)

        return bend_trf_1, bend_trf_2
