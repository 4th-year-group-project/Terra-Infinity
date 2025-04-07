import cv2
import numpy as np
import matplotlib.pyplot as plt

from master_script.master_script import main

if __name__ == "__main__":
    params = {
        "seed": 23,
        "cx": 100,
        "cy": 100,
        "biome": None,
        "debug": True,
        "biome_size": 50,
        "ocean_coverage": 50,
        "land_water_scale": 50,
        "global_max_height": 100,
        "temperate_rainforest": {
            "max_height": 30,
        },
        "boreal_forest": {
            "max_height": 40,
        },
        "grassland": {
            "max_height": 40,
        },
        "tundra": {
            "max_height": 50,
        },
        "savanna": {
            "max_height": 25,
        },
        "woodland": {
            "max_height": 40,
        },
        "tropical_rainforest": {
            "max_height": 35,
        },
        "temperate_seasonal_forest": {
            "max_height": 100,
        },
        "subtropical_desert": {
            "max_height": 30,
        },
    }
    effective_zero_x = 200
    effective_zero_y = 200

    large_heightmap = []  # Will store rows
    large_biomemap = []  # Will store rows

    for i in range(-2, 3):  # Controls vertical stacking
        row_heightmaps = []  # Will store horizontally stacked heightmaps
        row_biomemaps = []

        for j in range(-2, 3):  # Controls horizontal stacking
            params["cx"] = j
            params["cy"] = i
            heightmap, biome = main(params)
            row_heightmaps.append(heightmap)
            row_biomemaps.append(biome)


        # Stack all heightmaps in a row horizontally
        large_heightmap.append(np.hstack(row_heightmaps))
        large_biomemap.append(np.hstack(row_biomemaps))

    # Stack all rows vertically to get final image
    heightmap_rows = np.vstack(large_heightmap)
    biomemap_rows = np.vstack(large_biomemap)
    biomemap_rows = biomemap_rows.astype(np.uint8)
    biomemap_rows *= 10

    print(heightmap_rows.dtype)

    # Save the final image
    cv2.imwrite("master_script/imgs/combined2.png", heightmap_rows)
    cv2.imwrite("master_script/imgs/biome_combined.png", biomemap_rows)

    biome_id = 100
    height_threshold = 0.2 * 65535
    count = np.sum((biomemap_rows == biome_id) & (heightmap_rows > height_threshold))
    print(f"Number of pixels with biome id {biome_id} and height > {height_threshold}: {count}")

    #plt imshow them on same fig
    plt.figure(figsize=(10, 10))
    plt.subplot(1, 2, 1)
    plt.imshow(heightmap_rows, cmap='gray')
    plt.title("Heightmap")
    plt.subplot(1, 2, 2)
    plt.imshow(biomemap_rows, cmap='gray')
    plt.title("Biome Map")
    plt.show()

