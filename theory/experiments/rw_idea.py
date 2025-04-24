
import pygame as pg ##for visuals
import random
import numpy as np 
import numba as nb ##for speed
import matplotlib as mpl ##for colors




n = 9 ## do not put above 10!!
grid_size = 2 ** n
n_particles = (n**2) * grid_size

grid = np.zeros((grid_size, grid_size), dtype=np.int32)

##Initialise the seed
seed = (grid_size // 2, grid_size // 2) 
grid[seed] = 1


x,y = seed
particle_counter = 0
inside = True
arrivals = []



while particle_counter < n_particles:
    x += random.choice([-1, 0, 1])
    y += random.choice([-1, 0, 1])
    x %= grid_size
    y %= grid_size
    if grid[x, y] == 0 and inside:
        grid[x, y] = 1
        particle_counter += 1
        inside = False
        arrivals.append((x, y))

        if particle_counter % 100 == 0:
            print(particle_counter)
    else:
        if grid[x, y] == 1:
            inside = True

    






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

pg.image.save(screen, "rw.png")

##Run the game loop (very simple)
while running:
    for event in pg.event.get():
        if event.type == pg.QUIT:
            running = False
pg.quit()


