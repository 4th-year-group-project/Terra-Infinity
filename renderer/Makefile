# Write me a simple makefile to compile the renderer
CXX = g++
CXXFLAGS = -Wall -Wextra -g -lstdc++ -Wno-missing-field-initializers -Werror
OPENCVFLAGS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio
GLFWFLAGS = -lglfw -ldl
INCLUDES = -I/dcs/large/efogahlewem/.local/include/opencv4/opencv2 -I/dcs/large/efogahlewem/.local/include/opencv4 -I./include -I/dcs/large/efogahlemew/.local/include
GLAD = /dcs/large/efogahlewem/.local/lib/glad/glad.c
ADDITIONAL_FILES = /dcs/large/efogahlewem/.local/lib/stb/stb_image_write.cpp /dcs/large/efogahlewem/.local/lib/stb/stb_image.cpp
DEFINES = -DDEPARTMENT_BUILD

OBJ_DIR = ./build
SRC_DIR = ./src
SRC = $(shell find $(SRC_DIR)/prism -type f -name "*.cpp")
OBJ = $(patsubst $(SRC_DIR)/prism/%.cpp, $(OBJ_DIR)/prism/%.o, $(SRC))

SRC_EXP = $(shell find $(SRC_DIR)/experimental -type f -name "*.cpp")
OBJ_EXP = $(patsubst $(SRC_DIR)/experimental/%.cpp, $(OBJ_DIR)/experimental/%.o, $(SRC_EXP))

TARGET = $(OBJ_DIR)/renderer
TARGET_EXP = $(OBJ_DIR)/experimental_renderer

.PHONY: all
all: release

.PHONY: setup
setup:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/prism
	@mkdir -p $(OBJ_DIR)/experimental

.PHONY: release
release: setup
release: $(TARGET)

.PHONY: experiment
experiment: setup
experiment: $(TARGET_EXP)

# We are using the department setup including library paths to compile the renderer
.PHONY: department
department: release

.PHONY: department_experiment
department_experiment: experiment

# We are using the home setup (unique to each user) to compile the renderer so we want to
# disable the department build definition
.PHONY: home
home: DEFINES = -DHOME_BUILD
home: INCLUDES = -I./home_dependencies -I./include
home: ADDITIONAL_FILES = ./home_dependencies/stb_image.cpp
home: CXXFLAGS = -Wall -Wextra -g -lstdc++ -Wno-missing-field-initializers
home: GLAD = ./home_dependencies/glad.c
home: release

.PHONY: home_experiment
home_experiment: DEFINES = -DHOME_BUILD
home_experiment: INCLUDES = -I./home_dependencies -I./include
home_experiment: ADDITIONAL_FILES = ./home_dependencies/stb_image.cpp
home_experiment: CXXFLAGS = -Wall -Wextra -g -lstdc++ -Wno-missing-field-initializers
home_experiment: GLAD = ./home_dependencies/glad.c
home_experiment: experiment

.PHONY: help
help:
	@echo "make all: Builds the renderer using home setup"
	@echo "make setup: Creates the build directory"
	@echo "make release: Builds the renderer using the home setup"
	@echo "make experiment: Builds the experimental renderer using the home setup"
	@echo "make department: Builds the renderer using the department setup"
	@echo "make department_experiment: Builds the experimental renderer using the department setup"
	@echo "make clean: Cleans the build directory"

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)

# Linking the final executable
$(TARGET_EXP): $(OBJ_EXP)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(OPENCVFLAGS)  $^ $(GLAD) $(ADDITIONAL_FILES) $(GLFWFLAGS) -o $@
	# $(CXX) $(CXXFLAGS) $^ -o $@

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(OPENCVFLAGS) $^ $(GLAD) $(ADDITIONAL_FILES) $(GLFWFLAGS)  -o $@

# Compiling the object files
$(OBJ_DIR)/prism/%.o: $(SRC_DIR)/prism/%.cpp
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(OPENCVFLAGS) $(GLFWFLAGS) -c $< -o $@

$(OBJ_DIR)/experimental/%.o: $(SRC_DIR)/experimental/%.cpp
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(OPENCVFLAGS) $(GLFWFLAGS) -c $< -o $@
	# $(CXX) $(CXXFLAGS) -c $< -o $@



