#pragma once

#include "pch.h"

class Renderer;
class Window;
class Scene;

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
    std::unique_ptr<Window> AppWindow;
    std::unique_ptr<Scene> MainScene;
    
    static std::shared_ptr<Application> Instance;
};
