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

void Window::initWindow(){
    glfwSetErrorCallback(Window::error_callback);
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return;
    }
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);

    window = glfwCreateWindow(width, height, title.c_str(), monitor, NULL);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return;
    }
}

void Window::setWindowHints(){
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Window::setFramebufferSizeCallback(void (*framebufferSizeCallback)(GLFWwindow*, int, int)){
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
}

void Window::setCursorPosCallback(void (*mouse_callback)(GLFWwindow*, double, double)){
    glfwSetCursorPosCallback(window, mouse_callback);
}

void Window::setScrollCallback(void (*scroll_callback)(GLFWwindow*, double, double)){
    glfwSetScrollCallback(window, scroll_callback);
}

void Window::setInputMode(int mode, int value){
    glfwSetInputMode(window, mode, value);
}

void Window::makeContextCurrent(){
    glfwMakeContextCurrent(window);
}