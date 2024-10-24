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

Window::Window()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window with graphics context
    NativeWindow = glfwCreateWindow(1280, 720, "Mesh Editor", nullptr, nullptr);
    if (NativeWindow == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    
    glfwMakeContextCurrent(NativeWindow);
    glfwSwapInterval(1); // Enable vsync

    glfwSetCursorPosCallback(NativeWindow, MouseCallback);
}

Window::~Window()
{
    glfwTerminate();
}
