import math
import random
import time
from collections import deque

import numpy as np

# For visualization
# import matplotlib.pyplot as plt
# from mpl_toolkits.mplot3d import Axes3D
# from mpl_toolkits.mplot3d.art3d import Poly3DCollection
# Fast visualization
import vedo


class StochasticLSystem:
    def __init__(self, axiom, rules, angle=25, thickness_scale=0.75, angle_variance=5):
        self.axiom = axiom
        self.rules = rules  # Now rules can be dict with lists of (production, probability) tuples
        self.angle = angle * math.pi / 180  # Convert to radians
        self.thickness_scale = thickness_scale
        self.angle_variance = angle_variance * math.pi / 180  # Variation in angles
        self.result = axiom

    def generate(self, iterations):
        """Generate the L-system string through iterations with stochastic rules"""
        result = self.axiom
        for _ in range(iterations):
            new_result = ""
            for char in result:
                if char in self.rules:
                    rule_options = self.rules[char]

                    # Handle both deterministic and stochastic rules
                    if isinstance(rule_options, str):
                        # Deterministic rule (string)
                        new_result += rule_options
                    else:
                        # Stochastic rule (list of tuples)
                        # Extract productions and their probabilities
                        productions = [prod for prod, _ in rule_options]
                        probabilities = [prob for _, prob in rule_options]
                        # Normalize probabilities if needed
                        if sum(probabilities) != 1.0:
                            probabilities = [p/sum(probabilities) for p in probabilities]
                        # Choose production based on probability
                        chosen_production = random.choices(productions, weights=probabilities, k=1)[0]
                        new_result += chosen_production
                else:
                    new_result += char
            result = new_result
        self.result = result
        return result

    def interpret_to_3d(self, initial_length=1.0, initial_thickness=0.1):
        """Interpret the L-system string as 3D turtle graphics commands
        Returns vertices and edges that represent the tree structure
        """
        # Use a stack to keep track of positions and orientations
        stack = deque()

        # Initial position and orientation vectors
        position = np.array([0, 0, 0])
        heading = np.array([0, 0, 1])  # Initially pointing up (z-axis)
        left = np.array([1, 0, 0])     # Initial left direction (x-axis)
        up = np.array([0, 1, 0])       # Initial up direction (y-axis)

        # Store vertices and edges
        vertices = [position.copy()]
        edges = []
        # Store branch thicknesses for each vertex
        thicknesses = [initial_thickness]

        length = initial_length
        thickness = initial_thickness

        # Process each character in the L-system result
        for char in self.result:
            if char == 'F':
                # Move forward and draw
                new_position = position + heading * length
                vertices.append(new_position.copy())
                edges.append((len(vertices) - 2, len(vertices) - 1))
                thicknesses.append(thickness)
                position = new_position
            elif char == 'f':
                # Move forward without drawing
                position = position + heading * length
            elif char == '+':
                # Turn right with randomized angle
                angle = self.angle + random.uniform(-self.angle_variance, self.angle_variance)
                heading, left = self._rotate(heading, left, up, angle)
            elif char == '-':
                # Turn left with randomized angle
                angle = self.angle + random.uniform(-self.angle_variance, self.angle_variance)
                heading, left = self._rotate(heading, left, up, -angle)
            elif char == '&':
                # Pitch down with randomized angle
                angle = self.angle + random.uniform(-self.angle_variance, self.angle_variance)
                heading, up = self._rotate(heading, up, left, -angle)
            elif char == '^':
                # Pitch up with randomized angle
                angle = self.angle + random.uniform(-self.angle_variance, self.angle_variance)
                heading, up = self._rotate(heading, up, left, angle)
            elif char == '\\':
                # Roll left with randomized angle
                angle = self.angle + random.uniform(-self.angle_variance, self.angle_variance)
                left, up = self._rotate(left, up, heading, angle)
            elif char == '/':
                # Roll right with randomized angle
                angle = self.angle + random.uniform(-self.angle_variance, self.angle_variance)
                left, up = self._rotate(left, up, heading, -angle)
            elif char == '|':
                # Turn around (180 degrees)
                heading = -heading
                left = -left
            elif char == '[':
                # Push state
                stack.append((position.copy(), heading.copy(), left.copy(), up.copy(), length, thickness))
            elif char == ']':
                # Pop state
                position, heading, left, up, length, thickness = stack.pop()
                vertices.append(position.copy())  # Add the position after returning to it
                thicknesses.append(thickness)  # Add the thickness for this position
            elif char == '!':
                # Decrease thickness
                thickness *= self.thickness_scale
            elif char == '"':
                # Decrease length
                length *= 0.75
            elif char == '\'':
                # Increase length
                length /= 0.75
            elif char == '~':
                # Add random variation to position (for more natural look)
                rand_vector = np.random.normal(0, 0.01, 3)
                position += rand_vector

        return vertices, edges, thicknesses

    def _rotate(self, v1, v2, axis, angle):
        """Rotate v1 and v2 around axis by angle (in radians)"""
        # Normalize the axis
        axis = axis / np.linalg.norm(axis)

        # Rodrigues' rotation formula
        def rotate_vector(v):
            return (v * math.cos(angle) +
                    np.cross(axis, v) * math.sin(angle) +
                    axis * np.dot(axis, v) * (1 - math.cos(angle)))

        return rotate_vector(v1), rotate_vector(v2)

