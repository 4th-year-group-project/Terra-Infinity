import numpy as np


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
                point = (t_inv**3 * p0 +
                         3 * t_inv**2 * t * p1 +
                         3 * t_inv * t**2 * p2 +
                         t**3 * p3)

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
        self.widths = [w / 2 for w in self.widths]
        return self.create_branch_mesh(self.poss, self.widths)

    def __str__(self):
        return f"Branch {self.pos}"
