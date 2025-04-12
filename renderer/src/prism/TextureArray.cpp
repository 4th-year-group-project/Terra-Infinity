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

// This function loads the texture data into RAM using stbi
void TextureArray::loadTextureData() {
    stbi_set_flip_vertically_on_load(true);
    for (size_t i = 0; i < paths.size(); ++i) {
        int w, h, c;
        unsigned char* image = stbi_load(paths[i].c_str(), &w, &h, &c, 4);
        if (!image) {
            throw std::runtime_error("Failed to load texture: " + paths[i]);
        }

        if (i == 0) {
            width = w;
            height = h;
            channels = c;
        } else if (w != width || h != height) {
            stbi_image_free(image);
            throw std::runtime_error("All textures must be the same size: " + paths[i]);
        }

        imageData.push_back(image);

    }
}

// This function uploads the texture data to the GPU
void TextureArray::uploadToGPU() {
    if (imageData.empty()) {
        throw std::runtime_error("No texture data loaded. Call loadTextureData() first.");
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);

    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8,
        width, height, static_cast<GLsizei>(imageData.size()),
        0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );

    for (size_t i = 0; i < imageData.size(); ++i) {
        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,
            0, 0, static_cast<GLint>(i),
            width, height, 1,
            GL_RGBA, GL_UNSIGNED_BYTE,
            imageData[i]
        );
        stbi_image_free(imageData[i]);
    }
    imageData.clear();

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    uploaded = true;
}

TextureArray::TextureArray(vector<string> paths, string type, string name) 
                                : paths(paths), type(type), name(name) {
    // Load the texture array image data into RAM
    loadTextureData();
    uploaded = false; // Set to false until uploaded to GPU
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
