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

    void loadTextureArray();
public:
    TextureArray(vector<string> paths, string type, string name);
    ~TextureArray();

    // Getters and setters
    unsigned int getId(){return id;}
    string getType(){return type;}
    vector<string> getPaths(){return paths;}
    string getName(){return name;}

    void setId(unsigned int inId){id = inId;}
    void setType(string inType){type = inType;}
    void setPaths(vector<string> inPaths){paths = inPaths;}
    void setName(string inName){name = inName;}

    void bind(int textureNumber);
    void unbind(int textureNumber);
};

#endif // TEXTURE_ARRAY_HPP