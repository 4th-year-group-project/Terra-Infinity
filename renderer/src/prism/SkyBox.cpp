/**
 * 
 */

#include <vector>
#include <memory>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glm/gtc/matrix_transform.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
#else
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glad/glad.h>
#endif
#include <stb/stb_image.h>

#include "SkyBox.hpp"
#include "Settings.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"


SkyBox::SkyBox(
    vector<string> inFaceTextures,
    Settings settings
):
    faceTextures(inFaceTextures)
{
    vertices = {
        Vertex(  // 0
            glm::vec3(-1.0f,  1.0f, -1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec2(0.0f, 0.0f)
        ),
        Vertex(  // 1
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec2(0.0f, 1.0f)
        ),
        Vertex(  // 2
            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec2(1.0f, 1.0f)
        ),
        Vertex(  // 3
            glm::vec3(1.0f, 1.0f, -1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec2(1.0f, 0.0f)
        ),
        Vertex(  // 4
            glm::vec3(-1.0f, -1.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec2(0.0f, 0.0f)
        ),
        Vertex(  // 5
            glm::vec3(-1.0f, 1.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec2(0.0f, 1.0f)
        ),
        Vertex(  // 6
            glm::vec3(1.0f, -1.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec2(1.0f, 1.0f)
        ),
        Vertex(  // 7
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec2(1.0f, 0.0f)
        )

    };

    indices = {
        0, 1, 2,
        2, 3, 0,

        4, 1, 0,
        0, 5, 4,

        2, 6, 7,
        7, 3, 2,

        4, 5, 7,
        7, 6, 4,

        0, 3, 7,
        7, 5, 0,

        1, 4, 2,
        2, 4, 6
    };

    textureId = loadCubeMap(faceTextures);

    string shaderRoot = getenv("SHADER_ROOT");
    shader = make_shared<Shader>(
        shaderRoot + settings.getFilePathDelimitter() + "skybox_shader.vs",
        shaderRoot + settings.getFilePathDelimitter() + "skybox_shader.fs"
    );

    setupData();
}

SkyBox::~SkyBox(){
}

unsigned int SkyBox::loadCubeMap(vector<string> faces){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void SkyBox::render(
    glm::mat4 view,
    glm::mat4 projection,
    vector<shared_ptr<Light>> lights,
    glm::vec3 viewPos
){
    glDepthFunc(GL_LEQUAL);

    shader->use();

    shader->setInt("skybox", 0);
    view = glm::mat4(glm::mat3(view));
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    // Bind the vertex array
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}
#pragma GCC diagnostic pop

void SkyBox::setupData(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // We pass in the vertex data as a single buffer
    // This should be ordered as position, normal, texCoords per vertex
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // We set the mesh EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // We now need to set the vertex attribute pointers
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);
    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) * 2));
    glEnableVertexAttribArray(2);

    // Unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void SkyBox::updateData(){
    // Do Nothing
}