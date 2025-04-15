## Currently Accepted Parameters

See [here](#body) for it in JSON form.


| Parameter | Type | Description |
|-----------|------|-------------|
| **Global Configurations** |||
| `mock_data` | boolean | Whether to use precomputed mock data instead of actually generating superchunks.
| `seed` | int | Seed value for generating terrain |
| `cx` | int | X coordinate of the chunk |
| `cy` | int | Y coordinate of the chunk |
| `biome` | string | Specific biome (nullable) |
| `debug` | boolean | Enable debugging |
| `biome_size` | int | Size of biome regions |
| `ocean_coverage` | int | Percentage of ocean coverage |
| `land_water_scale` | int | World scale/Continent size |
| `global_max_height` | int | Maximum height of terrain globally |
| **Biome Configurations** |||
| `temperate_rainforest` | object | Configuration for this biome |
| &emsp;`max_height` | int | Max height for this biome |
| `boreal_forest` | object | Configuration for this biome |
| &emsp;`flats` | object | Configuration for flats |
| &emsp;&emsp;`max_height` | int | Max height |
| &emsp;`hills` | object | Configuration for hills |
| &emsp;&emsp;`max_height` | int | Max height |
| &emsp;`dla` | object | Configuration for DLA mountains |
| &emsp;&emsp;`max_height` | int | Max height |
| `grassland` | object | Configuration for this biome |
| &emsp;`max_height` | int | Max height for this biome |
| `tundra` | object | Configuration for this biome |
| &emsp;`max_height` | int | Max height for this biome |
| `savanna` | object | Configuration for this biome |
| &emsp;`max_height` | int | Max height for this biome |
| `woodland` | object | Configuration for this biome |
| &emsp;`max_height` | int | Max height for this biome |
| `tropical_rainforest` | object | Configuration for this biome |
| &emsp;`max_height` | int | Max height for this biome |
| `temperate_seasonal_forest` | object | Configuration for this biome |
| &emsp;`max_height` | int | Max height for this biome |
| `subtropical_desert` | object | Configuration for this biome |
| &emsp;`max_height` | int | Max height for this biome |

## Calling Master Script (Server)

### Example Usage
To start the master script server, use the following command:
```sh
python3 -m master_script.master_script_server --host localhost --port 8000
```

Then, send a `POST` request to:
```
http://localhost:8000/superchunk
```

### Request Format
#### Headers:
- `Content-Type: application/json`

#### Body:
```json
{
  "mock_data": false,
  "seed": 23,
  "cx": 0,
  "cy": 0,
  "global_max_height": 100,
  "global_tree_density": 50,
  "ocean_coverage": 50,
  "biome_size": 50,
  "warmth": 50,
  "wetness": 50,
  "debug": false,
  "global_ruggedness": 50,
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
    "selected": true,
    "flat_seabed": {
      "max_height": 50,
      "evenness": 50,
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
      "occurrence_probability": 50,
      "size": 0.4
    },
    "trenches": {
      "density": 0.5,
      "occurrence_probability": 50,
      "trench_width": 0.4,
      "smoothness": 0.3
    }
  }
}
```

If `mock_data` is set to `true`, you will need to request with seed 23 and both cx and cy in the set {-2, -1, 0, 1, 2} (a 5x5 around (0,0)). The mock data returned corresponds to data that would be generated using the example parameters shown above when it comes to biome size etc.. 

### Response Format
#### Headers:
- `Content-Type: application/octet-stream`
- `Content-Disposition: attachment; filename="heightmap_<seed>_<cx>_<cy>.bin"`
- `Content-Length`: Calculated based on response data

#### Body:
- Binary packet data, as with the original master script response.

## Calling Master Script (Command Line)

To run the master script, run:

```sh
python3 -m master_script.master_script --parameters '{"mock_data":false,"seed":13,"cx":0,"cy":0,"global_max_height":100,"global_tree_density":50,"ocean_coverage":50,"biome_size":50,"warmth":50,"wetness":50,"debug":false,"boreal_forest":{"selected":true,"plains":{"max_height":30,"occurrence_probability":50,"evenness":50,"tree_density":50},"hills":{"max_height":40,"occurrence_probability":50,"bumpiness":50,"tree_density":50},"mountains":{"max_height":70,"occurrence_probability":50,"ruggedness":0.6,"tree_density":50}},"grassland":{"selected":true,"plains":{"max_height":30,"occurrence_probability":50,"evenness":50,"tree_density":50},"hills":{"max_height":40,"occurrence_probability":50,"bumpiness":50,"tree_density":50},"rocky_fields":{"max_height":40,"occurrence_probability":50,"rockiness":0.6,"tree_density":50},"terraced_fields":{"max_height":40,"occurrence_probability":0,"size":0.5,"tree_density":50,"smoothness":0.7,"number_of_terraces":5}},"tundra":{"selected":true,"plains":{"max_height":40,"occurrence_probability":50,"evenness":50,"tree_density":50},"blunt_mountains":{"max_height":100,"occurrence_probability":50,"ruggedness":0.7,"tree_density":50},"pointy_mountains":{"max_height":100,"occurrence_probability":50,"steepness":0.8,"frequency":0.5,"tree_density":50}},"savanna":{"selected":true,"plains":{"max_height":30,"occurrence_probability":50,"evenness":50,"tree_density":50},"mountains":{"max_height":50,"occurrence_probability":50,"ruggedness":0.6,"tree_density":50}},"woodland":{"selected":true,"hills":{"max_height":40,"occurrence_probability":50,"bumpiness":50,"tree_density":50}},"tropical_rainforest":{"selected":true,"plains":{"max_height":40,"occurrence_probability":50,"evenness":50,"tree_density":50},"mountains":{"max_height":80,"occurrence_probability":50,"ruggedness":0.7,"tree_density":50},"hills":{"max_height":50,"occurrence_probability":50,"bumpiness":50,"tree_density":50},"volcanoes":{"max_height":60,"occurrence_probability":50,"size":0.6,"tree_density":50,"thickness":0.7,"density":0.3}},"temperate_rainforest":{"selected":true,"hills":{"max_height":40,"occurrence_probability":50,"bumpiness":50,"tree_density":50},"mountains":{"max_height":80,"occurrence_probability":50,"ruggedness":0.6,"tree_density":50},"swamp":{"max_height":30,"occurrence_probability":50,"wetness":0.8,"tree_density":50}},"temperate_seasonal_forest":{"selected":true,"hills":{"max_height":40,"occurrence_probability":50,"bumpiness":50,"tree_density":50,"autumnal_occurrence":0.5},"mountains":{"max_height":80,"occurrence_probability":50,"ruggedness":0.6,"tree_density":50,"autumnal_occurrence":0.5}},"subtropical_desert":{"selected":true,"dunes":{"max_height":30,"occurrence_probability":50,"size":0.5,"tree_density":50,"dune_frequency":0.6,"dune_waviness":0.7,"bumpiness":50},"mesas":{"max_height":40,"occurrence_probability":50,"size":0.6,"tree_density":50,"number_of_terraces":3,"steepness":0.7},"ravines":{"max_height":40,"occurrence_probability":50,"density":0.5,"tree_density":50,"ravine_width":0.4,"smoothness":0.3,"steepness":0.8},"oasis":{"max_height":30,"occurrence_probability":50,"size":0.3,"flatness":0.8,"tree_density":50,"dune_frequency":0.3},"cracked":{"max_height":30,"occurrence_probability":50,"size":0.5,"flatness":0.6,"tree_density":50}},"ocean":{"selected":true,"flat_seabed":{"max_height":50,"evenness":50,"occurrence_probability":50},"volcanic_islands":{"max_height":20,"occurrence_probability":0,"size":0.4,"thickness":0.5,"density":0.3},"water_stacks":{"max_height":20,"occurrence_probability":50,"size":0.4},"trenches":{"density":0.5,"occurrence_probability":50,"trench_width":0.4,"smoothness":0.3}}}'
```