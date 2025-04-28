<p align="center">
  <img src="https://github.com/user-attachments/assets/332cf526-08fa-4e81-a07f-842105ee7ec8" width="50%" />
</p>

# TerraInfinity - Renderer

This directory contains our code for our custom renderer and UI. 

## Directory structure

```plaintext
renderer/
├── build/
├── deprecated/
├── home_dependencies/
├── include/
├── resources/
├── src/
├── test/
├── CMakeLists.txt
└── Makefile
```

**build** - Contains the renderer build files and collected dependencies.

**deprecated** - Contains deprecated code that was previously, used this is not commented or kept up to date.

**home_dependencies** - Contains some small dependencies and libraries that we decided to directly include in the repository.

**include** - Contains all of our header files.

**resources** - Contains all of our fonts and texture files.

**src** - Contains all of our source files and shader programs.

**tests** - Contains our unit tests that have been written for the renderer.

**CMakeLists.txt** - Our CMake file which compiles the code.

**Makefile** - Our old Makefile that might not be compatible anymore.