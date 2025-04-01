import os
from copy import deepcopy

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation
from matplotlib.colors import LinearSegmentedColormap
from scipy.ndimage import convolve


class Growth_And_Crowding_CA:
    """A class to represent a cellular automaton model of growth and crowding.

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

    """
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
                 delta=0.2,
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
        self.direction_grid = np.zeros((size, size), int)
        self.time = 0
        np.random.seed(self.seed)
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
        """Applies the cell growth rule to the life grid.

        A cell grows if:
        - It is near a certain number of living cells.
        - It is adjacent to a certain number of direct neighbours.
        - There is enough food at the cell's location.
        - A random number is greater than 0.6.
        """
        num_neighbours = self.count_alive_neighbours()
        direct_neighbours = self.count_alive_neighbours(neighbourhood_size=1)
        crowding_values = np.vectorize(self.crowding_map.get)(num_neighbours, 0)
        growth_attempt = crowding_values * self.food_grid
        new_life_cells = (growth_attempt > self.growth_threshold) & (self.random_grid > 0.6) & (direct_neighbours > 0)
        self.update_directions(new_life_cells)
        self.new_life_grid = np.where(new_life_cells, 1, self.life_grid)
        self.birth_time_grid[new_life_cells] = self.time



    def apply_food_rule(self):
        """Applies the food distribution rule to the food grid."""
        if (self.food_algorithm == "Average"):
            self.new_food_grid = self.average_food()
        elif (self.food_algorithm == "Radial"):
            self.new_food_grid = self.radial()
        elif (self.food_algorithm == "Diffuse"):
            self.new_food_grid = self.diffuse()


    def average_food(self, neighbourhood_size=1):
        """One of the three food distribution algorithms.

        Calculates the average food distribution in the neighbourhood of each cell.


        Parameters:
        neighbourhood_size (int): The size of the neighbourhood.

        Returns:
        average_neighbors (np.ndarray): The average food value in the neighbourhood of each cell.
        """
        kernel_size = 2 * neighbourhood_size + 1
        kernel = np.ones((kernel_size, kernel_size), dtype=int)
        kernel[neighbourhood_size, neighbourhood_size] = 0
        neighbor_sums = convolve(self.food_grid, kernel, mode="reflect")
        valid_neighbors = convolve(self.food_mask.astype(int), kernel, mode="reflect")
        average_neighbors = np.where(valid_neighbors > 0, neighbor_sums / valid_neighbors, 0)
        return average_neighbors

    def diffuse(self):
        """One of the three food distribution algorithms. Diffuses food across the grid."""
        mean = self.average_food()
        return np.where(
            self.food_mask,
            (1 - self.delta) * self.food_grid + self.delta * mean,
            self.food_grid,
        )

    def radial(self):
        """One of the three food distribution algorithms. Distributes food in a radial pattern."""
        radius = self.time * 0.5
        reduction_mask = self.distances < radius
        return np.where(reduction_mask, np.maximum(0, self.food_grid - 10),
                                    self.food_grid)


    def life_eats_food(self):
        """Represents the process of cells eating food."""
        self.food_grid -= self.eat_value * self.life_grid

    def update_directions(self, new_life_cells):
        """For each new alive cell assign a number that represents which cell it attaches to."""
        kernel = np.array([
            [1, 2, 4],
            [8, 0, 16],
            [32, 64, 128],
        ])

            # Compute new neighbour directions
        new_neighbour_directions = convolve(self.life_grid, kernel, mode="reflect")

        # Preserve existing directions, update only for new cells
        self.direction_grid[new_life_cells == 1] = new_neighbour_directions[new_life_cells == 1]


    def count_alive_neighbours(self, neighbourhood_size=2):
        """Function to count the number of living cells in the neighbourhood of each cell."""
        kernel_size = 2 * neighbourhood_size + 1
        kernel = np.ones((kernel_size, kernel_size), dtype=int)
        kernel[neighbourhood_size, neighbourhood_size] = 0
        neighbor_counts = convolve(self.life_grid, kernel, mode="reflect")
        return neighbor_counts


    def count_direct_neighbours(self):
        """Function to count the number of direct neighbours of each cell."""
        kernel = np.array([[1, 1, 1], [1, 1, 1], [1, 1, 1]])
        neighbor_counts = convolve(self.life_grid, kernel, mode="reflect")
        return neighbor_counts


    def step(self):
        """The main function that represents a time step in the cellular automaton."""
        if self.time == 0:
            self.update_directions(self.life_grid)
            self.birth_time_grid[self.life_grid == 1] = self.time
        if (self.time % self.steps_between_growth == 0):
            self.apply_life_rule()
            self.life_grid = deepcopy(self.new_life_grid)
        self.apply_food_rule()
        self.food_grid = deepcopy(self.new_food_grid)
        self.life_eats_food()
        self.time = self.time + 1


