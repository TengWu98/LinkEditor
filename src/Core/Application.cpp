#include "Application.h"
#include "Core/Window/Window.h"
#include "Scene/Scene.h"
#include "Renderer/Camera/Camera.h"
#include "Renderer/Mesh/Mesh.h"
#include "Renderer/Renderer.h"
#include "Renderer/Texture/Texture.h"
#include "Renderer/Texture/Texture2D/Texture2D.h"
#include "Renderer/Ray/Ray.h"

#include <nfd.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glm/ext/quaternion_transform.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

MESH_EDITOR_NAMESPACE_BEGIN

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
            
            AppScene->SceneRenderer->GetFrameBuffer()->Bind();
            AppScene->Render();
            AppScene->RenderGizmos();
            AppScene->SceneRenderer->GetFrameBuffer()->Unbind();

            // LOG_INFO("Current Color: {0}", AppScene->SceneRenderer->GetFrameBuffer()->ReadPixel(0, 1, 1));
            
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
            if (ImGui::MenuItem("Import Mesh"))
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
                ImGui::Combo("Shader Type", (int*)&AppScene->SceneRenderer->CurrentShaderPipeline, "Flat\0Depth\0Phong\0");
                if(AppScene->SceneRenderer->CurrentShaderPipeline == ShaderPipelineType::Flat)
                {
                    ImGui::ColorEdit4("Flat Color", (float*)&AppScene->SceneRenderer->ShaderData.FlatColor);
                }
                else if(AppScene->SceneRenderer->CurrentShaderPipeline == ShaderPipelineType::Depth)
                {
                    ImGui::SliderFloat("ZMin", &AppScene->SceneRenderer->ShaderData.NearPlane, 0.1f, 10.0f, "%.3f", ImGuiSliderFlags_None);
                    ImGui::SliderFloat("ZMax", &AppScene->SceneRenderer->ShaderData.FarPlane, 10.0f, 100.0f, "%.3f", ImGuiSliderFlags_None);
                }
                else if(AppScene->SceneRenderer->CurrentShaderPipeline == ShaderPipelineType::Phong)
                {
                    ImGui::SliderFloat("Shininess", &AppScene->SceneRenderer->ShaderData.Shininess, 0.0f, 256.0f, "%.3f", ImGuiSliderFlags_None);
                }
                
                ImGui::TreePop();
                ImGui::Spacing();
            }

            if(ImGui::TreeNode("Render Mode"))
            {
                ImGui::Combo("Render Mode", (int*)&AppScene->SceneRenderer->Mode, "None\0Faces\0Vertices\0Edges\0");
                
                ImGui::TreePop();
                ImGui::Spacing();
            }
        }

        if(ImGui::CollapsingHeader("Selection"))
        {
            ImGui::Combo("Selection Mode", (int*)&AppScene->SelectionMode, "Object\0Element\0");
            if(AppScene->SelectionMode == SelectionMode::Element)
            {
                ImGui::Combo("Mesh Element Type", (int*)&AppScene->SelectionMeshElementType, "None\0Face\0Vertex\0Edge\0");
                ImGui::Text("Selected %s: %s", MeshElementTypeToString(AppScene->SelectionMeshElementType).c_str(), AppScene->SelectedElement.IsValid() ? std::to_string(AppScene->SelectedElement.Idx()).c_str() : "None");
            }
            else if(AppScene->SelectionMode == SelectionMode::Object)
            {
                if(AppScene->SelectedEntity != entt::null)
                {
                    ImGui::Text("Selected Object: %s", AppScene->GetEntityName(AppScene->SelectedEntity).c_str());
                }
            }
        }
    
        ImGui::End();
    }

    // show viewport
    if(bIsShowViewport)
    {
        float ViewportWidth = static_cast<float>(AppWindow->GetWidth());
        float ViewportHeight = static_cast<float>(AppWindow->GetHeight());
        LOG_INFO("Pos: {0}, {1}", ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
        
        ImGuiWindowFlags ViewportWindowFlags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse;
        ImGui::Begin("Viewport", nullptr, ViewportWindowFlags);
        {
            auto WindowSize = ImGui::GetWindowSize();
            auto CentralizedCursorPos = ImVec2((WindowSize.x - ViewportWidth) * 0.5f, (WindowSize.y - ViewportHeight) * 0.5f);
            ImGui::SetCursorPos(CentralizedCursorPos);

            if(ImGui::IsWindowHovered())
            {
                bIsViewportHovered = true;
                if(Input::IsMouseButtonPressed(AppWindow->GetNativeWindow(), MouseCode::ButtonLeft))
                {
                    const glm::vec2 MousePos = ToGlm(ImGui::GetMousePos()) - ToGlm(ImGui::GetCursorScreenPos()) / glm::vec2(ViewportWidth, ViewportHeight);
                    const glm::vec2 MousePosNDC = glm::normalize(glm::vec2(2 + MousePos.x - 1, 1 - 2 * MousePos.y));
                    // LOG_INFO("Pos: {0}, {1}", ImGui::GetMousePos().x, ImGui::GetMousePos().y);
                    Ray MouseRay = AppScene->SceneCamera.ClipPosToWorldRay(MousePosNDC);
                    
                    if(AppScene->SelectedEntity != entt::null && AppScene->SelectionMode == SelectionMode::Element
                        && AppScene->SelectionMeshElementType != MeshElementType::None) // Select Mesh Element
                    {
                        const auto PreviousSelectedElementType = AppScene->SelectionMeshElementType;
                        AppScene->SelectedElement = MeshElementIndex(PreviousSelectedElementType, -1);
                        const auto& SelectedMesh = AppScene->GetSelectedMesh();
                        if(AppScene->SelectionMeshElementType == MeshElementType::Face)
                        {
                            AppScene->SelectedElement = Mesh::ElementIndex{SelectedMesh.FindNearestIntersectingFace(MouseRay)};
                        }
                        else if(AppScene->SelectionMeshElementType == MeshElementType::Vertex)
                        {
                            AppScene->SelectedElement = Mesh::ElementIndex{SelectedMesh.FindNearestVertex(MouseRay)};
                        }
                        else if(AppScene->SelectionMeshElementType == MeshElementType::Edge)
                        {
                            AppScene->SelectedElement = Mesh::ElementIndex{SelectedMesh.FindNearestEdge(MouseRay)};
                        }
                    }
                    else if(AppScene->SelectionMode == SelectionMode::Object) // Select Mesh Object
                    {
                        static std::multimap<float, entt::entity> HoveredEntitiesByDistance;
                        HoveredEntitiesByDistance.clear();

                        // AppScene->Registry.view<Mesh>().each(
                        //     [AppScene, &MouseRay])(entt::entity Entity, const auto& ModelMatrix)
                        // {
                        //     
                        // }
                    }
                }

                if(Input::IsMouseButtonPressed(AppWindow->GetNativeWindow(), ButtonRight))
                {
                    
                }
            }
            else
            {
                bIsViewportHovered = false;
            }

            // copy framebuffer to the viewport
            uint32_t ColorAttachmentRendererID = AppScene->SceneRenderer->GetFrameBuffer()->GetColorAttachmentRendererID();
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
    if(!bIsViewportHovered || !AppWindow || !AppWindow->GetNativeWindow())
    {
        return false;
    }

    // if(Input::IsMouseButtonPressed(AppWindow->GetNativeWindow(), MouseCode::ButtonLeft))
    // {
    //     const glm::vec2 MousePos = ToGlm(ImGui::GetMousePos()) - ToGlm(ImGui::GetCursorScreenPos()) / ToGlm(ImGui::GetContentRegionAvail());
    //     const glm::vec2 MousePosNDC = glm::vec2(2 + MousePos.x - 1, 1- 2 * MousePos.y);
    //     
    // }
    
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
    
    return true;
}

bool Application::OnMouseScrolledEvent(MouseScrolledEvent& InEvent)
{
    if(!AppScene || !bIsViewportHovered)
    {
        return false;
    }
    
    // TODO(WT) 有问题
    // float NewDistance = AppScene->SceneCamera.GetCurrentDistance() * (1.f - InEvent.GetYOffset() * AppScene->SceneCamera.MouseScrollCameraSpeed);
    // AppScene->SceneCamera.SetDistance(NewDistance);
    float NewDistance = AppScene->SceneCamera.GetCurrentDistance() - InEvent.GetYOffset() * AppScene->SceneCamera.MouseScrollCameraSpeed;
    AppScene->SceneCamera.SetDistance(NewDistance);
    return true;
}

bool Application::OnKeyPressedEvent(KeyPressedEvent& InEvent)
{
    if(!AppScene)
    {
        return false;
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
