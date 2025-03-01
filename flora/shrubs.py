import math
import random
import turtle
import tkinter as tk


axiom = 'Y'
num_iters = 3

rules = {'A' : (['A', 0.3], ['A[+A][-A]', 0.1], ['AA', 0.25], ['A[+A]', 0.15], ['A[-A]', 0.15])}
rules = {'X': (['X[-FFF][+FFF]FX', 1]), 'Y' : (['YFX[+Y][-Y]', 1]), 'F' : (['F', 1])}

base_angle = 25.7
angle_variation = 0.5

def apply_rules(c):
    #choices, probabilities = zip(*rules.get(c, [(c, 1)]))
    if c not in rules:
        return c
    #print('C:', c)
    choices = rules.get(c)[0]
    probabilities = rules.get(c)[1]
    #print(choices, probabilities)
    #return random.choices(choices, probabilities)[0]
    return choices

def process_string(old_str):
    new_str = ''
    for c in old_str:
        new_str = new_str + apply_rules(c)
    return new_str                        

def create_l_system(num_iters, axiom):
    print("Axiom:", axiom)
    start_string = axiom
    end_string = ''
    for i in range(num_iters):
        print(i)
        end_string = process_string(start_string)
        start_string = end_string
    print(end_string)
    return end_string   

def draw_l_system(inst, base_angle, distance):
    stack = []
    for cmd in inst:
        if cmd == 'F' or cmd == 'X' or cmd == 'Y':
            turtle.forward(distance)
        elif cmd == '+':
            angle = base_angle + random.uniform(-angle_variation, angle_variation)
            turtle.right(angle)
        elif cmd == '-':
            angle = base_angle + random.uniform(-angle_variation, angle_variation)
            turtle.left(angle)
        elif cmd == '[':
            stack.append((turtle.position(), turtle.heading()))
        elif cmd == ']':
            position, heading = stack.pop()
            turtle.penup()
            turtle.setposition(position)
            turtle.setheading(heading)
            turtle.pendown()

def main():
    print("L-system Visualization")
    turtle.setup(800, 600)
    turtle.title("L-system Visualization")
    turtle.bgcolor("black")
    turtle.color("white")
    turtle.speed(0)
    turtle.hideturtle()
    turtle.tracer(0, 0)

    print("Generating L-system...")
    l_system = create_l_system(num_iters, axiom)


    turtle.penup()
    turtle.goto(0, -200)
    turtle.left(90)
    turtle.pendown()

    draw_l_system(l_system, base_angle, 10)

    turtle.update()

    turtle.done()


if __name__ == "__main__":
    main()