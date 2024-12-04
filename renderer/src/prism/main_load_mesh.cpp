#include <iostream>
#include <string>

// We need to define two different include guards in the same file as we could be building the project with different
// configurations. To solve this we will use the #indef directive to check if the a specific guard is defined.
#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/GLFW/glfw3.h"
    #include "/dcs/large/efogahlewem/.local/include/stb_image.h"
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glm/gtc/matrix_transform.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glm/gtc/type_ptr.hpp"
#else
    #include <glad/glad.h>
    #include <GLFW/glfw3.h>
    #include <stb_image.h>
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtc/type_ptr.hpp>
#endif

// Local includes
#include <shader_m.h>
#include <camera.h>
#include <load_obj.h>

/*
================================================================================================
                                Function prototypes
================================================================================================
*/
void error_callback(int error, const char* description);
GLFWwindow* initOpenGL();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadCubemap(std::vector<std::string> faces);
unsigned int loadTexture(char const * path);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// Get the PROJECT_ROOT environment variable
const char* projectRoot = getenv("PROJECT_ROOT");

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
const glm::vec3 startPos = glm::vec3(0.0f, 100.0f, 0.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// Lighting properties
// glm::vec3 startingLightPos = glm::vec3(0.0f, 80.0f, 0.0f);
glm::vec3 startingLightPos = glm::vec3(1500.0f, 500.0f, 0.0f);
const glm::vec3 meshColour = glm::vec3(1.0f, 0.5f, 0.31f);
const glm::vec3 lightColour = glm::vec3(1.0f, 1.0f, 1.0f);
const glm::vec3 whiteColour = glm::vec3(1.0f, 1.0f, 1.0f);
const glm::vec3 blackColour = glm::vec3(0.0f, 0.0f, 0.0f);
const float ambientStrength = 0.1f;
const float specularStrength = 0.5f;

// Mesh properties
int meshSize = 1024;
int peakHeight = 96;

void error_callback(int error, const char* description) {
    std::cerr << "Error" << error <<": " << description << std::endl;
}

GLFWwindow* initOpenGL(){
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    std::cout << "Monitor width: " << mode->width << " Monitor height: " << mode->height << std::endl;

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Shark fin", monitor, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }
    // Return the vendor and hint to the model
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);

    std::cout << "Vendor: " << vendor << std::endl;
    std::cout << "Renderer: " << renderer << std::endl;


    return window;
}

