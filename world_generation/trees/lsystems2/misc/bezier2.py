import math
import random

import numpy as np
import vedo


# Simple wrapper for each symbol in the L-System
# We allow each symbol to be parameterized
class LSymbol:
    def __init__(self, name, params=None):
        self.name = name
        self.params = params if params else {}

    def __str__(self):
        return self.name


# Main L-System class
class LSystem:
    def __init__(
        self,
        axiom,
        rules,
        tropism=np.array([0, 0, 0]),
        thickness=0.5,
        bendiness=0,
        leaf_shape=0,
        leaf_scale=0.3,
        leaf_bend=0.7,
    ):
        # We store our "strings" as list of LSymbol objects
        # Rules is a dictionary from sym.name to arbitrary python functions
        self.axiom = axiom
        self.rules = rules
        self.current_string = axiom
        self.tropism = tropism
        self.thickness = thickness
        self.bendiness = bendiness
        self.leaf_shape = leaf_shape
        self.leaf_scale = leaf_scale
        self.leaf_bend = leaf_bend

    # Apply one iteration of our rules
    def apply_rules(self):
        new_string = []
        for sym in self.current_string:
            if sym.name in self.rules:
                rule = self.rules[sym.name]
                result = rule(sym)
                new_string += result
            else:
                new_string.append(sym)

        self.current_string = new_string.copy()
        return self.current_string

    # Apply multiple iterations of our rules
    def generate(self, iterations):
        for _ in range(iterations):
            self.apply_rules()
        return self.current_string

    def __str__(self):
        return "".join([str(sym) for sym in self.current_string])

    def process_string(self):
        position = np.array([0, 0, 0])
        heading = np.array([0, 0, 1])
        left = np.array([1, 0, 0])
        up = np.array([0, 1, 0])
        stack = []
        width = self.thickness

        # Store all branches separately
        branches = []
        current_branch = []
        current_branch_directions = []
        current_branch_thicknesses = []

        for sym in self.current_string:
            if "a" in sym.params:
                angle = sym.params["a"] * np.pi / 180

            if sym.name == "!":
                width = sym.params["w"]

            elif sym.name == "F":  # Draw forward
                # Store current position
                current_branch.append(position.copy())
                current_branch_directions.append(heading.copy())
                current_branch_thicknesses.append(width)

                # Move forward
                position = position + heading * sym.params["l"]

                # Add new position to branch
                current_branch.append(position.copy())
                current_branch_directions.append(heading.copy())
                current_branch_thicknesses.append(width * 0.98)  # Slight taper

                # Apply tropism
                if np.linalg.norm(self.tropism) > 0:
                    heading = heading + self.tropism * 0.1
                    heading = heading / np.linalg.norm(heading)

                # Update perpendicular vectors to maintain orthogonality
                left = np.cross(up, heading)
                left = left / np.linalg.norm(left)
                up = np.cross(heading, left)
                up = up / np.linalg.norm(up)

            elif sym.name == "+":  # Turn right
                heading, left = self.rotate(heading, left, up, angle)
            elif sym.name == "-":  # Turn left
                heading, left = self.rotate(heading, left, up, -angle)
            elif sym.name == "&":  # Pitch down
                heading, up = self.rotate(heading, up, left, -angle)
            elif sym.name == "^":  # Pitch up
                heading, up = self.rotate(heading, up, left, angle)
            elif sym.name == "\\":  # Roll left
                left, up = self.rotate(left, up, heading, angle)
            elif sym.name == "/":  # Roll right
                left, up = self.rotate(left, up, heading, -angle)
            elif sym.name == "[":  # Start branching
                # Push state
                stack.append((
                    position.copy(),
                    heading.copy(),
                    left.copy(),
                    up.copy(),
                    width,
                    current_branch.copy(),
                    current_branch_directions.copy(),
                    current_branch_thicknesses.copy(),
                ))

                # Start new branch
                if len(current_branch) > 0:
                    branches.append((current_branch, current_branch_directions, current_branch_thicknesses))

                current_branch = [position.copy()]
                current_branch_directions = [heading.copy()]
                current_branch_thicknesses = [width]

            elif sym.name == "]":  # End branching
                # Store current branch if it has points
                if len(current_branch) > 1:
                    branches.append((current_branch, current_branch_directions, current_branch_thicknesses))

                # Pop state
                (
                    position,
                    heading,
                    left,
                    up,
                    width,
                    current_branch,
                    current_branch_directions,
                    current_branch_thicknesses,
                ) = stack.pop()

            elif sym.name == "L":
                # Create leaf (placeholder)
                pass

            elif sym.name == "A" or sym.name == "%":
                # Close branch
                if len(current_branch) > 1:
                    branches.append((current_branch, current_branch_directions, current_branch_thicknesses))
                current_branch = [position.copy()]
                current_branch_directions = [heading.copy()]
                current_branch_thicknesses = [width]

            elif sym.name == "$":  # Reset orientation
                heading = np.array([0, 0, 1])
                left = np.array([1, 0, 0])
                up = np.array([0, 1, 0])

        # Add final branch if not empty
        if len(current_branch) > 1:
            branches.append((current_branch, current_branch_directions, current_branch_thicknesses))

        return branches

    def rotate(self, v1, v2, axis, angle):
        """Rotate v1 and v2 around axis by angle (in radians)"""
        # Normalize the axis
        axis = axis / np.linalg.norm(axis)

        # Calculate the rotation matrix using Rodrigues' rotation formula
        def rotate_vector(v):
            return (
                v * math.cos(angle)
                + np.cross(axis, v) * math.sin(angle)
                + axis * np.dot(axis, v) * (1 - math.cos(angle))
            )

        return rotate_vector(v1), rotate_vector(v2)

    def construct_bezier(self, points, directions, thicknesses, resolution=10):
        """Construct a Bezier curve from a series of points and directions.

        Args:
            points: List of 3D points
            directions: List of direction vectors at each point
            thicknesses: List of thicknesses at each point
            resolution: Number of segments between each pair of control points

        Returns:
            vertices: List of points along the Bezier curve
            radii: List of radii at each point
        """
        if len(points) < 2:
            return [], []

        # Create Bezier control points
        bezier_points = []
        bezier_radii = []

        # Create a sequence of cubic Bezier curves (with shared endpoints)
        for i in range(len(points) - 1):
            p0 = points[i]
            p3 = points[i + 1]

            # Calculate direction at each end point
            dir0 = directions[i]
            dir1 = directions[min(i + 1, len(directions) - 1)]

            # Calculate distance between points for scaling control points
            distance = np.linalg.norm(p3 - p0)

            # Control point scaling based on distance and bendiness
            ctrl_scale = distance * (0.33 + self.bendiness * 0.1)

            # Add random variation to control points for natural-looking curves
            variation = self.bendiness * np.random.uniform(-0.2, 0.2, 3)

            # Calculate control points
            p1 = p0 + dir0 * ctrl_scale + variation
            p2 = p3 - dir1 * ctrl_scale + variation

            # Generate points along the curve
            for t_idx in range(resolution):
                t = t_idx / resolution

                # Cubic Bezier formula: B(t) = (1-t)^3 * P0 + 3(1-t)^2 * t * P1 + 3(1-t) * t^2 * P2 + t^3 * P3
                t_inv = 1 - t
                point = t_inv**3 * p0 + 3 * t_inv**2 * t * p1 + 3 * t_inv * t**2 * p2 + t**3 * p3

                # Linear interpolation for thickness
                radius = t_inv * thicknesses[i] + t * thicknesses[min(i + 1, len(thicknesses) - 1)]

                bezier_points.append(point)
                bezier_radii.append(radius)

        # Add the last point
        bezier_points.append(points[-1])
        bezier_radii.append(thicknesses[-1])

        return bezier_points, bezier_radii

    def create_branch_mesh(self, points, radii, segments=8):
        """Create a tubular mesh for a branch.

        Args:
            points: List of points along the branch centerline
            radii: List of radii at each point
            segments: Number of segments in the cross-section circle
        Returns:
            vedo.Mesh: A vedo mesh representing the branch
        """
        if len(points) < 2:
            return None

        # Convert points to numpy array
        points = np.array(points)

        # Create a list to store vertices and faces
        vertices = []
        faces = []

        # Generate tangent vectors along the curve
        tangents = np.zeros_like(points)
        tangents[0] = points[1] - points[0]
        tangents[-1] = points[-1] - points[-2]

        for i in range(1, len(points) - 1):
            tangents[i] = (points[i + 1] - points[i - 1]) / 2

        # Normalize tangents
        for i in range(len(tangents)):
            norm = np.linalg.norm(tangents[i])
            if norm > 0:
                tangents[i] /= norm

        # Create initial normal vector
        initial_normal = np.array([0, 1, 0])
        if abs(np.dot(tangents[0], initial_normal)) > 0.9:
            initial_normal = np.array([1, 0, 0])

        # Make it perpendicular to tangent
        normal = initial_normal - np.dot(initial_normal, tangents[0]) * tangents[0]
        normal = normal / np.linalg.norm(normal)

        # Create cross section circles along the curve
        for i, (point, radius) in enumerate(zip(points, radii, strict=False)):
            # Update normal to remain perpendicular to tangent
            normal = normal - np.dot(normal, tangents[i]) * tangents[i]
            normal = normal / np.linalg.norm(normal)

            # Create binormal
            binormal = np.cross(tangents[i], normal)
            binormal = binormal / np.linalg.norm(binormal)

            # Update normal to ensure orthogonality
            normal = np.cross(binormal, tangents[i])

            # Create circle
            for j in range(segments):
                angle = 2 * np.pi * j / segments
                # Calculate circle point
                circle_point = point + radius * (np.cos(angle) * normal + np.sin(angle) * binormal)
                vertices.append(circle_point)

        # Create faces (triangles)
        for i in range(len(points) - 1):
            for j in range(segments):
                # Current and next point indices
                idx1 = i * segments + j
                idx2 = i * segments + (j + 1) % segments
                idx3 = (i + 1) * segments + (j + 1) % segments
                idx4 = (i + 1) * segments + j

                # Add two triangles to form a quad
                faces.append([idx1, idx2, idx3])
                faces.append([idx1, idx3, idx4])

        # Create end caps
        # Base cap
        center_idx = len(vertices)
        vertices.append(points[0])
        for j in range(segments):
            idx1 = j
            idx2 = (j + 1) % segments
            faces.append([center_idx, idx1, idx2])

        # Tip cap
        center_idx = len(vertices)
        vertices.append(points[-1])
        base_idx = (len(points) - 1) * segments
        for j in range(segments):
            idx1 = base_idx + j
            idx2 = base_idx + (j + 1) % segments
            faces.append([center_idx, idx2, idx1])

        # Create the mesh
        return vedo.Mesh([vertices, faces])

    def visualise(self):
        """Create and visualize the L-system using Bezier curves and vedo."""
        # Process the L-system to get branches
        branches = self.process_string()

        # Create a plotter
        plotter = vedo.Plotter()

        # Generate meshes for each branch
        for branch_points, branch_dirs, branch_thicknesses in branches:
            if len(branch_points) > 1:
                # Create Bezier curve from branch points
                bezier_points, bezier_radii = self.construct_bezier(branch_points, branch_dirs, branch_thicknesses)

                # Create mesh for the branch
                branch_mesh = self.create_branch_mesh(bezier_points, bezier_radii)

                if branch_mesh:
                    plotter += branch_mesh

        # Add axes and show the plot
        plotter.show(axes=1, viewup="z", interactive=True)


