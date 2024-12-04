#pragma once

#include "pch.h"
#include "GLFW/glfw3.h"
#include "Renderer/RenderContext/RenderContext.h"
#include "Core/Event/Event.h"

LINK_EDITOR_NAMESPACE_BEGIN

class Camera;
using EventCallbackFn = std::function<void(Event&)>;

struct WindowProps
{
    std::string Name;
    unsigned int Width;
    unsigned int Height;

    EventCallbackFn EventCallback;

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
    void Update();

    unsigned int GetWidth() const;
    unsigned int GetHeight() const;
    void ResizeWindow();

    void SetEventCallback(const EventCallbackFn& Callback);

    GLFWwindow* GetNativeWindow() const;

public:
    const char* glsl_version = "#version 150";
    
private:
    GLFWwindow* NativeWindow = nullptr;
    std::shared_ptr<RenderContext> Context;
    WindowProps WindowData;
};

LINK_EDITOR_NAMESPACE_END
