#pragma once

#include "pch.h"
#include "GLFW/glfw3.h"

MESH_EDITOR_NAMESPACE_BEGIN

class RenderContext
{
public:
    RenderContext(GLFWwindow* InWindowHandle);
    
    void Init();
    void SwapBuffers();
    
private:
    GLFWwindow* WindowHandle;
};

MESH_EDITOR_NAMESPACE_END
