/**
 * This file contains a class for the UI object that will be used to control the renderer and customise the terrain. 
 */

#ifndef UI_HPP
#define UI_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>
 
#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/GLFW/glfw3.h"
#else
    #include <glad/glad.h>
    #include <glm/glm.hpp>
    #include <GLFW/glfw3.h>
#endif
 
#include "Object.hpp"
#include "Settings.hpp"
#include "Window.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
 
using namespace std;
 
class UI {
private:
  
public:
    UI(GLFWwindow *context);
    ~UI();

    void render(shared_ptr<Settings> settings);

};

#endif // UI_HPP