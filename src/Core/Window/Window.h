#pragma once

#include "pch.h"
#include "GLFW/glfw3.h"

class Window
{
public:
    Window();
    ~Window();
    
public:
    GLFWwindow* NativeWindow = nullptr;
};
