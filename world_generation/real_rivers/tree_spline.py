import matplotlib.pyplot as plt
import networkx as nx
import numpy as np


class TreeSpline:
    def __init__(self, edges, centroids):
        self.edges = edges
        self.centroids = centroids
        self.G = nx.DiGraph()

        for parent, child in edges:
            self.G.add_edge(parent, child)

        if not nx.is_tree(self.G.to_undirected()):
            print("Warning: Input graph is not a tree!")

        self.root = [n for n, d in self.G.in_degree() if d == 0]
        if len(self.root) != 1:
            print(f"Warning: Found {len(self.root)} root nodes: {self.root}")
        self.root = self.root[0] if self.root else None

        self.paths = {}
        self.calculate_paths()

        self.splines = {}
        self.control_points = {}

        self.curviness = 0.5
        self.meander = 0.25

    def calculate_paths(self):
        leaves = [n for n, d in self.G.out_degree() if d == 0]
        for leaf in leaves:
            path = nx.shortest_path(self.G, self.root, leaf)
            self.paths[leaf] = path

    def compute_tangent_at_node(self, node):
        neighbors = list(self.G.predecessors(node)) + list(self.G.successors(node))
        if not neighbors:
            return np.array([0, 0])

        tangent = np.array([0.0, 0.0])
        node_pos = self.centroids[node]

        for neighbor in neighbors:
            neighbor_pos = self.centroids[neighbor]
            direction = neighbor_pos - node_pos
            norm = np.linalg.norm(direction)
            if norm > 0:
                direction = direction / norm
                if neighbor in self.G.predecessors(node):
                    direction = -direction
                tangent += direction

        norm = np.linalg.norm(tangent)
        if norm > 0:
            tangent = tangent / norm

        return tangent

    def calculate_control_points(self):
        for parent, child in self.edges:
            p0 = self.centroids[parent]
            p3 = self.centroids[child]

            t_parent = self.compute_tangent_at_node(parent)
            t_child = self.compute_tangent_at_node(child)

            dist = np.linalg.norm(p3 - p0)

            perp_parent = np.array([-t_parent[1], t_parent[0]])
            perp_child = np.array([-t_child[1], t_child[0]])

            p1 = p0 + t_parent * (dist * self.curviness) + perp_parent * (dist * self.meander)
            p2 = p3 - t_child * (dist * self.curviness) - perp_child * (dist * self.meander)

            self.control_points[(parent, child)] = (p0, p1, p2, p3)

    def get_bezier_points(self, control_points, num_points=20):
        p0, p1, p2, p3 = control_points
        t = np.linspace(0, 1, num_points).reshape(-1, 1)
        points = (1 - t) ** 3 * p0 + 3 * (1 - t) ** 2 * t * p1 + 3 * (1 - t) * t ** 2 * p2 + t ** 3 * p3
        return points

    def smooth_tree(self, curviness=None, meander=None):
        if curviness is not None:
            self.curviness = curviness
        if meander is not None:
            self.meander = meander
        self.calculate_control_points()

    def plot_tree(self, figsize=(10, 8), plot_original=True, plot_control_points=False):
        plt.figure(figsize=figsize)

        node_x = [self.centroids[i][0] for i in self.G.nodes()]
        node_y = [self.centroids[i][1] for i in self.G.nodes()]
        plt.scatter(node_x, node_y, c='blue', s=50, zorder=3)

        for node in self.G.nodes():
            plt.annotate(str(node), self.centroids[node], xytext=(5, 5), textcoords='offset points')

        if plot_original:
            for parent, child in self.edges:
                plt.plot(
                    [self.centroids[parent][0], self.centroids[child][0]],
                    [self.centroids[parent][1], self.centroids[child][1]],
                    'k--', alpha=0.3, zorder=1
                )

        for _edge, points in self.control_points.items():
            bezier_points = self.get_bezier_points(points)
            plt.plot(bezier_points[:, 0], bezier_points[:, 1], 'g-', linewidth=2, zorder=2)

            if plot_control_points:
                p0, p1, p2, p3 = points
                plt.plot([p0[0], p1[0]], [p0[1], p1[1]], 'r:', alpha=0.5)
                plt.plot([p2[0], p3[0]], [p2[1], p3[1]], 'r:', alpha=0.5)
                plt.scatter([p1[0], p2[0]], [p1[1], p2[1]], c='red', s=30, alpha=0.5)

        plt.title(f'Tree with River-like Curves (Curviness: {self.curviness}, Meander: {self.meander})')
        plt.grid(True, alpha=0.3)
        plt.axis('equal')
        plt.tight_layout()
        plt.show()

    def get_spline_points(self, resolution=100):
        return {
            edge: self.get_bezier_points(control_points, resolution)
            for edge, control_points in self.control_points.items()
        }

    def offset_curve(self, points, widths):
        left, right = [], []
        for i in range(len(points)):
            if i == 0:
                tangent = points[i + 1] - points[i]
            elif i == len(points) - 1:
                tangent = points[i] - points[i - 1]
            else:
                tangent = points[i + 1] - points[i - 1]

            tangent = tangent / np.linalg.norm(tangent)
            normal = np.array([-tangent[1], tangent[0]])
            w = widths[i]
            left.append(points[i] + normal * w)
            right.append(points[i] - normal * w)
        return np.array(left), np.array(right)