int main()
{
    /*
    ================================================================================================
                                        Program configuration
    ================================================================================================
    */
    std::string dirPath = std::string(projectRoot) + "/renderer/src/prism";
    std::string dataPath = std::string(projectRoot) + "/data";
    std::string shaderPath = std::string(projectRoot) + "/renderer/src/prism/shaders";
    std::string texturePath = std::string(projectRoot) + "/renderer/resources/textures";

    GLFWwindow* window = initOpenGL();

//     // glfw: initialize and configure
//     // ------------------------------
//     glfwInit();
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// #ifdef __APPLE__
//     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// #endif

//     // glfw window creation
//     // --------------------
//     // GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "All done", NULL, NULL);
//     // if (window == NULL)
//     // {
//     //     std::cout << "Failed to create GLFW window" << std::endl;
//     //     glfwTerminate();
//     //     return -1;
//     // }

//      // Get the primary monitor and its video mode
//     GLFWmonitor* monitor = glfwGetPrimaryMonitor();
//     const GLFWvidmode* mode = glfwGetVideoMode(monitor);
//     std::cout << "Monitor width: " << mode->width << " Monitor height: " << mode->height << std::endl;
//     // Set GLFW window hints for the OpenGL version you want to use
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//     // Create a full-screen window
//     GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Shark fin", monitor, NULL);
//     if (!window) {
//         std::cerr << "Failed to create GLFW window" << std::endl;
//         glfwTerminate();
//         return -1;
//     }

//     glfwMakeContextCurrent(window);
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//     glfwSetCursorPosCallback(window, mouse_callback);
//     glfwSetScrollCallback(window, scroll_callback);

//     // tell GLFW to capture our mouse
//     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

//     // glad: load all OpenGL function pointers
//     // ---------------------------------------
//     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//     {
//         std::cout << "Failed to initialize GLAD" << std::endl;
//         return -1;
//     }

    /*
    ================================================================================================
                                    Configure global OpenGL state
    ================================================================================================
    */

    glEnable(GL_DEPTH_TEST);


    /*
    ================================================================================================
                                    Creating all of the shaders
    ================================================================================================
    */

    Shader meshShader((shaderPath + "/colour_shader.vs").c_str(), (shaderPath + "/colour_shader.fs").c_str());
    Shader textureShader((shaderPath + "/texture.vs").c_str(), (shaderPath + "/texture.fs").c_str());
    Shader lightShader((shaderPath + "/light_source_shader.vs").c_str(), (shaderPath + "/light_source_shader.fs").c_str());
    Shader normalShader((shaderPath + "/normals_shader.vs").c_str(), (shaderPath + "/normals_shader.fs").c_str());
    Shader lightVectorShader((shaderPath + "/light_vecs_shader.vs").c_str(), (shaderPath + "/light_vecs_shader.fs").c_str());
    Shader axisShader((shaderPath + "/axis_shader.vs").c_str(), (shaderPath + "/axis_shader.fs").c_str());
    Shader skyboxShader((shaderPath + "/skybox.vs").c_str(), (shaderPath + "/skybox.fs").c_str());
    Shader quadShader((shaderPath + "/quad_shader.vs").c_str(), (shaderPath + "/quad_shader.fs").c_str());

    /*
    ================================================================================================
                                    Creating all of the vertex data
    ================================================================================================
    */

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    // std::string obj =  dataPath + "/simple_mesh_with_normals_5.obj";
    std::string obj =  dataPath + "/noise_map3.obj";
    bool res = loadObj(obj.c_str(), vertices, normals, indices);
    if (!res) {
        std::cerr << "Failed to load object file" << std::endl;
        return -1;
    }

    // Print the size of the vertices and normals and indices
    std::cout << "Vertices size: " << vertices.size() << std::endl;
    std::cout << "Normals size: " << normals.size() << std::endl;
    std::cout << "Indices size: " << indices.size() << std::endl;

    // Setting up the light source (a simple sphere)
    // std::vector<glm::vec3> lightVertices;
    // std::vector<glm::vec3> lightNormals;
    // std::string lightObj = dirPath + "/sphere.obj";
    // res = loadObj(lightObj.c_str(), lightVertices, lightNormals);
    // if (!res) {
    //     std::cerr << "Failed to load light object file" << std::endl;
    //     return -1;
    // }

    float cube_vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-(meshSize / 2), 0.0f, -(meshSize / 2)));
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

    // To display the normals we want a list of vertices which alternate between the vertex position and the normal scaled by a factor of 5
    std::vector<glm::vec3> normalVertices;
    for (size_t i = 0; i < vertices.size(); i++) {
        normalVertices.push_back(vertices[i]);
        normalVertices.push_back(vertices[i] + normalMatrix * normals[i] * 5.0f);
    }

    // To display the incident light vectors we want to look at the vector between the vertex and the light source
    std::vector<glm::vec3> incidentLightVertices;
    std::vector<glm::vec3> incidentLightColours;
    // If we have vertex (a,b,c) we transform it to world space (x,y,z) using the model matrix
    // We can then compute the light incident vector (lightPos - (x,y,z)) = (i,j,k)
    // We then have vertices (x,y,z) and (x+i, y+j, z+k) which we can use to draw the incident light vector
    // We then have (x,y,z) be white and (x+i, y+j, z+k) be black
    for (size_t i = 0; i < vertices.size(); i++) {
        glm::vec3 transformedVertex = model * glm::vec4(vertices[i], 1.0f);
        incidentLightVertices.push_back(transformedVertex);
        incidentLightColours.push_back(blackColour);
        glm::vec3 incidentLight = glm::normalize(startingLightPos - transformedVertex);
        incidentLightVertices.push_back(transformedVertex + incidentLight * 5.0f);
        incidentLightColours.push_back(whiteColour);
    }


    // We want to display a simple axis to help with orientation
    std::vector<glm::vec3> axisVertices = {
        startPos,
        startPos + glm::vec3(15.0f, 0.0f, 0.0f),
        startPos,
        startPos + glm::vec3(0.0f, 15.0f, 0.0f),
        startPos,
        startPos + glm::vec3(0.0f, 0.0f, 15.0f)
    };

    // Quad plane
    std::vector<glm::vec3> quadVertices = {
        glm::vec3(-(meshSize / 2), peakHeight * 0.2, -(meshSize / 2)), // Bottom-left
        glm::vec3((meshSize / 2), peakHeight * 0.2, -(meshSize / 2)),  // Bottom-right
        glm::vec3((meshSize / 2), peakHeight * 0.2, (meshSize / 2)),   // Top-right
        glm::vec3(-(meshSize / 2), peakHeight * 0.2, -(meshSize / 2)), // Bottom-left
        glm::vec3((meshSize / 2), peakHeight * 0.2, (meshSize / 2)),   // Top-right
        glm::vec3(-(meshSize / 2), peakHeight * 0.2, (meshSize / 2))   // Top-left
    };

    // Skybox
    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    /*
    ================================================================================================
                                    Creating all VAO and VBO buffers
    ================================================================================================
    */

    // Creating the VAO and VBO for the mesh object
    unsigned int meshVAO, meshVBO, meshEBO;
    glGenVertexArrays(1, &meshVAO);
    glGenBuffers(1, &meshVBO);
    glGenBuffers(1, &meshEBO);

    glBindVertexArray(meshVAO);

    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
    // We need to pass in the vertices and normals as a single buffer
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3) * 2, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), &vertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.size() * sizeof(glm::vec3), &normals[0]);

    // Set the mesh EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // std::cout << "Size of indices times size of unsigned int: " << indices.size() * sizeof(unsigned int) << std::endl;

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(vertices.size() * sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    // Creating the VAO and VBO for the light object
    // As these are different pieces of data we have to create a new VAO and VBO for the light object

    unsigned int lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);

    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    // We need to pass in the vertices and normals as a single buffer
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // Creating the VAO and VBO for the normals
    unsigned int normalVAO, normalVBO;
    glGenVertexArrays(1, &normalVAO);
    glGenBuffers(1, &normalVBO);

    glBindVertexArray(normalVAO);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normalVertices.size() * sizeof(glm::vec3), &normalVertices[0], GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // // Normal attribute (location = 1)
    // // Offset by the size of the vertex data
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(vertices.size() * sizeof(glm::vec3)));
    // glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

    glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(glm::vec3), quadVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0); // Vertex positions
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(quadVertices.size() * sizeof(glm::vec3))); // Normals
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Creating the VAO and VBO for the incident light vectors
    unsigned int incidentLightVAO, incidentLightVBO;
    glGenVertexArrays(1, &incidentLightVAO);
    glGenBuffers(1, &incidentLightVBO);

    glBindVertexArray(incidentLightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, incidentLightVBO);
    glBufferData(GL_ARRAY_BUFFER, incidentLightVertices.size() * sizeof(glm::vec3) * 2, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, incidentLightVertices.size() * sizeof(glm::vec3), &incidentLightVertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, incidentLightVertices.size() * sizeof(glm::vec3), incidentLightVertices.size() * sizeof(glm::vec3), &incidentLightColours[0]);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    // Colour attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(incidentLightVertices.size() * sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Creating the VAO and VBO for the axis
    unsigned int axisVAO, axisVBO;
    glGenVertexArrays(1, &axisVAO);
    glGenBuffers(1, &axisVBO);
    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, axisVertices.size() * sizeof(glm::vec3), &axisVertices[0], GL_STATIC_DRAW);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // Skybox VAO and VBO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    /*
    ================================================================================================
                                    Load all of the required textures
    ================================================================================================
    */
    // Load mesh textures
    unsigned int grass = loadTexture((texturePath + "/grass.jpg").c_str());
    unsigned int rock = loadTexture((texturePath + "/rock.jpg").c_str());
    unsigned int snow = loadTexture((texturePath + "/snow.jpg").c_str());
    unsigned int sand = loadTexture((texturePath + "/sand.jpg").c_str());


    std::vector<std::string> faces
    {
        (texturePath + "/right.bmp"),
        (texturePath + "/left.bmp"),
        (texturePath + "/top.bmp"),
        (texturePath + "/bottom.bmp"),
        (texturePath + "/front.bmp"),
        (texturePath + "/back.bmp")
    };

    unsigned int cubemapTexture = loadCubemap(faces);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // This code is required for wireframe rendering
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /*
    ================================================================================================
                                            Rendering loop
    ================================================================================================
    */

    // Set the starting camera position
    camera.SetPosition(startPos);
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // calculate the new light position assuming it rotates 2pi 60 seconds
        float lightX = 1500.0f * cos(currentFrame / 10.0f);
        float lightZ = 1500.0f * sin(currentFrame / 10.0f);
        glm::vec3 lightPos = glm::vec3(lightX, 500.0f, lightZ);
        // glm::vec3 lightPos = startingLightPos;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDepthFunc(GL_LESS); // set depth function as default


        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1500.0f);

        // active axis shader
        axisShader.use();
        axisShader.setMat4("projection", projection);
        axisShader.setMat4("view", view);
        glm::mat4 axisModel = glm::mat4(1.0f);
        axisModel = glm::translate(axisModel, startPos);
        axisShader.setMat4("model", axisModel);
        glBindVertexArray(axisVAO);
        glDrawArrays(GL_LINES, 0, axisVertices.size());


        // activate shader
        textureShader.use();

        // // Set the min and max heights for the object
        // meshShader.setVec3("objectColour", meshColour);
        // meshShader.setVec3("lightColour", lightColour);
        // meshShader.setVec3("lightPos", lightPos);
        // meshShader.setVec3("viewPos", camera.Position);

        // pass projection matrix and view matrix to shader
        textureShader.setMat4("projection", projection);
        textureShader.setMat4("view", view);
        textureShader.setMat4("model", model);
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        textureShader.setMat3("normalMatrix", normalMatrix);

        // Bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grass);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, rock);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, snow);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, sand);

        textureShader.setInt("grassTexture", 0);
        textureShader.setInt("rockTexture", 1);
        textureShader.setInt("snowTexture", 2);
        textureShader.setInt("sandTexture", 3);

        // Set the light properties
        textureShader.setFloat("ambientStrength", ambientStrength);
        textureShader.setFloat("specularStrength", specularStrength);
        textureShader.setVec3("lightPos", lightPos);
        textureShader.setVec3("viewPos", camera.Position);
        textureShader.setVec3("lightColour", lightColour);


        // Compute the normal matrix

        // // Set the ambient strength and specular strength
        // meshShader.setFloat("ambientStrength", ambientStrength);
        // meshShader.setFloat("specularStrength", specularStrength);

        // Render the mesh
        glBindVertexArray(meshVAO);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshEBO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Render the water plane
        quadShader.use();
        quadShader.setMat4("projection", projection);
        quadShader.setMat4("view", view);
        glm::mat4 modelQuad = glm::mat4(1.0f); // No transformation
        quadShader.setMat4("model", modelQuad);
        quadShader.setVec3("quadColor", glm::vec3(0.0f, 0.2f, 0.5f)); // Blue color
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, quadVertices.size());
        glBindVertexArray(0);

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        skyboxShader.setInt("skybox", 0);
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        if (!(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)) {
            // Render the light source
            lightShader.use();
            lightShader.setMat4("projection", projection);
            lightShader.setMat4("view", view);

            // Model matrix
            glm::mat4 lightModel = glm::mat4(1.0f);
            lightModel = glm::translate(lightModel, lightPos);
            lightModel = glm::scale(lightModel, glm::vec3(40.0f)); // Make the light source larger
            lightShader.setMat4("model", lightModel);

            // Render the light source
            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

        }

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            // Render the normals
            normalShader.use();

            normalShader.setMat4("projection", projection);
            normalShader.setMat4("view", view);
            normalShader.setMat4("model", model);


            glBindVertexArray(normalVAO);
            glDrawArrays(GL_LINES, 0, normalVertices.size());

            // Render the incident light vectors
            lightVectorShader.use();

            lightVectorShader.setMat4("projection", projection);
            lightVectorShader.setMat4("view", view);
            lightVectorShader.setMat4("model", glm::mat4(1.0f));

            // Recompute all of th incident light vectors based on the lightPos
            incidentLightVertices.clear();
            incidentLightColours.clear();
            for (size_t i = 0; i < vertices.size(); i++) {
                glm::vec3 transformedVertex = model * glm::vec4(vertices[i], 1.0f);
                incidentLightVertices.push_back(transformedVertex);
                incidentLightColours.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
                glm::vec3 incidentLight = glm::normalize(lightPos - transformedVertex);
                incidentLightVertices.push_back(transformedVertex + incidentLight * 5.0f);
                incidentLightColours.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
            }
            glBindBuffer(GL_ARRAY_BUFFER, incidentLightVBO);
            glBufferData(GL_ARRAY_BUFFER, incidentLightVertices.size() * sizeof(glm::vec3) * 2, NULL, GL_STATIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, incidentLightVertices.size() * sizeof(glm::vec3), &incidentLightVertices[0]);
            glBufferSubData(GL_ARRAY_BUFFER, incidentLightVertices.size() * sizeof(glm::vec3), incidentLightVertices.size() * sizeof(glm::vec3), &incidentLightColours[0]);


            glBindVertexArray(incidentLightVAO);
            glDrawArrays(GL_LINES, 0, incidentLightVertices.size());
        }

        camera.OnRender();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &meshVAO);
    glDeleteBuffers(1, &meshVBO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &lightVBO);
    glDeleteVertexArrays(1, &normalVAO);
    glDeleteBuffers(1, &normalVBO);
    glDeleteVertexArrays(1, &incidentLightVAO);
    glDeleteBuffers(1, &incidentLightVBO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, shiftPressed, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, shiftPressed, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, shiftPressed, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, shiftPressed, deltaTime);

    // New input for up/down movement
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)  // Move up with SPACE key
        camera.ProcessKeyboard(UP, false, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)  // Move down with LEFT CNTRL key
        camera.ProcessKeyboard(DOWN, false, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
#pragma GCC diagnostic pop


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    // get current window size
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    camera.ProcessMouseMovement(xpos, ypos, xoffset, yoffset, width, height);
}
#pragma GCC diagnostic pop


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
// Disable unused-parameter warning for this function
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
#pragma GCC diagnostic pop

// loads a texture from a file and returns the texture ID
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}


// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(std::vector<std::string> faces)
{
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