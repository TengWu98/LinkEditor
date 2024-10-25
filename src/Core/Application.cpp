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
    // Camera暂时写死
    std::shared_ptr<Camera> RenderCamera = std::make_shared<Camera>(glm::vec3(0.0f, 20.0f, 30.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90, -40);
    AppWindow = std::make_unique<Window>(RenderCamera);
    NFD_Init();
    SetupImGui();
}

Application::~Application()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(AppWindow->NativeWindow);
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
    ImVec4 ClearColor = ImVec4(1.f, 0.f, 0.f, 1.f);
    
    while (!glfwWindowShouldClose(AppWindow->NativeWindow))
    {
        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();
        if (glfwGetWindowAttrib(AppWindow->NativeWindow, GLFW_ICONIFIED) != 0)
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
        int display_w, display_h;
        glfwGetFramebufferSize(AppWindow->NativeWindow, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(ClearColor.x * ClearColor.w, ClearColor.y * ClearColor.w, ClearColor.z * ClearColor.w, ClearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        
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
        
        glfwSwapBuffers(AppWindow->NativeWindow);
    }
}

void Application::SetupImGui()
{
    const char* glsl_version = "#version 130";
    
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

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(AppWindow->NativeWindow, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void Application::RenderImGUI()
{
    const float LeftSide = 250;
    const float RightSide = 300;
    // const float TopSide = 250;
    const float BottomSide = 250;
    
    static bool bIsFullScreen = true;
    static bool bIsPadding = true;

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

    if(!bIsPadding)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PopStyleVar();
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

    // show status bar
    ImGui::Text(" %.3f ms/frame (%.1f FPS)", 1000.0f / IO.Framerate, IO.Framerate);
    
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
            ImGui::Checkbox("ControlPanel", &bIsShowControlWindow);
            ImGui::Checkbox("Viewport", &bIsShowViewport);
            ImGui::Checkbox("Log", &bIsShowLogWindow);
    
            ImGui::EndMenu();
        }
        
        ImGui::EndMenuBar();
    }
    
    ImGui::End();

    // show control window
    static float FloatValue = 0.0f;
    static int Counter = 0;
    static ImVec4 ClearColor = ImVec4(1.f, 0.f, 0.f, 1.f);

    if(bIsShowControlWindow)
    {
        ImGui::Begin("Basic Control");
    
        ImGui::SeparatorText("General Setting");
        {
            ImGui::SliderFloat("Camera Speed", &AppWindow->RenderCamera->MovementSpeed, 0.0f, 50.0f);
        }
    
        ImGui::SeparatorText("Rendering Setting");
        {
            ImGui::ColorEdit3("Clear Color", (float*)&AppWindow->ClearColor);
        }
    
        ImGui::End();
    }

    // show viewport
    if(bIsShowViewport)
    {
        ImGui::Begin("Viewport");
    
        ImGui::End();
    }

    // show log message
    if(bIsShowLogWindow)
    {
        ImGui::Begin("Log");
    
        ImGui::End();
    }
    
    ImGui::Render();

    {
        // ImGui::DockBuilderRemoveNode(mainID); // Clear out existing layout
        // ImGui::DockBuilderAddNode(mainID); // Add empty node
        // ImGui::DockBuilderSetNodeSize(mainID, ImGui::GetMainViewport()->Size);
    }
}
