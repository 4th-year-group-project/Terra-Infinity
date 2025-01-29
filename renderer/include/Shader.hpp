/*
    This class represents a shader that will contain both a vertex shader and a fragment shader.
    This will allow objects to be rendered to the screen using OpenGL
*/

#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <optional>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glad/glad.h>
    #include <glm/glm.hpp>
#endif

using namespace std;

class Shader{
private:
    unsigned int id;
    unsigned int vertex;
    unsigned int fragment;
    optional<unsigned int> geometry;
    string vertexPath;
    string fragmentPath;
    optional<string> geometryPath;

    void checkCompileErrors(GLuint shader, string type);

    string readFile(const char* filePath);
    string readFile(const string filePath);

    void constructShaders(
        const char* vertexCode,
        const char* fragmentCode,
        optional<const char*> geometryCode
    );

public:
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);
    Shader(const char* vertexPath, const char* fragmentPath);
    Shader(const string vertexPath, const string fragmentPath, const string geometryPath);
    Shader(const string vertexPath, const string fragmentPath);
    ~Shader();

    // Getters and setters
    unsigned int getId(){return id;}
    unsigned int getVertex(){return vertex;}
    unsigned int getFragment(){return fragment;}
    optional<unsigned int> getGeometry(){return geometry;}
    string getVertexPath(){return vertexPath;}
    string getFragmentPath(){return fragmentPath;}
    optional<string> getGeometryPath(){return geometryPath;}
    void setId(unsigned int id){this->id = id;}
    void setVertex(unsigned int vertex){this->vertex = vertex;}
    void setFragment(unsigned int fragment){this->fragment = fragment;}
    void setGeometry(optional<unsigned int> geometry){this->geometry = geometry;}
    void setVertexPath(string vertexPath){this->vertexPath = vertexPath;}
    void setFragmentPath(string fragmentPath){this->fragmentPath = fragmentPath;}
    void setGeometryPath(optional<string> geometryPath){this->geometryPath = geometryPath;}

    // Activate the shader
    void use();
    void deactivate();

    // Utility uniform functions
    void setBool(const string &name, bool value) const;
    void setInt(const string &name, int value) const;
    void setFloat(const string &name, float value) const;
    void setDouble(const string &name, double value) const;
    void setVec2(const string &name, const glm::vec2 &value) const;
    void setVec2(const string &name, float x, float y) const;
    void setVec2(const string &name, double x, double y) const;
    void setVec3(const string &name, const glm::vec3 &value) const;
    void setVec3(const string &name, float x, float y, float z) const;
    void setVec3(const string &name, double x, double y, double z) const;
    void setVec4(const string &name, const glm::vec4 &value) const;
    void setVec4(const string &name, float x, float y, float z, float w) const;
    void setVec4(const string &name, double x, double y, double z, double w) const;
    void setMat2(const string &name, const glm::mat2 &mat) const;
    void setMat3(const string &name, const glm::mat3 &mat) const;
    void setMat4(const string &name, const glm::mat4 &mat) const;
};

#endif // SHADER_HPP