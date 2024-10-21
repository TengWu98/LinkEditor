#include "Window.h"

Window::Window()
    : Name(""), Visible(true)
{
}

Window::Window(std::string InName, bool bIsVisible)
    : Name(InName), Visible(bIsVisible)
{
}

Window::~Window()
{
}
