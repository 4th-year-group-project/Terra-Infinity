# This is the makefile for the renderer project. It is used to compile the renderer and an
# experimental program. The makefile is designed to work for both linux and windows using mingw64.

# =================================================================================================
#                                      GLOBAL VARIABLES
# =================================================================================================

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

# We are setting up the parameters for the makefile based on the operating system
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

# =================================================================================================
# 							PHONY RULES (Entry points for the makefile)
# =================================================================================================

# For windows we do not have opencv as a dependency so we need to compile the renderer differently
# we also need to take into account the different path separators
# windows: parameter_setup
.PHONY: windows
windows: windows_setup
windows: info_dumps
windows:
	@echo "Compiling the renderer"
	@$(MAKE) $(WINDOWS_TARGET)
	@echo "Compiling the experimental renderer"
	@$(MAKE) $(WINDOWS_TARGET_EXP)

# For linux we have opencv as a dependency so we need to compile the renderer differently this will also allow for
# the use of different path separators between windows and linux
.PHONY: linux
linux: linux_setup
linux: info_dumps
linux:
	@echo "Compiling the renderer"
	@$(MAKE) $(LINUX_TARGET)
	@echo "Compiling the experimental renderer"
	@$(MAKE) $(LINUX_TARGET_EXP)

# When compiling on the department machines we can make use of the libraries that were installed in the common file
# system. We need to make sure that we have the correct includes and libraries set up for the department machines
.PHONY: department
department: DEFINES = -DDEPARTMENT_BUILD
department: INCLUDES = -I./home_dependencies -I/dcs/large/efogahlewem/.local/include/opencv4/opencv2 -I/dcs/large/efogahlewem/.local/include/opencv4 -I./include -I/dcs/large/efogahlemew/.local/include
department: GLAD = /dcs/large/efogahlewem/.local/lib/glad/glad.c
department: STB_FILES = /dcs/large/efogahlewem/.local/lib/stb/stb_image_write.cpp /dcs/large/efogahlewem/.local/lib/stb/stb_image.cpp
department: linux_setup
department: info_dumps
department:
	@echo "Compiling the renderer"
	@$(MAKE) $(LINUX_TARGET)
	@echo "Compiling the experimental renderer"
	@$(MAKE) $(LINUX_TARGET_EXP)

# =================================================================================================
#                                      DIRECTORY SETUP RULES
# =================================================================================================

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

# Building the build directory for linux
.PHONY: linux_setup
linux_setup: SRC_SUBDIRS = $(patsubst $(SRC_DIR)%/, %, $(sort $(shell find $(SRC_DIR) -type d)))
linux_setup: OBJ_SUBDIRS = $(patsubst $(SRC_DIR)%, $(OBJ_DIR)%, $(SRC_SUBDIRS))
linux_setup:
	@echo "Building the build directory"
	@mkdir -p $(OBJ_DIR)
    # For each subdirectory in the source directory print the subdirectory
	@for dir in $(OBJ_SUBDIRS); do mkdir -p $$dir; done

# =================================================================================================
#                                          UTILITY RULES
# =================================================================================================

# A useful step that dumps out all of the information used for compilation
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

# =================================================================================================
#                                          CLEANING RULES
# =================================================================================================
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

# =================================================================================================
#                                          COMPILATION RULES
# =================================================================================================

# Windows compilation rules
$(WINDOWS_TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) $^ $(GLAD) $(STB_FILES) $(LIB) -o $@

$(WINDOWS_TARGET_EXP): $(OBJ_EXP)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) $^ $(GLAD) $(STB_FILES) $(LIB) -o $@

$(OBJ_DIR)\prism\\%.o: $(SRC_DIR)\prism\\%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(OBJ_DIR)\experimental\\%.o: $(SRC_DIR)\experimental\\%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

# Linux compilation rules
$(LINUX_TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) $^ $(GLAD) $(STB_FILES) $(LIB) -o $@

$(LINUX_TARGET_EXP): $(OBJ_EXP)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) $^ $(GLAD) $(STB_FILES) $(LIB) -o $@

$(OBJ_DIR)/prism/%.o: $(SRC_DIR)/prism/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

$(OBJ_DIR)/experimental/%.o: $(SRC_DIR)/experimental/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@