def create_cylinder_mesh(start, end, radius_start, radius_end, segments=8):
    """Create a cylinder mesh between two points with varying radius"""
    # Direction vector
    direction = end - start
    length = np.linalg.norm(direction)
    if length < 1e-6:  # Avoid division by zero
        return None, None

    direction = direction / length

    # Create a basis for the cylinder's circular cross-section
    if abs(direction[0]) < 0.1 and abs(direction[1]) < 0.1:
        # If direction is close to z-axis, use x-axis
        perpendicular = np.array([1, 0, 0])
    else:
        # Otherwise, use z-axis
        perpendicular = np.array([0, 0, 1])

    # Find perpendicular vectors
    basis1 = np.cross(direction, perpendicular)
    basis1 = basis1 / np.linalg.norm(basis1)
    basis2 = np.cross(direction, basis1)

    # Create vertices for cylinder
    vertices = []
    indices = []

    # Create circle vertices at both ends
    for i in range(segments):
        angle = 2 * math.pi * i / segments
        cos_val = math.cos(angle)
        sin_val = math.sin(angle)

        # Start circle
        vertex_start = start + (basis1 * cos_val + basis2 * sin_val) * radius_start
        vertices.append(vertex_start)

        # End circle
        vertex_end = end + (basis1 * cos_val + basis2 * sin_val) * radius_end
        vertices.append(vertex_end)

    # Create triangles
    for i in range(segments):
        # Get indices
        i0 = i * 2
        i1 = (i * 2 + 2) % (segments * 2)
        i2 = i * 2 + 1
        i3 = (i * 2 + 3) % (segments * 2)

        # Add two triangles for each segment
        indices.append([i0, i1, i2])
        indices.append([i1, i3, i2])

    return vertices, indices

def create_tree_mesh(vertices, edges, thicknesses, segments=8):
    """Create a complete tree mesh from vertices, edges, and thicknesses"""
    all_vertices = []
    all_faces = []
    vertex_count = 0

    # Make sure we have valid edges and sufficient thicknesses
    for edge_idx, (start_idx, end_idx) in enumerate(edges):
        # Check if we have valid indices
        if start_idx >= len(vertices) or end_idx >= len(vertices):
            continue

        start_point = np.array(vertices[start_idx])
        end_point = np.array(vertices[end_idx])

        # Get thicknesses for this branch
        # Use safer approach to get thicknesses
        start_thickness = thicknesses[start_idx] if start_idx < len(thicknesses) else 0.01
        end_thickness = thicknesses[end_idx] if end_idx < len(thicknesses) else start_thickness * 0.75

        # Create cylinder mesh for this branch
        branch_vertices, branch_faces = create_cylinder_mesh(
            start_point, end_point, start_thickness, end_thickness, segments)

        if branch_vertices is not None and branch_faces is not None:
            # Add vertices and faces to the complete mesh
            all_vertices.extend(branch_vertices)
            for face in branch_faces:
                all_faces.append([f + vertex_count for f in face])
            vertex_count += len(branch_vertices)

    return all_vertices, all_faces

def save_obj_file(vertices, faces, filename="tree.obj"):
    """Save the mesh as a Wavefront OBJ file"""
    with open(filename, 'w') as f:
        # Write vertices
        for v in vertices:
            f.write(f"v {v[0]} {v[1]} {v[2]}\n")

        # Write faces (OBJ uses 1-based indexing)
        for face in faces:
            f.write(f"f {face[0]+1} {face[1]+1} {face[2]+1}\n")

    print(f"Mesh saved to {filename}")

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




#Matlpotlib too slow
# def visualize_tree_mesh_old(vertices, faces):
#     """Visualize the tree mesh using Matplotlib"""
#     fig = plt.figure(figsize=(10, 8))
#     ax = fig.add_subplot(111, projection='3d')

