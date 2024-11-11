# Write me a simple makefile to compile the renderer
CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -g -lstdc++ -Wno-missing-field-initializers
OPENCVFLAGS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio
GLFWFLAGS = -lglfw -ldl
INCLUDES = -I/dcs/large/efogahlemew/.local/include/opencv4/opencv2 -I/dcs/large/efogahlemew/.local/include/opencv4 -I./include -I/dcs/large/efogahlemew/.local/include
GLAD = /dcs/large/efogahlewem/.local/lib/glad/glad.c
ADDITIONAL_FILES = /dcs/large/efogahlewem/.local/lib/stb/stb_image_write.cpp /dcs/large/efogahlewem/.local/lib/stb/stb_image.cpp

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


# Linking the final executable
$(TARGET_EXP): $(OBJ_EXP)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPENCVFLAGS) $(GLFWFLAGS) $^ $(GLAD) $(ADDITIONAL_FILES) -o $@
	# $(CXX) $(CXXFLAGS) $^ -o $@

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPENCVFLAGS) $(GLFWFLAGS) $^ $(GLAD) $(ADDITIONAL_FILES) -o $@

# Compiling the object files
$(OBJ_DIR)/prism/%.o: $(SRC_DIR)/prism/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPENCVFLAGS) $(GLFWFLAGS) -c $< -o $@

$(OBJ_DIR)/experimental/%.o: $(SRC_DIR)/experimental/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPENCVFLAGS) $(GLFWFLAGS) -c $< -o $@
	# $(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)


