#pragma once

#include <imgui.h>

#include "pch.h"

class Renderer;
class Window;
class Scene;

struct ApplicationCommandLineArgs
{
    int Count = 0;
    char** Args = nullptr;

    const char* operator[](int index) const
    {
        MESH_EDITOR_ASSERT(index < Count);
        return Args[index];
    }
};

class Application
{
public:
    Application();
    ~Application();

    static std::shared_ptr<Application> GetInstance();
    
    void Run();

    void SetupImGui();
    void RenderImGUI();

private:
    ApplicationCommandLineArgs CommandLineArgs;
    
    std::unique_ptr<Window> AppWindow;
    std::unique_ptr<Scene> MainScene;
    
    static std::shared_ptr<Application> Instance;

    bool bIsFirstTick = true;

    ImGuiID DockSpaceId;
};
