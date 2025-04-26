![logo](https://github.com/user-attachments/assets/360d609f-4bda-4155-8f72-a2a4e696fd70)

# TerraInfinity


Current tools for generating interesting, believable terrain are complex and geared towards finite worlds, while existing infinite terrain generation
techniques often fail to match the quality produced by finite techniques. ​This leads to our main project aim:

​
**To create an accessible tool for generating and exploring
customisable infinite worlds that supports non-technical user
interaction while maintaining believability, interest and visual appeal.​**

​
To achieve this, we identified a high-quality, finite technique (DLA) and
developed novel ways of applying DLA approximations to make it fast
enough for real-time infinite terrain generation and exploration. We also
developed a custom renderer with an accessible user interface (UI).​

## Installation

Installation commands and stuff here.

## Directory Structure

The top-level directory is given below, with a brief description of the purpose of each directory.

```plaintext
TerraInfinity/
├── world-generation/
├── renderer/
├── data/
├── environment/
├── saves/
└── scripts/
```

**world_generation** - Contains all code related to world generation. View the ```README.md``` in this directory for more information. 

**renderer** - Contains all code related to the renderer. View the ```README.md``` in this directory for more information. 

**data** - Contains mock data used for testing the world generation and renderer components independently.

**environment** - Contains scripts that manage dependencies and builds the environment.

**saves** - Stores saved worlds, and screenshots of worlds taken in the renderer. 

**scripts** - Contains scripts used for automatic testing.
