import math
from dataclasses import dataclass

import numpy as np
import vedo


@dataclass
class TurtleState:
    position: np.ndarray  # 3D position
    heading: np.ndarray  # Forward direction
    left: np.ndarray  # Left direction
    up: np.ndarray  # Up direction
    radius: float  # Current branch radius

    def copy(self):
        return TurtleState(
            position=self.position.copy(),
            heading=self.heading.copy(),
            left=self.left.copy(),
            up=self.up.copy(),
            radius=self.radius,
        )


@dataclass
class BezierPoint:
    point: np.ndarray  # The point position
    control_in: np.ndarray  # Control point for incoming curve
    control_out: np.ndarray  # Control point for outgoing curve
    radius: float  # Branch radius at this point


class Branch:
    def __init__(self, start_point: np.ndarray, start_radius: float, direction: np.ndarray):
        self.points: list[BezierPoint] = []

        # Initialize with first point
        first_point = BezierPoint(
            point=start_point,
            control_in=start_point - direction * 0.1,  # Small offset in opposite direction
            control_out=start_point + direction * 0.1,  # Small offset in direction
            radius=start_radius,
        )
        self.points.append(first_point)

    def add_point(self, turtle: TurtleState, segment_length: float, random_variation: float = 0.1):
        # Apply random variation to create natural winding
        if random_variation > 0:
            # Create slightly perturbed direction vector
            random_offset = np.random.uniform(-random_variation, random_variation, 3)
            direction = turtle.heading + random_offset * 0.1
            direction = direction / np.linalg.norm(direction)
        else:
            direction = turtle.heading

        # Calculate the new point position
        new_position = turtle.position.copy()

        # Get the previous point (the last one added)
        prev_point = self.points[-1]

        # Update the control points for the previous point
        # Make the tangent aligned with the direction of the new segment
        tangent = direction * segment_length * 0.33  # 1/3 of segment length is common for Bézier control points
        prev_point.control_out = prev_point.point + tangent

        # Create the new point
        new_point = BezierPoint(
            point=new_position,
            control_in=new_position - tangent,  # incoming control point
            control_out=new_position + tangent,  # outgoing control point
            radius=turtle.radius,
        )

        # Add new point to the branch
        self.points.append(new_point)

        # Update the radius at the start point to ensure smooth tapering
        # Linear interpolation of radius
        if len(self.points) > 1:
            prev_prev_point = self.points[-2]
            self.points[-1].radius = prev_prev_point.radius * 0.9 + turtle.radius * 0.1

    def create_mesh(self, segments: int = 8, resolution: int = 10):
        """Create a mesh from the Bézier curve.

        Args:
            segments: Number of segments around the circumference
            resolution: Number of segments along the branch length

        Returns:
            vertices, faces
        """
        if len(self.points) < 2:
            return [], []

        vertices = []
        faces = []

        # For each section between Bézier points
        for i in range(len(self.points) - 1):
            p0 = self.points[i]
            p1 = self.points[i + 1]

            # Generate points along the Bézier curve
            for t_idx in range(resolution + 1):
                t = t_idx / resolution

                # Cubic Bézier curve formula
                # B(t) = (1-t)³P₀ + 3(1-t)²tP₁ + 3(1-t)t²P₂ + t³P₃
                # Where P₀ is start point, P₃ is end point, P₁ and P₂ are control points

                # For this implementation, P₀ = p0.point, P₁ = p0.control_out,
                # P₂ = p1.control_in, P₃ = p1.point

                t_inv = 1 - t
                b0 = t_inv**3
                b1 = 3 * t_inv**2 * t
                b2 = 3 * t_inv * t**2
                b3 = t**3

                point = b0 * p0.point + b1 * p0.control_out + b2 * p1.control_in + b3 * p1.point

                # Linear interpolation for radius
                radius = p0.radius * t_inv + p1.radius * t

                # Calculate the tangent at this point
                tangent = (
                    -3 * t_inv**2 * p0.point
                    + 3 * t_inv**2 * p0.control_out
                    - 6 * t_inv * t * p0.control_out
                    - 3 * t_inv**2 * p1.control_in
                    + 6 * t_inv * t * p1.control_in
                    + 3 * t**2 * p1.control_in
                    - 3 * t**2 * p1.point
                )

                # Normalize the tangent
                tangent = tangent / (np.linalg.norm(tangent) + 1e-10)

                # Find perpendicular vectors to create circle
                # First perpendicular vector (arbitrary)
                perp1 = np.array([1, 0, 0])
                if abs(np.dot(tangent, perp1)) > 0.9:
                    perp1 = np.array([0, 1, 0])

                perp1 = perp1 - np.dot(perp1, tangent) * tangent
                perp1 = perp1 / (np.linalg.norm(perp1) + 1e-10)

                # Second perpendicular vector (cross product)
                perp2 = np.cross(tangent, perp1)
                perp2 = perp2 / (np.linalg.norm(perp2) + 1e-10)

                # Create circle around this point
                circle_vertices = []
                for s in range(segments):
                    angle = 2 * np.pi * s / segments
                    # Calculate point on circle
                    circle_point = point + radius * np.cos(angle) * perp1 + radius * np.sin(angle) * perp2
                    vertices.append(circle_point)
                    circle_vertices.append(len(vertices) - 1)

                # Create faces (quads) between circles
                if t_idx > 0:
                    for s in range(segments):
                        # Get indices of vertices that form a quad
                        v1 = circle_vertices[s]
                        v2 = circle_vertices[(s + 1) % segments]
                        v3 = v2 - segments
                        v4 = v1 - segments

                        # Add two triangles to form a quad
                        faces.append([v1, v2, v3])
                        faces.append([v1, v3, v4])

        return np.array(vertices), np.array(faces)


