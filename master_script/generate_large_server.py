import cv2
import numpy as np
import matplotlib.pyplot as plt
import requests
import json
from generation import Display

json_text = '''
  {
    "mock_data": false,
    "seed": 13,
    "cx": 0,
    "cy": 0,
    "global_max_height": 100,
    "ocean_coverage": 50,
    "continent_size": 50,
    "biome_size": 50,
    "warmth": 50,
    "wetness": 50,
    "river_frequency": 50,
    "river_width": 50,
    "river_depth": 50,
    "river_meanderiness": 50,
    "debug": false,
    "boreal_forest": {
      "selected": true,
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
      "selected": true,
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
        "occurrence_probability": 0,
        "size": 0.5,
        "tree_density": 0.6,
        "smoothness": 0.7,
        "number_of_terraces": 5
      }
    },
    "tundra": {
      "selected": true,
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
        "occurrence_probability": 0,
        "steepness": 0.8,
        "frequency": 0.5,
        "tree_density": 0.1
      }
    },
    "savanna": {
      "selected": true,
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
      "selected": true,
      "hills": {
        "max_height": 40,
        "occurrence_probability": 0.5,
        "bumpiness": 0.4,
        "tree_density": 0.8
      }
    },
    "tropical_rainforest": {
      "selected": true,
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
        "occurrence_probability": 0,
        "size": 0.6,
        "tree_density": 0.4,
        "thickness": 0.7,
        "density": 0.3
      }
    },
    "temperate_rainforest": {
      "selected": true,
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
        "occurrence_probability": 0,
        "wetness": 0.8,
        "tree_density": 0.9
      }
    },
    "temperate_seasonal_forest": {
      "selected": true,
      "hills": {
        "max_height": 40,
        "occurrence_probability": 0.5,
        "bumpiness": 0.4,
        "tree_density": 0.7,
        "autumnal_occurrence": 0.5
      },
      "mountains": {
        "max_height": 80,
        "occurrence_probability": 0.5,
        "ruggedness": 0.6,
        "tree_density": 0.6,
        "autumnal_occurrence": 0.5
      }
    },
    "subtropical_desert": {
      "selected": true,
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
        "occurrence_probability": 0,
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
        "occurrence_probability": 0,
        "size": 0.3,
        "flatness": 0.8,
        "tree_density": 0.7,
        "dune_frequency": 0.3
      },
      "cracked": {
        "max_height": 30,
        "occurrence_probability": 0,
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
  }'''

center_x = 0  # Set your center x-coordinate
center_y = 0  # Set your center y-coordinate
radius = 2

# Set up headers for JSON content type
headers = {'Content-Type': 'application/json'}

# Parse base JSON text
params = json.loads(json_text)

# Create the large heightmap
large_heightmap = []  # Will store rows

for i in range(center_y - radius, center_y + radius + 1):  # Vertical stacking
    row_heightmaps = []  # Will store horizontally stacked heightmaps

    for j in range(center_x - radius, center_x + radius + 1):  # Horizontal stacking
        # Update cx and cy in the JSON for current request
        params["cx"] = j
        params["cy"] = i

        current_json = json.dumps(params)
        
        # Make the POST request with the raw JSON text
        response = requests.post(
            "http://localhost:8000/superchunk",
            data=current_json,
            headers=headers
        )

        if response.status_code == 200:
            heightmap_data = response.content
            heightmap_data = np.frombuffer(heightmap_data, dtype=np.uint16)
            heightmap_data = heightmap_data.reshape((1026, 1026))
            
            row_heightmaps.append(heightmap_data)
        else:
            print(f"Error with request for chunk ({j}, {i}): {response.status_code}")
            print(response.text)

    if row_heightmaps:
        large_heightmap.append(np.hstack(row_heightmaps))

if large_heightmap:
    heightmap_rows = np.vstack(large_heightmap)
    print("Final heightmap dtype:", heightmap_rows.dtype)
    print("Final heightmap shape:", heightmap_rows.shape)
    print("Final heightmap min:", np.min(heightmap_rows))
    print("Final heightmap max:", np.max(heightmap_rows))

    # Save the final image
    cv2.imwrite("master_script/imgs/combined2.png", heightmap_rows)

    display = Display(heightmap_rows, 1/255, "cliffs")
    display.display_heightmap()
else:
    print("No valid heightmaps were retrieved.")