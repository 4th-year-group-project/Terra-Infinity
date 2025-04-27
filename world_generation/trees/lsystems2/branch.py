import numpy as np

#Branch storage and mesh creation class
class Branch:
    def __init__(self, pos, heading, left, up, width):
        self.pos = pos
        self.heading = heading
        self.left = left
        self.up = up
        self.width = width
        self.poss = [pos]
        self.headings = [heading]
        self.lefts = [left]
        self.ups = [up]
        self.widths = [width]

    def add_point(self, pos, heading, left, up, width):
        self.poss.append(pos)
        self.headings.append(heading)
        self.lefts.append(left)
        self.ups.append(up)
        self.widths.append(width)

    def create_branch_mesh(self, points, radii, segments=8):
        """Create a tubular mesh for a branch.

            points: List of points along the branch centerline
            radii: List of radii at each point
            segments: Number of segments in the cross-section circle
        """
        if len(points) < 2:
            return None, None

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

        vertices = np.array(vertices)
        faces = np.array(faces)
        # Create the mesh
        return vertices, faces

    def copy(self):
        y = Branch(self.pos, self.heading, self.left, self.up, self.width)
        y.poss = self.poss.copy()
        y.headings = self.headings.copy()
        y.lefts = self.lefts.copy()
        y.ups = self.ups.copy()
        y.widths = self.widths.copy()
        return y

    def get_mesh(self):
        #Convert the widths to radii
        self.widths = [w / 2 for w in self.widths]
        return self.create_branch_mesh(self.poss, self.widths)

    def __str__(self):
        return f"Branch {self.pos}"