def process_lsystem_geometry(l_system_output, initial_state=None, angle=30, segment_length=1.0):
    """Process L-system output to create 3D geometry using turtle graphics.

    Args:
        l_system_output: String of L-system symbols
        initial_state: Initial turtle state (optional)
        angle: Rotation angle in degrees
        segment_length: Length of F segments

    Returns:
        List of branches and final turtle state
    """
    if initial_state is None:
        initial_state = TurtleState(
            position=np.array([0, 0, 0]),
            heading=np.array([0, 0, 1]),
            left=np.array([-1, 0, 0]),
            up=np.array([0, 1, 0]),
            radius=0.5,
        )

    turtle = initial_state.copy()
    branches = []
    current_branch = Branch(turtle.position, turtle.radius, turtle.heading)
    branch_stack = []

    angle_rad = math.radians(angle)

    for symbol in l_system_output:
        if symbol == "F":
            # Move forward and add point to current branch
            turtle.position = turtle.position + turtle.heading * segment_length
            current_branch.add_point(turtle, segment_length)

        elif symbol == "+":
            # Rotate right around the up vector
            rotation_matrix = rotation_matrix_about_axis(turtle.up, angle_rad)
            turtle.heading = np.dot(rotation_matrix, turtle.heading)
            turtle.left = np.dot(rotation_matrix, turtle.left)
            # up vector remains unchanged

        elif symbol == "-":
            # Rotate left around the up vector
            rotation_matrix = rotation_matrix_about_axis(turtle.up, -angle_rad)
            turtle.heading = np.dot(rotation_matrix, turtle.heading)
            turtle.left = np.dot(rotation_matrix, turtle.left)
            # up vector remains unchanged

        elif symbol == "&":
            # Pitch down around the left vector
            rotation_matrix = rotation_matrix_about_axis(turtle.left, angle_rad)
            turtle.heading = np.dot(rotation_matrix, turtle.heading)
            turtle.up = np.dot(rotation_matrix, turtle.up)
            # left vector remains unchanged

        elif symbol == "^":
            # Pitch up around the left vector
            rotation_matrix = rotation_matrix_about_axis(turtle.left, -angle_rad)
            turtle.heading = np.dot(rotation_matrix, turtle.heading)
            turtle.up = np.dot(rotation_matrix, turtle.up)
            # left vector remains unchanged

        elif symbol == "\\":
            # Roll clockwise around the heading vector
            rotation_matrix = rotation_matrix_about_axis(turtle.heading, angle_rad)
            turtle.left = np.dot(rotation_matrix, turtle.left)
            turtle.up = np.dot(rotation_matrix, turtle.up)
            # heading vector remains unchanged

        elif symbol == "/":
            # Roll counter-clockwise around the heading vector
            rotation_matrix = rotation_matrix_about_axis(turtle.heading, -angle_rad)
            turtle.left = np.dot(rotation_matrix, turtle.left)
            turtle.up = np.dot(rotation_matrix, turtle.up)
            # heading vector remains unchanged

        elif symbol == "|":
            # Turn around (180 degrees)
            turtle.heading = -turtle.heading
            turtle.left = -turtle.left
            # up vector remains unchanged

        elif symbol == "[":
            # Push current state onto stack
            branch_stack.append((turtle.copy(), current_branch))
            # Start a new branch
            current_branch = Branch(turtle.position, turtle.radius, turtle.heading)

        elif symbol == "]":
            # Pop state from stack
            if branch_stack:
                branches.append(current_branch)
                turtle, current_branch = branch_stack.pop()
            else:
                print("Warning: Unmatched closing bracket in L-system")

        # You can add more symbols and their interpretations as needed

    # Add the final branch if not empty
    if current_branch.points:
        branches.append(current_branch)

    return branches, turtle


