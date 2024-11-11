#include "Application.h"
#include "Core/Window/Window.h"
#include "Scene/Scene.h"
#include "Renderer/Camera/Camera.h"
#include "Renderer/Mesh/Mesh.h"
#include "Renderer/Renderer.h"
#include "Renderer/Texture/Texture.h"
#include "Renderer/Texture/Texture2D/Texture2D.h"

#include <nfd.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glm/ext/quaternion_transform.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <GL/gl.h>

#include "stb_image_write.h"

MESH_EDITOR_NAMESPACE_BEGIN

static void SaveFrameBufferToFile(const FrameBuffer& frameBuffer, const std::string& filename)
{
    int width = 1280;
    int height = 720;

    std::vector<uint8_t> buffer(width * height * 4);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
    
    stbi_write_png(filename.c_str(), width, height, 4, buffer.data(), width * 4);
}

static float LastFrameTime;
static glm::vec2 LastMousePos;
static bool bIsViewportHovered = false;

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
    AppWindow->SetEventCallback(MESH_EDITOR_BIND_EVENT_FN(Application::OnEvent));
    AppScene = std::make_unique<Scene>();
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

Scene& Application::GetScene() const
{
    return *AppScene;
}

Window& Application::GetWindow() const
{
    return *AppWindow;
}

void Application::OnEvent(Event& InEvent)
{
    EventDispatcher Dispatcher(InEvent);
    Dispatcher.Dispatch<WindowCloseEvent>(MESH_EDITOR_BIND_EVENT_FN(Application::OnWindowClose));
    Dispatcher.Dispatch<WindowResizeEvent>(MESH_EDITOR_BIND_EVENT_FN(Application::OnWindowResize));
    Dispatcher.Dispatch<MouseButtonPressedEvent>(MESH_EDITOR_BIND_EVENT_FN(Application::OnMouseButtonPressedEvent));
    Dispatcher.Dispatch<MouseButtonReleasedEvent>(MESH_EDITOR_BIND_EVENT_FN(Application::OnMouseButtonReleasedEvent));
    Dispatcher.Dispatch<MouseMovedEvent>(MESH_EDITOR_BIND_EVENT_FN(Application::OnMouseMovedEvent));
    Dispatcher.Dispatch<MouseScrolledEvent>(MESH_EDITOR_BIND_EVENT_FN(Application::OnMouseScrolledEvent));
    Dispatcher.Dispatch<KeyPressedEvent>(MESH_EDITOR_BIND_EVENT_FN(Application::OnKeyPressedEvent));
    Dispatcher.Dispatch<KeyReleasedEvent>(MESH_EDITOR_BIND_EVENT_FN(Application::OnKeyReleasedEvent));
    Dispatcher.Dispatch<KeyTypedEvent>(MESH_EDITOR_BIND_EVENT_FN(Application::OnKyeTypedEvent));
}

