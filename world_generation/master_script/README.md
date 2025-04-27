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
  "continent_size": 50,
  "biome_size": 50,
  "warmth": 50,
  "wetness": 50,
  "river_frequency": 50,
  "river_width": 50,
  "river_depth": 50,
  "river_meanderiness": 50,
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
      "rockiness": 50,
      "tree_density": 50
    },
    "terraced_fields": {
      "max_height": 40,
      "occurrence_probability": 30,
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
      "ruggedness": 100,
      "tree_density": 50
    },
    "pointy_mountains": {
      "max_height": 100,
      "occurrence_probability": 50,
      "steepness": 80,
      "frequency": 80,
      "tree_density": 20
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
      "size": 100,
      "tree_density": 50,
      "thickness": 60,
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
      "wetness": 80,
      "tree_density": 50
    }
  },
  "temperate_seasonal_forest": {
    "hills": {
      "max_height": 40,
      "occurrence_probability": 50,
      "bumpiness": 50,
      "tree_density": 50,
      "autumnal_occurrence": 50
    },
    "mountains": {
      "max_height": 80,
      "occurrence_probability": 50,
      "ruggedness": 100,
      "tree_density": 50,
      "autumnal_occurrence": 50
    }
  },
  "subtropical_desert": {
    "dunes": {
      "max_height": 50,
      "occurrence_probability": 50,
      "size": 50,
      "tree_density": 50,
      "dune_frequency": 30,
      "dune_waviness": 80,
      "bumpiness": 50
    },
    "mesas": {
      "max_height": 90,
      "occurrence_probability": 50,
      "size": 50,
      "tree_density": 50,
      "number_of_terraces": 50,
      "steepness": 50
    },
    "ravines": {
      "max_height": 90,
      "occurrence_probability": 50,
      "density": 50,
      "tree_density": 50,
      "ravine_width": 40,
      "smoothness": 50,
      "steepness": 30
    },
    "oasis": {
      "max_height": 30,
      "occurrence_probability": 50,
      "size": 50,
      "flatness": 50,
      "tree_density": 50,
      "dune_frequency": 0
    },
    "cracked": {
      "max_height": 70,
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
      "max_height": 50,
      "occurrence_probability": 50,
      "size": 50,
      "thickness": 50,
      "density": 50
    },
    "water_stacks": {
      "max_height": 20,
      "occurrence_probability": 50,
      "size": 50
    },
    "trenches": {
      "max_height": 50,
      "density": 10,
      "occurrence_probability": 50,
      "trench_width": 20,
      "smoothness": 20
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
The body itself is treated like a packet, with a "header" and a "body".

The body's headers are:

| Field              | Type   | Description                                |
|--------------------|--------|--------------------------------------------|
| `seed`             | `int32`| World generation seed                     |
| `cx`               | `int32`| Chunk X coordinate                        |
| `cy`               | `int32`| Chunk Y coordinate                        |
| `num_v`            | `int32`| Number of vertices (pixels)                 |
| `vx`               | `int32`| Number of pixels in x direction       |
| `vy`               | `int32`| Number of pixels in y direction        |
| `size`             | `int32`| Bit depth of heightmap data                       |
| `heightmap_len`    | `int32`| Length of heightmap data        |
| `biome_size`       | `int32`| Bit depth of biome data                     |
| `biome_len`        | `int32`| Length of biome data           |
| `size2`            | `int32`| Bit depth of tree data|
| `tree_placements_len` | `int32`| Length of tree placement data |

The body's body is:

1. **Heightmap** (`uint16`):  
   A 2D array of shape `(vy, vx)` representing terrain heights.

2. **Biome Data** (`uint8`):  
   A 2D array of shape `(vy, vx)` representing biome types.

3. **Tree Placements** (`float16`):  
   An array of coords represting tree placement locations.
