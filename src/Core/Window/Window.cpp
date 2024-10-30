#include "Window.h"

static void CursorPosCallback(GLFWwindow* Window, double XPos, double YPos)
{
    
}

static void ScrollCallback(GLFWwindow* Window, double XOffset, double YOffset)
{
    
}

static void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Mods)
{
    
}

static void KeyCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mods)
{
    
}

static void CharCallback(GLFWwindow* Window, unsigned int Char)
{
    
}

static void WindowSizeCallback(GLFWwindow* Window, int Width, int Height)
{
    WindowProps& WindowData = *static_cast<WindowProps*>(glfwGetWindowUserPointer(Window));
    WindowData.Width = Width;
    WindowData.Height = Height;
}

static void WindowCloseCallback(GLFWwindow* Window)
{
    WindowProps& WindowData = *static_cast<WindowProps*>(glfwGetWindowUserPointer(Window));
}

static void GLFWErrorCallback(int Error, const char* Description)
{
    LOG_ERROR("GLFW Error {0}: {1}\n", Error, Description);
}

Window::Window(const WindowProps& Props)
{
    Init(Props);
}

Window::~Window()
{
    Shutdown();
}

void Window::Init(const WindowProps& Props)
{
    WindowData.Name = Props.Name;
    WindowData.Width = Props.Width;
    WindowData.Height = Props.Height;
    
    glfwSetErrorCallback(GLFWErrorCallback);
    
    // glfw: initialize and configure
    // ------------------------------
    if(!glfwInit())
    {
        return;
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
    // GL 3.3 + GLSL 150
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    // glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
#endif

    // Create window with graphics context
    NativeWindow = glfwCreateWindow(static_cast<int>(WindowData.Width), static_cast<int>(WindowData.Height), WindowData.Name.c_str(), nullptr, nullptr);
    if (!NativeWindow)
    {
        LOG_ERROR("Failed to create GLFW window \n");
        glfwTerminate();
        return;
    }
     
    Context = std::make_shared<RenderContext>(NativeWindow);
    Context->Init();
    
    glfwSetWindowUserPointer(NativeWindow, &WindowData);

    // Set GLFW callback functions
    glfwSetWindowSizeCallback(NativeWindow, WindowSizeCallback);
    glfwSetWindowCloseCallback(NativeWindow, WindowCloseCallback);
    
    glfwSetCursorPosCallback(NativeWindow, CursorPosCallback);
    // glfwSetScrollCallback(NativeWindow, ScrollCallback);
    // glfwSetMouseButtonCallback(NativeWindow, MouseButtonCallback);
    //
    // glfwSetKeyCallback(NativeWindow, KeyCallback);
    // glfwSetCharCallback(NativeWindow, CharCallback);
}

void Window::Shutdown()
{
    glfwDestroyWindow(NativeWindow);
}

void Window::Update()
{
    glfwPollEvents();
    Context->SwapBuffers();
}

void Window::ResizeWindow()
{
}

GLFWwindow* Window::GetNativeWindow() const
{
    return NativeWindow;
}

unsigned int Window::GetWidth() const
{
    return WindowData.Width;
}

unsigned int Window::GetHeight() const
{
    return WindowData.Height;
}
