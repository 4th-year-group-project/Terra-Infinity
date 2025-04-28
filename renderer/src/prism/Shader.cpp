/**
 * @file Shader.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the Shader class.
 * @details This class represents a shader that will contain both a vertex shader and a fragment
 * shader. This will allow objects to be rendered to the screen using OpenGL.
 * @version 1.0
 * @date 2025
 * 
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
/**
 * @brief This function will read a shader program from a file and return the contents of the file
 * as a string.
 * 
 * @param filePath [in] const char* The path to the file
 * 
 * @return std::string The contents of the file
 * 
 */
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
        cout << "    File path: " << filePath << endl;
    }
    return content;
}

/**
 * @brief This function will read a shader program from a file and return the contents of the file
 * as a string.
 * 
 * @param filePath [in] std::string The path to the file
 * 
 * @return std::string The contents of the file
 * 
 */
string Shader::readFile(const string filePath){
    return readFile(filePath.c_str());
}

/**
 * @brief This method will check for compile errors in the shader program. 
 * 
 * @details It will check for errors in the vertex shader, fragment shader, and geometry shader. 
 * It will also check for errors in the combined program object.
 * 
 * @param shader [in] GLuint The shader object
 * @param type [in] std::string The type of shader (vertex, fragment, geometry, program)
 * @param shaderName [in] std::string The name of the shader
 * 
 * @return void
 * 
 */
void Shader::checkCompileErrors(GLuint shader, string type, string shaderName){
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM"){
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success){
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            cout << "ERROR::SHADER_COMPILATION_ERROR of type: " <<
                type << "\n" <<
                "For file: "<< shaderName << "\n" << infoLog <<
                "\n -- --------------------------------------------------- -- " << endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success){
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            cout << "ERROR::SHADER_COMPILATION_ERROR of type: " <<
                type << "\n" <<
                "For file: "<< shaderName << "\n" << infoLog <<
                "\n -- --------------------------------------------------- -- " << endl;
        }
    }
}

/**
 * @brief This function will construct a shader program from the individual shaders.
 * 
 * @details This function will compile the vertex shader, fragment shader, and geometry shader
 * (if present) and link them together to create a shader program. It will also check for errors
 * in the compilation and linking process.
 * 
 * @param vertexCode [in] const char* The vertex shader code
 * @param vertexName [in] std::string The name of the vertex shader
 * @param fragmentCode [in] const char* The fragment shader code
 * @param fragmentName [in] std::string The name of the fragment shader
 * @param geometryCode [in] optional<const char*> The geometry shader code
 * @param geometryName [in] optional<std::string> The name of the geometry shader
 * 
 * @return void
 * 
 */
void Shader::constructShaders(
    const char* vertexCode,
    string vertexName,
    const char* fragmentCode,
    string fragmentName,
    optional<const char*> geometryCode,
    optional<string> geometryName
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
    checkCompileErrors(vertex, "VERTEX", vertexName);
    glShaderSource(fragment, 1, &fragmentCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT", fragmentName);
    // We assume that if geometryCode is present then geometryName is also present
    if (geometry.has_value()){
        glShaderSource(geometry.value(), 1, &(geometryCode.value()), NULL);
        glCompileShader(geometry.value());
        checkCompileErrors(geometry.value(), "GEOMETRY", geometryName.value());
    }
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    if (geometry.has_value()){
        glAttachShader(id, geometry.value());
    }
    glLinkProgram(id);
    checkCompileErrors(id, "PROGRAM", vertexName + " " + fragmentName);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometry.has_value()){
        glDeleteShader(geometry.value());
    }
    cout << "Program ID: " << id << endl;
}

/**
 * @brief This function will create a shader object from the paths provided to each of the 
 * shaders files (vertex, fragment, geometry).
 * 
 * @details This function will read the shader files from the paths provided and compile them into
 * a shader program. It will also check for errors in the compilation and linking process.
 * 
 * @param vertexPath [in] const char* The path to the vertex shader file
 * @param fragmentPath [in] const char* The path to the fragment shader file
 * @param geometryPath [in] const char* The path to the geometry shader file
 *  
 */
Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath){
    this->vertexPath = vertexPath;
    this->fragmentPath = fragmentPath;
    this->geometryPath = geometryPath;

    // Get the vertex/fragment/geometery source code from the file paths
    const char *vertexCode = readFile(vertexPath).c_str();
    const char * fragmentCode = readFile(fragmentPath).c_str();
    const char * geometryCode = readFile(geometryPath).c_str();

    // We want to get the last part of the file path that will work with both file path delimiters
    string vertexName = this->vertexPath.substr(this->vertexPath.find_last_of("/\\") + 1);
    string fragmentName = this->fragmentPath.substr(this->fragmentPath.find_last_of("/\\") + 1);
    string geometryName = this->geometryPath.value().substr(this->geometryPath.value().find_last_of("/\\") + 1);
    // Compile the shaders
    constructShaders(vertexCode, vertexName, fragmentCode, fragmentName, geometryCode, geometryName);
}