def animate_simulation(frames=500):
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(18, 5))
    fig.suptitle("", fontsize=16)
    size = 100
    initial_grid = np.zeros((size, size))
    initial_grid[size // 2, size // 2] = 1

    ca = Growth_And_Crowding_CA(
        size,
        growth_threshold=2500,
        initial_food=100,
        food_algorithm="Diffuse",
        eat_value=15,
        steps_between_growth=3,
        delta=0.79,
        initial_life_grid=initial_grid,
        food_mask=np.ones((size, size), bool),
        seed=42,
    )

    cell_colors = [(0, "black"), (1, "lime")]
    cell_cmap = LinearSegmentedColormap.from_list("cell_colors", cell_colors)

    food_plot = ax1.imshow(ca.food_grid, cmap="ocean", vmin=0, vmax=100)
    cell_plot = ax2.imshow(ca.life_grid, cmap=cell_cmap, vmin=0, vmax=1)

    ax1.set_title("Food Distribution")
    ax2.set_title("Cell Distribution")

    plt.colorbar(food_plot, ax=ax1, label="Food Amount")
    plt.colorbar(cell_plot, ax=ax2, label="Cell Presence")

    def update(frame):

        if not hasattr(update, "previous_grids"):
            update.previous_grids = []

        # Stop if simulation stabilizes
        if len(update.previous_grids) >= 10:
            if (
                all(np.array_equal(update.previous_grids[0], grid) for grid in update.previous_grids[1:])
                or ca.time > 200
            ):
                print("Simulation stabilized or max time reached.")
                return None
            update.previous_grids.pop(0)

        food_plot.set_array(ca.food_grid)
        cell_cmap = plt.get_cmap("viridis")
        cell_plot.set_array(ca.life_grid)
        cell_plot.set_cmap(cell_cmap)

        update.previous_grids.append(ca.life_grid.copy())

        ca.step()

        return food_plot, cell_plot

    ani = FuncAnimation(fig, update, frames=frames, repeat=False, blit=False) # noqa: F841

    plt.tight_layout()
    plt.show()


def run_simulation(steps=100, save=False, save_path="simulation_data"):

    size = 100
    initial_grid = np.zeros((size, size))
    initial_grid[size // 2, size // 2] = 1

    ca = Growth_And_Crowding_CA(
        size,
        growth_threshold=2500,
        initial_food=100,
        food_algorithm="Diffuse",
        eat_value=15,
        steps_between_growth=3,
        delta=0.79,
        initial_life_grid=initial_grid,
        food_mask=np.ones((size, size), bool),
        seed=42,
    )

    for step in range(steps):
        print(f"Running step {step+1}/{steps}")
        ca.step()

    if save:
        os.makedirs(save_path, exist_ok=True)
        np.save(os.path.join(save_path, "life_grid.npy"), ca.life_grid)
        np.save(os.path.join(save_path, "food_grid.npy"), ca.food_grid)

    print(f"Simulation complete. Grids saved in {save_path}/")



if __name__ == "__main__":
    animate_simulation()
