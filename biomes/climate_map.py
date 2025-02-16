import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap, BoundaryNorm, Normalize
import matplotlib.cm as cm
import numpy as np
import random
import cv2
from PIL import ImageDraw, ImagePath, Image
import hashlib


from biomes.create_voronoi import get_chunk_polygons
from Noise.simplex import SimplexNoise
from coastline.geom import GeometryUtils

def classify_biome(temp, precip):
    '''
        Classify a biome based on temperature and precipitation values using the Whittaker diagram. Values are normally close to 0 with -1 and 1 being rare to occur.

        Biomes = "temperate rainforest", "boreal forest", "grassland", "tundra", "savanna", "woodland", "tropical rainforest", "temperate seasonal forest", "subtropical desert"
        
        Parameters:
        temp: temperature value between -1 and 1
        precip: precipitation value between -1 and 1

        Returns:
        biome: biome classification
    '''
    
    biomes = [10,20,30,40,50,60,70,80,90]
    if temp < -0.35:
        return biomes[3]
    
    elif temp < -0.1:
        if precip < -0.08:
            return biomes[2]
        elif precip < -0.05:
            return biomes[5]
        else:
            return biomes[1]
        
    elif temp < 0.25:
        if precip < -0.15:
            return biomes[2]
        elif precip < -0.08:
            return biomes[5]
        elif precip < 0.2:
            return biomes[7]
        else:
            return biomes[0]
        
    else:
        if precip < -0.1:
            return biomes[8]
        elif precip < 0.15:
            return biomes[4]
        else:
            return biomes[6]
      
def pnpoly(nvert, vertx, verty, testx, testy):
  '''
     Determine if a point is inside a polygon
  '''
  c = 0
  j = nvert-1
  for i in range(nvert):
    if ((verty[i]>testy) != (verty[j]>testy)) and (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]):
       c = not c
    j = i
  return c


