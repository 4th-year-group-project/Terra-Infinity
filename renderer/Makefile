# This is the makefile for the renderer project. It is used to compile the renderer and an
# experimental program. The makefile is designed to work for both linux and windows using mingw64. 

# All of our gobal variables are defined here so that we can easily change them if we need to
CXX = g++
CXXFLAGS = -Wall -Wextra -g -lstdc++ -Wno-missing-field-initializers -Werror
LIB = ""
INCLUDES = ""
GLAD = ./home_dependencies/glad.c
STB_FILES = ./home_dependencies/stb_image.cpp # ./home_dependencies/stb_image_write.cpp
DEFINES = ""
SRC_DIR = ./src
OBJ_DIR = ./build
SRC = ""
OBJ = ""
SRC_EXP = ""
OBJ_EXP = ""
WINDOWS_TARGET = .\build\renderer
WINDOWS_TARGET_EXP = .\build\experimental_renderer
LINUX_TARGET = ./build/renderer
LINUX_TARGET_EXP = ./build/experimental_renderer
PWD = ""


# .PHONY: parameter_setup
# parameter_setup:
ifeq ($(OS),Windows_NT)
    CXX = g++
    CXXFLAGS = -Wall -Wextra -g -lstdc++ -Wno-missing-field-initializers -Werror
    LIB = -lglfw3dll
    STB_FILES = .\home_dependencies\stb_image.cpp # .\home_dependencies\stb_image_write.cpp
    GLAD = .\home_dependencies\glad.c
    INCLUDES = -I.\home_dependencies -I.\include -IC:\mingw64\include
    DEFINES = -DWINDOWS_BUILD
    OBJ_DIR = .\build
    SRC_DIR = .\src
    PWD = $(shell cd)
    FULL_SRC = $(shell dir /s /b $(SRC_DIR)\prism\*.cpp)
    SRC := $(patsubst $(PWD)%,.%, $(FULL_SRC))
    # Remove the file linux_main.cpp from the list of files to compile
    SRC := $(filter-out .\src\prism\linux_main.cpp, $(SRC))
    OBJ = $(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o, $(SRC))
    FULL_SRC_EXP = $(shell dir /s /b $(SRC_DIR)\experimental\*.cpp)
    SRC_EXP = $(patsubst $(PWD)\\%,.\\%, $(FULL_SRC_EXP))
    OBJ_EXP = $(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o, $(SRC_EXP))
    WINDOWS_TARGET = $(OBJ_DIR)\renderer
    WINDOWS_TARGET_EXP = $(OBJ_DIR)\experimental_renderer
else
    CXX = g++
    CXXFLAGS = -Wall -Wextra -g -lstdc++ -Wno-missing-field-initializers -Werror
    LIB = -lglfw -ldl -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio
    INCLUDES = -I./home_dependencies -I./include -I/usr/include/opencv4
    GLAD = ./home_dependencies/glad.c
	DEFINES = -DLINUX_BUILD
    OBJ_DIR = ./build
    SRC_DIR = ./src
    SRC = $(shell find $(SRC_DIR)/prism -type f -name "*.cpp")
    SRC := $(filter-out ./src/prism/windows_main.cpp, $(SRC))
    # Remove the file windows_main.cpp from the list of files to compile
    OBJ = $(patsubst $(SRC_DIR)/prism/%.cpp, $(OBJ_DIR)/prism/%.o, $(SRC))
    SRC_EXP = $(shell find $(SRC_DIR)/experimental -type f -name "*.cpp")
    OBJ_EXP = $(patsubst $(SRC_DIR)/experimental/%.cpp, $(OBJ_DIR)/experimental/%.o, $(SRC_EXP))
    LINUX_TARGET = $(OBJ_DIR)/renderer
    LINUX_TARGET_EXP = $(OBJ_DIR)/experimental_renderer
endif


.PHONY: windows
# For windows we do not have opencv as a dependency so we need to compile the renderer differently
# we also need to take into account the different path separators
# windows: parameter_setup
windows: windows_setup
windows: info_dumps
windows:
	@echo "===================== LOCAL VARIABLES ====================="
	@echo "FULL_SRC: $(FULL_SRC)"
	@echo "Current directory: $(PWD)"
	@echo "Source directory: $(SRC_DIR)"
	@echo "Build directory: $(OBJ_DIR)"
	@echo "Target: $(WINDOWS_TARGET)"
	@echo "Target Exper: $(WINDOWS_TARGET_EXP)"
	@echo "OBJ: $(OBJ)"
	@echo "SRC: $(SRC)"
	@echo "SRC Exper: $(SRC_EXP)"
	@echo "OBJ Exper: $(OBJ_EXP)"
	@echo "===================== LOCAL VARIABLES ====================="
	@echo "Compiling the renderer"
	@$(MAKE) $(WINDOWS_TARGET)
	@echo "Compiling the experimental renderer"
	@$(MAKE) $(WINDOWS_TARGET_EXP)

