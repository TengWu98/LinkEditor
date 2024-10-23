#include "Application.h"

#include "imgui.h"
#include "imgui_internal.h"
#include <nfd.h>

static void ShowDockingDisabledMessage()
{
    ImGuiIO& IO = ImGui::GetIO();
    ImGui::Text("ERROR: Docking is not enabled! See Demo > Configuration.");
    ImGui::Text("Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your code, or ");
    ImGui::SameLine(0.0f, 0.0f);
    if (ImGui::SmallButton("click here"))
        IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

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
    static bool bIsPadding = true;
    static bool bIsShowLogWindow = false;
    
    // top main menu bar
    if (ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Load Mesh"))
            {
                static const std::vector<nfdfilteritem_t> Filters {
                    {"Mesh object", "obj,off,ply,stl,om"}
                };
                nfdchar_t *NFDPath;
                nfdresult_t Result = NFD_OpenDialog(&NFDPath, Filters.data(), Filters.size(), "");
                if (Result == NFD_OKAY)
                {
                    const auto Path = fs::path(NFDPath);
                    // MainScene->AddMesh(Path, {.Name = Path.filename().string()});
                    NFD_FreePath(NFDPath);
                }
                else if (Result != NFD_CANCEL) {
                    throw std::runtime_error(std::format("Error loading mesh file: {}", NFD_GetError()));
                }
                
                // TODO(WT) 添加导入、保存标签文件的处理
            }
            
            ImGui::EndMenu();
        }
        
        if(ImGui::BeginMenu("Options"))
        {
            ImGui::MenuItem("Fullscreen", nullptr, &bIsFullScreen);
            ImGui::MenuItem("Padding", nullptr, &bIsPadding);
            ImGui::MenuItem("Show Log Window", nullptr, &bIsShowLogWindow);
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
        
        ImGui::EndMainMenuBar();
    }

    const ImGuiViewport* Viewport = ImGui::GetMainViewport();
    if(!Viewport)
    {
        return;
    }

    static ImGuiDockNodeFlags DockSpaceFlags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoDocking;
    
    if(bIsFullScreen)
    {
        ImGui::SetNextWindowPos(Viewport->WorkPos);
        ImGui::SetNextWindowSize(Viewport->WorkSize);
        ImGui::SetNextWindowViewport(Viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        WindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        WindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        ImGui::PopStyleVar(2);
    }
    else
    {
        DockSpaceFlags &= ~ImGuiDockNodeFlags_PassthruCentralNode;   
    }

    if(!bIsPadding)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PopStyleVar();
    }
    
    if(DockSpaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
    {
        WindowFlags |= ImGuiWindowFlags_NoBackground;
    }

    // show docking space
    ImGui::Begin("Mesh Editor", nullptr, WindowFlags);
    
    ImGuiIO& IO = ImGui::GetIO();
    if(IO.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID DockSpaceId = ImGui::GetID("MeshEditorDockSpace");
        ImGui::DockSpace(DockSpaceId, ImVec2(0.0f, 0.0f), DockSpaceFlags);
    }
    else
    {
        ShowDockingDisabledMessage();
    }
    
    ImGui::End();

    // show control window
    static float FloatValue = 0.0f;
    static int Counter = 0;
    static ImVec4 ClearColor = ImVec4(1.f, 0.f, 0.f, 1.f);
    
    ImGui::Begin("Basic Control");
    
    ImGui::SliderFloat("float", &FloatValue, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&ClearColor); // Edit 3 floats representing a color
        
    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        Counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", Counter);
        
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / IO.Framerate, IO.Framerate);
    
    ImGui::End();

    // show viewport
    ImGui::Begin("Viewport");

    ImGui::End();

    // show log message
    if(bIsShowLogWindow)
    {
        ImGui::Begin("Log");
    
        ImGui::End();
    }
    
    ImGui::Render();
}
