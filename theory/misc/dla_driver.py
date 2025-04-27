##A driver file which runs a C program that generates a DLA cluster and then plots it using matplotlib

import os
import pygame as pg ##for visuals
import random
import numpy as np 
import matplotlib as mpl ##for colors
import time

t1 = time.time() 


##Run the C program
os.system("./dla_approx")

t2 = time.time()



#######################
###Produce the fractal
#######################



n_particles = 100_000
grid_size = 2 ** 10

##Load the data
with open("positions.txt", "r") as f:
    data = f.readlines()
    arrivals = np.zeros((n_particles, 2), dtype=np.int32)
    for i, line in enumerate(data):
        x, y = line.split()
        arrivals[i- 1] = (int(x), int(y))



##Set up the color map
##It will let us color the particles based on arrival time
x = np.linspace(0.0, 1.0, n_particles)
nice_colors = mpl.colormaps['plasma'](x)[np.newaxis, :, :3]


##Set up pygame
pg.init()
screen = pg.display.set_mode((grid_size, grid_size))
running = True

##Draw the particles with the colors
for i in range(n_particles):
    x, y = arrivals[i] 
    color = 255 * nice_colors[0][i]
    screen.set_at((x, y), color[:])

pg.display.flip()
#remove the seed
screen.set_at((grid_size // 2, grid_size // 2), (0, 0, 0))

pg.image.save(screen, "fractal_new.png")

t3 = time.time()

##Run the game loop (very simple)
while running:
    for event in pg.event.get():
        if event.type == pg.QUIT:
            running = False
pg.quit()


print("Time to compute grid: ", t2 - t1)
print("Time to produce fractal: ", t3 - t2)

