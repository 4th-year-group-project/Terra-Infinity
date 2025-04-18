/*
    This class represents a texture that will be used in rendering an object
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

Texture::Texture(string path, string type, string name){
    this->path = path;
    this->type = type;
    this->name = name;
    this->width = 1920;
    this->height = 1080;
    this->nrChannels = 3;
    loadTexture();
}

Texture::Texture(const char *path, string type, string name){
    this->path = string(path);
    this->type = type;
    this->name = name;
    this->width = 1920;
    this->height = 1080;
    this->nrChannels = 3;
    loadTexture();
}

Texture::Texture(string path, string type, string name, int width, int height, int nrChannels){
    this->path = path;
    this->type = type;
    this->name = name;
    this->width = width;
    this->height = height;
    this->nrChannels = nrChannels;
    loadTexture();
}

Texture::~Texture(){
    // glDeleteTextures(1, &id);
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
