#pragma once
#include "pch.h"

class Window
{
public:
    Window();
    Window(std::string InName, bool bIsVisible = true);
    ~Window();
    
public:
    std::string Name;
    bool Visible;
};
