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