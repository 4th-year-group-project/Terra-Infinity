from scipy.ndimage import sobel
import matplotlib.pyplot as plt
import numpy as np
from scipy.stats import qmc
from Noise.simplex import SimplexNoise
import time
import numba as nb

@nb.jit(fastmath=True)
def find_intersections(circle1_centre, circle2_centre, circle1_radius, circle2_radius): 

    x, y = circle1_centre
    x1, y1 = circle2_centre
    dist_between_centres = np.sqrt((x - x1)**2 + (y - y1)**2)
    if dist_between_centres > circle1_radius + circle2_radius:
        return None, None
    elif dist_between_centres < np.abs(circle1_radius - circle2_radius):
        return None, None
    elif dist_between_centres == 0 and circle1_radius == circle2_radius:
        return None, None
    else:
        a = (circle1_radius**2 - circle2_radius**2 + dist_between_centres**2) / (2 * dist_between_centres)
        h = np.sqrt(circle1_radius**2 - a**2)
        x2 = x + a * (x1 - x) / dist_between_centres
        y2 = y + a * (y1 - y) / dist_between_centres
        x3 = x2 + h * (y1 - y) / dist_between_centres
        y3 = y2 - h * (x1 - x) / dist_between_centres
        x4 = x2 - h * (y1 - y) / dist_between_centres
        y4 = y2 + h * (x1 - x) / dist_between_centres

        return (x3, y3), (x4, y4)
    
def poisson(min_x, max_x, min_y, max_y, seed, chunk_size, radius, density):
    rng = np.random.default_rng(seed)
    engine = qmc.PoissonDisk(d=2, radius=radius, seed=rng)
    l_bounds = np.array([min_x, min_y]) 
    u_bounds = np.array([max_x, max_y])
    points = chunk_size*chunk_size*density
    ind = engine.integers(l_bounds=l_bounds, u_bounds=u_bounds, n=points)
    return ind

@nb.jit(fastmath=True)
def packing(min_x, max_x, min_y, max_y, chunk_size, density=4):
    points = []
    pointRadius = density
    centre_A = (min_x - (chunk_size / 2) - 250, min_y - (chunk_size / 2) - 250)
    init_radius_A = 256
    centre_B = (min_x - (chunk_size / 2) - 250, max_y + (chunk_size / 2) + 250)
    init_radius_B = 256
    dist_between_rings = 2.3 * pointRadius
    for A_radius_step in range(0,1024):
        radius_A = init_radius_A + A_radius_step * dist_between_rings
        for B_radius_step in range(0,1024):

            radius_B = init_radius_B + B_radius_step * dist_between_rings

            val = np.random.uniform(0.15,0.95)
            radius_Ap = radius_A + (0.0 if (B_radius_step % 3 == 0) else (val * dist_between_rings))
            radius_Bp = radius_B + (0.0 if (A_radius_step % 3 == 0) else (val * dist_between_rings))
            #print(radius_Ap)    
            #print(radius_Bp)
            point1, point2 = find_intersections(centre_A, centre_B, radius_Ap, radius_Bp)

            if point1 is not None:
                x, y = point1
                if x >= min_x and x <= max_x and y >= min_y and y <= max_y:
                    points.append((x, y))
            if point2 is not None:
                x, y = point2
                if x >= min_x and x <= max_x and y >= min_y and y <= max_y:
                    points.append((x, y))
            


    return points
                    

