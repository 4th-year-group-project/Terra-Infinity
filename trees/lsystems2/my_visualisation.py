
import vedo
import math 
import numpy as np

from my_lsystem import LSystem, LSymbol
from lombardy_poplar import lpoplar
from quaking_aspen import qaspen 
from palm import palm

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

    def compute_bezier(self):
        # Compute the Bezier curve using the points in self.poss
        # This is a placeholder for the actual Bezier computation
        # You can use the De Casteljau's algorithm or any other method
        # to compute the Bezier curve from the control points.
        # For now, let's just return the points as a straight line
        return self.poss
    
    def copy(self):
        y = Branch(self.pos, self.heading, self.left, self.up, self.width)
        y.poss = self.poss.copy()
        y.headings = self.headings.copy()
        y.lefts = self.lefts.copy()
        y.ups = self.ups.copy()
        y.widths = self.widths.copy()
        return y
    
    def __str__(self):
        return f"Branch {self.pos}"
    

class Leaf: 
    def __init__(self, pos, direction, right): 
        self.pos = pos
        self.direction = direction
        self.right = right

    #TODO extend 
    def get_shape(): 
        g_scale = 25
        scale = 0.3 
        scale_x = 1
        verts, faces = [
                [0.005, 0, 0],
                [0.005, 0, 0.1],
                [0.15, 0, 0.15],
                [0.25, 0, 0.3],
                [0.2, 0, 0.6],
                [0, 0, 1],
                [-0.2, 0, 0.6],
                [-0.25, 0, 0.3],
                [-0.15, 0, 0.15],
                [-0.005, 0, 0.1],
                [-0.005, 0, 0]], [[0, 1, 9, 10], [1, 2, 3, 4], [4, 5, 6], [6, 7, 8, 9], [4, 6, 9, 1]]
        verts = np.array(verts)
        faces = np.array(faces)

        for v in verts: 
            v *= scale * g_scale 
            v[0] *= scale_x
        
        return verts, faces

    def get_mesh():
        pass 

    def calc_bend_trf():
        pass
    

class VisualizeTree:
    def __init__(self, lsystem):
        self.lsystem = lsystem
        self.current_string = self.lsystem.current_string 
        self.thickness = self.lsystem.thickness
        self.tropism = self.lsystem.tropism
        self.bendiness = self.lsystem.bendiness
        self.leaf_shape = self.lsystem.leaf_shape
        self.leaf_scale = self.lsystem.leaf_scale
        self.leaf_bend = self.lsystem.leaf_bend

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
                pass 
            elif sym.name == "A" or sym.name == "%": #close branch 
                self.branches.append(curr_branch.copy())
                curr_branch = None

            elif sym.name == "$": #reset orientation
                heading = np.array([0, 0, 1])
                left = np.array([1, 0, 0])
                up = np.array([0, 1, 0])
        
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

    def construct_bezier(self, p0, p1, p2, p3, t):
        pass


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
    
    def main_visualise(self): 

        vertices, edges, thicknesses = self.process_string()
        # Create a vedo Line object
        lines = vedo.Line(vertices, edges)
        # Create a vedo Plotter object
        plotter = vedo.Plotter()
        # Add the lines to the plotter
        plotter += lines
        # Show the plot
        plotter.show(lines, axes=1, viewup="z", interactive=True)


    
lsys = qaspen() 
visualiser = VisualizeTree(lsys)
visualiser.process_string()
# print(len(visualiser.branches))
visualiser.line_visualise()