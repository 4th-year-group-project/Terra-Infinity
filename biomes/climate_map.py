"""Classifies biomes for each polygon"""
import hashlib

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.colors import ListedColormap, Normalize
from PIL import Image, ImageDraw

from biomes.create_voronoi import get_chunk_polygons
from generation import Noise, normalize


def classify_biome(temp, precip):
    """Classify a biome based on temperature and precipitation values using the Whittaker diagram. Values are normally close to 0 with -1 and 1 being rare to occur.

    Biomes = "temperate rainforest", "boreal forest", "grassland", "tundra", "savanna", "woodland", "tropical rainforest", "temperate seasonal forest", "subtropical desert"

    Parameters:
    temp: temperature value between -1 and 1
    precip: precipitation value between -1 and 1

    Returns:
    biome: biome classification
    """

    biomes = [10,20,30,40,50,60,70,80,90]
    biome_values = [[0.22, 0.2], [-0.15, 0.05], [-0.05, -0.1], [-0.25, -0.05], [0.25, 0.15], [-0.05, -0.05], [0.3, 0.18],[0, 0], [0.28, -0.3]]

    smallest_dist = np.inf
    for i in range(len(biome_values)):
        dist = np.sqrt((temp - biome_values[i][0])**2 + (precip - biome_values[i][1])**2)
        if dist < smallest_dist:
            smallest_dist = dist
            biome = biomes[i]
    return biome
    # if temp < -0.35:
    #     return biomes[3]

    # elif temp < -0.1:
    #     if precip < -0.08:
    #         return biomes[2]
    #     elif precip < -0.05:
    #         return biomes[5]
    #     else:
    #         return biomes[1]

    # elif temp < 0.25:
    #     if precip < -0.15:
    #         return biomes[2]
    #     elif precip < -0.08:
    #         return biomes[5]
    #     elif precip < 0.2:
    #         return biomes[7]
    #     else:
    #         return biomes[0]

    # else:
    #     if precip < -0.1:
    #         return biomes[8]
    #     elif precip < 0.15:
    #         return biomes[4]
    #     else:
    #         return biomes[6]

def pnpoly(nvert, vertx, verty, testx, testy):
  """Determine if a point is inside a polygon"""
  c = 0
  j = nvert-1
  for i in range(nvert):
    if ((verty[i]>testy) != (verty[j]>testy)) and (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]):
       c = not c
    j = i
  return c