def rotation_matrix_about_axis(axis, angle):
    """Create a rotation matrix for rotation about an arbitrary axis.

    Args:
        axis: 3D vector specifying rotation axis
        angle: Rotation angle in radians

    Returns:
        3x3 rotation matrix
    """
    # Normalize the axis
    axis = axis / np.linalg.norm(axis)
    x, y, z = axis

    # Compute sine and cosine of angle
    c = np.cos(angle)
    s = np.sin(angle)
    t = 1 - c

    # Create the rotation matrix
    matrix = np.array([
        [t * x * x + c, t * x * y - s * z, t * x * z + s * y],
        [t * x * y + s * z, t * y * y + c, t * y * z - s * x],
        [t * x * z - s * y, t * y * z + s * x, t * z * z + c],
    ])

    return matrix


def create_tree_mesh(l_system_output, angle=30, segment_length=1.0):
    """Create a complete tree mesh from an L-system output.

    Args:
        l_system_output: String of L-system symbols
        angle: Rotation angle in degrees
        segment_length: Length of F segments

    Returns:
        vertices, faces
    """
    branches, _ = process_lsystem_geometry(l_system_output, angle=angle, segment_length=segment_length)

    all_vertices = []
    all_faces = []
    vertex_offset = 0

    for branch in branches:
        vertices, faces = branch.create_mesh()

        if len(vertices) > 0:
            # Adjust face indices based on existing vertices
            faces = faces + vertex_offset

            # Add to the collection
            all_vertices.extend(vertices)
            all_faces.extend(faces)

            # Update offset for next branch
            vertex_offset += len(vertices)

    return np.array(all_vertices), np.array(all_faces)


# Example usage:
def example():
    # Simple L-system rule for demonstration
    l_system = r"!//&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^!F/&![F^F%]!^\^]!^F/&![A]!^F/&![A]!^F/&![A]!^F/&![A]!^FQ"

    # Create mesh
    vertices, faces = create_tree_mesh(l_system, angle=25, segment_length=0.8)
    visualize_tree_mesh(vertices, faces)
    # At this point, you can export the mesh to a file or render it
    print(f"Generated mesh with {len(vertices)} vertices and {len(faces)} faces")

    # For visualization, you might want to use libraries like Panda3D, PyOpenGL, or output to a file format
    # Example of exporting to OBJ file:
    export_obj("tree.obj", vertices, faces)


def export_obj(filename, vertices, faces):
    """Export mesh to OBJ file format."""
    with open(filename, "w") as f:
        f.write("# OBJ file created by L-system tree generator\n")

        # Write vertices
        for v in vertices:
            f.write(f"v {v[0]} {v[1]} {v[2]}\n")

        # Write faces (OBJ uses 1-based indexing)
        for face in faces:
            f.write(f"f {face[0] + 1} {face[1] + 1} {face[2] + 1}\n")

    print(f"Mesh exported to {filename}")


def visualize_tree_mesh(vertices, faces):
    """Visualize the tree mesh using vedo instead of matplotlib"""

    # Create a mesh from vertices and faces
    mesh = vedo.Mesh([vertices, faces])

    # Set appearance
    mesh.color("green8").lighting("plastic").alpha(0.9)

    # Create a vedo plotter with a white background
    plotter = vedo.Plotter(title="3D Tree Mesh", bg="white")

    # Add the mesh to the plotter
    plotter.add(mesh)

    # Add axes for reference
    axes = vedo.Axes(mesh, xygrid=True)
    plotter.add(axes)

    # Display the mesh with interactive controls
    plotter.show()

    # Return the plotter in case it's needed elsewhere
    return plotter


if __name__ == "__main__":
    example()
