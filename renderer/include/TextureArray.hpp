/**
 * This class represents a 2D texture array to be used in the OpenGL context. 
*/

#ifndef TEXTURE_ARRAY_HPP
#define TEXTURE_ARRAY_HPP

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


class TextureArray{
private:
    unsigned int id;
    std::vector<std::string> paths;
    std::string type;
    std::string name;
    int width;
    int height;
    int channels;
    std::vector<unsigned char*> imageData; // Temporary storage for all the texture image data before uploading to GPU
    bool uploaded = false; // Set to true after uploading imageData to GPU
public:
    TextureArray(std::vector<std::string> paths, std::string type, std::string name);
    ~TextureArray();

    // Getters and setters
    unsigned int getId(){return id;}
    std::string getType(){return type;}
    std::vector<std::string> getPaths(){return paths;}
    std::string getName(){return name;}
    int getWidth(){return width;}
    int getHeight(){return height;}
    int getChannels(){return channels;}
    int getUploaded(){return uploaded;}
    std::vector<unsigned char*> getImageData(){return imageData;}

    void setId(unsigned int inId){id = inId;}
    void setType(std::string inType){type = inType;}
    void setPaths(std::vector<std::string> inPaths){paths = inPaths;}
    void setName(std::string inName){name = inName;}
    void setWidth(int inWidth){width = inWidth;}
    void setHeight(int inHeight){height = inHeight;}
    void setChannels(int inChannels){channels = inChannels;}
    void setImageData(std::vector<unsigned char*> inImageData){imageData = inImageData;}
    void setUploaded(bool inUploaded){uploaded = inUploaded;}

    void bind(int textureNumber); // Bind the texture to a texture unit
    void unbind(int textureNumber); // Unbind the texture from a texture unit

    void loadTextureData();  // Loads pixel data into RAM using stbi
    void uploadToGPU();  // Uploads to GPU and builds the OpenGL texture
};

#endif // TEXTURE_ARRAY_HPP