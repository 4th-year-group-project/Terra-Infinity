"""Classifies biomes for each polygon"""
import hashlib

import numpy as np
from PIL import Image, ImageDraw

from generation import Noise


def zero_preserving_softmax(x):
    """Compute softmax values for values in x, preserving zeros.
    
    Args:
        x: input array

    Returns:
        result: softmax values
    """
    x = np.array(x, dtype=float)
    mask = (x != 0)

    result = np.zeros_like(x, dtype=float)

    if np.any(mask):
        x_masked = x[mask]
        e_x = np.exp(x_masked - np.max(x_masked))
        softmax_masked = e_x / e_x.sum()

        result[mask] = softmax_masked

    return result

def determine_subbiome(biome, parameters, seed):
    """Determine the sub-biome of a biome based on its classification.

    Args:
        biome: biome classification

    Returns:
        subbiome: sub-biome classification
    """
    np.random.seed(seed)

    if biome == 1:
        softmax_probabilities = zero_preserving_softmax([
        parameters.get("boreal_forest").get("plains").get("occurrence_probability", 50),
        parameters.get("boreal_forest").get("hills").get("occurrence_probability", 50),
        parameters.get("boreal_forest").get("mountains").get("occurrence_probability", 50)
        ])

        choice = np.random.choice([1, 2, 3], p=softmax_probabilities)

        return choice
    elif biome == 10:
        softmax_probabilities = zero_preserving_softmax([
        parameters.get("grassland").get("plains").get("occurrence_probability", 25),
        parameters.get("grassland").get("hills").get("occurrence_probability", 25),
        parameters.get("grassland").get("rocky_fields").get("occurrence_probability", 25),
        parameters.get("grassland").get("terraced_fields").get("occurrence_probability", 25)
        ])

        choice = np.random.choice([10, 11, 12, 13], p=softmax_probabilities)

        return choice
    elif biome == 20:
        softmax_probabilities = zero_preserving_softmax([
        parameters.get("tundra").get("plains").get("occurrence_probability", 50),
        parameters.get("tundra").get("blunt_mountains").get("occurrence_probability", 50),
        parameters.get("tundra").get("pointy_mountains").get("occurrence_probability", 50),
        ])

        choice = np.random.choice([20, 21, 22], p=softmax_probabilities)

        return choice
    elif biome == 30:
        softmax_probabilities = zero_preserving_softmax([
        parameters.get("savanna").get("plains").get("occurrence_probability", 50),
        parameters.get("savanna").get("mountains").get("occurrence_probability", 50)
        ])

        choice = np.random.choice([30, 31], p=softmax_probabilities)

        return choice
    elif biome == 40:
        return 40

    elif biome == 50:
        softmax_probabilities = zero_preserving_softmax([
        parameters.get("tropical_rainforest").get("plains").get("occurrence_probability", 50),
        parameters.get("tropical_rainforest").get("mountains").get("occurrence_probability", 50),
        parameters.get("tropical_rainforest").get("volcanoes").get("occurrence_probability", 50),
        parameters.get("tropical_rainforest").get("hills").get("occurrence_probability", 50)
        ])

        choice = np.random.choice([50, 51, 52, 53], p=softmax_probabilities)

        return choice

    elif biome == 60:
        softmax_probabilities = zero_preserving_softmax([
        parameters.get("temperate_rainforest").get("hills").get("occurrence_probability", 50),
        parameters.get("temperate_rainforest").get("mountains").get("occurrence_probability", 50),
        parameters.get("temperate_rainforest").get("swamp").get("occurrence_probability", 50),
        ])

        choice = np.random.choice([60, 61, 62], p=softmax_probabilities)

        return choice
    elif biome == 70:
        softmax_probabilities = zero_preserving_softmax([
        parameters.get("temperate_seasonal_forest").get("hills").get("occurrence_probability", 50),
        parameters.get("temperate_seasonal_forest").get("mountains").get("occurrence_probability", 50)
        ])

        choice = np.random.choice([72, 73], p=softmax_probabilities)

        if choice == 72:
            autumnul_hills = parameters.get("temperate_seasonal_forest").get("hills").get("autumnal_occurrence", 50)
            softmax_probabilities = zero_preserving_softmax([autumnul_hills, 1-autumnul_hills])
            choice = np.random.choice([70, 72], p= softmax_probabilities)
        else:
            autumnul_mountains = parameters.get("temperate_seasonal_forest").get("mountains").get("autumnal_occurrence", 50)
            softmax_probabilities = zero_preserving_softmax([autumnul_mountains, 1-autumnul_mountains])
            choice = np.random.choice([71, 73], p= softmax_probabilities)
        return choice
    elif biome == 80:
        softmax_probabilities = zero_preserving_softmax([
        parameters.get("subtropical_desert").get("mesas").get("occurrence_probability", 50),
        parameters.get("subtropical_desert").get("dunes").get("occurrence_probability", 50),
        parameters.get("subtropical_desert").get("oasis").get("occurrence_probability", 50),
        parameters.get("subtropical_desert").get("ravines").get("occurrence_probability", 50),
        parameters.get("subtropical_desert").get("cracked").get("occurrence_probability", 50)
        ])

        choice = np.random.choice([80, 81, 82, 83, 84], p=softmax_probabilities)
        return choice
    else:
        softmax_probabilities = zero_preserving_softmax([
        parameters.get("ocean").get("flat_seabed").get("occurrence_probability", 50),
        parameters.get("ocean").get("trenches").get("occurrence_probability", 50),
        parameters.get("ocean").get("volcanic_islands").get("occurrence_probability", 50),
        parameters.get("ocean").get("water_stacks").get("occurrence_probability", 50)
        ])

        choice = np.random.choice([90, 91, 92, 93], p=softmax_probabilities)

        return choice

