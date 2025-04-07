import random
import math
import numpy as np
import vedo

# Simple wrapper for each symbol in the L-System
# We allow each symbol to be parameterized 
class LSymbol: 
    def __init__(self, name, params = None): 
        self.name = name
        self.params = params if params else {}
    
    def __str__(self):
        return self.name


# Main L-System class
class LSystem:
    def __init__(self, axiom, rules, 
        tropism =  np.array([0 , 0 , 0]),
        thickness = 0.5,
        bendiness = 0,
        leaf_shape = 0,
        leaf_scale = 0.3,
        leaf_bend = 0.7):
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
    
    #Apply one iteration of our rules
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
    
    #Apply multiple iterations of our rules
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
        c = 0

        vertices = []
        edges = []
        thicknesses = []

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
                stack.append((position.copy(), heading.copy(), left.copy(), up.copy(), width))
            elif sym.name == ']': # #end branching
                # Pop state
                position, heading, left, up, width = stack.pop()
                # Make the last entries the "correct" ones
                vertices.append(position.copy())
                thicknesses.append(width) 
            elif sym.name == "L":
                #create leaf
                pass 
            elif sym.name == "A" or sym.name == "%":
                #close branch 
                pass 
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
    

    def visualise(self): 
 

        vertices, edges, thicknesses = self.process_string()
        # Create a vedo Line object
        lines = vedo.Line(vertices, edges)
        # # Create a vedo Plotter object
        plotter = vedo.Plotter()
        # Add the lines to the plotter
        plotter += lines
        # Show the plot
        plotter.show(lines, axes=1, viewup="z", interactive=True)


    

#Example Code

# def A_prod(sym): 
#     return [LSymbol("A", {"x": sym.params["x"] * 2}), LSymbol("B", {"y": 0})]

# def B_prod(sym):
#     res = [sym]
#     if sym.params["y"] <= 3: 
#         if random. random() <= 0.3:
#             res = [LSymbol("B", {"y": sym.params["y"] - 1})]
#         else:
#             res = [LSymbol("B", {"y": sym.params["y"] + 3})]
#     elif sym.params["y"] > 4:
#         res = [LSymbol("B", {"y": sym.params["y"]}), LSymbol("A", {"x": sym.params["y"]})] 
#     return res

# sys = LSystem(axiom = [LSymbol("A", {"x": 1})], rules = {"A": A_prod, "B": B_prod})
# sys.generate(5)
# print(sys)