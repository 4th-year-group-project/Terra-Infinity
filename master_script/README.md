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
python3 -m master_script.master_script --parameters '{"mock_data":false,"seed":13,"cx":0,"cy":0,"global_max_height":100,"ocean_coverage":50,"biome_size":50,"debug":false,"boreal_forest":{"plains":{"max_height":30,"occurrence_probability":0.5,"evenness":0.8,"tree_density":0.6},"hills":{"max_height":40,"occurrence_probability":0.3,"bumpiness":0.5,"tree_density":0.7},"mountains":{"max_height":70,"occurrence_probability":0.2,"ruggedness":0.6,"tree_density":0.4}},"grassland":{"plains":{"max_height":30,"occurrence_probability":0.6,"evenness":0.9,"tree_density":0.8},"hills":{"max_height":40,"occurrence_probability":0.2,"bumpiness":0.4,"tree_density":0.7},"rocky_fields":{"max_height":40,"occurrence_probability":0.1,"rockiness":0.6,"tree_density":0.5},"terraced_fields":{"max_height":40,"occurrence_probability":0,"size":0.5,"tree_density":0.6,"smoothness":0.7,"number_of_terraces":5}},"tundra":{"plains":{"max_height":40,"occurrence_probability":0.5,"evenness":0.8,"tree_density":0.3},"blunt_mountains":{"max_height":100,"occurrence_probability":0.3,"ruggedness":0.7,"tree_density":0.2},"pointy_mountains":{"max_height":100,"occurrence_probability":0,"steepness":0.8,"frequency":0.5,"tree_density":0.1}},"savanna":{"plains":{"max_height":30,"occurrence_probability":0.7,"evenness":0.8,"tree_density":0.5},"mountains":{"max_height":50,"occurrence_probability":0.3,"ruggedness":0.6,"tree_density":0.3}},"woodland":{"hills":{"max_height":40,"occurrence_probability":0.5,"bumpiness":0.4,"tree_density":0.8}},"tropical_rainforest":{"plains":{"max_height":40,"occurrence_probability":0.4,"evenness":0.7,"tree_density":0.9},"mountains":{"max_height":80,"occurrence_probability":0.3,"ruggedness":0.7,"tree_density":0.8},"hills":{"max_height":50,"occurrence_probability":0.2,"bumpiness":0.5,"tree_density":0.9},"volcanoes":{"max_height":60,"occurrence_probability":0,"size":0.6,"tree_density":0.4,"thickness":0.7,"density":0.3}},"temperate_rainforest":{"hills":{"max_height":40,"occurrence_probability":0.4,"bumpiness":0.5,"tree_density":0.8},"mountains":{"max_height":80,"occurrence_probability":0.3,"ruggedness":0.6,"tree_density":0.7},"swamp":{"max_height":30,"occurrence_probability":0.3,"wetness":0.8,"tree_density":0.9}},"temperate_seasonal_forest":{"hills":{"max_height":40,"occurrence_probability":0.5,"bumpiness":0.4,"tree_density":0.7,"autumnal_occurrence":0.5},"mountains":{"max_height":80,"occurrence_probability":0.5,"ruggedness":0.6,"tree_density":0.6,"autumnal_occurrence":0.5}},"subtropical_desert":{"dunes":{"max_height":30,"occurrence_probability":0.4,"size":0.5,"tree_density":0.1,"dune_frequency":0.6,"dune_waviness":0.7,"bumpiness":0.4},"mesas":{"max_height":40,"occurrence_probability":0.2,"size":0.6,"tree_density":0.1,"number_of_terraces":3,"steepness":0.7},"ravines":{"max_height":40,"occurrence_probability":0.2,"density":0.5,"tree_density":0.2,"ravine_width":0.4,"smoothness":0.3,"steepness":0.8},"oasis":{"max_height":30,"occurrence_probability":0.1,"size":0.3,"flatness":0.8,"tree_density":0.7,"dune_frequency":0.3},"cracked":{"max_height":30,"occurrence_probability":0.1,"size":0.5,"flatness":0.6,"tree_density":0.05}},"ocean":{"flat_seabed":{"max_height":50,"evenness":0.8,"occurrence_probability":0.6},"volcanic_islands":{"max_height":20,"occurrence_probability":0,"size":0.4,"thickness":0.5,"density":0.3},"water_stacks":{"max_height":20,"occurrence_probability":0,"size":0.4},"trenches":{"density":0.5,"occurrence_probability":0,"trench_width":0.4,"smoothness":0.3}}}'
```
