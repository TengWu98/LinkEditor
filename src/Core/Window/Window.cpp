#include "Window.h"

static void MouseCallback(GLFWwindow* Window, double XPos, double YPos)
{
    
}

static void ScrollCallback(GLFWwindow* Window, double XOffset, double YOffset)
{
    
}

static void MouseClickCallback(GLFWwindow* Window, int Button, int Action, int Mods)
{
    
}

static void KeyCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mods)
{
    
}

static void GLFWErrorCallback(int Error, const char* Description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", Error, Description);
}

Window::Window()
{
    glfwSetErrorCallback(GLFWErrorCallback);
    
    // glfw: initialize and configure
    // ------------------------------
    if(!glfwInit())
    {
        glfwTerminate();
    }
    
    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    NativeWindow = glfwCreateWindow(1280, 720, "Mesh Editor", nullptr, nullptr);
    if (!NativeWindow)
    {
        std::cout << "Failed to create GLFW window \n";
        glfwTerminate();
        return;
    }
    
    glfwMakeContextCurrent(NativeWindow);
    glfwSwapInterval(1); // Enable vsync

    // Set GLFW callback functions
    glfwSetCursorPosCallback(NativeWindow, MouseCallback);
}

Window::~Window()
{
    glfwTerminate();
}
