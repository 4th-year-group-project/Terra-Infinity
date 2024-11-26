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

#include <shader_m.h>
#include <camera.h>
#include <load_obj.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

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

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

const glm::vec3 meshColour = glm::vec3(1.0f, 0.5f, 0.31f);
const glm::vec3 lightColour = glm::vec3(1.0f, 1.0f, 1.0f);
const float ambientStrength = 0.1f;
const float specularStrength = 0.5f;

int main()
{
    std::string dirPath = std::string(projectRoot) + "/renderer/src/prism";
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    // GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "All done", NULL, NULL);
    // if (window == NULL)
    // {
    //     std::cout << "Failed to create GLFW window" << std::endl;
    //     glfwTerminate();
    //     return -1;
    // }

     // Get the primary monitor and its video mode
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    std::cout << "Monitor width: " << mode->width << " Monitor height: " << mode->height << std::endl;
    // Set GLFW window hints for the OpenGL version you want to use
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a full-screen window
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Shark fin", monitor, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }


    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    // std::string meshVertexShader =  "/home/ethfar01/Documents/World-Generation/renderer/src/prism/colour_shader.vs";
    // std::string meshFragmentShader ="/home/ethfar01/Documents/World-Generation/renderer/src/prism/colour_shader.fs";
    std::string meshVertexShader =  dirPath + "/shaders/colour_shader.vs";
    std::string meshFragmentShader = dirPath + "/shaders/colour_shader.fs";
    Shader meshShader(meshVertexShader.c_str(), meshFragmentShader.c_str());

    std::string lightVertexShader =  dirPath + "/shaders/light_source_shader.vs";
    std::string lightFragmentShader = dirPath + "/shaders/light_source_shader.fs";
    Shader lightShader(lightVertexShader.c_str(), lightFragmentShader.c_str());


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    // std::string obj =  "/home/ethfar01/Documents/World-Generation/renderer/src/prism/simple_mesh_with_normals.obj";
    std::string obj =  dirPath + "/simple_mesh_with_normals.obj";
    bool res = loadObj(obj.c_str(), vertices, normals);
    if (!res) {
        std::cerr << "Failed to load object file" << std::endl;
        return -1;
    }

    // Creating the VAO and VBO for the mesh object
    unsigned int meshVAO, meshVBO;
    glGenVertexArrays(1, &meshVAO);
    glGenBuffers(1, &meshVBO);

    glBindVertexArray(meshVAO);

    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
    // We need to pass in the vertices and normals as a single buffer
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3) * 2, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), &vertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.size() * sizeof(glm::vec3), &normals[0]);


    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    // Setting up the light source (a simple sphere)
    std::vector<glm::vec3> lightVertices;
    std::vector<glm::vec3> lightNormals;
    std::string lightObj = dirPath + "/sphere.obj";
    res = loadObj(lightObj.c_str(), lightVertices, lightNormals);
    if (!res) {
        std::cerr << "Failed to load light object file" << std::endl;
        return -1;
    }
    // Creating the VAO and VBO for the light object
    // As these are different pieces of data we have to create a new VAO and VBO for the light object

    unsigned int lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);

    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    // We need to pass in the vertices and normals as a single buffer
    glBufferData(GL_ARRAY_BUFFER, lightVertices.size() * sizeof(glm::vec3) * 2, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, lightVertices.size() * sizeof(glm::vec3), &lightVertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, lightVertices.size() * sizeof(glm::vec3), lightVertices.size() * sizeof(glm::vec3), &lightNormals[0]);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    // float minHeight = std::numeric_limits<float>::max();
    // float maxHeight = std::numeric_limits<float>::min();

    // for (const auto& vertex : vertices) {
    //     if (vertex.y < minHeight) {
    //         minHeight = vertex.y;
    //     }
    //     if (vertex.y > maxHeight) {
    //         maxHeight = vertex.y;
    //     }
    // }

    // This code is required for wireframe rendering
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glm::vec3 startingLightPos = glm::vec3(0.0f, 150.0f, -1000.0f);
    glm::vec3 startingCameraPos = glm::vec3(0.0f, 100.0f, 0.0f);

    // render loop
    // -----------
    // Set the starting camera position
    camera.SetPosition(startingCameraPos);
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // calculate the new light position assuming it rotates 2pi 60 seconds
        float lightX = 1000.0f * cos(currentFrame);
        float lightZ = 1000.0f * sin(currentFrame);
        glm::vec3 lightPos = glm::vec3(lightX, 150.0f, lightZ);

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        meshShader.use();

        // // Set the min and max heights for the object
        // meshShader.setFloat("minHeight", minHeight);
        // meshShader.setFloat("maxHeight", maxHeight);
        meshShader.setVec3("objectColour", meshColour);
        meshShader.setVec3("lightColour", lightColour);
        meshShader.setVec3("lightPos", startingLightPos);
        meshShader.setVec3("viewPos", camera.Position);

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1500.0f);
        meshShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        meshShader.setMat4("view", view);

        // Model matrix
        glm::mat4 model = glm::mat4(1.0f); // Adjust this if you want to move the object
        // Transform the mesh backwards by 256 and up by 256 to map (0,y,0) to (-256, y, -256)
        model = glm::translate(model, glm::vec3(-256.0f, 0.0f, -256.0f));
        meshShader.setMat4("model", model);

        // Compute the normal matrix
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        meshShader.setMat3("normalMatrix", normalMatrix);

        // Render the mesh
        glBindVertexArray(meshVAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        // activate shader
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        // Model matrix
        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, startingLightPos);
        lightModel = glm::scale(lightModel, glm::vec3(20.0f)); // Make the light source larger
        lightShader.setMat4("model", lightModel);
        glm::mat3 lightNormalMatrix = glm::transpose(glm::inverse(glm::mat3(lightModel)));
        lightShader.setMat3("normalMatrix", lightNormalMatrix);

        // Render the light source
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, lightVertices.size());


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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // New input for up/down movement
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)  // Move up with SPACE key
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)  // Move down with LEFT SHIFT key
        camera.ProcessKeyboard(DOWN, deltaTime);
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
