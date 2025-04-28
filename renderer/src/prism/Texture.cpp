/**
 * @file Texture.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the Texture class.
 * @version 1.0
 * @date 2025
 * 
 */
#include <filesystem>
#include <string>
#include <iostream>
#include <vector>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/stb/stb_image.h"
    #include "/dcs/large/efogahlewem/.local/include/stb/stb_image_resize2.h"
    #include "/dcs/large/efogahlewem/.local/include/stb/stb_image_write.h"
#else
    #include <glad/glad.h>
    #include <stb/stb_image.h>
    #include <stb/stb_image_resize2.h>
    #include <stb/stb_image_write.h>
#endif

#include "Texture.hpp"

using namespace std;
namespace fs = std::filesystem;


/**
 * This function loads the texture from the given path. If the type of texture being loaded is a preview image, it will load that instead. 
 * If the preview image does not exist, it will create one on the first time loading that image preview by resizing the original image, then save it and load it.
 */
/**
 * @brief This function loads the texture from the given path.
 * 
 * @details If the type of texture being loaded is a preview image, it will load that instead.
 * If the preview image does not exist, it will create one on the first time loading that image 
 * preview by resizing the original image, then save it and load it.
 * 
 * @note This function uses the stb_image library to load the image data and the stb_image_resize2
 * library to resize the image. It also uses the stb_image_write library to save the resized image
 * as a PNG in the previews directory.
 * 
 * @return void
 * 
 */
void Texture::loadTexture(){
    // Create the texture ID and bind it
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Tell stb_image.h to flip on the y-axis if the image is not the logo
    if (type != "logo") {
        stbi_set_flip_vertically_on_load(true); 
    } else {
        stbi_set_flip_vertically_on_load(false); // Do not flip the logo image
    }
    unsigned char *data = nullptr;

    // Get the path to the preview version of the image 
    string previewsRoot = getenv("PREVIEWS_ROOT");
    std::string previewPath = previewsRoot + std::filesystem::path(path).stem().string() + "_preview.png";

    if ((type == "preview") && std::filesystem::exists(previewPath)) {
        // If the preview already exists, load this instead of the original image
        path = previewPath;
    } 

    try{
        // Load the image data
        data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    } catch (exception &e){
        cout << "ERROR::TEXTURE::FAILED_TO_LOAD_TEXTURE: " << e.what() << endl;
    }

    if (data){
        if (type == "preview" && path != previewPath) {
            // Resize the original image and save the preview in the previews directory
            const int previewWidth = 120;
            const int previewHeight = 120;
            stbir_pixel_layout comp = (nrChannels == 4) ? STBIR_RGBA : STBIR_RGB; // Determine the stbir pixel layout based on the number of channels  

            unsigned char* resized = (unsigned char*)malloc(previewWidth * previewHeight * comp); // Allocate memory for the resized image

            stbir_resize_uint8_srgb(data, width, height, 0, resized, previewWidth, previewHeight, 0, comp); // Resize the image

            stbi_write_png(previewPath.c_str(), previewWidth, previewHeight, comp, resized, previewWidth * comp); // Save the resized image as a PNG in the previews directory

            free(resized); // Free the resized image memory
            stbi_image_free(data); // Free the original image memory

            // Load the newly created preview instead
            data = stbi_load(previewPath.c_str(), &width, &height, &nrChannels, 0);
        }
        if (nrChannels == 3){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // Load the texture data
        } else if (nrChannels == 4){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // Load the texture data
        }
        if (type != "preview")
            glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps for the texture if it is not a preview
    } else {
        cout << "ERROR::TEXTURE::FAILED_TO_LOAD_TEXTURE: " << path << endl;
    }
    stbi_image_free(data);
}

/**
 * @brief Construct a new Texture object with the given arguments
 * 
 * @param path [in] std::string The path to the texture
 * @param type [in] std::string The type of the texture
 * @param name [in] std::string The name of the texture
 * 
 */
Texture::Texture(string path, string type, string name){
    this->path = path;
    this->type = type;
    this->name = name;
    this->width = 1920;
    this->height = 1080;
    this->nrChannels = 3;
    loadTexture();
}

/**
 * @brief Construct a new Texture object with the given arguments
 * 
 * @param path [in] const char* The path to the texture
 * @param type [in] std::string The type of the texture
 * @param name [in] std::string The name of the texture
 * 
 */
Texture::Texture(const char *path, string type, string name){
    this->path = string(path);
    this->type = type;
    this->name = name;
    this->width = 1920;
    this->height = 1080;
    this->nrChannels = 3;
    loadTexture();
}

/**
 * @brief Construct a new Texture object with the given arguments
 * 
 * @param path [in] std::string The path to the texture
 * @param type [in] std::string The type of the texture
 * @param name [in] std::string The name of the texture
 * @param width [in] int The width of the texture
 * @param height [in] int The height of the texture
 * @param nrChannels [in] int The number of channels in the texture
 * 
 */
Texture::Texture(string path, string type, string name, int width, int height, int nrChannels){
    this->path = path;
    this->type = type;
    this->name = name;
    this->width = width;
    this->height = height;
    this->nrChannels = nrChannels;
    loadTexture();
}

/**
 * @brief Default destructor for the Texture class allowing for standard cleanup
 * 
 */
Texture::~Texture(){
}

/**
 * @brief This function will bind the texture to the given texture number. It will
 * also activate the texture unit for the given texture number.
 * 
 * @param textureNumber [in] int The texture number to bind the texture to
 * 
 * @details This function will check that the texture number is between 0 and 31. If it is not,
 * it will print an error message and return. It will then activate the texture unit for the given
 * texture number and bind the texture to that unit. This will allow the texture to be used in
 * the shader program for rendering.
 * 
 * @return void
 * 
 */
void Texture::bind(int textureNumber){
    // Check that the texture number is between 0 and 31
    if (textureNumber < 0 || textureNumber > 31){
        cout << "ERROR::TEXTURE::BIND::TEXTURE_NUMBER_OUT_OF_RANGE: " << textureNumber << endl;
        return;
    }
    glActiveTexture(GL_TEXTURE0 + textureNumber);
    glBindTexture(GL_TEXTURE_2D, id);
}

/**
 * @brief This function will unbind the texture from the given texture number. It will
 * also deactivate the texture unit for the given texture number.
 * 
 * @param textureNumber [in] int The texture number to unbind the texture from
 * 
 * @details This function will check that the texture number is between 0 and 31. If it is not,
 * it will print an error message and return. It will then deactivate the texture unit for the given
 * texture number and unbind the texture from that unit.
 * 
 * @return void
 * 
 */
void Texture::unbind(int textureNumber){
    // Check that the texture number is between 0 and 31
    if (textureNumber < 0 || textureNumber > 31){
        cout << "ERROR::TEXTURE::UNBIND::TEXTURE_NUMBER_OUT_OF_RANGE: " << textureNumber << endl;
        return;
    }
    glActiveTexture(GL_TEXTURE0 + textureNumber);
    glBindTexture(GL_TEXTURE_2D, 0);
}
