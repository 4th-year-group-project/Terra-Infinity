# # import pygame as pg ##for visuals
# import random
# import numpy as np 
# import numba as nb ##for speed
# import matplotlib as mpl ##for colors

# originally from https://runestone.academy/ns/books/published/thinkcspy/Strings/TurtlesandStringsandLSystems.html

import turtle
import random

def createLSystem(numIters,axiom):
    startString = axiom
    endString = ""
    for i in range(numIters):
        endString = processString(startString)
        startString = endString

    return endString

def processString(oldStr):
    newstr = ""
    for ch in oldStr:
        newstr = newstr + applyRules_new(ch)

    return newstr

def applyRules_c(ch):
    newstr = ""
    if ch == 'F':
        newstr = 'FF-[-F+F+F]+[+F-F-F]'   # Rule 1    
    else:
        newstr = ch    # no rules apply so keep the character

    return newstr

def applyRules_f(ch):
    newstr = ""
    if ch == "X":
        newstr = "F-[[X]+X]+F[+FX]-X"
    elif ch == "F":
        newstr = "FF"
    else:
        newstr = ch

    return newstr

def applyRules_new(ch):
    newstr = ""
    if ch == "X" and random.random() < 0.5:
        newstr = "F-[[X]+X]+F[+FX]-X"
    elif ch == "X":
        newstr = "F[+X]F[-X]+X"
    elif ch == "F":
        newstr = "FF"
    else:
        newstr = ch

    return newstr


def drawLsystem(aTurtle, instructions, angle, distance):
    stack = []
    for cmd in instructions:
        angle = random.uniform(20, 30)
        if cmd == "[":
            #Store turtle position
            stack.append((aTurtle.position(), aTurtle.heading())) 
        elif cmd == "]":
            #Retrieve turtle position
            aTurtle.up()
            position, heading = stack.pop()
            aTurtle.goto(position)
            aTurtle.setheading(heading)
            aTurtle.down()
        if cmd == 'F':
            aTurtle.forward(distance)
        elif cmd == 'B':
            aTurtle.backward(distance)
        elif cmd == '+':
            aTurtle.right(angle)
        elif cmd == '-':
            aTurtle.left(angle)

def main():
    inst = createLSystem(5, "X")   # create the string
    print(len(inst))
    t = turtle.Turtle()            # create the turtle
    wn = turtle.Screen()
    t.left(90)
    t.up()
    t.back(400)
    t.down()
    t.speed(0)
    drawLsystem(t, inst, 22.5, 6)   # draw the picture
                                  # angle 60, segment length 5
    wn.exitonclick()

main()
