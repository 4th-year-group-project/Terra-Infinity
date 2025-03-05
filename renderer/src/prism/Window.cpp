/*
    This class is a wrapper around the GLFW window object that will allow us to extend the
    functionality of the window object.
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

void Window::error_callback(int error, const char* description){
    cerr << "Error" << error <<": " << description << endl;
}

Window::Window(GLFWwindow* window, int inWidth, int inHeight, string inTitle, bool inHideCursor):
    window(window), width(inWidth), height(inHeight), title(inTitle), hideCursor(inHideCursor){
    monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);
    vendor = glGetString(GL_VENDOR);
    renderer = glGetString(GL_RENDERER);
}


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

void Window::setWindowHints(){
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

void Window::setFramebufferSizeCallback(void (*framebufferSizeCallback)(GLFWwindow*, int, int)){
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
}

void Window::setCursorPosCallback(void (*mouse_callback)(GLFWwindow*, double, double)){
    // Get the current set callback function
    GLFWcursorposfun currentCallback = glfwSetCursorPosCallback(window, mouse_callback);
    cout << "The current callback function is: " << currentCallback << endl;
}

void Window::setScrollCallback(void (*scroll_callback)(GLFWwindow*, double, double)){
    glfwSetScrollCallback(window, scroll_callback);
}

void Window::setKeyCallback(void (*key_callback)(GLFWwindow*, int, int, int, int)){
    glfwSetKeyCallback(window, key_callback);
}

void Window::makeContextCurrent(){
    glfwMakeContextCurrent(window);
}
