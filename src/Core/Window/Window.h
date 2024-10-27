#pragma once

#include "pch.h"
#include "GLFW/glfw3.h"

class Camera;

struct WindowProps
{
    std::string Name;
    unsigned int Width;
    unsigned int Height;

    WindowProps(const std::string& InName = "Mesh Editor", unsigned int InWidth = 1280, unsigned int InHeight = 720)
        : Name(InName), Width(InWidth), Height(InHeight) {}
};

class Window
{
public:
    Window(const WindowProps& Props);
    ~Window();

    void Init(const WindowProps& Props);
    void Shutdown();

    unsigned int GetWidth() const;
    unsigned int GetHeight() const;
    void ResizeWindow();

    GLFWwindow* GetNativeWindow() const;

public:
    const char* glsl_version = "#version 150";
    
private:
    GLFWwindow* NativeWindow = nullptr;
    WindowProps WindowData;
};
