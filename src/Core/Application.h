#pragma once

#include "pch.h"

class Renderer;
class Window;

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
    static std::shared_ptr<Application> Instance;
};
