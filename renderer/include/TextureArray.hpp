/*
    This class represents a texture that will be used in rendering an object
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

using namespace std;

class TextureArray{
private:
    unsigned int id;
    vector<string> paths;
    string type;
    string name;
    int width;
    int height;
    int channels;
    std::vector<unsigned char*> imageData;
    bool uploaded = false;
public:
    TextureArray(vector<string> paths, string type, string name);
    ~TextureArray();

    // Getters and setters
    unsigned int getId(){return id;}
    string getType(){return type;}
    vector<string> getPaths(){return paths;}
    string getName(){return name;}
    int getWidth(){return width;}
    int getHeight(){return height;}
    int getChannels(){return channels;}
    int getUploaded(){return uploaded;}
    std::vector<unsigned char*> getImageData(){return imageData;}

    void setId(unsigned int inId){id = inId;}
    void setType(string inType){type = inType;}
    void setPaths(vector<string> inPaths){paths = inPaths;}
    void setName(string inName){name = inName;}
    void setWidth(int inWidth){width = inWidth;}
    void setHeight(int inHeight){height = inHeight;}
    void setChannels(int inChannels){channels = inChannels;}
    void setImageData(std::vector<unsigned char*> inImageData){imageData = inImageData;}
    void setUploaded(bool inUploaded){uploaded = inUploaded;}

    void bind(int textureNumber);
    void unbind(int textureNumber);

    void loadTextureData();     // Loads pixel data into RAM using stbi
    void uploadToGPU();         // Uploads to GPU and builds the OpenGL texture
};

#endif // TEXTURE_ARRAY_HPP