# Windows compilation rules
$(WINDOWS_TARGET): $(OBJ)
	@echo "Version 2"
	@echo "Windows Target: $(WINDOWS_TARGET)"
	@echo "OBJ: $(OBJ)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) $^ $(GLAD) $(STB_FILES) $(LIB) -o $@

$(WINDOWS_TARGET_EXP): $(OBJ_EXP)
	@echo "Version 2"
	@echo "Windows Target: $(WINDOWS_TARGET_EXP)"
	@echo "OBJ: $(OBJ_EXP)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) $^ $(GLAD) $(STB_FILES) $(LIB) -o $@

$(OBJ_DIR)\prism\\%.o: $(SRC_DIR)\prism\\%.cpp
	@echo "Compiling $<"
	@echo "Output: $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(OBJ_DIR)\experimental\\%.o: $(SRC_DIR)\experimental\\%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

# Building the build directory for windows
.PHONY: windows_setup
windows_setup: TEMP_PWD = $(patsubst C:%, %, $(PWD))
windows_setup: SRC_SUBDIRS = $(patsubst \Code\World-Generation\renderer%\, .%, $(sort $(shell for /r $(SRC_DIR) %%A in (*.*) do @echo %%~pA)))
windows_setup: OBJ_SUBDIRS = $(patsubst .\src%, .\build%, $(SRC_SUBDIRS))
windows_setup:
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
# Ensure that we have the correct execution policy set for our profile
	@powershell -Command "Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy Unrestricted"
# For each subdirectory in the source directory print the subdirectory we get the unexpected at this time error
	@powershell -Command "foreach ($$dir in '$(OBJ_SUBDIRS)'.split(' ')) {if ($$dir -ne '') {if (!(Test-Path $$dir)) {mkdir $$dir}}}"



.PHONY: linux
linux: linux_setup
linux: info_dumps
linux:
	@echo "===================== LOCAL VARIABLES ====================="
	@echo "FULL_SRC: $(FULL_SRC)"
	@echo "Current directory: $(PWD)"
	@echo "Source directory: $(SRC_DIR)"
	@echo "Build directory: $(OBJ_DIR)"
	@echo "Target: $(LINUX_TARGET)"
	@echo "Target Exper: $(LINUX_TARGET_EXP)"
	@echo "OBJ: $(OBJ)"
	@echo "SRC: $(SRC)"
	@echo "SRC Exper: $(SRC_EXP)"
	@echo "OBJ Exper: $(OBJ_EXP)"
	@echo "===================== LOCAL VARIABLES ====================="
	@echo "Compiling the renderer"
	@$(MAKE) $(LINUX_TARGET)
	@echo "Compiling the experimental renderer"
	@$(MAKE) $(LINUX_TARGET_EXP)


$(LINUX_TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) $^ $(GLAD) $(STB_FILES) $(LIB) -o $@

$(LINUX_TARGET_EXP): $(OBJ_EXP)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) $^ $(GLAD) $(STB_FILES) $(LIB) -o $@

$(OBJ_DIR)/prism/%.o: $(SRC_DIR)/prism/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(OBJ_DIR)/experimental/%.o: $(SRC_DIR)/experimental/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

# .PHONY: linux
# linux:     CXX = g++)
# linux:     CXXFLAGS = -Wall -Wextra -g -lstdc++ -Wno-missing-field-initializers -Werror)
# linux:     LIB = -lglfw -ldl -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio)
# linux:     INCLUDES = -I./home_dependencies -I./include -I/usr/include/opencv4)
# linux:     GLAD = ./home_dependencies/glad.c)
# linux:
# 	CXX = g++
# 	CXXFLAGS = -Wall -Wextra -g -lstdc++ -Wno-missing-field-initializers -Werror
# 	LIB = -lglfw -ldl -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio

# Building the build directory for linux
.PHONY: linux_setup
linux_setup: SRC_SUBDIRS = $(patsubst $(SRC_DIR)%/, %, $(sort $(shell find $(SRC_DIR) -type d)))
linux_setup: OBJ_SUBDIRS = $(patsubst $(SRC_DIR)%, $(OBJ_DIR)%, $(SRC_SUBDIRS))
linux_setup:
	@echo "Building the build directory"
	@mkdir -p $(OBJ_DIR)
    # For each subdirectory in the source directory print the subdirectory
	@for dir in $(OBJ_SUBDIRS); do mkdir -p $$dir; done