def classify_biome(temp, precip, parameters, seed, wanted_biomes):
    """Classify a biome based on temperature and precipitation values using the Whittaker diagram. Values are normally close to 0 with -1 and 1 being rare to occur.

    Biomes = boreal forest, grassland, tundra, savanna, woodland, tropical rainforest, temperate rainforest, temperate seasonal forest, subtropical desert

    Args:
        temp: temperature value between -1 and 1
        precip: precipitation value between -1 and 1
        parameters: biome parameters
        seed: the world seed
        wanted_biomes: list of biomes to classify

    Returns:
        biome: biome classification
    """

    biomes = [1, 10, 20, 30, 40, 50, 60 ,70 ,80]
    biome_values = [[-0.15, 0.05], [-0.05, -0.1], [-0.25, -0.05], [0.25, 0.15], [-0.05, -0.05], [0.3, 0.18], [0.22, 0.2], [0, 0], [0.28, -0.3]]

    # Remove biomes that are not wanted
    biomes = [biomes[i] for i in range(len(biomes)) if wanted_biomes[i] == 1]
    biome_values = [biome_values[i] for i in range(len(biome_values)) if wanted_biomes[i] == 1]

    # Find the closest biome point to the temperature and precipitation point
    smallest_dist = np.inf
    for i in range(len(biome_values)):
        dist = np.sqrt((temp - biome_values[i][0])**2 + (precip - biome_values[i][1])**2)
        if dist < smallest_dist:
            smallest_dist = dist
            biome = biomes[i]
    return biome

def in_polygon(nvert, vertx, verty, testx, testy):
    """Determine if a point is inside a polygon
  
    Args:
        nvert: number of vertices in the polygon
        vertx: x coordinates of the vertices
        verty: y coordinates of the vertices
        testx: x coordinate of the test point
        testy: y coordinate of the test point

    Returns:
        c: 1 if the point is inside the polygon, 0 otherwise
    """
    c = 0
    j = nvert-1
    for i in range(nvert):
        if ((verty[i]>testy) != (verty[j]>testy)) and (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]):
            c = not c
        j = i
    return c