def determine_biomes(chunk_coords, polygon_edges, polygon_points, landmass_classifications, offsets,  seed, specified_biome=None, chunk_size=1024):
    """Determine the biome of each polygon using a temperature and precipitation map

    Parameters:
    polygon_edges: list of edges of the polygons
    polygon_points: list of points of the polygons
    landmass_classifications: list of classifications of the landmasses
    seed: the world seed
    """

    (offset_x, offset_y) = offsets

    x_points = [point[k][0] for point in polygon_points for k in range(len(point))]
    y_points = [point[k][1] for point in polygon_points for k in range(len(point))]

    overall_min_x = min(x_points)
    overall_max_x = max(x_points)
    overall_min_y = min(y_points)
    overall_max_y = max(y_points)

    xpix, ypix = 100, 100
    tempmap = np.zeros((xpix, ypix))
    precipmap = np.zeros((xpix, ypix))

    min_x = chunk_coords[0] * chunk_size
    min_y = chunk_coords[1] * chunk_size
    xpix, ypix = int(np.ceil(overall_max_x - overall_min_x)), int(np.ceil(overall_max_y - overall_min_y))

    chunk_seed = f"{seed:b}" + f"{min_x+(1<<32):b}" + f"{min_y+(1<<32):b}"
    hashed_seed = int(hashlib.sha256(chunk_seed.encode()).hexdigest(), 16) % (2**32)

    np.random.seed(hashed_seed)

    noise = Noise(seed=seed, width=xpix, height=ypix)

    tempmap = noise.fractal_simplex_noise(seed=seed, noise="open", x_offset=int(offset_x), y_offset=int(offset_y), scale=1200, octaves=5, persistence=0.5, lacunarity=2)
    #tempmap = normalize(tempmap, a=-1, b=1)/2
    tempmap = tempmap/2

    precipmap = noise.fractal_simplex_noise(seed=seed+1, noise="open", x_offset=int(offset_x), y_offset=int(offset_y), scale=1200, octaves=5, persistence=0.5, lacunarity=2)
    #precipmap = normalize(precipmap, a=-1, b=1)/2
    precipmap = precipmap/2

    biomes = np.zeros((xpix, ypix))
    biomes = []

    mask = np.zeros((4500, 4500))

    # For each polygon find average temperature and precipitation
    for i in range(len(polygon_points)):
        if landmass_classifications[i] == 0:
            biome = 100 if specified_biome is None else specified_biome
            biomes.append(biome)

            polygon = polygon_points[i]

            polygon_tupled = [(point[0], point[1]) for point in polygon]

            img = Image.new("L", (4500, 4500), 0)
            im = ImageDraw.Draw(img)
            im.polygon(polygon_tupled,fill="#eeeeff", outline="black")
            img_arr = np.array(img)

            mask[img_arr > 0] = biome
        else:
            polygon = polygon_points[i]
            x_points = [point[0] for point in polygon]
            y_points = [point[1] for point in polygon]

            min_polygon_x = int(np.floor(min(x_points)))
            max_polygon_x = int(np.ceil(max(x_points)))
            min_polygon_y = int(np.floor(min(y_points)))
            max_polygon_y = int(np.ceil(max(y_points)))

            diff_x = max_polygon_x - min_polygon_x
            diff_y = max_polygon_y - min_polygon_y

            polygon_tupled = [(point[0], point[1]) for point in polygon]

            img = Image.new("L", (4500, 4500), 0)
            im = ImageDraw.Draw(img)
            im.polygon(polygon_tupled,fill="#eeeeff", outline="black")
            img_arr = np.array(img)

            t_values = np.zeros(100)
            p_values = np.zeros(100)

            if specified_biome is None:
                # Check if the random points are in the polygon
                count = 0
                polygon_seed = f"{diff_x+(1<<32):b}" + f"{diff_y+(1<<32):b}"
                hashed_polygon_seed = int(hashlib.sha256(polygon_seed.encode()).hexdigest(), 16) % (2**32)
                checked_points = set()
                np.random.seed(hashed_polygon_seed)
                while count < 100:
                    point = (np.random.randint(int(min(x_points)), int(max(x_points))), np.random.randint(int(min(y_points)), int(max(y_points))))
                    point = (np.random.randint(int(min(x_points)), int(max(x_points))), np.random.randint(int(min(y_points)), int(max(y_points))))
                    if pnpoly(len(x_points), x_points, y_points, point[0], point[1]) == 1 and point not in checked_points:
                        checked_points.add(point)
                        noise_x = point[0]
                        noise_y = point[1]
                        t_value = tempmap[noise_y][noise_x]
                        p_value = precipmap[noise_y][noise_x]
                        t_values[count] = t_value
                        p_values[count] = p_value
                        count += 1

                # Calculate median temperature value for the polygon
                t_average = np.median(t_values)

                # Calculate median precipitation value for the polygon
                p_average = np.median(p_values)

                biome = classify_biome(t_average, p_average)
            else:
                biome = specified_biome

            mask[img_arr > 0] = biome

            biomes.append(biome)

    # values = np.array([0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100])
    # colors = ['white', 'teal', 'seagreen', 'darkkhaki', 'lightsteelblue', 'yellowgreen', 'darkgoldenrod', 'darkgreen', 'mediumturquoise', 'orange', 'blue']

    # cmap = ListedColormap(colors)
    # norm = Normalize(vmin=0, vmax=100, clip=True)

    # plt.imshow(mask, norm=norm, cmap=cmap)
    # plt.show(block=False)
    return biomes, mask

# nut = np.random.randint(100, 200)
# print(nut)
# chunk_coords = (0,0)
# polygon_edges, polygon_points, _, _ = get_chunk_polygons((0,0), nut)
# min_x = min([point[0] for point in polygon_points for point in point])
# min_y = min([point[1] for point in polygon_points for point in point])
# positive_polygon_points = [[(point[0] - min_x, point[1] - min_y) for point in polygon] for polygon in polygon_points]
# landmass_classifications = [1 for i in range(len(polygon_points))]
# b = determine_biomes(chunk_coords, polygon_edges, positive_polygon_points, landmass_classifications,[min_x, min_y], nut)
# print(b)
