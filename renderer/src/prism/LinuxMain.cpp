#include <iostream>

#include <renderer.hpp>
#include <Settings.hpp>

int main(int argc, char** argv)
{
    // A simple controller program for the renderer that is compiled on Linux
    std::cout << "Hello, World!" << std::endl;

    // Initialize the settings for the renderer
    Settings settings = Settings(1920, 1080, false, 100, 1024, 32, 32, '/');

    return renderer(argc, argv);
}