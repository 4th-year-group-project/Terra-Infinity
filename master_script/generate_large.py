import cv2
import numpy as np

from master_script.master_script import main

if __name__ == "__main__":
    params = {
  "seed": 13,
  "cx": 0,
  "cy": 0,
  "global_max_height": 100,
  "ocean_coverage": 50,
  "biome_size": 50,
  "warmth": 75,
  "wetness": 25,
  "debug": True,
  "boreal_forest": {
    "plains": {"max_height": 30},
    "hills": {"max_height": 40},
    "dla": {"max_height": 70}
  },
  "grassland": {
    "plains": {"max_height": 30},
    "hills": {"max_height": 40},
    "rocky_fields": {"max_height": 40},
    "terraced_fields": {"max_height": 40, "occurrence": 0}
  },
  "tundra": {
    "plains": {"max_height": 40},
    "blunt_mountains": {"max_height": 100},
    "pointy_mountains": {"max_height": 100}
  },
  "savanna": {
    "plains": {"max_height": 30},
    "mountains": {"max_height": 50}
  },
  "woodland": {
    "hills": {"max_height": 40}
  },
  "tropical_rainforest": {
    "flats": {"max_height": 40},
    "mountains": {"max_height": 80},
    "hills": {"max_height": 50},
    "volcanoes": {"max_height": 60}
  },
  "temperate_rainforest": {
    "hills": {"max_height": 40},
    "mountains": {"max_height": 80},
    "swamp": {"max_height": 30}
  },
  "temperate_seasonal_forest": {
    "hills": {"max_height": 40},
    "mountains": {"max_height": 80}
  },
  "subtropical_desert": {
    "selected": False,
    "dunes": {"max_height": 30},
    "mesas": {"max_height": 40},
    "ravines": {"max_height": 40},
    "oasis": {"max_height": 30, "occurrence": 0},
    "cracked": {"max_height": 30}
  },
  "ocean": {
    "seabed": {"max_height": 50},
    "volcanic_islands": {"max_height": 20},
    "water_stacks": {"max_height": 20},
    "trenches": {}
  }
}
    effective_zero_x = 200
    effective_zero_y = 200

    large_heightmap = []  # Will store rows

    for i in range(-3, -1):  # Controls vertical stacking
        row_heightmaps = []  # Will store horizontally stacked heightmaps

        for j in range(-3, -1):  # Controls horizontal stacking
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
