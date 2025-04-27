<p align="center">
  <img src="https://github.com/user-attachments/assets/332cf526-08fa-4e81-a07f-842105ee7ec8" width="50%" />
</p>


# TerraInfinity - World Generation

This directory contains our code for terrain and world generation features. World features include:

- Terrain
  - DLA-based terrain generation
  - Efficient approximations of DLA-based terrain generation
  - Uber noise
  - Fractal Simplex Noise
  - Many more noise variations
- Biomes
- Rivers
- Flora

All of these oare parameterised and configurable.

The overall world generation process, when the renderer requests a chunk, is shown by the Figure below.

<img width="465" alt="image" src="https://github.com/user-attachments/assets/a5be0af2-059b-4b95-9d8c-479fc142db0c" />


## Directory structure

```plaintext
world_generation/
├── biomes/
├── cellular_automata/
├── coastline/
├── flora/
├── generation/
├── integration_tests/
├── master_script/
├── path_planning/
├── real_rivers/
├── trees/
└── utils/
```

**biomes** - Contains code that defines biome shapes and types.

**cellular_automata** - Contains code for a cellular automaton approximation of DLA-based terrain.

**coastline** - Contains code for creating realistic, detailed coastlines.

**flora** - Contains code for flora placement.

**generation** - Contains implementations of existing (non-DLA based) terrain techniques, some used in the final product, others (like erosion) used for comparisons in the final report.

**integration_tests** - Pseudorandom end-to-end tests.

**master_script** - Contains code that combines world generation features into a generation pipeline, and runs the server that the renderer communicates with.

**path_planning** - Contains code for a path-planning approximation of DLA-based terrain.

**real_rivers** - Code for generating the river mask and applying it to terrain.

**utils** - Helpful utility functions used in many places.
