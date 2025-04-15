import cv2
import numpy as np
import matplotlib.pyplot as plt



from master_script.master_script import main

if __name__ == "__main__":
    params ={
    "mock_data": False,
    "seed": 23,
    "cx": 0,
    "cy": 0,
    "global_max_height": 100,
    "global_tree_density": 50,
    "ocean_coverage": 50,
    "biome_size": 50,
    "warmth": 50,
    "wetness": 50,
    "debug": False,
    "global_ruggedness": 50,
    "biome": 60,
    "boreal_forest": {
        "plains": {
            "max_height": 30,
            "occurrence_probability": 50,
            "evenness": 50,
            "tree_density": 50
        },
        "hills": {
            "max_height": 40,
            "occurrence_probability": 50,
            "bumpiness": 50,
            "tree_density": 50
        },
        "mountains": {
            "max_height": 70,
            "occurrence_probability": 50,
            "ruggedness": 100,
            "tree_density": 50
        }
    },
    "grassland": {
        "plains": {
            "max_height": 30,
            "occurrence_probability": 50,
            "evenness": 50,
            "tree_density": 50
        },
        "hills": {
            "max_height": 40,
            "occurrence_probability": 50,
            "bumpiness": 50,
            "tree_density": 50
        },
        "rocky_fields": {
            "max_height": 40,
            "occurrence_probability": 50,
            "rockiness": 0.6,
            "tree_density": 50
        },
        "terraced_fields": {
            "max_height": 40,
            "occurrence_probability": 0,
            "size": 0.5,
            "tree_density": 50,
            "smoothness": 0.7,
            "number_of_terraces": 5
        }
    },
    "tundra": {
        "plains": {
            "max_height": 40,
            "occurrence_probability": 50,
            "evenness": 50,
            "tree_density": 50
        },
        "blunt_mountains": {
            "max_height": 100,
            "occurrence_probability": 50,
            "ruggedness": 100,
            "tree_density": 50
        },
        "pointy_mountains": {
            "max_height": 100,
            "occurrence_probability": 50,
            "steepness": 0.8,
            "frequency": 0.5,
            "tree_density": 50
        }
    },
    "savanna": {
        "plains": {
            "max_height": 30,
            "occurrence_probability": 50,
            "evenness": 50,
            "tree_density": 50
        },
        "mountains": {
            "max_height": 50,
            "occurrence_probability": 50,
            "ruggedness": 100,
            "tree_density": 50
        }
    },
    "woodland": {
        "hills": {
            "max_height": 40,
            "occurrence_probability": 50,
            "bumpiness": 50,
            "tree_density": 50
        }
    },
    "tropical_rainforest": {
        "plains": {
            "max_height": 40,
            "occurrence_probability": 50,
            "evenness": 50,
            "tree_density": 50
        },
        "mountains": {
            "max_height": 80,
            "occurrence_probability": 50,
            "ruggedness": 100,
            "tree_density": 50
        },
        "hills": {
            "max_height": 50,
            "occurrence_probability": 50,
            "bumpiness": 50,
            "tree_density": 50
        },
        "volcanoes": {
            "max_height": 60,
            "occurrence_probability": 50,
            "size": 0.6,
            "tree_density": 50,
            "thickness": 0.7,
            "density": 0.3
        }
    },
    "temperate_rainforest": {
        "hills": {
            "max_height": 40,
            "occurrence_probability": 0,
            "bumpiness": 50,
            "tree_density": 50
        },
        "mountains": {
            "max_height": 80,
            "occurrence_probability": 0,
            "ruggedness": 100,
            "tree_density": 50
        },
        "swamp": {
            "max_height": 10,
            "occurrence_probability": 50,
            "wetness": 50,
            "tree_density": 50
        }
    },
    "temperate_seasonal_forest": {
        "hills": {
            "max_height": 40,
            "occurrence_probability": 50,
            "bumpiness": 50,
            "tree_density": 50,
            "autumnal_occurrence": 0.5
        },
        "mountains": {
            "max_height": 80,
            "occurrence_probability": 50,
            "ruggedness": 100,
            "tree_density": 50,
            "autumnal_occurrence": 0.5
        }
    },
    "subtropical_desert": {
        "dunes": {
            "max_height": 30,
            "occurrence_probability": 50,
            "size": 0.5,
            "tree_density": 50,
            "dune_frequency": 0.6,
            "dune_waviness": 0.7,
            "bumpiness": 50
        },
        "mesas": {
            "max_height": 40,
            "occurrence_probability": 50,
            "size": 0.6,
            "tree_density": 50,
            "number_of_terraces": 3,
            "steepness": 0.7
        },
        "ravines": {
            "max_height": 40,
            "occurrence_probability": 50,
            "density": 0.5,
            "tree_density": 50,
            "ravine_width": 0.4,
            "smoothness": 0.3,
            "steepness": 0.8
        },
        "oasis": {
            "max_height": 30,
            "occurrence_probability": 50,
            "size": 0.3,
            "flatness": 0.8,
            "tree_density": 50,
            "dune_frequency": 0.3
        },
        "cracked": {
            "max_height": 30,
            "occurrence_probability": 50,
            "size": 0.5,
            "flatness": 0.6,
            "tree_density": 50
        }
    },
    "ocean": {
        "flat_seabed": {
            "max_height": 90,
            "evenness": 0,
            "occurrence_probability": 50
        },
        "volcanic_islands": {
            "max_height": 20,
            "occurrence_probability": 0,
            "size": 0.4,
            "thickness": 0.5,
            "density": 0.3
        },
        "water_stacks": {
            "max_height": 20,
            "occurrence_probability": 0,
            "size": 0.4
        },
        "trenches": {
            "density": 0.5,
            "occurrence_probability": 0,
            "trench_width": 0.4,
            "smoothness": 0.3
        }
    }
}
    effective_zero_x = 200
    effective_zero_y = 200

    large_heightmap = []  # Will store rows

    for i in range(-1, 0):  # Controls vertical stacking
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

    # plt.show()
