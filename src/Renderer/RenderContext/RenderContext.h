#pragma once

#include "pch.h"
#include "GLFW/glfw3.h"

class RenderContext
{
public:
    RenderContext(GLFWwindow* InWindowHandle);
    
    void Init();
    void SwapBuffers();
    
private:
    GLFWwindow* WindowHandle;
};
