/**
 * This file contains the class for the skybox object that will be rendered in the scene
 */

#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <vector>
#include <memory>
#include <string>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/stb/stb_image.h"
    #include "/dcs/large/efogahlewem/.local/include/glm/gtc/matrix_transform.hpp"
#else
    #include <glm/glm.hpp>
    #include <glad/glad.h>
    #include <stb/stb_image.h>
    #include <glm/gtc/matrix_transform.hpp>
#endif

#include "IRenderable.hpp"
#include "Object.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"
#include "Settings.hpp"

using namespace std;

class SkyBox: public IRenderable, public Object {
private:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<string> faceTextures;
    unsigned int textureId;

    unsigned int loadCubeMap(vector<string> faces);
public:
    SkyBox(
        vector<string> inFaceTextures,
        Settings settings
    );
    ~SkyBox();

    int getTextureId(){return textureId;}
    void setTextureId(unsigned int inTextureId){textureId = inTextureId;}
    vector<string> getFaceTextures(){return faceTextures;}
    void setFaceTextures(vector<string> inFaceTextures){faceTextures = inFaceTextures;}

    void setupData() override;
    void render(
        glm::mat4 view,
        glm::mat4 projection,
        vector<shared_ptr<Light>> lights,
        glm::vec3 viewPos
    ) override;
    void updateData() override;
};


#endif // SKYBOX_HPP