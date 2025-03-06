/*
    This class represents a texture that will be used in rendering an object
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

using namespace std;

class Texture{
private:
    unsigned int id;
    string type;
    string path;
    string name;
    int width;
    int height;
    int nrChannels;

    void loadTexture();
public:
    Texture(){}; // Default constructor
    Texture(string path, string type, string name);
    Texture(const char *path, string type, string name);
    Texture(string path, string type, string name, int width, int height, int nrChannels);
    ~Texture();

    // Getters and setters
    unsigned int getId(){return id;}
    string getType(){return type;}
    string getPath(){return path;}
    string getName(){return name;}
    int getWidth(){return width;}
    int getHeight(){return height;}
    int getNrChannels(){return nrChannels;}

    void setId(unsigned int inId){id = inId;}
    void setType(string inType){type = inType;}
    void setPath(string inPath){path = inPath;}
    void setName(string inName){name = inName;}
    void setWidth(int inWidth){width = inWidth;}
    void setHeight(int inHeight){height = inHeight;}
    void setNrChannels(int inNrChannels){nrChannels = inNrChannels;}

    void bind(int textureNumber);
    void unbind(int textureNumber);
};


#endif // TEXTURE_HPP