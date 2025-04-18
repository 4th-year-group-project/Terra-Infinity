import cv2
import numpy as np
import matplotlib.pyplot as plt
import requests
import json
from generation import Display

" python3 -m master_script.master_script_server --host localhost --port 8000 "

json_text = '''
{
  "mock_data": false,
  "seed": 13,
  "cx": 0,
  "cy": 0,
  "global_max_height": 100,
  "global_tree_density": 50,
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
  "global_ruggedness": 100,
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
      "rockiness": 50,
      "tree_density": 50
    },
    "terraced_fields": {
      "max_height": 70,
      "occurrence_probability": 0,
      "size": 40,
      "tree_density": 50,
      "smoothness": 50,
      "number_of_terraces": 50
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
      "ruggedness": 50,
      "tree_density": 50
    },
    "pointy_mountains": {
      "max_height": 100,
      "occurrence_probability": 50,
      "steepness": 90,
      "frequency": 90,
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
      "max_height": 100,
      "occurrence_probability": 50,
      "size": 100,
      "tree_density": 50,
      "thickness": 50,
      "density": 80
    }
  },
  "temperate_rainforest": {
    "hills": {
      "max_height": 40,
      "occurrence_probability": 50,
      "bumpiness": 50,
      "tree_density": 50
    },
    "mountains": {
      "max_height": 80,
      "occurrence_probability": 50,
      "ruggedness": 100,
      "tree_density": 50
    },
    "swamp": {
      "max_height": 30,
      "occurrence_probability": 50,
      "wetness": 0.8,
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
      "max_height": 50,
      "occurrence_probability": 50,
      "size": 50,
      "tree_density": 50,
      "dune_frequency": 20,
      "dune_waviness": 80,
      "bumpiness": 50
    },
    "mesas": {
      "max_height": 80,
      "occurrence_probability": 50,
      "size": 50,
      "tree_density": 50,
      "number_of_terraces": 50,
      "steepness": 50
    },
    "ravines": {
      "max_height": 100,
      "occurrence_probability": 50,
      "density": 30,
      "tree_density": 50,
      "ravine_width": 40,
      "smoothness": 50,
      "steepness": 30
    },
    "oasis": {
      "max_height": 60,
      "occurrence_probability": 50,
      "size": 50,
      "flatness": 50,
      "tree_density": 50,
      "dune_frequency": 0
    },
    "cracked": {
      "max_height": 60,
      "occurrence_probability": 50,
      "size": 10,
      "flatness": 50,
      "tree_density": 50
    }
  },
  "ocean": {
    "selected": true,
    "flat_seabed": {
      "max_height": 50,
      "evenness": 50,
      "occurrence_probability": 50
    },
    "volcanic_islands": {
      "max_height": 100,
      "occurrence_probability": 0,
      "size": 50,
      "thickness": 50,
      "density": 50
    },
    "water_stacks": {
      "max_height": 80,
      "occurrence_probability": 50,
      "size": 50
    },
    "trenches": {
      "max_height": 100,
      "density": 10,
      "occurrence_probability": 50,
      "trench_width": 20,
      "smoothness": 20
    }
  }
}
'''

center_x = -10  # Set your center x-coordinate
center_y = 3  # Set your center y-coordinate
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