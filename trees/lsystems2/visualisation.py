
import math

import numpy as np
import vedo
from branch import Branch
from leaf import Leaf
from params.palm import palm


class VisualizeTree:
    def __init__(self, lsystem, leaf_scale_x, g_scale, g_scale_v):
        self.lsystem = lsystem
        self.current_string = self.lsystem.current_string
        self.thickness = self.lsystem.thickness
        self.tropism = self.lsystem.tropism
        self.bendiness = self.lsystem.bendiness
        self.leaf_shape = self.lsystem.leaf_shape
        self.leaf_scale = self.lsystem.leaf_scale
        self.leaf_bend = self.lsystem.leaf_bend

        self.leaf_scale_x = leaf_scale_x
        self.g_scale = g_scale
        self.g_scale_v = g_scale_v

        self.tree_scale = self.g_scale + self.g_scale_v

    def process_string(self):
        position = np.array([0, 0, 0])
        heading = np.array([0, 0, 1])
        left = np.array([1, 0, 0])
        up = np.array([0, 1, 0])
        stack = []
        width = self.thickness
        c = 0

        vertices = []
        edges = []
        thicknesses = []

        self.branches = []
        curr_branch = Branch(position, heading, left, up, width)

        self.leaves = []

        for sym in self.current_string:

            if "a" in sym.params:
                angle = sym.params["a"] * np.pi / 180

            if sym.name == "!":
                width = sym.params["w"]

            elif sym.name == 'F': # Draw forward

                position = position + heading * sym.params["l"]
                vertices.append(position.copy())
                edges.append((c, c + 1))
                thicknesses.append(width)
                if curr_branch == None:
                    curr_branch = Branch(position, heading, left, up, width)
                else:
                    curr_branch.add_point(position.copy(), heading.copy(), left.copy(), up.copy(), width)
                c += 1

                if "leaves" in sym.params:
                    p0 = position - heading * sym.params["l"]
                    p1 = position.copy()
                    eps = (p1 - p0) / sym.params["leaves"]
                    d = heading
                    for i in range(sym.params["leaves"]):
                        p = p0 + eps * i
                        leaf = Leaf(p.copy(), d.copy(), left.copy())
                        self.leaves.append(leaf)

            elif sym.name == '+': # Turn right
                heading, left = self.rotate(heading, left, up, angle)
            elif sym.name == '-': #Turn left
                heading, left = self.rotate(heading, left, up, -angle)
            elif sym.name == '&': #Pitch down
                heading, up = self.rotate(heading, up, left, -angle)
            elif sym.name == '^': #Pitch up
                heading, up = self.rotate(heading, up, left, angle)
            elif sym.name == '\\': #Roll left
                left, up = self.rotate(left, up, heading, angle)
            elif sym.name == '/': #Roll right
                left, up = self.rotate(left, up, heading, -angle)
            elif sym.name == '[': #start branching
                # Push state
                stack.append((position.copy(), heading.copy(), left.copy(), up.copy(), width, curr_branch.copy()))
                curr_branch = Branch(position, heading, left, up, width)
            elif sym.name == ']': # #end branching
                # Pop state
                position, heading, left, up, width, curr_branch = stack.pop()
                # Make the last entries the "correct" ones
                vertices.append(position.copy())
                thicknesses.append(width)
            elif sym.name == "L": #create leaf
                l = Leaf(position.copy(), heading.copy(), left.copy())
                self.leaves.append(l)
                vertices.append(position.copy())

            elif sym.name == "A" or sym.name == "%": #close branch

                if curr_branch != None:

                    self.branches.append(curr_branch.copy())
                curr_branch = None

            elif sym.name == "$": #reset orientation
                heading = np.array([0, 0, 1])
                left = np.array([1, 0, 0])
                up = np.array([0, 1, 0])

        if curr_branch != None:
            self.branches.append(curr_branch.copy())

        return vertices, edges, thicknesses

    def rotate(self, v1, v2, axis, angle):
        """Rotate v1 and v2 around axis by angle (in radians)"""
        # Normalize the axis
        axis = axis / np.linalg.norm(axis)
        # Calculate the rotation matrix using Rodrigues' rotation formula
        def rotate_vector(v):
            return (v * math.cos(angle) +
                    np.cross(axis, v) * math.sin(angle) +
                    axis * np.dot(axis, v) * (1 - math.cos(angle)))

        return rotate_vector(v1), rotate_vector(v2)

    def line_visualise(self):

        vertices, edges, thicknesses = self.process_string()
        # Create a vedo Line object
        lines = vedo.Line(vertices, edges)
        # # Create a vedo Plotter object
        plotter = vedo.Plotter()
        # Add the lines to the plotter
        plotter += lines
        # Show the plot
        plotter.show(lines, axes=1, viewup="z", interactive=True)

    def visualise(self, full_leaves=True, full_branches=True, line_leaves=False, line_branches=False):

        v_count = 0
        lines = vedo.Mesh([[],[]])
        leaf_vertices = []
        leaf_faces = []
        branch_vertices = []
        branch_faces = []
        lverts, lfaces = Leaf(0,0,0).get_shape(self.leaf_shape, self.tree_scale / self.g_scale, self.leaf_scale, self.leaf_scale_x)

        for leaf in self.leaves:
            verts, faces = leaf.get_mesh(self.leaf_bend, [lverts.copy(), lfaces.copy()])
            v_count += len(verts)
            if full_leaves:
                mesh = vedo.Mesh([verts, faces])
                lines += mesh
            if line_leaves:
                leaf_vertices.extend(verts.copy())
                leaf_faces.extend(faces.copy())

        if line_leaves:
            leaf_mesh = vedo.Line(leaf_vertices, leaf_faces)
            lines += leaf_mesh


        for branch in self.branches:
            verts, faces = branch.get_mesh()
            if verts is None or faces is None:
                continue
            v_count += len(verts)

            if full_branches:
                mesh = vedo.Mesh([verts, faces])
                lines += mesh
            if line_branches:
                branch_vertices.extend(verts)
                branch_faces.extend(faces)


        if line_branches:
            branch_mesh = vedo.Line(branch_vertices, branch_faces)
            lines += branch_mesh


        print(v_count)
        # Create a vedo Plotter object
        plotter = vedo.Plotter()
        # Show the plot
        plotter.show(lines, axes=1, viewup="z", interactive=True)
        # plotter.show(branch_mesh, axes=1, viewup="z", interactive=True)




#For palm
lsys = palm()
leaf_scale_x = 0.12
g_scale = 14
g_scale_v = 3

#For Lombardy Poplar
# lsys = lpoplar()
# leaf_scale_x = 1
# g_scale = 25
# g_scale_v = 5

#For Quaking Aspen
# lsys = qaspen()
# leaf_scale_x = 1
# g_scale = 13
# g_scale_v = 3


visualiser = VisualizeTree(lsys, leaf_scale_x, g_scale, g_scale_v)
visualiser.process_string()
# print(len(visualiser.branches))
visualiser.visualise(True, True, False, False)
