#include "Application.h"

#include "imgui.h"

Application::Application()
{
}

Application::~Application()
{
}

void Application::Run()
{
    RenderScene();
    RenderUI();
}

void Application::RenderScene()
{
}

void Application::RenderUI()
{
    static bool bIsFullScreen = true;
    static ImGuiDockNodeFlags DockspaceFlags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    
    if(bIsFullScreen)
    {
        const ImGuiViewport* Viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(Viewport->WorkPos);
        ImGui::SetNextWindowSize(Viewport->WorkSize);
        ImGui::SetNextWindowViewport(Viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        WindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        WindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        DockspaceFlags &= ~ImGuiDockNodeFlags_PassthruCentralNode;   
    }
    
    if(DockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
    {
        WindowFlags |= ImGuiWindowFlags_NoBackground;
    }
    
    if(bIsFullScreen)
    {
        ImGui::PopStyleVar(2);
    }
    
    ImGui::Begin("Mesh Editor", nullptr, WindowFlags);
    
    ImGuiIO& IO = ImGui::GetIO();
    if (IO.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID DockspaceId = ImGui::GetID("MeshEditorDockSpace");
        ImGui::DockSpace(DockspaceId, ImVec2(0.0f, 0.0f), DockspaceFlags);
    }
    
    if (ImGui::BeginMenuBar())
    {
        if(ImGui::BeginMenu("Options"))
        {
            ImGui::MenuItem("Fullscreen", nullptr, &bIsFullScreen);
            ImGui::EndMenu();
        }
    
        if(ImGui::BeginMenu("Themes"))
        {
            if(ImGui::MenuItem("Dark"))
            {
                ImGui::StyleColorsDark();
            }
    
            if(ImGui::MenuItem("Light"))
            {
                ImGui::StyleColorsLight();
            }
    
            if(ImGui::MenuItem("Classic"))
            {
                ImGui::StyleColorsClassic();
            }
    
            ImGui::EndMenu();
        }
        
        ImGui::EndMenuBar();
    }
    
    ImGui::End();
}
