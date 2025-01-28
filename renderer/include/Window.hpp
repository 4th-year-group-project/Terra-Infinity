/*
    This class is a wrapper around the GLFW window object that will allow us to extend the
    functionality of the window object.
*/
#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <iostream>
#include <string>

#ifdef DEPARTMENT_BUILD
    // #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/GLFW/glfw3.h"
#else
    // #include <glad/glad.h>
    #include <GLFW/glfw3.h>
#endif

using namespace std;

class Window {
private:
    GLFWwindow* window; // The GLFW window object
    int width; // The width of the window
    int height; // The height of the window
    string title; // The title of the window
    GLFWmonitor* monitor; // The monitor that the window is on
    const GLFWvidmode* mode; // The video mode of the window
    const GLubyte* vendor; // The vendor of the window
    const GLubyte* renderer; // The renderer of the window
    bool hideCursor; // Whether the cursor is hidden or not

public:


    Window(
        int inWidth,
        int inHeight,
        string inTitle,
        bool inHideCursor
    ): width(inWidth), height(inHeight), title(inTitle), hideCursor(inHideCursor) {
        mode = nullptr;
        monitor = nullptr;
        vendor = nullptr;
        renderer = nullptr;
        window = nullptr;
    }
    Window(): Window(1920, 1080, "Prism", false) {}

    ~Window(){
        glfwTerminate();
    }

    static void error_callback(int error, const char* description);

    // Getters and setters
    GLFWwindow *getWindow(){return window;}
    int getWidth(){return width;}
    int getHeight(){return height;}
    string getTitle(){return title;}
    GLFWmonitor *getMonitor(){return monitor;}
    const GLFWvidmode *getMode(){return mode;}
    const GLubyte *getVendor(){return vendor;}
    const GLubyte *getRenderer(){return renderer;}
    bool getHideCursor(){return hideCursor;}
    void setWindow(GLFWwindow *inWindow){window = inWindow;}
    void setWidth(int inWidth){width = inWidth;}
    void setHeight(int inHeight){height = inHeight;}
    void setTitle(string inTitle){title = inTitle;}
    void setMonitor(GLFWmonitor *inMonitor){monitor = inMonitor;}
    void setMode(const GLFWvidmode *inMode){mode = inMode;}
    void setVendor(const GLubyte *inVendor){vendor = inVendor;}
    void setRenderer(const GLubyte *inRenderer){renderer = inRenderer;}
    void setHideCursor(bool inHideCursor){hideCursor = inHideCursor;}

    void initWindow();
    void setWindowHints();
    void setFramebufferSizeCallback(void (*framebufferSizeCallback)(GLFWwindow*, int, int));
    void setCursorPosCallback(void (*mouse_callback)(GLFWwindow*, double, double));
    void setScrollCallback(void (*scroll_callback)(GLFWwindow*, double, double));
    void setInputMode(int mode, int value);
    void makeContextCurrent();


};
#endif // WINDOW_HPP