void Application::Update()
{
    while (!glfwWindowShouldClose(AppWindow->GetNativeWindow()))
    {
        // Poll and handle events (inputs, window resize, etc.)
        if (glfwGetWindowAttrib(AppWindow->GetNativeWindow(), GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Calculate Delta Time
        float CurrentTime = static_cast<float>(glfwGetTime());
        DeltaTime = CurrentTime - LastFrameTime;
        LastFrameTime = CurrentTime;

        // If Window is minimized, skip rendering
        if(!bIsMinimized)
        {
            // Render Scene
            AppScene->SetViewportSize(AppWindow->GetWidth(), AppWindow->GetHeight());
            
            AppScene->MainRenderPipeline->GetFrameBuffer()->Bind();
            AppScene->Render();
            AppScene->RenderGizmos();
            AppScene->MainRenderPipeline->GetFrameBuffer()->Unbind();

            // LOG_INFO("Current Color: {0}", AppScene->MainRenderPipeline->GetFrameBuffer()->ReadPixel(0, 1, 1));
            
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            
            // Render ImGui
            RenderImGUI();
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
    else
    {
        LOG_ERROR("Font file not found: {0}", MSYHPath);
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(AppWindow->GetNativeWindow(), true);
    ImGui_ImplOpenGL3_Init(AppWindow->glsl_version);
}

void Application::RenderImGUI()
{
    static bool bIsFullScreen = true;

    // for windows
    static bool bIsShowControlWindow = true;
    static bool bIsShowViewport = true;
    static bool bIsShowLogWindow = false;

    // for stats
    static bool bIsShowStats = false;

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
                        {"Mesh object", "obj,off,ply,stl,om"}
                };
                nfdchar_t *NFDPath;
                nfdresult_t Result = NFD_OpenDialog(&NFDPath, Filters.data(), Filters.size(), "");
                if (Result == NFD_OKAY)
                {
                    const auto Path = fs::path(NFDPath);
                    
                    MeshCreateInfo MeshCreateInfo;
                    MeshCreateInfo.Name = Path.filename().string();
                    AppScene->AddMesh(Path, MeshCreateInfo);
                    
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
        
        if(bIsShowStats)
        {
            ImGui::SeparatorText("Stats");
            ImGui::Text("FPS : %.1f", IO.Framerate);
        }
        
        if(ImGui::CollapsingHeader("General Settings"))
        {
            ImGui::Checkbox("Show Stats", &bIsShowStats);
        }

        if(ImGui::CollapsingHeader("Rendering Settings"))
        {
            if(ImGui::TreeNode("Camera"))
            {
                ImGui::SeparatorText("Viewport Movement");
                ImGui::SliderFloat("Camera Speed", &AppScene->SceneCamera.CameraSpeed, 0.0f, 50.0f, "%.3f", ImGuiSliderFlags_None);
                ImGui::SliderFloat("Scroll Camera Speed", &AppScene->SceneCamera.MouseScrollCameraSpeed, 1.0f, 8.0f, "%.3f", ImGuiSliderFlags_None);
                ImGui::SliderFloat("Mouse Sensitivity", &AppScene->SceneCamera.MouseSensitivity, 0.01f, 1.f, "%.3f", ImGuiSliderFlags_None);
                
                ImGui::SeparatorText("Camera Settings");
                ImGui::Combo("Projection Mode", (int*)&AppScene->SceneCamera.ProjectionMode, "Perspective\0Orthographic\0");

                if(AppScene->SceneCamera.ProjectionMode == CameraProjectionMode::Orthographic)
                {
                    ImGui::SliderFloat("Ortho Width", &AppScene->SceneCamera.OrthoWidth, 128, 1920, "%.3f", ImGuiSliderFlags_None);
                }
                else
                {
                    ImGui::SliderFloat("Field Of View", &AppScene->SceneCamera.FieldOfView, 5.0f, 170.0f, "%.3f", ImGuiSliderFlags_None);
                }
                
                ImGui::Text("Aspect Ratio: %.3f", AppScene->SceneCamera.AspectRatio);
                
                
                ImGui::TreePop();
                ImGui::Spacing();
            }

            if(ImGui::TreeNode("Colors"))
            {
                ImGui::SeparatorText("Background Color");
                ImGui::ColorEdit4("Background Color", (float*)&AppScene->BackgroundColor, ImGuiColorEditFlags_NoLabel);

                ImGui::SeparatorText("HighLight Color");
                ImGui::ColorEdit4("HighLight Color", (float*)&Mesh::HighlightColor, ImGuiColorEditFlags_NoLabel);

                ImGui::SeparatorText("Face Color");
                ImGui::ColorEdit4("Face Color", (float*)&Mesh::FaceColor, ImGuiColorEditFlags_NoLabel);

                ImGui::SeparatorText("Vertex Color");
                ImGui::ColorEdit4("Vertex Color", (float*)&Mesh::VertexColor, ImGuiColorEditFlags_NoLabel);

                ImGui::SeparatorText("Edge Color");
                ImGui::ColorEdit4("Edge Color", (float*)&Mesh::EdgeColor, ImGuiColorEditFlags_NoLabel);

                ImGui::TreePop();
                ImGui::Spacing();
            }

            if(ImGui::TreeNode("Shaders"))
            {
                ImGui::Combo("Shader Type", (int*)&AppScene->MainRenderPipeline->CurrentShaderPipeline, "Flat\0VertexColor\0");
                if(AppScene->MainRenderPipeline->CurrentShaderPipeline == ShaderPipelineType::Flat)
                {
                    ImGui::ColorEdit4("Flat Color", (float*)&AppScene->MainRenderPipeline->ShaderData.FlatColor);
                }
                else if(AppScene->MainRenderPipeline->CurrentShaderPipeline == ShaderPipelineType::VertexColor)
                {
                    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }
                
                ImGui::TreePop();
                ImGui::Spacing();
            }
            
            ImGui::SeparatorText("Render Mode");
        }

        if(ImGui::CollapsingHeader("Selection"))
        {
            ImGui::Combo("Selection Mode", (int*)&AppScene->SelectionMode, "Object\0Element\0");
            ImGui::Combo("Mesh Element Type", (int*)&AppScene->SelectionMeshElementType, "None\0Face\0Vertex\0Edge\0");
        }
    
        ImGui::End();
    }

    // show viewport
    if(bIsShowViewport)
    {
        float ViewportWidth = static_cast<float>(AppWindow->GetWidth());
        float ViewportHeight = static_cast<float>(AppWindow->GetHeight());
        // ImGui::SetNextWindowSize(ImVec2(ViewportWidth, ViewportHeight), ImGuiCond_Always);
        // ImGui::SetNextWindowPos(ImVec2(150, 0), ImGuiCond_Appearing);
        //
        // ImGuiWindowClass ViewportWindowClass;
        // ViewportWindowClass.DockingAllowUnclassed = true;
        // ViewportWindowClass.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoCloseButton;
        // ImGui::SetNextWindowClass(&ViewportWindowClass);
        //
        ImGuiWindowFlags ViewportWindowFlags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        ImGui::Begin("Viewport", nullptr, ViewportWindowFlags);
        {
            // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
            // ImGui::PopStyleVar(2);
            //
            // auto InitialCursorPos = ImGui::GetCursorPos();
            // auto WindowSize = ImGui::GetWindowSize();
            // auto CentralizedCursorPos = ImVec2((WindowSize.x - ViewportWidth) * 0.5f, (WindowSize.y - ViewportHeight + 20) * 0.5f);
            // ImGui::SetCursorPos(CentralizedCursorPos);
    
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

            if(ImGui::IsWindowHovered())
            {
                bIsViewportHovered = true;
            }
            else
            {
                bIsViewportHovered = false;
            }

            // copy framebuffer to the viewport
            uint32_t ColorAttachmentRendererID = AppScene->MainRenderPipeline->GetFrameBuffer()->GetColorAttachmentRendererID();
            ImGui::Image(ColorAttachmentRendererID, ImVec2(ViewportWidth, ViewportHeight), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
            ImGui::End();
        }
    }

    // show log message
    if(bIsShowLogWindow)
    {
        ImGui::Begin("Log");
    
        ImGui::End();
    }
    
    ImGui::Render();
}

bool Application::OnWindowClose(WindowCloseEvent& InEvent)
{
    bIsRunning = false;
    return true;
}

bool Application::OnWindowResize(WindowResizeEvent& InEvent)
{
    if (InEvent.GetWidth() == 0 || InEvent.GetHeight() == 0)
    {
        bIsMinimized = true;
        return false;
    }

    bIsMinimized = false;
    AppScene->SetViewportSize(InEvent.GetWidth(), InEvent.GetHeight());
    return true;
}

bool Application::OnMouseButtonPressedEvent(MouseButtonPressedEvent& InEvent)
{
    return true;
}

bool Application::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& InEvent)
{
    return true;
}

bool Application::OnMouseMovedEvent(MouseMovedEvent& InEvent)
{
    if(!AppWindow || !AppScene)
    {
        return false;
    }
    
    if(Input::IsMouseButtonPressed(AppWindow->GetNativeWindow(), MouseCode::ButtonRight))
    {
        glm::vec2 CurrentMousePos = Input::GetMousePosition(AppWindow->GetNativeWindow());
        glm::vec2 DeltaPos = CurrentMousePos - LastMousePos;
        LastMousePos = CurrentMousePos;
        
        DeltaPos *= AppScene->SceneCamera.MouseSensitivity;

        AppScene->SceneCamera.Front += - AppScene->SceneCamera.Right * DeltaPos.x * AppScene->SceneCamera.GetCurrentDistance() * DeltaTime;
        AppScene->SceneCamera.Front += - AppScene->SceneCamera.Up * DeltaPos.y * AppScene->SceneCamera.GetCurrentDistance() * DeltaTime;
    }
    
    return true;
}

bool Application::OnMouseScrolledEvent(MouseScrolledEvent& InEvent)
{
    if(!AppScene || !bIsViewportHovered)
    {
        return false;
    }
    
    // TODO(WT) 有问题
    float NewDistance = AppScene->SceneCamera.GetCurrentDistance() * (1.f - InEvent.GetYOffset() * AppScene->SceneCamera.MouseScrollCameraSpeed);
    AppScene->SceneCamera.SetDistance(NewDistance);
    return true;
}

bool Application::OnKeyPressedEvent(KeyPressedEvent& InEvent)
{
    if(!AppScene)
    {
        return false;
    }
    
    const KeyCode EventKeyCode = InEvent.GetKeyCode();
    if(EventKeyCode == KeyCode::W || EventKeyCode == KeyCode::Up)
    {
        AppScene->SceneCamera.Position += AppScene->SceneCamera.Front * AppScene->SceneCamera.CameraSpeed * DeltaTime;
    }

    if(EventKeyCode == KeyCode::S || EventKeyCode == KeyCode::Down)
    {
        AppScene->SceneCamera.Position -= AppScene->SceneCamera.Front * AppScene->SceneCamera.CameraSpeed * DeltaTime;
    }

    if(EventKeyCode == KeyCode::A || EventKeyCode == KeyCode::Left)
    {
        AppScene->SceneCamera.Position -= AppScene->SceneCamera.Right * AppScene->SceneCamera.CameraSpeed * DeltaTime;
    }

    if(EventKeyCode == KeyCode::D || EventKeyCode == KeyCode::Right)
    {
        AppScene->SceneCamera.Position += AppScene->SceneCamera.Right * AppScene->SceneCamera.CameraSpeed * DeltaTime;
    }
    
    return true;
}

bool Application::OnKeyReleasedEvent(KeyReleasedEvent& InEvent)
{
    return true;
}

bool Application::OnKyeTypedEvent(KeyEvent& InEvent)
{
    return true;
}

MESH_EDITOR_NAMESPACE_END
