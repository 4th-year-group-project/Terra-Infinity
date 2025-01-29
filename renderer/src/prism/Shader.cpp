/*
    This class represents a shader that will contain both a vertex shader and a fragment shader.
    This will allow objects to be rendered to the screen using OpenGL
*/

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

#include "Shader.hpp"

using namespace std;

string Shader::readFile(const char* filePath){
    string content;
    ifstream fileStream;
    fileStream.exceptions(ifstream::failbit | ifstream::badbit);
    try{
        fileStream.open(filePath);
        stringstream fileStringStream;
        fileStringStream << fileStream.rdbuf();
        fileStream.close();
        content = fileStringStream.str();
    } catch (ifstream::failure& e){
        cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << endl;
    }
    return content;
}

string Shader::readFile(const string filePath){
    return readFile(filePath.c_str());
}


void Shader::checkCompileErrors(GLuint shader, string type){
    GLint success;
    GLchar infoLog[2048];
    if (type != "PROGRAM"){
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success){
            glGetShaderInfoLog(shader, 2048, NULL, infoLog);
            cout << "ERROR::SHADER_COMPILATION_ERROR of type: " <<
                type << "\n" << infoLog <<
                "\n -- --------------------------------------------------- -- " << endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success){
            glGetProgramInfoLog(shader, 2048, NULL, infoLog);
            cout << "ERROR::PROGRAM_LINKING_ERROR of type: " <<
                type << "\n" << infoLog <<
                "\n -- --------------------------------------------------- -- " << endl;
        }
    }
}


void Shader::constructShaders(
    const char* vertexCode,
    const char* fragmentCode,
    optional<const char*> geometryCode
){
    vertex = glCreateShader(GL_VERTEX_SHADER);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    // Check if the geometry shader is present
    if (geometryCode.has_value()){
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
    } else {
        geometry = nullopt;
    }
    glShaderSource(vertex, 1, &vertexCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    glShaderSource(fragment, 1, &fragmentCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    if (geometry.has_value()){
        glShaderSource(geometry.value(), 1, &(geometryCode.value()), NULL);
        glCompileShader(geometry.value());
        checkCompileErrors(geometry.value(), "GEOMETRY");
    }
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    if (geometry.has_value()){
        glAttachShader(id, geometry.value());
    }
    glLinkProgram(id);
    checkCompileErrors(id, "PROGRAM");
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometry.has_value()){
        glDeleteShader(geometry.value());
    }
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath){
    this->vertexPath = vertexPath;
    this->fragmentPath = fragmentPath;
    this->geometryPath = geometryPath;

    // Get the vertex/fragment/geometery source code from the file paths
    const char *vertexCode = readFile(vertexPath).c_str();
    const char * fragmentCode = readFile(fragmentPath).c_str();
    const char * geometryCode = readFile(geometryPath).c_str();

    // Compile the shaders
    constructShaders(vertexCode, fragmentCode, geometryCode);
}

Shader::Shader(const char* vertexPath, const char* fragmentPath){
    this->vertexPath = vertexPath;
    this->fragmentPath = fragmentPath;
    this->geometryPath = nullopt;

    // Get the vertex/fragment source code from the file paths
    const char *vertexCode = readFile(vertexPath).c_str();
    const char * fragmentCode = readFile(fragmentPath).c_str();

    // Compile the shaders
    constructShaders(vertexCode, fragmentCode, nullopt);
}

Shader::Shader(string vertexPath, string fragmentPath, string geometryPath){
    this->vertexPath = vertexPath;
    this->fragmentPath = fragmentPath;
    this->geometryPath = geometryPath;

    // Get the vertex/fragment/geometery source code from the file paths
    const char *vertexCode = readFile(vertexPath).c_str();
    const char * fragmentCode = readFile(fragmentPath).c_str();
    const char * geometryCode = readFile(geometryPath).c_str();

    // Compile the shaders
    constructShaders(vertexCode, fragmentCode, geometryCode);
}

Shader::Shader(string vertexPath, string fragmentPath){
    this->vertexPath = vertexPath;
    this->fragmentPath = fragmentPath;
    this->geometryPath = nullopt;

    // Get the vertex/fragment source code from the file paths
    const char *vertexCode = readFile(vertexPath).c_str();
    const char * fragmentCode = readFile(fragmentPath).c_str();

    // Compile the shaders
    constructShaders(vertexCode, fragmentCode, nullopt);
}

Shader::~Shader(){
    glDeleteProgram(id);
}

void Shader::use(){
    glUseProgram(id);
}

void Shader::deactivate(){
    glUseProgram(0);
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setDouble(const std::string &name, double value) const
{
    glUniform1d(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}

void Shader::setVec2(const std::string &name, double x, double y) const
{
    glUniform2d(glGetUniformLocation(id, name.c_str()), x, y);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string &name, double x, double y, double z) const
{
    glUniform3d(glGetUniformLocation(id, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}

void Shader::setVec4(const std::string &name, double x, double y, double z, double w) const
{
    glUniform4d(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
