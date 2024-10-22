#pragma once

#include "pch.h"

class Application
{
public:
    Application();
    ~Application();
    
    void Run();

    void RenderScene();
    void RenderUI();
};
