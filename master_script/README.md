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
| &emsp;`max_height` | int | Max height for this biome |
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
    "mock_data": true,
    "seed": 23,
    "cx": 1,
    "cy": 0,
    "biome": null,
    "debug": true,
    "biome_size": 50,
    "ocean_coverage": 50,
    "land_water_scale": 50,
    "global_max_height": 100,
    "temperate_rainforest": { "max_height": 30 },
    "boreal_forest": { "max_height": 40 },
    "grassland": { "max_height": 40 },
    "tundra": { "max_height": 50 },
    "savanna": { "max_height": 25 },
    "woodland": { "max_height": 40 },
    "tropical_rainforest": { "max_height": 35 },
    "temperate_seasonal_forest": { "max_height": 100 },
    "subtropical_desert": { "max_height": 30 }
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
python3 -m master_script.master_script --parameters "{\
    \"seed\": 123,\
    \"cx\": 100,\
    \"cy\": 100,\
    \"biome\": null,\
    \"debug\": true,\
    \"biome_size\": 30,\
    \"ocean_coverage\": 50,\
    \"land_water_scale\": 20,\
    \"temperate_rainforest\": {\
        \"max_height\": 30\
    },\
    \"boreal_forest\": {\
        \"max_height\": 40\
    },\
    \"grassland\": {\
        \"max_height\": 40\
    },\
    \"tundra\": {\
        \"max_height\": 50\
    },\
    \"savanna\": {\
        \"max_height\": 25\
    },\
    \"woodland\": {\
        \"max_height\": 40\
    },\
    \"tropical_rainforest\": {\
        \"max_height\": 35\
    },\
    \"temperate_seasonal_forest\": {\
        \"max_height\": 90\
    },\
    \"subtropical_desert\": {\
        \"max_height\": 30\
    }\
}"
```

We annoyingly need to escape all quotes.