#     # Create a Poly3DCollection
#     poly3d = Poly3DCollection([[vertices[i] for i in face] for face in faces],
#                               alpha=0.7, edgecolor='k', linewidth=0.5)

#     # Add the collection to the plot
#     ax.add_collection3d(poly3d)

#     # Auto-scale to the mesh size
#     all_pts = np.array(vertices)
#     ax.auto_scale_xyz(all_pts[:, 0], all_pts[:, 1], all_pts[:, 2])

#     plt.title("3D Tree Mesh")
#     plt.tight_layout()
#     plt.show()

def save_seed_and_params(seed, rules, iterations, angle, filename="tree_params.txt"):
    """Save the random seed and parameters used to generate the tree"""
    with open(filename, 'w') as f:
        f.write(f"Random Seed: {seed}\n")
        f.write(f"Iterations: {iterations}\n")
        f.write(f"Angle: {angle}\n")
        f.write("Rules:\n")
        for key, value in rules.items():
            f.write(f"  {key} -> {value}\n")

    print(f"Parameters saved to {filename}")

def generate_stochastic_tree(seed=None, iterations=3, filename="stochastic_tree.obj"):
    """Generate a tree using stochastic L-system with the given parameters"""
    # Set random seed for reproducibility if provided
    if seed is not None:
        random.seed(seed)
        np.random.seed(seed)
    else:
        # Generate a random seed
        seed = random.randint(0, 10000)
        random.seed(seed)
        np.random.seed(seed)

    print(f"Using random seed: {seed}")

    t1 = time.time()

    # Define stochastic L-system rules for a tree
    axiom = "A"

    # Define stochastic rules
    # Format: {symbol: [(production1, probability1), (production2, probability2), ...]}
    rules = {
        "A": [
            ("F[&FL!A]/////[&FL!A]///////[&FL!A]", 0.6),
            ("F[&FL!A]/////[&FL!A]", 0.4)
        ],
        "F": [
            ("S/////F", 0.7),
            ("SF", 0.3)
        ],
        "S": "FL",
        "L": [
            ("['''∧∧{-f+f+f-|-f+f+f}]", 0.5),
            ("['''∧∧{-f+f+f}]", 0.5)
        ]
    }

    # Parameters
    angle = 22 + random.uniform(-5, 5)  # Base angle with some randomness
    thickness_scale = 0.7 + random.uniform(-0.1, 0.1)  # Random thickness scale
    angle_variance = 5  # Variance in angle for more natural look

    # Create and generate L-system
    lsystem = StochasticLSystem(
        axiom=axiom,
        rules=rules,
        angle=angle,
        thickness_scale=thickness_scale,
        angle_variance=angle_variance
    )
    lsystem.generate(iterations=iterations)

    # Save the parameters for reproducibility
    save_seed_and_params(seed, rules, iterations, angle)

    # Interpret L-system to 3D structure
    initial_length = 0.4 + random.uniform(-0.1, 0.1)
    initial_thickness = 0.08 + random.uniform(-0.02, 0.02)
    vertices, edges, thicknesses = lsystem.interpret_to_3d(
        initial_length=initial_length,
        initial_thickness=initial_thickness
    )

    # Ensure thicknesses list has the same length as vertices
    if len(thicknesses) < len(vertices):
        # Fill in missing thicknesses with decreasing values
        last_thickness = thicknesses[-1]
        for i in range(len(thicknesses), len(vertices)):
            last_thickness *= 0.9  # Gradually decrease thickness
            thicknesses.append(last_thickness)

    print(f"Vertices: {len(vertices)}, Edges: {len(edges)}, Thicknesses: {len(thicknesses)}")

    # Create 3D mesh
    segments = 8  # Number of segments for cylinder cross-section
    mesh_vertices, mesh_faces = create_tree_mesh(vertices, edges, thicknesses, segments=segments)

    if not mesh_vertices or not mesh_faces:
        print("Error: Failed to create mesh. Check your L-system rules and parameters.")
        return False

    print(f"Created mesh with {len(mesh_vertices)} vertices and {len(mesh_faces)} faces")

    # Save to OBJ file
    save_obj_file(mesh_vertices, mesh_faces, filename)

    t2 = time.time()

    print(f"Time for object creation: {t2 - t1:.6f} seconds")

    # Try to visualize (if matplotlib is available)
    try:
        visualize_tree_mesh(mesh_vertices, mesh_faces)
    except Exception as e:
        print(f"Visualization failed: {e}")
        print(f"The mesh has been saved to '{filename}' and can be viewed in any 3D viewer")

    return True



def main():
    seed = 42
    iterations = 4
    generate_stochastic_tree(seed, iterations)

if __name__ == "__main__":
    main()
