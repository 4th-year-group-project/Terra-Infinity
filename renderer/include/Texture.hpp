/**
 * @file Texture.hpp
 * @author King Attalus II
 * @brief This file contains the Texture class, which is used to load and manage textures for rendering.
 * @version 1.0
 * @date 2025
 *
 */

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <iostream>
#include <memory>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/stb/stb_image.h"
#else
    #include <glad/glad.h>
    #include <stb/stb_image.h>
#endif

/**
 * @brief This class is used to load and manage individual textures for rendering. It handles loading the texture
 * data from a file, uploading it to the GPU, and binding/unbinding the texture for rendering.
 *
 */
class Texture{
private:
    unsigned int id;
    std::string type;
    std::string path;
    std::string name;
    int width;
    int height;
    int nrChannels;

    void loadTexture(); // Load the texture data into RAM using stbi and upload to GPU
public:
    Texture(){}; // Default constructor
    Texture(std::string path, std::string type, std::string name);
    Texture(const char *path, std::string type, std::string name);
    Texture(std::string path, std::string type, std::string name, int width, int height, int nrChannels);
    ~Texture();

    // Getters and setters
    unsigned int getId(){return id;}
    std::string getType(){return type;}
    std::string getPath(){return path;}
    std::string getName(){return name;}
    int getWidth(){return width;}
    int getHeight(){return height;}
    int getNrChannels(){return nrChannels;}

    void setId(unsigned int inId){id = inId;}
    void setType(std::string inType){type = inType;}
    void setPath(std::string inPath){path = inPath;}
    void setName(std::string inName){name = inName;}
    void setWidth(int inWidth){width = inWidth;}
    void setHeight(int inHeight){height = inHeight;}
    void setNrChannels(int inNrChannels){nrChannels = inNrChannels;}

    void bind(int textureNumber);
    void unbind(int textureNumber);
};


#endif // TEXTURE_HPP
