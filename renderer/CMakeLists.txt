cmake_minimum_required(VERSION 3.15) # Increased for better FetchContent support
project(WorldGenerationRenderer VERSION 1.0.0 LANGUAGES CXX C)

# Include FetchContent for dependency management
include(FetchContent)

# Set the build directory
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Prevent CMake from looking in .env directory
# Get real path of .env to handle relative paths and symlinks
get_filename_component(ENV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.env" REALPATH)
get_filename_component(TESTING_ENV_DIR "~/.testing_env" REALPATH)
message(STATUS "Excluding Python environment directory from search:")
message(STATUS "  - ${ENV_DIR}")
message(STATUS "  - ${TESTING_ENV_DIR}")

# Manipulate find_package search paths to exclude .env
list(APPEND CMAKE_IGNORE_PATH "${ENV_DIR}" "${TESTING_ENV_DIR}")
list(APPEND CMAKE_IGNORE_PATH "${ENV_DIR}/lib" "${TESTING_ENV_DIR}/lib")
list(APPEND CMAKE_IGNORE_PATH "${ENV_DIR}/include" "${TESTING_ENV_DIR}/include")

# Only look in system paths and our specified locations
set(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH TRUE)
set(CMAKE_FIND_USE_CMAKE_ENVIRONMENT_PATH TRUE)
set(CMAKE_FIND_USE_CMAKE_PATH TRUE)
set(CMAKE_FIND_USE_CMAKE_SYSTEM_PATH TRUE)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags
add_compile_options(-fopenmp)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# We want to output the build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Add option to build tests
option(BUILD_TESTS "Build tests" OFF)
message(STATUS "Building tests: ${BUILD_TESTS}")

set(STRICT_COMPILE_FLAGS
    -Wall
    -Wextra
    -Werror
    -Wno-missing-field-initializers
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-unused-function
    -Wno-unused-value
    -Wno-unused-label
    -fopenmp
)

# Platform-specific definitions
if(WIN32)
    add_definitions(-DWINDOWS_BUILD)
else()
    add_definitions(-DLINUX_BUILD)
endif()





# Always-required system packages
find_package(OpenMP REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

# Try to find packages, with fallbacks to fetch and build
# ----------------------------------------------------

# 1. Try GLFW
find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
    message(STATUS "GLFW not found - fetching and building from source")
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW examples")
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW tests")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW docs")
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
    )
    FetchContent_MakeAvailable(glfw)
else()
    message(STATUS "GLFW found - using system version")
endif()

# 2. Try Assimp
find_package(assimp QUIET)
if(NOT assimp_FOUND)
    message(STATUS "Assimp not found - fetching and building from source")
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Build Assimp tests")
    set(ASSIMP_INSTALL OFF CACHE BOOL "Install Assimp")
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Build Assimp tools")
    # Disable the function cast warning for assimp
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-cast-function-type -Wno-implicit-fallthrough")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-cast-function-type -Wno-implicit-fallthrough")

        # Temporarily remove -Werror for assimp build
        string(REPLACE "-Werror" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REPLACE "-Werror" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    endif()
    FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.2.5
    )
    FetchContent_MakeAvailable(assimp)
    set(ASSIMP_INCLUDE_DIRS ${assimp_SOURCE_DIR}/include)
    set(ASSIMP_LIBRARIES assimp)
else()
    message(STATUS "Assimp found - using system version")
    # If Assimp is found, we need to set the include directories and libraries
    set(ASSIMP_INCLUDE_DIRS ${assimp_INCLUDE_DIRS})
    set(ASSIMP_LIBRARIES assimp)
endif()

# 3. Try GLM
find_package(glm QUIET)
if(NOT glm_FOUND)
    message(STATUS "GLM not found - fetching and building from source")
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8
    )
    FetchContent_MakeAvailable(glm)
    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE ${glm_SOURCE_DIR})
    add_library(glm::glm ALIAS glm)
else()
    message(STATUS "GLM found - using system version")
endif()

# 4. Try OpenCV (fallback to minimal set if not found)
find_package(OpenCV QUIET COMPONENTS core highgui imgproc imgcodecs videoio)
if(NOT OpenCV_FOUND)
    message(STATUS "OpenCV not found - fetching and building from source (minimal build)")
    set(BUILD_TESTS OFF CACHE BOOL "Build OpenCV tests")
    set(BUILD_PERF_TESTS OFF CACHE BOOL "Build OpenCV performance tests")
    set(BUILD_EXAMPLES OFF CACHE BOOL "Build OpenCV examples")
    set(BUILD_opencv_apps OFF CACHE BOOL "Build OpenCV applications")
    set(BUILD_LIST core,highgui,imgproc,imgcodecs,videoio CACHE STRING "OpenCV modules to build")
    
    

    FetchContent_Declare(
        opencv
        GIT_REPOSITORY https://github.com/opencv/opencv.git
        GIT_TAG 4.8.0
    )
    FetchContent_MakeAvailable(opencv)
else()
    message(STATUS "OpenCV found - using system version")
    # If OpenCV is found, we need to set the include directories and libraries
    set(OpenCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIRS})
    set(OpenCV_LIBS ${OpenCV_LIBS})
endif()

