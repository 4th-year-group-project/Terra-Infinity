import cv2
import numpy as np

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

    for i in range(-2, 0):  # Controls vertical stacking
        row_heightmaps = []  # Will store horizontally stacked heightmaps

        for j in range(-2, 0):  # Controls horizontal stacking
            params["cx"] = j
            params["cy"] = i
            heightmap = main(params)
            row_heightmaps.append(heightmap)

        # Stack all heightmaps in a row horizontally
        large_heightmap.append(np.hstack(row_heightmaps))

    # Stack all rows vertically to get final image
    heightmap_rows = np.vstack(large_heightmap)

    print(heightmap_rows.dtype)

    # Save the final image
    cv2.imwrite("master_script/imgs/combined2.png", heightmap_rows)
