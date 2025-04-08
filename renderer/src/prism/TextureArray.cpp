/*
    This class represents a texture that will be used in rendering an object
*/

#include <filesystem>
#include <string>
#include <iostream>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/stb/stb_image.h"
#else
    #include <glad/glad.h>
    #include <stb/stb_image.h>
#endif

#include <stb/stb_image_resize2.h>
#include <stb/stb_image_write.h>

#include <vector>

#include "TextureArray.hpp"

using namespace std;
namespace fs = std::filesystem;


/**
 * This function loads the texture from the given path. If the type of texture being loaded is a preview image, it will load that instead. 
 * If the preview image does not exist, it will create one on the first time loading that image preview by resizing the original image, then save it and load it.
 */
void TextureArray::loadTextureArray(){
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    // Load the first image to get size and format
    unsigned char* data = stbi_load(paths[0].c_str(), &width, &height, &nrChannels, 4);
    if (!data) throw std::runtime_error("Failed to load texture: " + paths[0]);
    stbi_image_free(data);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);

    // Allocate the full texture array
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, 
        width, height, static_cast<GLsizei>(paths.size()), 
        0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );

    // Upload each texture to a layer in the array
    for (size_t i = 0; i < paths.size(); ++i) {
        int w, h, c;
        unsigned char* image = stbi_load(paths[i].c_str(), &w, &h, &c, 4);
        if (!image) {
            throw std::runtime_error("Failed to load texture: " + paths[i]);
        }

        if (w != width || h != height) {
            stbi_image_free(image);
            throw std::runtime_error("All textures must be the same size: " + paths[i]);
        }

        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,
            0, 0, static_cast<GLint>(i),
            width, height, 1,
            GL_RGBA, GL_UNSIGNED_BYTE,
            image
        );

        stbi_image_free(image);
    }

    // Mipmap + filtering
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Unbind 
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}


TextureArray::TextureArray(vector<string> paths, string type, string name) 
                                : paths(paths), type(type), name(name) {
    // Load the texture array
    loadTextureArray();
}

TextureArray::~TextureArray(){
    // glDeleteTextures(1, &id);
}

void TextureArray::bind(int textureNumber){
    // Check that the texture number is between 0 and 31
    if (textureNumber < 0 || textureNumber > 31){
        cout << "ERROR::TEXTURE::BIND::TEXTURE_NUMBER_OUT_OF_RANGE: " << textureNumber << endl;
        return;
    }
    glActiveTexture(GL_TEXTURE0 + textureNumber);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}

void TextureArray::unbind(int textureNumber){
    // Check that the texture number is between 0 and 31
    if (textureNumber < 0 || textureNumber > 31){
        cout << "ERROR::TEXTURE::UNBIND::TEXTURE_NUMBER_OUT_OF_RANGE: " << textureNumber << endl;
        return;
    }
    glActiveTexture(GL_TEXTURE0 + textureNumber);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
