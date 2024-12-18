﻿#pragma once

#include "pch.h"
#include "Core/Event/Event.h"
#include "Core/Event/KeyEvent.h"
#include "Core/Event/MouseEvent.h"
#include "Core/Event/ApplicationEvent.h"
#include <imgui.h>

LINK_EDITOR_NAMESPACE_BEGIN

class Renderer;
class Window;
class Scene;

struct ApplicationCommandLineArgs
{
    int Count = 0;
    char** Args = nullptr;

    const char* operator[](int index) const
    {
        LINK_EDITOR_ASSERT(index < Count);
        return Args[index];
    }
};

class Application
{
public:
    Application();
    ~Application();

    static std::shared_ptr<Application> GetInstance();
    Scene& GetScene() const;
    Window& GetWindow() const;

    void OnEvent(Event& InEvent);
    
    void Update();

    void SetupImGui();
    void RenderImGUI();

    // Events
    bool OnWindowClose(WindowCloseEvent& InEvent);
    bool OnWindowResize(WindowResizeEvent& InEvent);
    
    bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& InEvent);
    bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& InEvent);
    bool OnMouseMovedEvent(MouseMovedEvent& InEvent);
    bool OnMouseScrolledEvent(MouseScrolledEvent& InEvent);

    bool OnKeyPressedEvent(KeyPressedEvent& InEvent);
    bool OnKeyReleasedEvent(KeyReleasedEvent& InEvent);
    bool OnKyeTypedEvent(KeyEvent& InEvent);

private:
    ApplicationCommandLineArgs CommandLineArgs;
    
    std::unique_ptr<Window> AppWindow;
    std::unique_ptr<Scene> AppScene;
    
    static std::shared_ptr<Application> Instance;
    
    bool bIsRunning = true;
    bool bIsMinimized = false;

    float DeltaTime = 0.0f;

    ImGuiID DockSpaceId;
};

LINK_EDITOR_NAMESPACE_END