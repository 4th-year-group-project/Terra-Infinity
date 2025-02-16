#include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
#include "/dcs/large/efogahlewem/.local/include/GLFW/glfw3.h"
#include "/dcs/large/efogahlewem/.local/include/opencv4/opencv2/opencv.hpp"
#include <iostream>
#include <vector>

// Compile with:
// g++ headless.cpp glad.c -ldl -lglfw -o a3.out -I/usr/include/opencv4/ -lopencv_core -lopencv_imgcodecs -lstdc++

// Compile with (on dcs):
// g++ headless.cpp /dcs/large/efogahlewem/.local/lib/glad/glad.c -ldl -lglfw -o a9.out -I/dcs/large/efogahlewem/.local/include/opencv4/opencv2 -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_videoio -lstdc++
// Run with xvfb-run -a ./a9.out
// This tricks linux into thinking there is a display attached, so that glfw can create a window

const unsigned int WIDTH = 1920;
const unsigned int HEIGHT = 1080;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

void error_callback(int error, const char* description) {
    std::cerr << "Error: " << description << std::endl;
}

// Initialize GLFW and GLEW, set up an offscreen context
GLFWwindow* initOpenGL() {
    glfwSetErrorCallback(error_callback);
    std::cout << "Initializing GLFW" << std::endl;
    int temp = glfwInit();
    std::cout << "temp: " << temp << std::endl;
    if (!temp) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }
    std::cout << "GLFW initialized" << std::endl;
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Hide the window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Offscreen", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    // if (glewInit() != GLEW_OK) {
    //     std::cerr << "Failed to initialize GLEW" << std::endl;
    //     return nullptr;
    // }

    return window;
}

GLuint createShaderProgram() {
    // Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Check for compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Check for compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Shader Program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Clean up shaders as they’re now linked into our program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}


GLuint createFramebuffer() {
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create a texture to render into
    GLuint textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    // Create a renderbuffer object for depth and stencil attachment (optional)
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        return 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind
    return framebuffer;
}

void renderScene() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Define triangle vertices
    GLfloat vertices[] = {
         0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint shaderProgram = createShaderProgram();
    glUseProgram(shaderProgram);  // Use our compiled and linked shader program

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Clean up
    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}


void saveFramebufferToImage() {
    std::vector<unsigned char> pixels(WIDTH * HEIGHT * 3);
    glReadPixels(0, 0, WIDTH, HEIGHT, GL_BGR, GL_UNSIGNED_BYTE, pixels.data());
    // std::cout << pixels.data() << std::endl; // Print the pixel data to the terminal

    // OpenCV expects the data in a cv::Mat format, and the image is upside down by default, so we flip it
    cv::Mat image(HEIGHT, WIDTH, CV_8UC3, pixels.data());
    cv::flip(image, image, 0);  // Flip vertically

    // Save image using OpenCV
    if (!cv::imwrite("output_batch.png", image)) {
        std::cerr << "Could not save image with OpenCV" << std::endl;
    }
}

int main() {
    GLFWwindow* window = initOpenGL();
    if (!window) return -1;

    GLuint framebuffer = createFramebuffer();

    // Render to the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    renderScene();
    // We do not want to unbind the framebuffer here, as we want to save the image from it
    // glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind when done

    // Save to an image file
    saveFramebufferToImage();

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}