###Lombardy Poplar Tree
__iterations__ = 10.0
__base_width__ = 0.7


def q_prod(sym):
    ret = []
    prev_ang = 0
    for _ in range(int(random() * 2 + 3)):
        ang = random() * 10 + 30
        ret.extend([
            LSymbol("/", {"a": prev_ang + 75 + random() * 10}),
            LSymbol("&", {"a": ang}),
            LSymbol("!", {"w": sym.params["w"] * 0.2}),
            LSymbol("["),
            LSymbol("A", {"w": sym.params["w"] * 0.3, "l": 1.5 * math.sqrt(sym.params["w"]) * (random() * 0.2 + 0.9)}),
            LSymbol("]"),
            LSymbol("!", {"w": sym.params["w"]}),
            LSymbol("^", {"a": ang}),
            LSymbol("F", {"l": sym.params["l"]}),
        ])
    ret.append(LSymbol("Q", {"w": max(0, sym.params["w"] - __base_width__ / 14), "l": sym.params["l"]}))
    return ret


def a_prod(sym):
    ret = []
    n = int(random() * 5 + 22.5)
    w_d = sym.params["w"] / (n - 1)
    prev_rot = 0
    for ind in range(n):
        wid = sym.params["w"] - ind * w_d
        ang = random() * 10 + 25
        ret.extend([
            LSymbol("!", {"w": wid}),
            LSymbol("F", {"l": sym.params["l"] / 3}),
            LSymbol("/", {"a": prev_rot + 140}),
            LSymbol("&", {"a": ang}),
            LSymbol("!", {"w": wid * 0.3}),
            LSymbol("["),
            LSymbol(
                "F",
                {
                    "l": 0.75 * math.sqrt(n - ind) * sym.params["l"] / 3,
                    "leaves": 25,
                    "leaf_d_ang": 40,
                    "leaf_r_ang": 140,
                },
            ),
            LSymbol("^", {"a": 20}),
            LSymbol(
                "F",
                {
                    "l": 0.75 * math.sqrt(n - ind) * sym.params["l"] / 3,
                    "leaves": 25,
                    "leaf_d_ang": 40,
                    "leaf_r_ang": 140,
                },
            ),
            LSymbol("%"),
            LSymbol("]"),
            LSymbol("!", {"w": wid}),
            LSymbol("^", {"a": ang}),
            LSymbol("\\", {"a": prev_rot + 140}),
            LSymbol("^", {"a": 1.2}),
        ])
        prev_rot += 140
    return ret


def system():
    """Initialize and iterate the system as appropriate"""
    l_sys = LSystem(
        axiom=[
            LSymbol("!", {"w": __base_width__}),
            LSymbol("/", {"a": 45}),
            LSymbol("Q", {"w": __base_width__, "l": 0.5}),
        ],
        rules={"Q": q_prod, "A": a_prod},
        tropism=np.array([0, 0, 0]),
        thickness=0.5,
        bendiness=0,
        leaf_shape=0,
        leaf_scale=0.3,
        leaf_bend=0.7,
    )
    l_sys.generate(15)
    return l_sys


# Example: Create and visualize a tree
if __name__ == "__main__":
    # Create the L-system
    lsys = system()
    lsys.visualise()