def determine_biomes(chunk_coords, polygon_edges, polygon_points, landmass_classifications, offsets,  seed):
    '''
        Determine the biome of each polygon using a temperature and precipitation map

        Parameters:
        polygon_edges: list of edges of the polygons
        polygon_points: list of points of the polygons
        landmass_classifications: list of classifications of the landmasses
        seed: the world seed
    '''

    (offset_x, offset_y) = offsets

    x_points = [point[k][0] for point in polygon_points for k in range(len(point))]
    y_points = [point[k][1] for point in polygon_points for k in range(len(point))]

    overall_min_x = min(x_points)
    overall_max_x = max(x_points) + 1024
    overall_min_y = min(y_points)
    overall_max_y = max(y_points) + 1024

    xpix, ypix = 100, 100
    tempmap = np.zeros((xpix, ypix))
    precipmap = np.zeros((xpix, ypix))

    min_x = chunk_coords[0] * 1024
    min_y = chunk_coords[1] * 1024
    xpix, ypix = int(np.ceil(overall_max_x - overall_min_x)), int(np.ceil(overall_max_y - overall_min_y))

    chunk_seed = "{0:b}".format(seed) + "{0:b}".format(min_x+(1<<32)) + "{0:b}".format(min_y+(1<<32))
    hashed_seed = int(hashlib.sha256(chunk_seed.encode()).hexdigest(), 16) % (2**32)

    np.random.seed(hashed_seed)
    simplex_noise = SimplexNoise(seed=seed, width=xpix, height=ypix, scale=1000, octaves=2, persistence=0.5, lacunarity=2)
    noise_map = simplex_noise.fractal_noise(noise="open", x_offset=int(offset_x), y_offset=int(offset_y))
    tempmap = (noise_map) / 2

    simplex_noise = SimplexNoise(seed=seed+1, width=xpix, height=ypix, scale=1000, octaves=2, persistence=0.5, lacunarity=2)
    noise_map = simplex_noise.fractal_noise(noise="open", x_offset=int(offset_x), y_offset=int(offset_y))
    precipmap = (noise_map) / 2

    biomes = np.zeros((xpix, ypix))
    biomes = []


    overall_mask = np.zeros((ypix, xpix))

    #biomes = assign_biomes(polygon_points, tempmap, precipmap, overall_min_x, overall_min_y, xpix, ypix, landmass_classifications)
    # For each polygon find average temperature and precipitation
    for i in range(len(polygon_points)):
        if landmass_classifications[i] == 0:
            biome = 100
            biomes.append(biome)
            polygon = polygon_points[i]
            x_offset = max(0 - overall_min_x, 0)
            y_offset = max(0 - overall_min_y, 0)
            polygon_offset = []
            polygon_offset = [(point[0] + x_offset, point[1] + y_offset) for point in polygon]

            img = Image.new('L', (xpix, ypix), 0)
            im = ImageDraw.Draw(img)
            im.polygon(polygon_offset,fill="#eeeeff", outline="black")
            img_arr = np.array(img)

            mask = np.zeros((ypix, xpix))
            mask[img_arr > 0] = biome
            overall_mask += mask
        else:
            polygon = polygon_points[i]
            x_points = [point[0] for point in polygon]
            y_points = [point[1] for point in polygon]     

            min_polygon_x = int(min(x_points))
            max_polygon_x = int(max(x_points))
            min_polygon_y = int(min(y_points))
            max_polygon_y = int(max(y_points))

            diff_x = max_polygon_x - min_polygon_x
            diff_y = max_polygon_y - min_polygon_y





            x_offset = max(0 - overall_min_x, 0)
            y_offset = max(0 - overall_min_y, 0)
            polygon_offset = []
            polygon_offset = [(point[0] + x_offset, point[1] + y_offset) for point in polygon]

            x_points = [point[0] for point in polygon_offset]
            y_points = [point[1] for point in polygon_offset]


            img = Image.new('L', (xpix, ypix), 0)
            im = ImageDraw.Draw(img)
            im.polygon(polygon_offset,fill="#eeeeff", outline="black")
            img_arr = np.array(img)

            mask = np.zeros((ypix, xpix))
            mask[img_arr > 0] = 1
            # plt.imshow(mask)
            # plt.show()
            # applied_tempmask = np.multiply(mask, tempmap)
            # values = applied_tempmask[np.nonzero(applied_tempmask)]

            t_values = np.zeros(100)
            p_values = np.zeros(100) 

            # Check if the random points are in the polygon
            
            count = 0
            polygon_seed = "{0:b}".format(diff_x+(1<<32)) + "{0:b}".format(diff_y+(1<<32))
            hashed_polygon_seed = int(hashlib.sha256(polygon_seed.encode()).hexdigest(), 16) % (2**32)
            np.random.seed(hashed_polygon_seed)
            while count < 100:
                point = (np.random.randint(int(min(x_points)), int(max(x_points))), np.random.randint(int(min(y_points)), int(max(y_points))))
                point = (np.random.randint(int(min(x_points)), int(max(x_points))), np.random.randint(int(min(y_points)), int(max(y_points))))
                if pnpoly(len(x_points), x_points, y_points, point[0], point[1]) == 1:
                    noise_x = point[0] 
                    noise_y = point[1] 
                    t_value = tempmap[noise_y][noise_x]
                    p_value = precipmap[noise_y][noise_x]
                    t_values[count] = t_value
                    p_values[count] = p_value
                    count += 1

            # Calculate median temperature value for the polygon
            t_average = np.median(t_values)

            # applied_precipmask = np.multiply(mask, precipmap)
            # values = applied_precipmask[np.nonzero(applied_precipmask)]

            # Calculate median precipitation value for the polygon
            p_average = np.median(p_values)

            biome = classify_biome(t_average, p_average)

            # transform = GeometryUtils.mask_transform(mask, spread_rate=0.2)

            # transform[transform < 1/8] = 1/8
            # transform[transform >= 1/8] = 1/6

            mask[mask == 1] = biome
            overall_mask += mask

            biomes.append(biome)

    values = np.array([0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100])
    colors = ['white', 'teal', 'seagreen', 'darkkhaki', 'lightsteelblue', 'yellowgreen', 'darkgoldenrod', 'darkgreen', 'mediumturquoise', 'orange', 'blue']

    cmap = ListedColormap(colors)
    norm = Normalize(vmin=0, vmax=100, clip=True)

    plt.imshow(overall_mask, norm=norm, cmap=cmap)
    plt.gca().invert_yaxis()
    plt.show(block=False)
    return biomes

# nut = random.randint(0, 100)
# chunk_coords = (1,1)    
# polygon_edges, polygon_points, _, _ = get_chunk_polygons((1,1), 20)
# landmass_classifications = [1 for i in range(len(polygon_points))]
# b = determine_biomes(chunk_coords, polygon_edges, polygon_points, landmass_classifications, 21)
# print(b)