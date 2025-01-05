from copy import deepcopy
import numpy as np
from scipy.ndimage import convolve


class Growth_And_Crowding_CA:
    '''
    A class to represent a cellular automaton model of growth and crowding.

    Attributes:
    size (int): The size of the grid.
    growth_threshold (int): The threshold for growth.
    food_algorithm (str): The algorithm used to calculate food distribution.
    eat_value (int): The value of food eaten by cells.
    delta (float): The diffusion rate of food.
    seed (int): The seed for the random number generator.
    steps_between_growth (int): The number of time steps between growth stages.
    new_food_grid (np.ndarray): The food grid that represents the food distribution at the next time step.
    new_life_grid (np.ndarray): The life grid that represents the cell distribution at the next time step.
    food_grid (np.ndarray): The food grid that represents the food distribution at the current time step.
    food_mask (np.ndarray): The mask that represents the food distribution.
    life_grid (np.ndarray): The life grid that represents the cell distribution at the current time step.
    birth_time_grid (np.ndarray): The grid that represents the birth time of cells.
    time (int): The current time step.
    random_grid (np.ndarray): The random grid used for random number generation.
    crowding_map (dict): The map that represents the crowding values.
    center_x (int): The x-coordinate of the center of the grid.
    center_y (int): The y-coordinate of the center of the grid.
    x_coords (np.ndarray): The x-coordinates of the grid.
    y_coords (np.ndarray): The y-coordinates of the grid.
    distances (np.ndarray): The distances of the grid cells from the center.

    '''
    def __init__(self, 
                 size, 
                 growth_threshold, 
                 initial_food,
                 food_algorithm,
                 eat_value,
                 steps_between_growth,
                 initial_life_grid,
                 food_mask,
                 seed,
                 delta=0.2
                 ):
        self.size = size
        self.growth_threshold = growth_threshold
        self.food_algorithm = food_algorithm
        self.eat_value = eat_value
        self.delta = delta
        self.seed = seed
        self.steps_between_growth = steps_between_growth
        self.new_food_grid = np.zeros((size,size), float)
        self.new_life_grid = np.zeros((size,size))
        self.food_grid = np.full((size,size), -1, float)
        self.food_grid[food_mask] = initial_food
        self.food_mask = food_mask
        self.life_grid = initial_life_grid
        self.birth_time_grid = np.full((size, size), -1)
        self.time = 0
        self.random_grid = np.random.rand(self.size, self.size)
        self.crowding_map = {i: (30 if i == 1 else 
                                 30 if i == 2 else 
                                 30 if i in [3, 4] else 
                                 30 if i == 5 else 
                                 30 if i in [5, 6, 7] else 
                                 0) for i in range(22)}
        center_x, center_y = self.size // 2, self.size // 2
        x_coords, y_coords = np.indices((self.size, self.size))
        self.distances = np.sqrt((x_coords - center_x) ** 2 + (y_coords - center_y) ** 2)
    

    def apply_life_rule(self):
        '''
        Applies the cell growth rule to the life grid.

        A cell grows if:
        - It is near a certain number of living cells.
        - It is adjacent to a certain number of direct neighbours.
        - There is enough food at the cell's location.
        - A random number is greater than 0.6.
        '''
        num_neighbours = self.count_alive_neighbours()
        direct_neighbours = self.count_alive_neighbours(neighbourhood_size=1)
        crowding_values = np.vectorize(self.crowding_map.get)(num_neighbours, 0)
        growth_attempt = crowding_values * self.food_grid
        np.random.seed(self.seed)
        random_grid = np.random.random((self.size, self.size))
        new_life_cells = (growth_attempt > self.growth_threshold) & (random_grid > 0.6) & (direct_neighbours > 0)
        self.new_life_grid = np.where(new_life_cells, 1, self.life_grid)
        self.birth_time_grid[new_life_cells] = self.time 
        

    def apply_food_rule(self):
        '''
        Applies the food distribution rule to the food grid.
        '''
        if (self.food_algorithm == "Average"):
            self.new_food_grid = self.average_food()
        elif (self.food_algorithm == "Radial"):
            self.new_food_grid = self.radial()
        elif (self.food_algorithm == "Diffuse"):
            self.new_food_grid = self.diffuse()


    def average_food(self, neighbourhood_size=1):
        '''
        One of the three food distribution algorithms. 
        Calculates the average food distribution in the neighbourhood of each cell.

        Parameters:
        neighbourhood_size (int): The size of the neighbourhood.

        Returns:
        average_neighbors (np.ndarray): The average food value in the neighbourhood of each cell.
        '''
        kernel_size = 2 * neighbourhood_size + 1
        kernel = np.ones((kernel_size, kernel_size), dtype=int)
        kernel[neighbourhood_size, neighbourhood_size] = 0 
        neighbor_sums = convolve(self.food_grid, kernel, mode='reflect')
        valid_neighbors = convolve(self.food_mask.astype(int), kernel, mode='reflect')
        average_neighbors = np.where(valid_neighbors > 0, neighbor_sums / valid_neighbors, 0)
        return average_neighbors

    def diffuse(self):
        '''
        One of the three food distribution algorithms. Diffuses food across the grid.
        '''
        mean = self.average_food()
        return np.where(
            self.food_mask, 
            (1 - self.delta) * self.food_grid + self.delta * mean, 
            self.food_grid
        )

    def radial(self):
        '''
        One of the three food distribution algorithms. Distributes food in a radial pattern.
        '''
        radius = self.time * 0.5
        reduction_mask = self.distances < radius
        return np.where(reduction_mask, np.maximum(0, self.food_grid - 10), 
                                    self.food_grid)


    def life_eats_food(self):
        '''
        Represents the process of cells eating food.
        '''
        self.food_grid -= self.eat_value * self.life_grid
        

    def count_alive_neighbours(self, neighbourhood_size=2):
        '''
        Function to count the number of living cells in the neighbourhood of each cell.
        '''
        kernel_size = 2 * neighbourhood_size + 1
        kernel = np.ones((kernel_size, kernel_size), dtype=int)
        kernel[neighbourhood_size, neighbourhood_size] = 0 
        neighbor_counts = convolve(self.life_grid, kernel, mode='reflect')
        return neighbor_counts
    

    def count_direct_neighbours(self):
        '''
        Function to count the number of direct neighbours of each cell.
        '''
        kernel = np.array([[0, 1, 0], [1, 0, 1], [0, 1, 0]])
        neighbor_counts = convolve(self.life_grid, kernel, mode='reflect')
        return neighbor_counts
    

    def step(self):
        '''
        The main function that represents a time step in the cellular automaton.
        '''
        if self.time == 0:
            self.birth_time_grid[self.life_grid == 1] = self.time
        if (self.time % self.steps_between_growth == 0):
            self.apply_life_rule()
            self.life_grid = deepcopy(self.new_life_grid)
        self.apply_food_rule()
        self.food_grid = deepcopy(self.new_food_grid)
        self.life_eats_food()
        self.time = self.time + 1


