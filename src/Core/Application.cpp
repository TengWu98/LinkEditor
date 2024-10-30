#include "Application.h"
#include "Core/Window/Window.h"
#include "Scene/Scene.h"
#include "Renderer/Camera/Camera.h"

#include <nfd.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

std::shared_ptr<Application> Application::Instance = nullptr;

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
    AppWindow = std::make_unique<Window>(WindowProps());
    MainScene = std::make_unique<Scene>();
    NFD_Init();
    SetupImGui();
}

Application::~Application()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    NFD_Quit();
}

std::shared_ptr<Application> Application::GetInstance()
{
    if(!Instance)
    {
        Instance = std::make_shared<Application>();
    }

    return Instance;
}

void Application::Run()
{
    while (!glfwWindowShouldClose(AppWindow->GetNativeWindow()))
    {
        // Poll and handle events (inputs, window resize, etc.)
        if (glfwGetWindowAttrib(AppWindow->GetNativeWindow(), GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render ImGui
        RenderImGUI();

        // Render Scene
        MainScene->Render();
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Update and Render additional Platform Windows
        ImGuiIO& IO = ImGui::GetIO();
        if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* BackupCurrentContext = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(BackupCurrentContext);
        }
        
        AppWindow->Update();
    }
}

void Application::SetupImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &IO = ImGui::GetIO();
    (void)IO;
    IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    IO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;        // Enable Gamepad Controls
    IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    // IO.ConfigViewportsNoAutoMerge = true;
    // IO.ConfigViewportsNoTaskBarIcon = true;

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& Style = ImGui::GetStyle();
    if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        Style.WindowRounding = 0.0f;
        Style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Fonts(TODO(WT) 当前字体只有windows系统能拿到)
    const char* MSYHPath = "c:\\Windows\\Fonts\\msyh.ttc";
    if (std::filesystem::exists(MSYHPath))
    {
        IO.Fonts->AddFontFromFileTTF(MSYHPath, 18.0f,nullptr,
        IO.Fonts->GetGlyphRangesChineseFull());
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(AppWindow->GetNativeWindow(), true);
    ImGui_ImplOpenGL3_Init(AppWindow->glsl_version);
}

void Application::RenderImGUI()
{
    const float LeftSide = 250;
    const float RightSide = 300;
    // const float TopSide = 250;
    const float BottomSide = 250;
    
    static bool bIsFullScreen = true;

    // for windows
    static bool bIsShowControlWindow = true;
    static bool bIsShowViewport = true;
    static bool bIsShowLogWindow = false;

    const ImGuiViewport* Viewport = ImGui::GetMainViewport();
    if(!Viewport)
    {
        return;
    }

    static ImGuiDockNodeFlags DockSpaceFlags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;
    
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
    
    if(DockSpaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
    {
        WindowFlags |= ImGuiWindowFlags_NoBackground;
    }

    ImGuiIO& IO = ImGui::GetIO();

    // show docking space
    ImGui::Begin("Mesh Editor", nullptr, WindowFlags);

    if(IO.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        DockSpaceId = ImGui::GetID("MeshEditorDockSpace");
        ImGui::DockSpace(DockSpaceId, ImVec2(0.0f, 0.0f), DockSpaceFlags);
    }
    else
    {
        ShowDockingDisabledMessage();
    }
    
    // top menu bar
    if (ImGui::BeginMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Load Mesh"))
            {
                static const std::vector<nfdfilteritem_t> Filters {
                        {"Mesh object", "obj, off, ply, stl, om"}
                };
                nfdchar_t *NFDPath;
                nfdresult_t Result = NFD_OpenDialog(&NFDPath, Filters.data(), Filters.size(), "");
                if (Result == NFD_OKAY)
                {
                    const auto Path = fs::path(NFDPath);
                    
                    MeshCreateInfo MeshCreateInfo;
                    MeshCreateInfo.Name = Path.filename().string();
                    MainScene->AddMesh(Path, MeshCreateInfo);
                    
                    NFD_FreePath(NFDPath);
                }
                else if (Result != NFD_CANCEL) {
                    LOG_ERROR("Error loading mesh file: {0}", NFD_GetError());
                }
                
                // TODO(WT) 添加导入、保存标签文件的处理
            }
            
            ImGui::EndMenu();
        }
        
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

        if(ImGui::BeginMenu("Windows"))
        {
            ImGui::Checkbox("Control Panel", &bIsShowControlWindow);
            ImGui::Checkbox("Viewport", &bIsShowViewport);
            ImGui::Checkbox("Log", &bIsShowLogWindow);
    
            ImGui::EndMenu();
        }
        
        ImGui::EndMenuBar();
    }
    
    ImGui::End();

    // ImGui::ShowDemoWindow();

    // show control window
    if(bIsShowControlWindow)
    {
        ImGui::Begin("Control Panel");
        ImGui::Text("FPS : %.1f", IO.Framerate);
        if(ImGui::CollapsingHeader("General Setting"))
        {
            if(ImGui::TreeNode("Camera"))
            {
                ImGui::SliderFloat("Camera Speed", &MainScene->Camera.MovementSpeed, 0.0f, 50.0f);

                ImGui::TreePop();
                ImGui::Spacing();
            }
        }

        if(ImGui::CollapsingHeader("Rendering Setting"))
        {
            ImGui::SeparatorText("Background Color");
            ImGui::ColorEdit3("Color", (float*)&MainScene->BackgroundColor);
            
            ImGui::SeparatorText("Render Mode");
        }

        if(ImGui::CollapsingHeader("Selection"))
        {
            ImGui::Combo("Selection Mode", (int*)&MainScene->SelectionMode, "Object\0Element\0");

            int CurrentMeshElement = static_cast<int>(MainScene->SelectionMeshElement);
            ImGui::Combo("Mesh Element", (int*)&MainScene->SelectionMeshElement, "None\0Face\0Vertex\0Edge\0");
        }
    
        ImGui::End();
    }

    // show viewport
    if(bIsShowViewport)
    {
        // unsigned int Width = AppWindow->GetWidth();
        // unsigned int Height = AppWindow->GetHeight();
        //
        // ImGui::SetNextWindowSize(ImVec2(Width, Height), ImGuiCond_Always);
        // ImGui::SetNextWindowPos(ImVec2(LeftSide, 0), ImGuiCond_Appearing);
        //
        // ImGuiWindowClass ViewportWindowClass;
        // ViewportWindowClass.DockingAllowUnclassed = true;
        // ViewportWindowClass.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoCloseButton;
        // ImGui::SetNextWindowClass(&ViewportWindowClass);
        //
        // ImGuiWindowFlags ViewportWindowFlags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        //
        // ImGui::Begin("Viewport", nullptr, ViewportWindowFlags);
        ImGui::Begin("Viewport");
        //
        // {
        //     ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        //     ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        //
        //     ImVec2 UVMin = ImVec2(0.0f, 1.0f);      // Top-left
        //     ImVec2 UVMax = ImVec2(1.0f, 0.0f);    // Lower-right
        //
        //     auto InitialCursorPos = ImGui::GetCursorPos();
        //     auto WindowSize = ImGui::GetWindowSize();
        //     auto CentralizedCursorPos = ImVec2((WindowSize.x - Width) * 0.5f, (WindowSize.y - Height + 20) * 0.5f);
        //     ImGui::SetCursorPos(CentralizedCursorPos);
        //
        //     ImGui::PopStyleVar(2);
        //
        //     // ImGui::Image((GLuint *)scene->render_pipeline.postprocess_manager->output_rt->color_buffer, ImVec2(Width, Height), UVMin, UVMax, ImVec4(1,1,1,1), ImVec4(1,1,1,1));
        //     // ImGui::SetItemUsingMouseWheel();
        //     // if (ImGui::IsItemHovered())
        //     // {
        //     //     scene->window->render_camera->ProcessMouseScroll(0, ImGui::GetIO().MouseWheel);
        //     // }
        //     
        //     // if (ImGui::IsWindowHovered()){
        //     //     Input::processInputRenderPanel(window->Window, *window->render_camera, deltaTime);
        //     // }
        // }
        
        ImGui::End();
    }

    // show log message
    if(bIsShowLogWindow)
    {
        ImGui::Begin("Log");
    
        ImGui::End();
    }
    
    ImGui::Render();
}