/**
 * @brief This function will create a shader object from the paths provided to each of the 
 * shaders files (vertex, fragment).
 * 
 * @details This function will read the shader files from the paths provided and compile them into
 * a shader program. It will also check for errors in the compilation and linking process.
 * 
 * @param vertexPath [in] const char* The path to the vertex shader file
 * @param fragmentPath [in] const char* The path to the fragment shader file
 *  
 */
Shader::Shader(const char* vertexPath, const char* fragmentPath){
    this->vertexPath = vertexPath;
    this->fragmentPath = fragmentPath;
    this->geometryPath = nullopt;

    // Get the vertex/fragment source code from the file paths
    const char *vertexCode = readFile(vertexPath).c_str();
    const char * fragmentCode = readFile(fragmentPath).c_str();

    string vertexName = this->vertexPath.substr(this->vertexPath.find_last_of("/\\") + 1);
    string fragmentName = this->fragmentPath.substr(this->fragmentPath.find_last_of("/\\") + 1);
    // Compile the shaders
    constructShaders(vertexCode, vertexName, fragmentCode, fragmentName, nullopt, nullopt);
}

/**
 * @brief This function will create a shader object from the paths provided to each of the 
 * shaders files (vertex, fragment).
 * 
 * @details This function will read the shader files from the paths provided and compile them into
 * a shader program. It will also check for errors in the compilation and linking process.
 * 
 * @param vertexPath [in] std::string The path to the vertex shader file
 * @param fragmentPath [in] std::string The path to the fragment shader file
 * @param geometryPath [in] std::optional<std::string> The path to the geometry shader file
 *  
 */
Shader::Shader(string vertexPath, string fragmentPath, string geometryPath){
    this->vertexPath = vertexPath;
    this->fragmentPath = fragmentPath;
    this->geometryPath = geometryPath;

    // Get the vertex/fragment/geometery source code from the file paths
    const char *vertexCode = readFile(vertexPath).c_str();
    const char * fragmentCode = readFile(fragmentPath).c_str();
    const char * geometryCode = readFile(geometryPath).c_str();

    string vertexName = this->vertexPath.substr(this->vertexPath.find_last_of("/\\") + 1);
    string fragmentName = this->fragmentPath.substr(this->fragmentPath.find_last_of("/\\") + 1);
    string geometryName = this->geometryPath.value().substr(this->geometryPath.value().find_last_of("/\\") + 1);
    // Compile the shaders
    constructShaders(vertexCode, vertexName, fragmentCode, fragmentName, geometryCode, geometryName);
}

/**
 * @brief This function will create a shader object from the paths provided to each of the 
 * shaders files (vertex, fragment).
 * 
 * @details This function will read the shader files from the paths provided and compile them into
 * a shader program. It will also check for errors in the compilation and linking process.
 * 
 * @param vertexPath [in] std::string The path to the vertex shader file
 * @param fragmentPath [in] std::string The path to the fragment shader file
 *  
 */
Shader::Shader(string vertexPath, string fragmentPath){
    this->vertexPath = vertexPath;
    this->fragmentPath = fragmentPath;
    this->geometryPath = nullopt;

    // Get the vertex/fragment source code from the file paths
    string vertexCodeStr = readFile(vertexPath);
    string fragmentCodeStr = readFile(fragmentPath);
    const char *vertexCode = vertexCodeStr.c_str();
    const char * fragmentCode = fragmentCodeStr.c_str();

    string vertexName = this->vertexPath.substr(this->vertexPath.find_last_of("/\\") + 1);
    string fragmentName = this->fragmentPath.substr(this->fragmentPath.find_last_of("/\\") + 1);
    // Compile the shaders
    constructShaders(vertexCode, vertexName, fragmentCode, fragmentName, nullopt, nullopt);
}

/**
 * @brief Default constructor for the Shader class
 * 
 * @details This constructor will create a shader object with the default values for the shader.
 * 
 */ 
Shader::~Shader(){
    // Do nothing here
}

/**
 * @brief This function will activate the shader program.
 * 
 * @details This function will set the current shader program to the one specified by the id binding
 * it to the OpenGL context and graphics pipeline. This will allow the shader to be used for
 * rendering any draw calls that follow.
 * 
 * @return void
 * 
 */
void Shader::use(){
    glUseProgram(id);
}

/**
 * @brief This function will deactivate the shader program.
 * 
 * @details This function will set the current shader program to 0, which will deactivate the shader
 * program. This will allow the default shader program to be used for rendering any draw calls that
 * follow.
 * 
 * @return void
 * 
 */
void Shader::deactivate(){
    glUseProgram(0);
}

/**
 * @brief This function will set a boolean uniform in the shader program.
 * 
 * 
 * @param name [in] const std::string& The name of the uniform
 * @param value [in] bool The value of the uniform
 * 
 * @return void
 * 
 */