# 5. Try CURL
find_package(CURL QUIET)
if(NOT CURL_FOUND)
    message(STATUS "CURL not found - fetching and building from source")
    set(BUILD_TESTING OFF CACHE BOOL "Build tests")
    set(BUILD_CURL_EXE OFF CACHE BOOL "Build curl executable")
    FetchContent_Declare(
        curl
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG curl-7_88_1
    )
    FetchContent_MakeAvailable(curl)
    set(CURL_INCLUDE_DIRS ${curl_SOURCE_DIR}/include)
    set(CURL_LIBRARIES ${curl_BINARY_DIR}/lib/libcurl.a)
else()
    message(STATUS "CURL found - using system version")
    # If CURL is found, we need to set the include directories and libraries
    set(CURL_INCLUDE_DIRS ${CURL_INCLUDE_DIRS})
    set(CURL_LIBRARIES ${CURL_LIBRARIES})
endif()

# Create local libraries
# ---------------------

# 1. Create ImGui library
file(GLOB IMGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/home_dependencies/imgui/*.cpp)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/home_dependencies)
target_link_libraries(imgui PUBLIC OpenGL::GL)

# 2. Create GLAD library
add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/home_dependencies/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/home_dependencies)
target_link_libraries(glad PUBLIC OpenGL::GL)

# 3. Create STB library
set(STB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/home_dependencies/stb/stb_image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/home_dependencies/stb/stb_image_write.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/home_dependencies/stb/stb_image_resize2.cpp
)
add_library(stb STATIC ${STB_SOURCES})
target_include_directories(stb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/home_dependencies)

# 4. Create header-only libraries
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/home_dependencies/nlohmann)

add_library(khr INTERFACE)
target_include_directories(khr INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/home_dependencies/KHR)

add_library(fontawesome INTERFACE)
target_include_directories(fontawesome INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/home_dependencies/fontawesome)

# Collect renderer source files
file(GLOB_RECURSE PRISM_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/prism/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/prism/*.h
)

# Remove platform-specific sources to avoid duplicates
if(WIN32)
    list(FILTER PRISM_SOURCES EXCLUDE REGEX "LinuxMain.cpp$")
else()
    list(FILTER PRISM_SOURCES EXCLUDE REGEX "WindowsMain.cpp$")
endif()

# Build the tests
if(BUILD_TESTS)
    # Enable testing
    enable_testing()
    include(FetchContent)
    fetchcontent_declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    fetchcontent_makeavailable(googletest)

    set(TEST_PRISM_SOURCES ${PRISM_SOURCES})
    if (WIN32)
        list(FILTER TEST_PRISM_SOURCES EXCLUDE REGEX "WindowsMain.cpp$")
    else()
        list(FILTER TEST_PRISM_SOURCES EXCLUDE REGEX "LinuxMain.cpp$")
    endif()

    message(STATUS "Test prism sources: ${TEST_PRISM_SOURCES}")

    # Add all of the test source files
    file(GLOB_RECURSE TEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h
    )

    # Add the test executable
    add_executable(runTests
        ${TEST_SOURCES}
        ${TEST_PRISM_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks/LinuxCallbackMocks.cpp
    )

    # Reomove platform-specific sources to avoid running the actual renderer
    if(WIN32)
        list(FILTER TEST_SOURCES EXCLUDE REGEX "WindowsMain.cpp$")
    else()
        list(FILTER TEST_SOURCES EXCLUDE REGEX "LinuxMain.cpp$")
    endif()

    # Ouput the list of test files
    message(STATUS "Test files: ${TEST_SOURCES}")

    target_include_directories(runTests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/home_dependencies
    )
    target_link_libraries(runTests PRIVATE
        gtest_main
        gtest
        gmock
        OpenMP::OpenMP_CXX
        Threads::Threads
        OpenGL::GL
        $<IF:$<TARGET_EXISTS:glfw>,glfw,glfw3>
        ${OpenCV_LIBS}
        ${ASSIMP_LIBRARIES}
        $<IF:$<TARGET_EXISTS:CURL::libcurl>,CURL::libcurl,curl>
        $<IF:$<TARGET_EXISTS:glm::glm>,glm::glm,glm>
        imgui
        glad
        stb
        nlohmann_json
        khr
        fontawesome
    )

    add_test(NAME AllTests
        COMMAND runTests
    )
endif()

# Create the main renderer executable
add_executable(renderer ${PRISM_SOURCES})

target_compile_options(renderer PRIVATE ${STRICT_COMPILE_FLAGS})

# Add include directories
target_include_directories(renderer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/home_dependencies
)

# Link libraries with correct handling for all cases
target_link_libraries(renderer PRIVATE
    # System libraries (handling both system-installed and fetched)
    OpenMP::OpenMP_CXX
    Threads::Threads
    OpenGL::GL
    
    # External libraries (handling both found and fetched)
    $<IF:$<TARGET_EXISTS:glfw>,glfw,glfw3>
    ${OpenCV_LIBS}
    ${ASSIMP_LIBRARIES}
    $<IF:$<TARGET_EXISTS:CURL::libcurl>,CURL::libcurl,curl>
    $<IF:$<TARGET_EXISTS:glm::glm>,glm::glm,glm>
    
    # Our local libraries
    imgui
    glad
    stb
    nlohmann_json
    khr
    fontawesome
)

# Special handling for filesystem
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(renderer PRIVATE stdc++fs)
endif()