.PHONY: department
department: DEFINES = -DDEPARTMENT_BUILD
department: INCLUDES = -I./home_dependencies -I/dcs/large/efogahlewem/.local/include/opencv4/opencv2 -I/dcs/large/efogahlewem/.local/include/opencv4 -I./include -I/dcs/large/efogahlemew/.local/include
department: GLAD = /dcs/large/efogahlewem/.local/lib/glad/glad.c
department: STB_FILES = /dcs/large/efogahlewem/.local/lib/stb/stb_image_write.cpp /dcs/large/efogahlewem/.local/lib/stb/stb_image.cpp
department: linux_setup
department: info_dumps
department:
	@echo "===================== LOCAL VARIABLES ====================="
	@echo "FULL_SRC: $(FULL_SRC)"
	@echo "Current directory: $(PWD)"
	@echo "Source directory: $(SRC_DIR)"
	@echo "Build directory: $(OBJ_DIR)"
	@echo "Target: $(LINUX_TARGET)"
	@echo "Target Exper: $(LINUX_TARGET_EXP)"
	@echo "OBJ: $(OBJ)"
	@echo "SRC: $(SRC)"
	@echo "SRC Exper: $(SRC_EXP)"
	@echo "OBJ Exper: $(OBJ_EXP)"
	@echo "===================== LOCAL VARIABLES ====================="
	@echo "Compiling the renderer"
	@$(MAKE) $(LINUX_TARGET)
	@echo "Compiling the experimental renderer"
	@$(MAKE) $(LINUX_TARGET_EXP)


.PHONY: w_clean
w_clean: OBJ_DIR = .\build
w_clean:
	@echo "Cleaning the build directory"
# We need to use the windows command to remove the directory
	@rmdir /s /q $(OBJ_DIR)

.PHONY: l_clean
l_clean: OBJ_DIR = ./build
l_clean:
	@echo "Cleaning the build directory"
	@rm -rf $(OBJ_DIR)

.PHONY: d_clean
d_clean: OBJ_DIR = ./build
d_clean:
	@echo "Cleaning the build directory"
	@rm -rf $(OBJ_DIR)


.PHONY: info_dumps
info_dumps:
	@echo "===================== INFO DUMPS ====================="
	@echo "CXX: $(CXX)"
	@echo "CXXFLAGS: $(CXXFLAGS)"
	@echo "LIB: $(LIB)"
	@echo "INCLUDES: $(INCLUDES)"
	@echo "GLAD: $(GLAD)"
	@echo "STB_FILES: $(STB_FILES)"
	@echo "DEFINES: $(DEFINES)"
	@echo "Current directory: $(PWD)"
	@echo "Source directory: $(SRC_DIR)"
	@echo "Build directory: $(OBJ_DIR)"
	@echo "Windows Target: $(WINDOWS_TARGET)"
	@echo "Windows Target Exper: $(WINDOWS_TARGET_EXP)"
	@echo "OBJ: $(OBJ)"
	@echo "SRC: $(SRC)"
	@echo "SRC Exper: $(SRC_EXP)"
	@echo "OBJ Exper: $(OBJ_EXP)"
	@echo "===================== INFO DUMPS ====================="


# Write me a simple makefile to compile the renderer
# CXX = g++
# CXXFLAGS = -Wall -Wextra -g -lstdc++ -Wno-missing-field-initializers -Werror
# OPENCVFLAGS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio
# GLFWFLAGS = -lglfw -ldl
# INCLUDES = -I/dcs/large/efogahlewem/.local/include/opencv4/opencv2 -I/dcs/large/efogahlewem/.local/include/opencv4 -I./include -I/dcs/large/efogahlemew/.local/include
# GLAD = /dcs/large/efogahlewem/.local/lib/glad/glad.c
# ADDITIONAL_FILES = /dcs/large/efogahlewem/.local/lib/stb/stb_image_write.cpp /dcs/large/efogahlewem/.local/lib/stb/stb_image.cpp
# DEFINES = -DDEPARTMENT_BUILD

# OBJ_DIR = ./build
# SRC_DIR = ./src
# SRC = $(shell find $(SRC_DIR)/prism -type f -name "*.cpp")
# OBJ = $(patsubst $(SRC_DIR)/prism/%.cpp, $(OBJ_DIR)/prism/%.o, $(SRC))