def determine_biomes(chunk_coords, polygon_edges, polygon_points, landmass_classifications, offsets,  seed, parameters, specified_biome=None, chunk_size=1024):
    """Determine the biome of each polygon using a temperature and precipitation map

    Args:
        chunk_coords: coordinates of the chunk
        polygon_edges: list of edges of the polygons
        polygon_points: list of points of the polygons
        landmass_classifications: list of classifications of the landmasses
        offsets: offsets of the chunk
        seed: the world seed
        parameters: biome parameters
        specified_biome: biome to use if specified
        chunk_size: size of the chunk

    Returns:
        biomes: list of biomes for each polygon
        mask: mask of the biomes
    """

    (offset_x, offset_y) = offsets

    # Get warmth and wetness values from parameters and normalise to be between -0.5 and 0.5
    warmth = parameters.get("warmth", 50)
    min_size = -0.5
    max_size = 0.5
    normalised_warmth = ((warmth / 100) * (max_size - min_size)) + min_size

    wetness = parameters.get("wetness", 50)
    min_size = -0.5
    max_size = 0.5
    normalised_wetness = ((wetness / 100) * (max_size - min_size)) + min_size

    # Get which biomes are selected
    temperate_rainforest = parameters.get("temperate_rainforest").get("selected", True)
    boreal_forest = parameters.get("boreal_forest").get("selected", True)
    grassland = parameters.get("grassland").get("selected", True)
    tundra = parameters.get("tundra").get("selected", True)
    savanna = parameters.get("savanna").get("selected", True)
    woodland = parameters.get("woodland").get("selected", True)
    tropical_rainforest = parameters.get("tropical_rainforest").get("selected", True)
    temperate_forest = parameters.get("temperate_seasonal_forest").get("selected", True)
    desert = parameters.get("subtropical_desert").get("selected", True)

    wanted_biomes = [boreal_forest, grassland, tundra, savanna, woodland, tropical_rainforest, temperate_rainforest, temperate_forest, desert]

    # Get the coordinates of the polygons
    x_points = [point[k][0] for point in polygon_points for k in range(len(point))]
    y_points = [point[k][1] for point in polygon_points for k in range(len(point))]

    # Get the minimum and maximum x and y coordinates of the polygons
    overall_min_x = min(x_points)
    overall_max_x = max(x_points)
    overall_min_y = min(y_points)
    overall_max_y = max(y_points)

    xpix, ypix = 100, 100
    tempmap = np.zeros((xpix, ypix))
    precipmap = np.zeros((xpix, ypix))

    # Get the coordinates of the chunk
    min_x = chunk_coords[0] * chunk_size
    min_y = chunk_coords[1] * chunk_size

    xpix, ypix = int(np.ceil(overall_max_x - overall_min_x)), int(np.ceil(overall_max_y - overall_min_y))

    # Get chunk seed
    chunk_seed = f"{seed:b}" + f"{min_x+(1<<32):b}" + f"{min_y+(1<<32):b}"
    hashed_seed = int(hashlib.sha256(chunk_seed.encode()).hexdigest(), 16) % (2**32)

    np.random.seed(hashed_seed)

    # Create temperature and precipitation noise maps
    noise = Noise(seed=seed, width=int(xpix), height=int(ypix))

    tempmap = noise.fractal_simplex_noise(seed=seed, noise="open", x_offset=int(offset_x), y_offset=int(offset_y), scale=1200, octaves=5, persistence=0.5, lacunarity=2)
    tempmap = (tempmap/2) + normalised_warmth

    precipmap = noise.fractal_simplex_noise(seed=seed+1, noise="open", x_offset=int(offset_x), y_offset=int(offset_y), scale=1200, octaves=5, persistence=0.5, lacunarity=2)
    precipmap = (precipmap/2) + normalised_wetness

    biomes = np.zeros((xpix, ypix))
    biomes = []

    # Create a mask for the biomes
    mask = np.zeros((4500, 4500))

    # For each polygon find average temperature and precipitation
    for i in range(len(polygon_points)):
        if landmass_classifications[i] == 0:
            biome = 90
            polygon = polygon_points[i]
            x_points = [point[0] for point in polygon]
            y_points = [point[1] for point in polygon]

            min_polygon_x = int(np.floor(min(x_points)))
            max_polygon_x = int(np.ceil(max(x_points)))
            min_polygon_y = int(np.floor(min(y_points)))
            max_polygon_y = int(np.ceil(max(y_points)))

            diff_x = max_polygon_x - min_polygon_x
            diff_y = max_polygon_y - min_polygon_y
            polygon_seed = f"{diff_x+(1<<32):b}" + f"{diff_y+(1<<32):b}"
            hashed_polygon_seed = int(hashlib.sha256(polygon_seed.encode()).hexdigest(), 16) % (2**32)
            sub_biome = determine_subbiome(biome, parameters, hashed_polygon_seed)
            sub_biome = np.uint8(sub_biome)
            biomes.append(sub_biome)

            polygon = polygon_points[i]

            polygon_tupled = [(point[0], point[1]) for point in polygon]

            img = Image.new("L", (4500, 4500), 0)
            im = ImageDraw.Draw(img)
            im.polygon(polygon_tupled,fill="#eeeeff", outline="black")
            img_arr = np.array(img)

            mask[img_arr > 0] = sub_biome

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

            polygon_seed = f"{diff_x+(1<<32):b}" + f"{diff_y+(1<<32):b}"
            hashed_polygon_seed = int(hashlib.sha256(polygon_seed.encode()).hexdigest(), 16) % (2**32)

            # Check if the random points are in the polygon
            count = 0

            checked_points = set()
            np.random.seed(hashed_polygon_seed)
            while count < 100:
                point = (np.random.randint(int(min(x_points)), int(max(x_points))), np.random.randint(int(min(y_points)), int(max(y_points))))
                point = (np.random.randint(int(min(x_points)), int(max(x_points))), np.random.randint(int(min(y_points)), int(max(y_points))))
                if in_polygon(len(x_points), x_points, y_points, point[0], point[1]) == 1 and point not in checked_points:
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

            # Find general biome classification
            biome = classify_biome(t_average, p_average, parameters, hashed_polygon_seed, wanted_biomes)

            # Determine sub-biome
            if specified_biome is None:
                sub_biome = determine_subbiome(biome, parameters, hashed_polygon_seed)
            else:
                sub_biome = specified_biome

            sub_biome = np.uint8(sub_biome)
            mask[img_arr > 0] = sub_biome

            biomes.append(sub_biome)

    return biomes, mask
