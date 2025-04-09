from my_lsystem import LSystem, LSymbol
from random import random
import math
import numpy as np


#Palm tree
__d_t__ = 4
__t_max__ = 350 
__p_max__ = 0.93

def q_prod(sym): 
    prop_off = sym.params["t"] / __t_max__ 
    if prop_off < 1:
        res = [LSymbol("!", {"w": 0.85 + 0.15 * math.sin(sym.params["t"])}), LSymbol("^", {"a": random() - 0.65})]

        if prop_off > __p_max__: 
            d_ang = 1 / (1 - __p_max__) * (1 - prop_off) * 110 + 15
            res.extend([LSymbol("!", {"w": 0.1})])
            for ind in range(int(random() * 2 + 5)): 
                r_ang = sym.params["t"] * 10 + ind * (random() * 50 + 40)
                e_d_ang = d_ang * (random() * 0.4 + 0.8)
                res.extend([
                    LSymbol("/", {"a": r_ang}),
                    LSymbol("&", {"a": e_d_ang}), 
                    LSymbol("["), 
                    LSymbol("A"),
                    LSymbol("]"), 
                    LSymbol("^", {"a": e_d_ang}), 
                    LSymbol("\\", {"a": r_ang})])
            res.append(LSymbol("F", {"l": 0.05}))
        else:
            res.append(LSymbol("F", {"l": 0.15}))
        res.append(LSymbol("Q", {"t": sym.params["t"] + __d_t__}))
    else:
        res = [LSymbol("!", {"w": 0}), LSymbol("F", {"l": 0.15})]
    
    return res

def a_prod(sym):
    res =[] 
    num = int(random() * 5 + 30)
    for ind in range(num): 
        d_ang = (num - 1 - ind) * (80 / num) 
        res.extend([
            LSymbol("!", {"w":0.1 - ind * 0.1 / 15}),
            LSymbol("F", {"l": 0.1}),
            LSymbol("L", {"r_ang": 50 * (random() * 0.4 + 0.8), "d_ang": d_ang * (random() * 0.4 + 0.8)}),
            LSymbol("L", {"r_ang": -50 * (random() * 0.4 + 0.8), "d_ang": d_ang * (random() * 0.4 + 0.8)}),
            LSymbol("&", {"a": 1})
            ])
    return res

def palm(): 
    l_sys = LSystem(axiom = [LSymbol("!", {"w":0.2}), 
                             LSymbol("/", {"a": random() * 360}),
                             LSymbol("Q", {"t": 0})],
                    rules = {"Q": q_prod, "A": a_prod},
                    tropism = np.array([0, 0, -1]), 
                    thickness = 0.2,
                    bendiness = 0,
                    leaf_shape = 10, #ignored leaf_scale_x=0.1!!
                    leaf_scale = 1,
                    leaf_bend = 0) 
    l_sys.generate(100)
    return l_sys 
                        