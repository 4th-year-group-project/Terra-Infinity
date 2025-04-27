import math
from random import random

import numpy as np
from my_lsystem import LSymbol, LSystem

## Quaking Aspen Tree
__base_width__ = 0.3
__base_length__ = 4


def q_prod(sym):
    ret = []
    prev_ang = 0
    for ind in range(8):
        offset = 1 - (__base_width__ - sym.params["w"]) / __base_width__
        offset += ind / 8 / 12
        dang = 30 * 85 * offset
        if offset <= 0.7:
            b_len = 0.4 + 0.6 * offset / 0.7
        else:
            b_len = 0.4 + 0.6 * (1 - offset) / 0.3
        ret.extend([
            LSymbol("/", {"a": prev_ang + 75 + random() * 10}),
            LSymbol("&", {"a": dang}),
            LSymbol("!", {"w": sym.params["w"] * 0.08 * b_len}),
            LSymbol("["),
            LSymbol("F", {"l": sym.params["w"] / 2}),
            LSymbol("A", {"w": sym.params["w"] * 0.08, "l": b_len * 0.6}),
            LSymbol("]"),
            LSymbol("!", {"w": sym.params["w"]}),
            LSymbol("^", {"a": dang}),
            LSymbol("F", {"l": sym.params["l"]}),
        ])
    ret.append(LSymbol("Q", {"w": max(0, sym.params["w"] - __base_width__ / 11), "l": sym.params["l"]}))
    return ret


def a_prod(sym):
    ret = []
    w_d = sym.params["w"] / 14
    prev_rot = 0
    num = int(random() * 3 + 15.5)
    for ind in range(num):
        wid = sym.params["w"] - ind * w_d
        l_count = int((math.sqrt(num - ind) + 2) * 4 * sym.params["l"])
        ret.extend([
            LSymbol("!", {"w": wid}),
            LSymbol("F", {"l": sym.params["l"] / 3}),
            LSymbol("/", {"a": prev_rot + 140}),
            LSymbol("&", {"a": 60}),
            LSymbol("!", {"w": wid * 0.4}),
            LSymbol("["),
            LSymbol(
                "F",
                {
                    "l": math.sqrt(num - ind) * sym.params["l"] / 3,
                    "leaves": l_count,
                    "leaf_d_ang": 40,
                    "leaf_r_ang": 140,
                },
            ),
            LSymbol("^", {"a": random() * 30 + 30}),
            LSymbol(
                "F",
                {
                    "l": math.sqrt(num - ind) * sym.params["l"] / 4,
                    "leaves": l_count,
                    "leaf_d_ang": 40,
                    "leaf_r_ang": 140,
                },
            ),
            LSymbol("%"),
            LSymbol("]"),
            LSymbol("!", {"w": wid}),
            LSymbol("^", {"a": 60}),
            LSymbol("\\", {"a": prev_rot + 140}),
            LSymbol("+", {"a": -5 + random() * 10}),
            LSymbol("^", {"a": -7.5 + random() * 15}),
        ])

        prev_rot += 140

    ret.append(LSymbol("F", {"l": sym.params["l"] / 2}))
    return ret


def qaspen():
    axiom = []
    con = int(__base_length__ / 0.1)
    s = random() * 0.2 + 0.9
    for ind in range(con):
        axiom.append(LSymbol("!", {"w": s * (__base_width__ + ((con - ind) / con) ** 6 * 0.2)}))
        axiom.append(LSymbol("F", {"l": s * 0.1}))
        axiom.append(LSymbol("Q", {"w": s * __base_width__, "l": s * 0.1}))
        l_sys = LSystem(
            axiom=axiom,
            rules={"Q": q_prod, "A": a_prod},
            tropism=np.array([0, 0, 0.2]),
            thickness=0.5,
            bendiness=0,
            leaf_shape=3,
            leaf_scale=0.17,
            leaf_bend=0.2,
        )
        l_sys.generate(12)
        return l_sys