void Shader::setBool(const std::string &name, bool value) const{
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

/**
 * @brief This function will set an integer uniform in the shader program.
 * 
 * 
 * @param name [in] const std::string& The name of the uniform
 * @param value [in] int The value of the uniform
 * 
 * @return void
 * 
 */
void Shader::setInt(const std::string &name, int value) const{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

/**
 * @brief This function will set a float uniform in the shader program.
 * 
 * 
 * @param name [in] const std::string& The name of the uniform
 * @param value [in] float The value of the uniform
 * 
 * @return void
 * 
 */
void Shader::setFloat(const std::string &name, float value) const{
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

/**
 * @brief This function will set a double uniform in the shader program.
 * 
 * 
 * @param name [in] const std::string& The name of the uniform
 * @param value [in] double The value of the uniform
 * 
 * @return void
 * 
 */
void Shader::setDouble(const std::string &name, double value) const{
    glUniform1d(glGetUniformLocation(id, name.c_str()), value);
}

/**
 * @brief This function will set a 2D vector uniform in the shader program.
 * 
 * 
 * @param name [in] const std::string& The name of the uniform
 * @param value [in] const glm::vec2& The value of the uniform
 * 
 * @return void
 * 
 */
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const{
    glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

/**
 * @brief This function will set a 2D vector uniform in the shader program.
 * 
 * 
 * @param name [in] const std::string& The name of the uniform
 * @param x [in] float The x value of the uniform
 * @param y [in] float The y value of the uniform
 * 
 * @return void
 * 
 */
void Shader::setVec2(const std::string &name, float x, float y) const{
    glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}

/**
 * @brief This function will set a 2D vector uniform in the shader program.
 * 
 * 
 * @param name [in] const std::string& The name of the uniform
 * @param x [in] double The x value of the uniform
 * @param y [in] double The y value of the uniform
 * 
 * @return void
 * 
 */
void Shader::setVec2(const std::string &name, double x, double y) const{
    glUniform2d(glGetUniformLocation(id, name.c_str()), x, y);
}

/**
 * @brief This function will set a 3D vector uniform in the shader program.
 * 
 * 
 * @param name [in] const std::string& The name of the uniform
 * @param value [in] const glm::vec3& The value of the uniform
 * 
 * @return void
 * 
 */
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const{
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

/**
 * @brief This function will set a 3D vector uniform in the shader program.
 * 
 * 
 * @param name [in] const std::string& The name of the uniform
 * @param x [in] float The x value of the uniform
 * @param y [in] float The y value of the uniform
 * @param z [in] float The z value of the uniform
 * 
 * @return void
 * 
 */
void Shader::setVec3(const std::string &name, float x, float y, float z) const{
    glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

/**
 * @brief This function will set a 3D vector uniform in the shader program.
 * 
 * 
 * @param name [in] const std::string& The name of the uniform
 * @param x [in] double The x value of the uniform
 * @param y [in] double The y value of the uniform
 * @param z [in] double The z value of the uniform
 * 
 * @return void
 * 
 */
void Shader::setVec3(const std::string &name, double x, double y, double z) const{
    glUniform3d(glGetUniformLocation(id, name.c_str()), x, y, z);
}

/**
 * @brief This function will set a 4D vector uniform in the shader program.
 * 
 * 
 * @param name [in] const std::string& The name of the uniform
 * @param value [in] const glm::vec4& The value of the uniform
 * 
 * @return void
 * 
 */
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const{
    glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

/**
 * @brief This function will set a 4D vector uniform in the shader program.
 * 
 * 
 * @param name [in] const std::string& The name of the uniform
 * @param x [in] float The x value of the uniform
 * @param y [in] float The y value of the uniform
 * @param z [in] float The z value of the uniform
 * @param w [in] float The w value of the uniform
 * 
 * @return void
 * 
 */
void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const{
    glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}

/**
 * @brief This function will set a 4D vector uniform in the shader program.
 * 
 * 
 * @param name [in] const std::string& The name of the uniform
 * @param x [in] double The x value of the uniform
 * @param y [in] double The y value of the uniform
 * @param z [in] double The z value of the uniform
 * @param w [in] double The w value of the uniform
 * 
 * @return void
 * 
 */
void Shader::setVec4(const std::string &name, double x, double y, double z, double w) const{
    glUniform4d(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}

/**
 * @brief This function will set a 2D matrix uniform in the shader program.
 * 
 * 
 * @param name [in] const std::string& The name of the uniform
 * @param mat [in] const glm::mat2& The value of the uniform
 * 
 * @return void
 * 
 */
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const{
    glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

/**
 * @brief This function will set a 3D matrix uniform in the shader program.
 * 
 * 
 * @param name [in] const std::string& The name of the uniform
 * @param mat [in] const glm::mat3& The value of the uniform
 * 
 * @return void
 * 
 */
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const{
    glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

/**
 * @brief This function will set a 4D matrix uniform in the shader program.
 * 
 * 
 * @param name [in] const std::string& The name of the uniform
 * @param mat [in] const glm::mat4& The value of the uniform
 * 
 * @return void
 * 
 */
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const{
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

/**
 * @brief This function will set a 1D array uniform in the shader program.
 * 
 * 
 * @param name [in] const std::string& The name of the uniform
 * @param values [in] const int* The value of the uniform
 * @param count [in] int The number of elements in the array
 * 
 * @return void
 * 
 */
void Shader::setIntArray(const std::string &name, const int* values, int count) const {
    glUniform1iv(glGetUniformLocation(id, name.c_str()), count, values);
}