def apply_sobel(heightmap,spread, seed):

    sobel_h = sobel(heightmap, 0)
    sobel_v = sobel(heightmap, 1)
    magnitude = np.sqrt(sobel_h**2 + sobel_v**2)
    magnitude *= 255.0 / np.max(magnitude)
    # plt.figure(figsize=(10, 5))
    # plt.subplot(1, 2, 1)
    # plt.imshow(heightmap, cmap='gray')
    # plt.axis('off')
    # plt.subplot(1, 2, 2)
    # plt.imshow(magnitude, cmap='gray')
    # plt.axis('off')
    # plt.show()

    width = heightmap.shape[1]
    height = heightmap.shape[0]
    noise = SimplexNoise(seed=seed, width=width, height=height, scale=100, octaves=13, persistence=spread, lacunarity=2)
    noise_map = noise.fractal_noise(noise="open")
    noise_map = (noise_map + 1) / 2
    # plt.imshow(noise_map, cmap='gray')
    # plt.axis('off')
    # plt.show()

    #print(magnitude.shape)
    print(np.max(magnitude))
    vegetation_map = np.zeros((height, width))
    np.random.seed(seed)
    for y in range(height):
        for x in range(width):
            noise = noise_map[y, x]
            prob = (255 - magnitude[y,x]) / 255

            # and noise > heightmap[y, x]
            # scaled_noise > magnitude[y, x] and 
            if np.random.random() < prob and noise > heightmap[y,x] and heightmap[y, x] > 0.2:
                vegetation_map[y, x] = noise

    # plt.figure(figsize=(10, 5))
    # plt.subplot(1, 3, 1)
    # plt.imshow(magnitude, cmap='gray')
    # plt.subplot(1, 3, 2)
    # plt.imshow(vegetation_map, cmap='gray')
    # plt.subplot(1, 3, 3)
    # plt.imshow(heightmap, cmap='gray')
    # plt.show()
    return vegetation_map

def place_plants(heightmap, seed, spread=0.01, density=4):
    s1 = time.time()
    np.random.seed(seed)
    points = packing(0, 1024, 0, 1024, 1024, density)
    #points = poisson(0, 1024, 0, 1024, seed, 1024,  0.01, 0.005)
    #print(points)
    

    
    #points = poisson(0, 1024, 0, 1024,seed, 1024,  radius, density)
    #
    print(time.time() - s1)
    #points2 = packing(-1, 1, 1, 3, 2, density=0.02)
    # crop points
    #points1 = [(x, y) for x, y in points1 if x >= -0.85 and x <= 0.85 and y >= -0.98 and y <= 0.98]
    #points2 = [(x, y) for x, y in points2 if x >= -0.5 and x <= 0.5 and y >= 0.5 and y <= 2.5]

    #points = np.concatenate((points1, points2))
    # scale points to fit 1024x1024
    #points = [(x * 8, y * 8) for x, y in points]
    print(len(points))
    print("placed points")
    s1 = time.time()
    mask = apply_sobel(heightmap, spread, seed)
    print(time.time() - s1)
    #mask = mask - 1024
    #points = [(x + 1024, y) for x, y in points]
    points = [(x, 1024-y) for x, y in points if mask[int(y), int(x)] > 0.61]
    plt.figure(figsize=(10, 5))
    plt.subplot(1, 3, 1)
    plt.scatter(*zip(*points), s=1)
    plt.xlim(0, 1024)
    plt.ylim(0,1024)

    plt.gca().set_aspect('equal', adjustable='box')
    plt.subplot(1, 3, 2)
    plt.imshow(heightmap, cmap='gray')
    plt.subplot(1, 3, 3)
    plt.imshow(mask, cmap='gray')
    plt.show()
# width = 1024
# height = 1024
# noise = SimplexNoise(seed=14, width=width, height=height, scale=400, octaves=4, persistence=0.5, lacunarity=2)
# noise_map = noise.fractal_noise(noise="open")
# noise_map = (noise_map + 1) / 2
# # apply_sobel(noise_map)


# place_plants(noise_map, 42)

#ps =poisson(-1024, 1024, -1024, 1024, 1024, 0.025)
#print(ps)



# points1 = packing(0, 1024, 0, 1024, 1024)
# points2 = packing(1024, 2048, 0, 1024, 1024)
# points = np.concatenate((points1, points2))

# plt.scatter(*zip(*points), s=1)
# plt.xlim(0, 2048)
# plt.ylim(0,1024)
# plt.gca().set_aspect('equal', adjustable='box')
# plt.show()