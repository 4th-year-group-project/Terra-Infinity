/**
 * @file Window.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the Window class.
 * @details This class is a wrapper around the GLFW window object that will allow us to extend the
 * functionality of the window object.
 * @version 1.0
 * @date 2025
 * 
 */
#include <iostream>
#include <string>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/GLFW/glfw3.h"
#else
    #include <glad/glad.h>
    #include <GLFW/glfw3.h>
#endif

#include "Window.hpp"

using namespace std;

/**
 * @brief This function will be called when an error occurs in GLFW
 * 
 * @param error [in] int The error code
 * @param description [in] const char* The error description 
 * 
 * @return void
 * 
 */
void Window::error_callback(int error, const char* description){
    cerr << "Error" << error <<": " << description << endl;
}

/**
 * @brief Construct a new Window object with the given parameters
 * 
 * @details This constructor will create a window with the given parameters. It will set the
 * window hints and create the window. It will also set the error callback function.
 * 
 * @param window [in] GLFWwindow* The GLFW window object
 * @param inWidth [in] int The width of the window
 * @param inHeight [in] int The height of the window
 * @param inTitle [in] std::string The title of the window
 * @param inHideCursor [in] bool Whether to hide the cursor or not
 * 
 */
Window::Window(GLFWwindow* window, int inWidth, int inHeight, string inTitle, bool inHideCursor):
    window(window), width(inWidth), height(inHeight), title(inTitle), hideCursor(inHideCursor){
    monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);
    vendor = glGetString(GL_VENDOR);
    renderer = glGetString(GL_RENDERER);
}

/**
 * @brief This function will initialize the window and set the window hints
 * 
 * @details This function will create a window with the given parameters. It will set the
 * window hints and create the window. It will also set the error callback function.
 * 
 * @return void
 * 
 */
void Window::initWindow(){
    monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);
    setWindowHints();
    cout << "Created window size: " << width << "x" << height << endl;
    // Get the size of the monitor
    cout << "Monitor width: " << mode->width << " Monitor height: " << mode->height << endl;
    // If they are not equal then we are not in fullscreen mode
    if (width != mode->width || height != mode->height){
        monitor = NULL;
    }
    window = glfwCreateWindow(width, height, title.c_str(), monitor, NULL);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        exit(1);
    }
    // Get the acutal size of the window created
    int tempWidth, tempHeight;
    glfwGetWindowSize(window, &tempWidth, &tempHeight);
    cout << "Actual window size: " << tempWidth << "x" << tempHeight << endl;
    makeContextCurrent();
    if (hideCursor){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to initialize GLAD" << endl;
        glfwTerminate();
        exit(1);
    }
    vendor = glGetString(GL_VENDOR);
    renderer = glGetString(GL_RENDERER);
    cout << "Vendor: " << vendor << endl;
    cout << "Renderer: " << renderer << endl;
}

/**
 * @brief This function will set the window hints for the window
 * 
 * @details This function will set the window hints for the window. It will set the OpenGL version
 * to 3.3 and the profile to core. It will also set the forward compatibility flag for Apple.
 * 
 * @return void
 * 
 */
void Window::setWindowHints(){
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

/**
 * @brief This function will set the framebuffer size callback function for the window
 * 
 * @details The callback function set is based on which platform the code is being run on.
 * 
 * @param framebufferSizeCallback [in] void (*framebufferSizeCallback)(GLFWwindow*, int, int) 
 *  The framebuffer size callback function
 * 
 * @return void
 * 
 */
void Window::setFramebufferSizeCallback(void (*framebufferSizeCallback)(GLFWwindow*, int, int)){
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
}

/**
 * @brief This function will set the cursor position callback function for the window
 * 
 * @details The callback function set is based on which platform the code is being run on.
 * 
 * @param mouse_callback [in] void (*mouse_callback)(GLFWwindow*, double, double) The mouse
 *  callback function
 * 
 * @return void
 * 
 */
void Window::setCursorPosCallback(void (*mouse_callback)(GLFWwindow*, double, double)){
    // Get the current set callback function
    GLFWcursorposfun currentCallback = glfwSetCursorPosCallback(window, mouse_callback);
    cout << "The current callback function is: " << currentCallback << endl;
}

/**
 * @brief This function will set the scroll callback function for the window
 * 
 * @details The callback function set is based on which platform the code is being run on.
 * 
 * @param scroll_callback [in] void (*scroll_callback)(GLFWwindow*, double, double) The scroll
 *  callback function
 * 
 * @return void
 * 
 */
void Window::setScrollCallback(void (*scroll_callback)(GLFWwindow*, double, double)){
    glfwSetScrollCallback(window, scroll_callback);
}

/**
 * @brief This function will set the key callback function for the window
 * 
 * @details The callback function set is based on which platform the code is being run on.
 * 
 * @param key_callback [in] void (*key_callback)(GLFWwindow*, int, int, int, int) The key
 *  callback function
 * 
 * @return void
 * 
 */
void Window::setKeyCallback(void (*key_callback)(GLFWwindow*, int, int, int, int)){
    glfwSetKeyCallback(window, key_callback);
}

/**
 * @brief This function will set the current glfw context to the window created
 * 
 * @details This function will set the current glfw context to the window created. This is used
 * to ensure that the window is the current context for OpenGL calls.
 * 
 * @return void
 * 
 */
void Window::makeContextCurrent(){
    glfwMakeContextCurrent(window);
}
