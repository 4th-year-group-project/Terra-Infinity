/*
    This class represents a texture that will be used in rendering an object
*/

#include <string>
#include <iostream>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/stb/stb_image.h"
#else
    #include <glad/glad.h>
    #include <stb/stb_image.h>
#endif

#include "Texture.hpp"

using namespace std;

void Texture::loadTexture(){
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip on the y-axis.
    unsigned char *data;
    try{
        data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    } catch (exception &e){
        cout << "ERROR::TEXTURE::FAILED_TO_LOAD_TEXTURE: " << e.what() << endl;
    }
    if (data){
        if (nrChannels == 3){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 4){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        cout << "ERROR::TEXTURE::FAILED_TO_LOAD_TEXTURE: " << path << endl;
    }
    stbi_image_free(data);
}

Texture::Texture(string path, string type){
    this->path = path;
    this->type = type;
    loadTexture();
}

Texture::Texture(const char *path, string type){
    this->path = string(path);
    this->type = type;
    loadTexture();
}

Texture::Texture(string path, string type, int width, int height, int nrChannels){
    this->path = path;
    this->type = type;
    this->width = width;
    this->height = height;
    this->nrChannels = nrChannels;
    loadTexture();
}

Texture::~Texture(){
    glDeleteTextures(1, &id);
}

void Texture::bind(int textureNumber){
    // Check that the texture number is between 0 and 31
    if (textureNumber < 0 || textureNumber > 31){
        cout << "ERROR::TEXTURE::BIND::TEXTURE_NUMBER_OUT_OF_RANGE: " << textureNumber << endl;
        return;
    }
    glActiveTexture(GL_TEXTURE0 + textureNumber);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind(int textureNumber){
    // Check that the texture number is between 0 and 31
    if (textureNumber < 0 || textureNumber > 31){
        cout << "ERROR::TEXTURE::UNBIND::TEXTURE_NUMBER_OUT_OF_RANGE: " << textureNumber << endl;
        return;
    }
    glActiveTexture(GL_TEXTURE0 + textureNumber);
    glBindTexture(GL_TEXTURE_2D, 0);
}
