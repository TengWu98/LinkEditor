#pragma once

#include "pch.h"
#include "GLFW/glfw3.h"

class Camera;

class Window
{
public:
    Window(std::shared_ptr<Camera> InCamera);
    ~Window();
    
public:
    GLFWwindow* NativeWindow = nullptr;
    std::shared_ptr<Camera> RenderCamera;
    glm::vec4 ClearColor = glm::vec4(1.f, 0.f, 0.f, 1.f);
};
