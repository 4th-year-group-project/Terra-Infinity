/*
    This class represents a 2D texture array. 
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
 * This function loads the texture images for each texture file in the paths vector into RAM using stbi
 */
void TextureArray::loadTextureData() {
    stbi_set_flip_vertically_on_load(true); // Flip the image vertically

    for (size_t i = 0; i < paths.size(); ++i) {
        int w, h, c;
        // Load the image data into RAM using stbi
        unsigned char* image = stbi_load(paths[i].c_str(), &w, &h, &c, 4);
        if (!image) {
            throw std::runtime_error("Failed to load texture: " + paths[i]);
        }
        // Set width, height, and channels based on those from first image
        if (i == 0) {
            width = w;
            height = h;
            channels = c;
        } else if (w != width || h != height) {
            stbi_image_free(image);
            // If the image dimensions do not match, throw an error
            throw std::runtime_error("All textures must be the same size: " + paths[i]);
        }
        // Add the image data to the vector
        imageData.push_back(image);

    }
}

/** 
 * This function uploads the texture data to the GPU
 */
void TextureArray::uploadToGPU() {
    // Throw an error if the image data is empty
    if (imageData.empty()) {
        throw std::runtime_error("No texture data loaded. Call loadTextureData() first.");
    }
    // Generate the texture ID
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);

    // Allocate the texture storage for the texture array
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8,
        width, height, static_cast<GLsizei>(imageData.size()),
        0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );

    // Upload each image to the texture array
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
    // Clear the image data from RAM
    imageData.clear();

    // Generate mipmaps for the texture array
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    uploaded = true; // Set to true after uploading to GPU
}


/**
 * Constructor for the TextureArray class   
 * @param paths The paths to the texture files
 * @param type The type of the texture (e.g. diffuse, specular)
 * @param name The name of the texture
 * @return A TextureArray object
 */
TextureArray::TextureArray(vector<string> paths, string type, string name) 
                                : paths(paths), type(type), name(name) {
    loadTextureData(); // Load the texture array image data into RAM
    uploaded = false; // Set to false until uploaded to GPU
}


/**
 * Destructor for the TextureArray class
 */
TextureArray::~TextureArray(){
    // glDeleteTextures(1, &id); 
}

/**
 * This function binds the texture to the specified texture unit
 * @param textureNumber The texture unit to bind the texture to
 */
void TextureArray::bind(int textureNumber){
    // Check that the texture number is between 0 and 31
    if (textureNumber < 0 || textureNumber > 31){
        cout << "ERROR::TEXTURE::BIND::TEXTURE_NUMBER_OUT_OF_RANGE: " << textureNumber << endl;
        return;
    }
    glActiveTexture(GL_TEXTURE0 + textureNumber);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}

/**
 * This function unbinds the texture from the specified texture unit
 * @param textureNumber The texture unit to unbind the texture from
 */
void TextureArray::unbind(int textureNumber){
    // Check that the texture number is between 0 and 31
    if (textureNumber < 0 || textureNumber > 31){
        cout << "ERROR::TEXTURE::UNBIND::TEXTURE_NUMBER_OUT_OF_RANGE: " << textureNumber << endl;
        return;
    }
    glActiveTexture(GL_TEXTURE0 + textureNumber);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