# SRC_EXP = $(shell find $(SRC_DIR)/experimental -type f -name "*.cpp")
# OBJ_EXP = $(patsubst $(SRC_DIR)/experimental/%.cpp, $(OBJ_DIR)/experimental/%.o, $(SRC_EXP))

# TARGET = $(OBJ_DIR)/renderer
# TARGET_EXP = $(OBJ_DIR)/experimental_renderer

# .PHONY: all
# all: release

# .PHONY: setup
# setup:
# 	@mkdir -p $(OBJ_DIR)
# 	@mkdir -p $(OBJ_DIR)/prism
# 	@mkdir -p $(OBJ_DIR)/experimental

# .PHONY: release
# release: setup
# release: $(TARGET)

# .PHONY: experiment
# experiment: setup
# experiment: $(TARGET_EXP)

# # We are using the department setup including library paths to compile the renderer
# .PHONY: department
# department: release

# .PHONY: department_experiment
# department_experiment: experiment

# # We are using the home setup (unique to each user) to compile the renderer so we want to
# # disable the department build definition
# .PHONY: home
# home: DEFINES = -DHOME_BUILD
# home: INCLUDES = -I./home_dependencies -I./include -I/usr/include/opencv4
# home: ADDITIONAL_FILES = ./home_dependencies/stb_image.cpp
# home: CXXFLAGS = -Wall -Wextra -g -lstdc++ -Wno-missing-field-initializers
# home: GLAD = ./home_dependencies/glad.c
# home: release

# .PHONY: home_experiment
# home_experiment: DEFINES = -DHOME_BUILD
# home_experiment: INCLUDES = -I./home_dependencies -I./include -I/usr/include/opencv4
# home_experiment: ADDITIONAL_FILES = ./home_dependencies/stb_image.cpp
# home_experiment: CXXFLAGS = -Wall -Wextra -g -lstdc++ -Wno-missing-field-initializers
# home_experiment: GLAD = ./home_dependencies/glad.c
# home_experiment: experiment

# .PHONY: help
# help:
# 	@echo "make all: Builds the renderer using home setup"
# 	@echo "make setup: Creates the build directory"
# 	@echo "make release: Builds the renderer using the home setup"
# 	@echo "make experiment: Builds the experimental renderer using the home setup"
# 	@echo "make department: Builds the renderer using the department setup"
# 	@echo "make department_experiment: Builds the experimental renderer using the department setup"
# 	@echo "make clean: Cleans the build directory"

# .PHONY: clean
# clean:
# 	rm -rf $(OBJ_DIR)

# windows: 
# 	g++ -Wall -Wextra -g -lstdc++ -Wno-missing-field-initializers -DWINDOWS_BUILD -I.\home_dependencies -I.\include -IC:mingw64\include\opencv4\opencv2 -IC:\mingw64\include -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -lglfw -ldl -c src\prism\main_load_mesh.cpp -o build\prism\main_load_mesh.o
# 	g++ -Wall -Wextra -g -lstdc++ -Wno-missing-field-initializers -DWINDOWS_BUILD  -I.\home_dependencies -I.\include -I\usr\include\opencv4 -IC:\mingw64\include -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -lglfw -ldl -c src\prism\load_obj.cpp -o build\prism\load_obj.o
# 	g++ -Wall -Wextra -g -lstdc++ -Wno-missing-field-initializers -DWINDOWS_BUILD  -IC:\mingw64\include -I.\home_dependencies -I.\include -I\usr\include\opencv4 build\prism\load_obj.o build\prism\main_load_mesh.o .\home_dependencies\glad.c .\home_dependencies\stb_image.cpp -lglfw3dll -o build\renderer

# Linking the final executable
# $(TARGET_EXP): $(OBJ_EXP)
# 	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(OPENCVFLAGS)  $^ $(GLAD) $(ADDITIONAL_FILES) $(GLFWFLAGS) -o $@
# 	# $(CXX) $(CXXFLAGS) $^ -o $@

# $(TARGET): $(OBJ)
# 	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $^ $(GLAD) $(ADDITIONAL_FILES) $(GLFWFLAGS) $(OPENCVFLAGS) -o $@

# # Compiling the object files
# $(OBJ_DIR)/prism/%.o: $(SRC_DIR)/prism/%.cpp
# 	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(OPENCVFLAGS) $(GLFWFLAGS) -c $< -o $@

# $(OBJ_DIR)/experimental/%.o: $(SRC_DIR)/experimental/%.cpp
# 	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) $(OPENCVFLAGS) $(GLFWFLAGS) -c $< -o $@
# 	# $(CXX) $(CXXFLAGS) -c $< -o $@



