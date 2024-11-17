import numpy as np
import matplotlib.pyplot as plt
from CA import Growth_And_Crowding_CA
from scipy.ndimage import convolve
import cv2
import numpy as np
import matplotlib.pyplot as plt

def upscale_bilinear(image, scale_factor):
    image = image.astype(np.float32)
    print(image.shape)
    print(image)
    height, width = image.shape[:2]
    new_width = int(width * scale_factor)
    new_height = int(height * scale_factor)
    upscaled_image = cv2.resize(image, (new_width, new_height), interpolation=cv2.INTER_LINEAR)
    return upscaled_image

def convolve_average(grid):
    kernel = np.array([[1, 1, 1],
                       [1, 0, 1],
                       [1, 1, 1]])
    neighbor_counts = convolve(grid, kernel, mode='reflect')
    return neighbor_counts / 8

def gaussian_blur(image, kernel_size=100, sigma=2.0):
    ax = np.linspace(-(kernel_size - 1) / 2., (kernel_size - 1) / 2., kernel_size)
    gauss = np.exp(-0.5 * np.square(ax) / np.square(sigma))
    kernel = np.outer(gauss, gauss)
    kernel /= np.sum(kernel)
    blurred_image = convolve(image, kernel, mode='reflect')
    
    return blurred_image


def upscale_shape_with_full_adjacency(small_grid, scale_factor):
    small_size = small_grid.shape[0]
    large_size = small_size * scale_factor
    large_grid = np.zeros((large_size, large_size), dtype=int)
    neighbors = [
        (-1, 0), (1, 0), (0, -1), (0, 1),   # 4 orthogonal neighbors
        (-1, -1), (-1, 1), (1, -1), (1, 1)  # 4 diagonal neighbors
    ]

    offset = scale_factor // 2

    max_stack_depth = 0  
    visited = set()
    
    def iterative_dfs(start_x, start_y):
        nonlocal max_stack_depth
        stack = [(start_x, start_y, 1)]
        visited.add((start_x, start_y))

        while stack:
            max_stack_depth = max(max_stack_depth, len(stack))

            x, y, depth = stack.pop()

            large_x = x * scale_factor + offset
            large_y = y * scale_factor + offset
            large_grid[large_x, large_y] = 1

            #This is UGLY and will be CHANGED
            for dx, dy in neighbors:
                nx, ny = x + dx, y + dy
                if 0 <= nx < small_size and 0 <= ny < small_size and small_grid[nx, ny] == 1 and (nx, ny) not in visited:
                    visited.add((nx, ny))

                    if dx == -1 and dy == 0:  # Connect upward
                        for i in range(scale_factor):
                            large_grid[large_x - i, large_y] = 1
                    elif dx == 1 and dy == 0:  # Connect downward
                        for i in range(scale_factor):
                            large_grid[large_x + i, large_y] = 1
                    elif dx == 0 and dy == -1:  # Connect leftward
                        for i in range(scale_factor):
                            large_grid[large_x, large_y - i] = 1
                    elif dx == 0 and dy == 1:  # Connect rightward
                        for i in range(scale_factor):
                            large_grid[large_x, large_y + i] = 1
                    elif dx == -1 and dy == -1:  # Connect top-left diagonal
                        for i in range(scale_factor):
                            large_grid[large_x - i, large_y - i] = 1
                    elif dx == -1 and dy == 1:  # Connect top-right diagonal
                        for i in range(scale_factor):
                            large_grid[large_x - i, large_y + i] = 1
                    elif dx == 1 and dy == -1:  # Connect bottom-left diagonal
                        for i in range(scale_factor):
                            large_grid[large_x + i, large_y - i] = 1
                    elif dx == 1 and dy == 1:  # Connect bottom-right diagonal
                        for i in range(scale_factor):
                            large_grid[large_x + i, large_y + i] = 1
                    stack.append((nx, ny, depth + 1))

    for x in range(small_size):
        for y in range(small_size):
            if small_grid[x, y] == 1 and (x, y) not in visited:
                iterative_dfs(x, y)

    print(f'Max stack depth: {max_stack_depth}')
    return large_grid


shape_grids = []
ca_size = 5
scale_factors = {
    0: 7,
    1: 6,
    2: 3,
    3: 3
}
zeros = np.zeros((ca_size, ca_size))
zeros[ca_size//2, ca_size//2] = 1
radius = ca_size // 3  # Radius of the circle
y, x = np.ogrid[:ca_size, :ca_size]
center = ca_size // 2
mask = (x - center)**2 + (y - center)**2 <= radius**2
ca = Growth_And_Crowding_CA(size=ca_size, 
                          growth_threshold=2500, 
                          initial_food=100,
                          food_algorithm="Diffuse",
                          eat_value=15,
                          steps_between_growth=2,
                          delta = 0.99,
                          initial_life_grid=zeros,
                          food_mask=mask)

while ca.time < 2:
        ca.step()
        life_grid = ca.life_grid
to_blur = life_grid
shape_grids.append(to_blur)
blurry_large = upscale_bilinear(to_blur, scale_factors.get(0))
blurry_large = gaussian_blur(blurry_large, sigma=4)
blurry_large *=1.5
shape_grids.append(blurry_large)

#Ugly 
for i in range (1,3):
    large_grid = upscale_shape_with_full_adjacency(life_grid, scale_factors.get(i-1))
    shape_grids.append(large_grid)
    print(large_grid.shape)
    non_zero_count = np.count_nonzero(large_grid)
    print(f'Non-zero count in large grid: {non_zero_count}')
    ca_size = ca_size * scale_factors.get(i-1)
    radius = ca_size // 3  # Radius of the circle
    y, x = np.ogrid[:ca_size, :ca_size]
    center = ca_size // 2
    mask = (x - center)**2 + (y - center)**2 <= radius**2
    
    ca = Growth_And_Crowding_CA(
        size=ca_size, 
        growth_threshold=2700, 
        initial_food=100,
        food_algorithm="Diffuse",
        eat_value=15,
        steps_between_growth=2,
        delta = 0.99,
        initial_life_grid = large_grid,
        food_mask=mask
    )
    while ca.time < 180:
        ca.step()
    life_grid = ca.life_grid
    shape_grids.append(life_grid)
    to_blur = blurry_large + (0.7 * scale_factors.get(i)/10 * life_grid)
    blurry_large = upscale_bilinear(to_blur, scale_factors.get(i))
    blurry_large = gaussian_blur(blurry_large, 100-5*i, 3+i*2)
    shape_grids.append(blurry_large)
    
    
large_grid = upscale_shape_with_full_adjacency(life_grid, scale_factors.get(i))
shape_grids.append(large_grid)
to_blur = blurry_large + 0.1 * large_grid
shape_grids.append(to_blur)
blurred = gaussian_blur(to_blur, kernel_size=11, sigma=2.5)
shape_grids.append(blurred)

plt.figure(figsize=(10, 10))
plt.imshow(shape_grids[-1], cmap='grey')
plt.axis('off')
plt.gca().set_position([0, 0, 1, 1]) 
#plt.savefig('imgs/final_grid.png', bbox_inches='tight', pad_inches=0)
plt.close()

fig, axes = plt.subplots(1, len(shape_grids), figsize=(15, 5))
for ax, grid in zip(axes, shape_grids):
    ax.imshow(grid, cmap='grey')
    ax.set_title('Grid at size {}'.format(grid.shape[0]))
    ax.axis('off')
plt.tight_layout()
plt.show()
