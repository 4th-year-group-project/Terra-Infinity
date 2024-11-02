from copy import deepcopy
import numpy as np
import numba as nb
import random
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.colors import LinearSegmentedColormap
from scipy.ndimage import convolve


class Growth_And_Crowding_CA:
    def __init__(self, 
                 size, 
                 growth_threshold, 
                 initial_food,
                 food_algorithm,
                 eat_value,
                 steps_between_growth,
                 delta=0.2):
        self.size = size
        self.growth_threshold = growth_threshold
        self.food_algorithm = food_algorithm
        self.eat_value = eat_value
        self.delta = delta
        self.steps_between_growth = steps_between_growth
        self.new_food_grid = np.zeros((size,size), float)
        self.new_life_grid = np.zeros((size,size))
        self.food_grid = np.full((size,size), initial_food, float)
        self.life_grid = np.zeros((size,size))
        self.life_grid[size//2,size//2] = 1
        self.time = 0
        self.random_grid = np.random.rand(self.size, self.size)
        self.crowding_map = {i: (30 if i == 1 else 
                                 30 if i == 2 else 
                                 30 if i in [3, 4] else 
                                 20 if i == 5 else 
                                 0) for i in range(22)}
        center_x, center_y = self.size // 2, self.size // 2
        x_coords, y_coords = np.indices((self.size, self.size))
        self.distances = np.sqrt((x_coords - center_x) ** 2 + (y_coords - center_y) ** 2)
    

    def apply_life_rule(self):
        num_neighbours = self.count_alive_neighbours()
        direct_neighbours = self.count_alive_neighbours(neighbourhood_size=1)
        crowding_values = np.vectorize(self.crowding_map.get)(num_neighbours, 0)
        growth_attempt = crowding_values * self.food_grid
        random_grid = generate_random_array(self.size)
        self.new_life_grid = np.where((growth_attempt > self.growth_threshold) 
                                      & (random_grid > 0.5) & (direct_neighbours > 0), 1, self.life_grid)


    def apply_food_rule(self):
        if (self.food_algorithm == "Average"):
            self.new_food_grid = self.average_food()
        elif (self.food_algorithm == "Radial"):
            self.new_food_grid = self.radial()
        elif (self.food_algorithm == "Diffuse"):
            self.new_food_grid = self.diffuse()


    def average_food(self, neighbourhood_size=1):
        kernel_size = 2 * neighbourhood_size + 1
        kernel = np.ones((kernel_size, kernel_size), dtype=int)
        kernel[neighbourhood_size, neighbourhood_size] = 0 
        neighbor_counts = convolve(self.food_grid, kernel, mode='reflect')
        return neighbor_counts / (kernel_size ** 2 - 1)
    

    def radial(self):
        radius = self.time * 0.5
        reduction_mask = self.distances < radius
        return np.where(reduction_mask, np.maximum(0, self.food_grid - 10), 
                                    self.food_grid)


    def diffuse(self):
        mean = self.average_food()
        return (1-self.delta) * self.food_grid + self.delta * mean


    def life_eats_food(self):
        self.food_grid -= self.eat_value * self.life_grid
        

    def count_alive_neighbours(self, neighbourhood_size=2):
        kernel_size = 2 * neighbourhood_size + 1
        kernel = np.ones((kernel_size, kernel_size), dtype=int)
        kernel[neighbourhood_size, neighbourhood_size] = 0 
        neighbor_counts = convolve(self.life_grid, kernel, mode='reflect')
        return neighbor_counts
    

    def count_direct_neighbours(self):
        kernel = np.array([[0, 1, 0], [1, 0, 1], [0, 1, 0]])
        neighbor_counts = convolve(self.life_grid, kernel, mode='reflect')
        return neighbor_counts
    

    def step(self):
        if (self.time % self.steps_between_growth == 0):
            self.apply_life_rule()
            self.life_grid = deepcopy(self.new_life_grid)
        self.apply_food_rule()
        self.food_grid = deepcopy(self.new_food_grid)
        self.life_eats_food()
        self.time = self.time + 1



@nb.njit('float64[:,:](int64)', parallel=True)
def generate_random_array(size):
    res = np.empty((size, size))

    for i in nb.prange(size):
        for j in range(size):
            res[i, j] = random.random()

    return res


def animate_simulation(frames=500):
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
    fig.suptitle('', fontsize=16)
    
    ca = Growth_And_Crowding_CA(size=100, 
                          growth_threshold=2610, 
                          initial_food=100,
                          food_algorithm="Diffuse",
                          eat_value=15,
                          steps_between_growth=3,
                          delta = 0.6)
    
    cell_colors = [(0, 'black'), (1, 'lime')]
    cell_cmap = LinearSegmentedColormap.from_list('cell_colors', cell_colors)
    
    food_plot = ax1.imshow(ca.food_grid, cmap='ocean', vmin=0, vmax=100)
    cell_plot = ax2.imshow(ca.life_grid, cmap=cell_cmap, vmin=0, vmax=1)
    
    ax1.set_title('Food Distribution')
    ax2.set_title('Cell Distribution')
    
    plt.colorbar(food_plot, ax=ax1, label='Food Amount')
    plt.colorbar(cell_plot, ax=ax2, label='Cell Presence')
    
    def update(frame):
        if ca.time < 1000:  
            food_plot.set_array(ca.food_grid)
            cell_cmap = plt.get_cmap('viridis')
            cell_plot.set_array(ca.life_grid)
            cell_plot.set_cmap(cell_cmap)
            ca.step()
            return food_plot, cell_plot
        else:
            return None

    anim = FuncAnimation(fig, update, frames=frames, interval=1, blit=True)
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    animate_simulation()
