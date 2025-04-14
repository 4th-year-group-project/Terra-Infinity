import cv2
import numpy as np
import matplotlib.pyplot as plt



from master_script.master_script import main

if __name__ == "__main__":
    params = {
    "mock_data": False,
    "seed": 13,
    "cx": 0,
    "cy": 0,
    "global_max_height": 100,
    "ocean_coverage": 0,
    "wetness": 10,
    "warmth": 10,
    "biome_size": 50,
    "debug": True,
    "boreal_forest": {
        "plains": {
        "max_height": 30,
        "occurrence_probability": 0.5,
        "evenness": 0.8,
        "tree_density": 0.6
        },
        "hills": {
        "max_height": 40,
        "occurrence_probability": 0.3,
        "bumpiness": 0.5,
        "tree_density": 0.7
        },
        "mountains": {
        "max_height": 70,
        "occurrence_probability": 0.2,
        "ruggedness": 0.6,
        "tree_density": 0.4
        }
    },
    "grassland": {
        "plains": {
        "max_height": 30,
        "occurrence_probability": 0.6,
        "evenness": 0.9,
        "tree_density": 0.8
        },
        "hills": {
        "max_height": 40,
        "occurrence_probability": 0.2,
        "bumpiness": 0.4,
        "tree_density": 0.7
        },
        "rocky_fields": {
        "max_height": 40,
        "occurrence_probability": 0.1,
        "rockiness": 0.6,
        "tree_density": 0.5
        },
        "terraced_fields": {
        "max_height": 40,
        "occurrence_probability": 0.1,
        "size": 0.5,
        "tree_density": 0.6,
        "smoothness": 0.7,
        "number_of_terraces": 5
        }
    },
    "tundra": {
        "plains": {
        "max_height": 40,
        "occurrence_probability": 0.5,
        "evenness": 0.8,
        "tree_density": 0.3
        },
        "blunt_mountains": {
        "max_height": 100,
        "occurrence_probability": 0.3,
        "ruggedness": 0.7,
        "tree_density": 0.2
        },
        "pointy_mountains": {
        "max_height": 100,
        "occurrence_probability": 0.2,
        "steepness": 0.8,
        "frequency": 0.5,
        "tree_density": 0.1
        }
    },
    "savanna": {
        "plains": {
        "max_height": 30,
        "occurrence_probability": 0.7,
        "evenness": 0.8,
        "tree_density": 0.5
        },
        "mountains": {
        "max_height": 50,
        "occurrence_probability": 0.3,
        "ruggedness": 0.6,
        "tree_density": 0.3
        }
    },
    "woodland": {
        "hills": {
        "max_height": 40,
        "occurrence_probability": 0.5,
        "bumpiness": 0.4,
        "tree_density": 0.8
        }
    },
    "tropical_rainforest": {
        "plains": {
        "max_height": 40,
        "occurrence_probability": 0.4,
        "evenness": 0.7,
        "tree_density": 0.9
        },
        "mountains": {
        "max_height": 80,
        "occurrence_probability": 0.3,
        "ruggedness": 0.7,
        "tree_density": 0.8
        },
        "hills": {
        "max_height": 50,
        "occurrence_probability": 0.2,
        "bumpiness": 0.5,
        "tree_density": 0.9
        },
        "volcanoes": {
        "max_height": 60,
        "occurrence_probability": 0.1,
        "size": 0.6,
        "tree_density": 0.4,
        "thickness": 0.7,
        "density": 0.3
        }
    },
    "temperate_rainforest": {
        "hills": {
        "max_height": 40,
        "occurrence_probability": 0.4,
        "bumpiness": 0.5,
        "tree_density": 0.8
        },
        "mountains": {
        "max_height": 80,
        "occurrence_probability": 0.3,
        "ruggedness": 0.6,
        "tree_density": 0.7
        },
        "swamp": {
        "max_height": 30,
        "occurrence_probability": 0.3,
        "wetness": 0.8,
        "tree_density": 0.9
        }
    },
    "temperate_seasonal_forest": {
        "hills": {
        "max_height": 40,
        "occurrence_probability": 0.5,
        "bumpiness": 0.4,
        "tree_density": 0,
        "autumnal_occurrence": 0.5
        },
        "mountains": {
        "max_height": 80,
        "occurrence_probability": 0.5,
        "ruggedness": 0.6,
        "tree_density": 0,
        "autumnal_occurrence": 0.5
        }
    },
    "subtropical_desert": {
        "dunes": {
        "max_height": 30,
        "occurrence_probability": 0.4,
        "size": 0.5,
        "tree_density": 0.1,
        "dune_frequency": 0.6,
        "dune_waviness": 0.7,
        "bumpiness": 0.4
        },
        "mesas": {
        "max_height": 40,
        "occurrence_probability": 0.2,
        "size": 0.6,
        "tree_density": 0.1,
        "number_of_terraces": 3,
        "steepness": 0.7
        },
        "ravines": {
        "max_height": 40,
        "occurrence_probability": 0.2,
        "density": 0.5,
        "tree_density": 0.2,
        "ravine_width": 0.4,
        "smoothness": 0.3,
        "steepness": 0.8
        },
        "oasis": {
        "max_height": 30,
        "occurrence_probability": 0.1,
        "size": 0.3,
        "flatness": 0.8,
        "tree_density": 0.7,
        "dune_frequency": 0.3
        },
        "cracked": {
        "max_height": 30,
        "occurrence_probability": 0.1,
        "size": 0.5,
        "flatness": 0.6,
        "tree_density": 0.05
        }
    },
    "ocean": {
        "flat_seabed": {
        "max_height": 50,
        "evenness": 0.8,
        "occurrence_probability": 0.6
        },
        "volcanic_islands": {
        "max_height": 20,
        "occurrence_probability": 0.1,
        "size": 0.4,
        "thickness": 0.5,
        "density": 0.3
        },
        "water_stacks": {
        "max_height": 20,
        "occurrence_probability": 0.1,
        "size": 0.4
        },
        "trenches": {
        "density": 0.5,
        "occurrence_probability": 0.2,
        "trench_width": 0.4,
        "smoothness": 0.3
        }
    }
    }
    effective_zero_x = 200
    effective_zero_y = 200

    large_heightmap = []  # Will store rows

    for i in range(-2, 1):  # Controls vertical stacking
        row_heightmaps = []  # Will store horizontally stacked heightmaps

        for j in range(-2, 1):  # Controls horizontal stacking
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

    # plt.show()
