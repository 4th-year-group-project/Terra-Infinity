import math
from random import random

import numpy as np
from my_lsystem import LSymbol, LSystem

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


def lpoplar():
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
