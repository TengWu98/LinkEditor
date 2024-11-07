#include "Window.h"

#include <imgui.h>

#include "Core/Event/Event.h"
#include "Core/Event/KeyEvent.h"
#include "Core/Event/MouseEvent.h"
#include "Core/Event/ApplicationEvent.h"

MESH_EDITOR_NAMESPACE_BEGIN

static void CursorPosCallback(GLFWwindow* Window, double XPos, double YPos)
{
    const WindowProps& WindowData = *static_cast<WindowProps*>(glfwGetWindowUserPointer(Window));
    MouseMovedEvent Event(static_cast<float>(XPos), static_cast<float>(YPos));
    WindowData.EventCallback(Event);
}

static void ScrollCallback(GLFWwindow* Window, double XOffset, double YOffset)
{
    const WindowProps& WindowData = *static_cast<WindowProps*>(glfwGetWindowUserPointer(Window));
    MouseScrolledEvent Event(static_cast<float>(XOffset), static_cast<float>(YOffset));
    WindowData.EventCallback(Event);
}

static void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Mods)
{
    const WindowProps& WindowData = *static_cast<WindowProps*>(glfwGetWindowUserPointer(Window));
    switch (Action)
    {
        case GLFW_PRESS:
        {
            MouseButtonPressedEvent Event(static_cast<MouseCode>(Button));
            WindowData.EventCallback(Event);
            break;
        }
        case GLFW_RELEASE:
        {
            MouseButtonReleasedEvent Event(static_cast<MouseCode>(Button));
            WindowData.EventCallback(Event);
            break;
        }
    }
}

static void KeyCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mods)
{
    const WindowProps& WindowData = *static_cast<WindowProps*>(glfwGetWindowUserPointer(Window));
    switch (Action)
    {
        case GLFW_PRESS:
        {
            KeyPressedEvent Event(static_cast<KeyCode>(Key), 0);
            WindowData.EventCallback(Event);
            break;
        }
        case GLFW_RELEASE:
        {
            KeyReleasedEvent Event(static_cast<KeyCode>(Key));
            WindowData.EventCallback(Event);
            break;
        }
        case GLFW_REPEAT:
        {
            KeyPressedEvent Event(static_cast<KeyCode>(Key), true);
            WindowData.EventCallback(Event);
            break;
        }
    }
}

static void CharCallback(GLFWwindow* Window, unsigned int Char)
{
    const WindowProps& WindowData = *static_cast<WindowProps*>(glfwGetWindowUserPointer(Window));
    KeyTypedEvent Event(static_cast<KeyCode>(Char));
    WindowData.EventCallback(Event);
}

static void WindowSizeCallback(GLFWwindow* Window, int Width, int Height)
{
    WindowProps& WindowData = *static_cast<WindowProps*>(glfwGetWindowUserPointer(Window));
    WindowData.Width = Width;
    WindowData.Height = Height;

    WindowResizeEvent Event(Width, Height);
    WindowData.EventCallback(Event);
}

static void WindowCloseCallback(GLFWwindow* Window)
{
    const WindowProps& WindowData = *static_cast<WindowProps*>(glfwGetWindowUserPointer(Window));
    WindowCloseEvent Event;
    WindowData.EventCallback(Event);
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
    
    // for windows: GL 4.5 + GLSL 150
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    // glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    
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
    glfwSetScrollCallback(NativeWindow, ScrollCallback);
    glfwSetMouseButtonCallback(NativeWindow, MouseButtonCallback);
    glfwSetKeyCallback(NativeWindow, KeyCallback);
    glfwSetCharCallback(NativeWindow, CharCallback);
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

void Window::SetEventCallback(const EventCallbackFn& Callback)
{
    WindowData.EventCallback = Callback;
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

MESH_EDITOR_NAMESPACE_END
