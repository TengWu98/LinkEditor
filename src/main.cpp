#include "pch.h"
#include "Core/Application.h"

int main(int argc, char** argv)
{
    auto App = MeshEditor::Application::GetInstance();
    if(App)
    {
        App->Run();
    }

    return 0;
}
