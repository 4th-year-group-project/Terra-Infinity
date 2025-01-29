from perlin_noise import PerlinNoise
import matplotlib.pyplot as plt
import numpy as np
import random
import cv2
from PIL import ImageDraw, ImagePath, Image

from biomes.create_voronoi import get_chunk_polygons


def classify_biome(temp, precip):
    biomes = ["temperate rainforest", "boreal forest", "grassland", "tundra", "savanna", "woodland", "tropical rainforest", "temperate seasonal forest", "subtropical desert"]
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
  c = 0
  j = nvert-1
  for i in range(nvert):
    if ((verty[i]>testy) != (verty[j]>testy)) and (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]):
       c = not c
    j = i
  return c;

def determine_biomes(ploygon_edges, polygon_points, landmass_classifications, seed):

    x_points = [point[k][0] for point in polygon_points for k in range(len(point))]
    y_points = [point[k][1] for point in polygon_points for k in range(len(point))]

    overall_min_x = min(x_points)
    overall_max_x = max(x_points)
    overall_min_y = min(y_points)
    overall_max_y = max(y_points)

    xpix, ypix = 100, 100
    tempmap = np.zeros((xpix, ypix))
    precipmap = np.zeros((xpix, ypix))
    for i in range(xpix):
        for j in range(ypix):
            noise_val = t_noise1([i/xpix, j/ypix])
            tempmap[i][j] = noise_val

            noise_val = p_noise1([i/xpix, j/ypix])
            precipmap[i][j] = noise_val



    # scale up the tempmap using interpolation
    tempmap = cv2.resize(tempmap, (int(np.ceil(overall_max_x - overall_min_x)), int(np.ceil(overall_max_y - overall_min_y))), interpolation=cv2.INTER_LINEAR)
    precipmap = cv2.resize(precipmap, (int(np.ceil(overall_max_x - overall_min_x)), int(np.ceil(overall_max_y - overall_min_y))), interpolation=cv2.INTER_LINEAR)
    plt.imshow(tempmap, cmap='gray')
    plt.show()
    plt.imshow(precipmap, cmap='gray')
    plt.show()
    xpix, ypix = int(np.ceil(overall_max_x - overall_min_x)), int(np.ceil(overall_max_y - overall_min_y))

    biomes = np.zeros((xpix, ypix))
    biomes = []

    for region in polygon_edges:
        for i in range(len(region)):
            x1, y1 = region[i][0]
            x2, y2 = region[i][1]

            plt.plot([x1, x2], [y1, y2], 'r-')
    plt.show()

    overall_mask = np.zeros((ypix, xpix))
    # for each polygon find average height within it
    for i in range(len(polygon_points)):
        if landmass_classifications[i] == 0:
            biome = 100
            biomes.append(biome)
        else:
            polygon = polygon_points[i]
            x_points = [point[0] for point in polygon]
            y_points = [point[1] for point in polygon]

            x_offset = max(0 - overall_min_x, 0)
            y_offset = max(0 - overall_min_y, 0)
            polygon_offset = []
            polygon_offset = [(point[0] + x_offset, point[1] + y_offset) for point in polygon]

            img = Image.new('L', (xpix, ypix), 0)
            im = ImageDraw.Draw(img)
            im.polygon(polygon_offset,fill="#eeeeff", outline="blue")
            img_arr = np.array(img)

            mask = np.zeros((ypix, xpix))
            mask[img_arr > 0] = 1
            applied_tempmask = np.multiply(mask, tempmap)
            values = applied_tempmask[np.nonzero(applied_tempmask)]
            t_average = np.median(values)
            #t_average = np.sum(applied_tempmask) / np.count_nonzero(mask)
            #print(t_average)

            applied_precipmask = np.multiply(mask, precipmap)
            # get non zero values
            values = applied_precipmask[np.nonzero(applied_precipmask)]

            p_average = np.median(values)
            #p_average = np.sum(applied_precipmask) / np.count_nonzero(mask)
            #print(p_average)

            biome = classify_biome(t_average, p_average)
            mask[mask == 1] = biome
            # plt.imshow(mask, cmap='gray')
            # plt.show()
            print(biome)
            overall_mask += mask

            biomes.append(biome)


            

                


    plt.imshow(overall_mask, cmap='turbo')
    plt.gca().invert_yaxis()
    plt.show()

nut = random.randint(0, 100)
print(nut)
t_noise1 = PerlinNoise(octaves=2, seed=98)

nut = random.randint(0, 100)
print(nut)
p_noise1 = PerlinNoise(octaves=2, seed=2)


nut = random.randint(0, 100)
print(nut)
polygon_edges, polygon_points = get_chunk_polygons((0,0), 36)
print(polygon_points)
landmass_classifications = [1 for i in range(len(polygon_points))]
determine_biomes(polygon_edges, polygon_points, landmass_classifications, nut)