#include <iostream>

#include <renderer.hpp>

int main(int argc, char** argv)
{
    // A simple controller program for the renderer that is compiled on Linux
    std::cout << "Hello, World!" << std::endl;

    return renderer(argc, argv);
}