﻿#include "Application.h"
#include "Core/Window/Window.h"
#include "Scene/Scene.h"
#include "Renderer/Camera/Camera.h"
#include "Renderer/Mesh/Mesh.h"
#include "Renderer/Renderer.h"
#include "Renderer/Texture/Texture.h"
#include "Renderer/Texture/Texture2D/Texture2D.h"
#include "Renderer/Ray/Ray.h"
#include "MeshSegmentation/MeshSegementation.h"

#include <nfd.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glm/ext/quaternion_transform.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "Renderer/Gizmo/Gizmo.h"

LINK_EDITOR_NAMESPACE_BEGIN
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
    AppWindow->SetEventCallback(LINK_EDITOR_BIND_EVENT_FN(Application::OnEvent));
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
    Dispatcher.Dispatch<WindowCloseEvent>(LINK_EDITOR_BIND_EVENT_FN(Application::OnWindowClose));
    Dispatcher.Dispatch<WindowResizeEvent>(LINK_EDITOR_BIND_EVENT_FN(Application::OnWindowResize));
    Dispatcher.Dispatch<MouseButtonPressedEvent>(LINK_EDITOR_BIND_EVENT_FN(Application::OnMouseButtonPressedEvent));
    Dispatcher.Dispatch<MouseButtonReleasedEvent>(LINK_EDITOR_BIND_EVENT_FN(Application::OnMouseButtonReleasedEvent));
    Dispatcher.Dispatch<MouseMovedEvent>(LINK_EDITOR_BIND_EVENT_FN(Application::OnMouseMovedEvent));
    Dispatcher.Dispatch<MouseScrolledEvent>(LINK_EDITOR_BIND_EVENT_FN(Application::OnMouseScrolledEvent));
    Dispatcher.Dispatch<KeyPressedEvent>(LINK_EDITOR_BIND_EVENT_FN(Application::OnKeyPressedEvent));
    Dispatcher.Dispatch<KeyReleasedEvent>(LINK_EDITOR_BIND_EVENT_FN(Application::OnKeyReleasedEvent));
    Dispatcher.Dispatch<KeyTypedEvent>(LINK_EDITOR_BIND_EVENT_FN(Application::OnKyeTypedEvent));
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
    const ImGuiViewport* Viewport = ImGui::GetMainViewport();
    if(!Viewport)
    {
        return;
    }

    static ImGuiDockNodeFlags DockSpaceFlags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;

    static bool bIsFullScreen = true;
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
    static bool bIsShowControlWindow = true;
    static bool bIsShowViewport = true;
    static bool bIsShowLogWindow = false;
    static bool bIsShowSceneHierarchy = false;
    
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
            }

            if(ImGui::MenuItem("Export Mesh"))
            {
                // TODO(WT)
            }

            if(ImGui::MenuItem("Export Mesh As..."))
            {
                // TODO(WT)
            }

            ImGui::Separator();

            if(ImGui::MenuItem("Recent Files"))
            {
                // TODO(WT)
            }
            
            ImGui::EndMenu();
        }
        
        if(ImGui::BeginMenu("Options"))
        {
            ImGui::MenuItem("Fullscreen", nullptr, &bIsFullScreen);

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
            
            ImGui::EndMenu();
        }

        if(ImGui::BeginMenu("Algorithms"))
        {
            if(ImGui::BeginMenu("Mesh Segmentation"))
            {
                if(ImGui::MenuItem("Load Seg File"))
                {
                    static const std::vector<nfdfilteritem_t> Filters {
                            {"Mesh Seg Labels", "seg"}
                    };
                    nfdchar_t *NFDPath;
                    nfdresult_t Result = NFD_OpenDialog(&NFDPath, Filters.data(), Filters.size(), "");
                    if (Result == NFD_OKAY)
                    {
                        const auto Path = fs::path(NFDPath);
                        MeshSegmentationManager::CurrentSegmentationInfo.SegLabels = MeshSegmentationManager::LoadSegLabels(Path.string());
                    
                        NFD_FreePath(NFDPath);
                    }
                    else if (Result != NFD_CANCEL) {
                        LOG_ERROR("Error loading label file: {0}", NFD_GetError());
                    }
                }

                if(ImGui::MenuItem("Save Seg File"))
                {
                    
                }


                ImGui::EndMenu();
            }
            
            ImGui::EndMenu();
        }
        
        if(ImGui::BeginMenu("Windows"))
        {
            ImGui::Checkbox("Control Panel", &bIsShowControlWindow);
            ImGui::Checkbox("Viewport", &bIsShowViewport);
            ImGui::Checkbox("Scene Hierarchy", &bIsShowSceneHierarchy);
            ImGui::Checkbox("Log", &bIsShowLogWindow);
    
            ImGui::EndMenu();
        }
        
        ImGui::EndMenuBar();
    }
    
    ImGui::End();

    // show control window
    static bool bIsShowStats = false;
    static bool bIsShowSelectedMeshInfo = true;
    static bool bIsShowFace = true;
    static bool bIsShowPoints = false;
    static bool bIsShowWireframe = false;
    static bool bIsShowNormal = false;
    static bool bIsShowBoundingBox = false;
    static bool bIsShowBVH = false;
    static bool bIsShowSegmentation = false;
    if(bIsShowControlWindow)
    {
        ImGui::Begin("Control Panel");
        
        if(bIsShowStats)
        {
            ImGui::SeparatorText("Stats");
            ImGui::Text("FPS : %.1f", IO.Framerate);
        }
        
        if(ImGui::CollapsingHeader("General"))
        {
            ImGui::Checkbox("Show Stats", &bIsShowStats);
        }

        if(ImGui::CollapsingHeader("Rendering"))
        {
            if(ImGui::TreeNode("Render Settings"))
            {
                ImGui::Checkbox("Show Face", &bIsShowFace);
                if(bIsShowFace)
                {
                    AppScene->SceneRenderer->Mode |= RenderMode::Face;
                }
                else
                {
                    AppScene->SceneRenderer->Mode &= ~RenderMode::Face;
                }
                
                ImGui::Checkbox("Show Points", &bIsShowPoints);
                if(bIsShowPoints)
                {
                    AppScene->SceneRenderer->Mode |= RenderMode::Points;
                    ImGui::SliderFloat("Point Size", &AppScene->SceneRenderer->PointSize, 1.0f, 10.0f, "%.3f", ImGuiSliderFlags_None);
                }
                else
                {
                    AppScene->SceneRenderer->Mode &= ~RenderMode::Points;
                }
                
                ImGui::Checkbox("Show Wireframe", &bIsShowWireframe);
                if(bIsShowWireframe)
                {
                    AppScene->SceneRenderer->Mode |= RenderMode::Wireframe;
                    ImGui::SliderFloat("Line Width", &AppScene->SceneRenderer->LineWidth, 1.0f, 10.0f, "%.3f", ImGuiSliderFlags_None);
                }
                else
                {
                    AppScene->SceneRenderer->Mode &= ~RenderMode::Wireframe;
                }
                
                ImGui::Checkbox("Show Normal", &bIsShowNormal);
                ImGui::Checkbox("Show Bounding Box", &bIsShowBoundingBox);
                ImGui::Checkbox("Show BVH", &bIsShowBVH);
                
                ImGui::TreePop();
                ImGui::Spacing();
            }
            
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

            if(ImGui::TreeNode("Lights"))
            {
                if(!AppScene->Lights.empty())
                {
                    auto DirLight = std::static_pointer_cast<DirectionalLight>(AppScene->Lights[0]);
                    ImGui::SliderFloat("Ambient", &DirLight->AmbientIntensity, 0.0f, 1.f, "%.3f", ImGuiSliderFlags_None);
                    ImGui::SliderFloat("Intensity", &DirLight->Intensity, 0.0f, 150.0f, "%.3f", ImGuiSliderFlags_None);
                    ImGui::ColorEdit3("Light Color", (float*)&DirLight->LightColor);
                    ImGui::SliderFloat3("Light Direction", (float*)&DirLight->Direction, -1.0f, 1.0f, "%.3f", ImGuiSliderFlags_None);
                }
                
                ImGui::TreePop();
                ImGui::Spacing();
            }

            if(ImGui::TreeNode("Shaders"))
            {
                ImGui::Combo("Shader Type", (int*)&AppScene->SceneRenderer->CurrentShaderPipeline, "Phong\0Depth\0");
                if(AppScene->SceneRenderer->CurrentShaderPipeline == ShaderPipelineType::Depth)
                {
                    ImGui::SliderFloat("ZMin", &AppScene->SceneRenderer->ShaderData.Depth_NearPlane, 0.1f, 10.0f, "%.3f", ImGuiSliderFlags_None);
                    ImGui::SliderFloat("ZMax", &AppScene->SceneRenderer->ShaderData.Depth_FarPlane, 10.0f, 100.0f, "%.3f", ImGuiSliderFlags_None);
                }
                else if(AppScene->SceneRenderer->CurrentShaderPipeline == ShaderPipelineType::Phong)
                {
                    ImGui::ColorEdit4("Phong Diffuse", (float*)&AppScene->SceneRenderer->ShaderData.Phong_Diffuse);
                    ImGui::ColorEdit4("Phong Specular", (float*)&AppScene->SceneRenderer->ShaderData.Phong_Specular);
                    ImGui::SliderFloat("Gloss", &AppScene->SceneRenderer->ShaderData.Phong_Gloss, 1.0f, 256.0f, "%.3f", ImGuiSliderFlags_None);
                }
                
                ImGui::TreePop();
                ImGui::Spacing();
            }
        }
        
        if(ImGui::CollapsingHeader("Selection"))
        {
            if(AppScene->SelectedEntity != entt::null)
            {
                if(ImGui::TreeNode("Transform"))
                {
                    const auto ModelMatrix = AppScene->GetModelMatrix(AppScene->SelectedEntity);
                    glm::vec3 Position, Rotation, Scale;
                    DecomposeTransform(ModelMatrix, Position, Rotation, Scale);
                    ImGui::DragFloat3("Position", &Position[0], 0.01f);
                    ImGui::DragFloat3("Rotation", &Rotation[0], 1, -90, 90, "%.0f");
                    ImGui::DragFloat3("Scale", &Scale[0], 0.01f, 0.01f, 10);

                    AppScene->SetModelMatrix(AppScene->SelectedEntity, ComposeTransform(Position, Rotation, Scale));
                
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
            }
            
            ImGui::Combo("Selection Mode", (int*)&AppScene->SelectionMode, "Object\0Element\0");
            if(AppScene->SelectionMode == SelectionMode::Element)
            {
                ImGui::Combo("Mesh Element Type", (int*)&AppScene->SelectionMeshElementType, "None\0Face\0Vertex\0Edge\0");
                ImGui::Text("Selected %s: %s", MeshElementTypeToString(AppScene->SelectionMeshElementType).c_str(), AppScene->SelectedElement.IsValid() ? std::to_string(AppScene->SelectedElement.Idx()).c_str() : "None");
            }
            else if(AppScene->SelectionMode == SelectionMode::Object)
            {
                
            }

            ImGui::Checkbox("Show Selected Mesh Info", &bIsShowSelectedMeshInfo);
            if(bIsShowSelectedMeshInfo)
            {
                if(AppScene->SelectedEntity != entt::null)
                {
                    const auto& SelectedMesh = AppScene->GetSelectedMesh();
                    
                    ImGui::Text("Selected Object: %s", AppScene->GetEntityName(AppScene->SelectedEntity).c_str());
                    ImGui::Text("Faces Num: %d", SelectedMesh.GetFaceCount());
                    ImGui::Text("Vertices Num: %d", SelectedMesh.GetVertexCount());
                    ImGui::Text("Edges Num: %d", SelectedMesh.GetEdgeCount());
                }
            }
        }

        if(ImGui::CollapsingHeader("Mesh Segmentation"))
        {
            ImGui::Text("Current SegLabels: %s", MeshSegmentationManager::CurrentSegmentationInfo.SegLabelsFilePath.empty() ? "None" : MeshSegmentationManager::CurrentSegmentationInfo.SegLabelsFilePath.c_str());
            ImGui::Checkbox("Show Segmentation", &bIsShowSegmentation);
            if(bIsShowSegmentation)
            {
                
            }
        }
    
        ImGui::End();
    }

    // show viewport
    if(bIsShowViewport)
    {
        ImGuiWindowFlags ViewportWindowFlags = ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse;
        ImGui::Begin("Viewport", nullptr, ViewportWindowFlags);
        {
            auto WindowSize = ImGui::GetWindowSize();

            if(ImGui::IsWindowHovered())
            {
                bIsViewportHovered = true;
                if(Input::IsMouseButtonPressed(AppWindow->GetNativeWindow(), MouseCode::ButtonLeft))
                {
                    const glm::vec2 MousePos = (ToGlm(ImGui::GetMousePos()) - ToGlm(ImGui::GetCursorScreenPos())) / ToGlm(WindowSize);
                    const glm::vec2 MousePosNDC = glm::vec2(2 * MousePos.x - 1, 1 - 2 * MousePos.y);
                    Ray MouseRay = AppScene->SceneCamera.ClipPosToWorldRay(MousePosNDC);
                    
                    if(AppScene->SelectedEntity != entt::null && AppScene->SelectionMode == SelectionMode::Element
                        && AppScene->SelectionMeshElementType != MeshElementType::None) // Select Mesh Element
                    {
                        const auto PreviousSelectedElement = AppScene->SelectedElement;
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

                        // if(AppScene->SelectedElement.Idx() != PreviousSelectedElement.Idx())
                        // {
                        //     AppScene->UpdateRenderBuffers(AppScene->GetParentEntity(AppScene->GetSelectedEntity()), AppScene->SelectedElement);
                        // }
                    }
                    else if(AppScene->SelectionMode == SelectionMode::Object) // Select Mesh Object
                    {
                        static std::multimap<float, entt::entity> HoveredEntitiesByDistance;
                        HoveredEntitiesByDistance.clear();

                        // TODO(WT)
                    }
                }
            }
            else
            {
                bIsViewportHovered = false;
            }

            // copy framebuffer to the viewport
            uint32_t ColorAttachmentRendererID = AppScene->SceneRenderer->GetFrameBuffer()->GetColorAttachmentRendererID();
            ImGui::Image(ColorAttachmentRendererID, WindowSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
            
            ImGui::End();
        }
    }

    // show log message
    if(bIsShowLogWindow)
    {
        ImGui::Begin("Log");
    
        ImGui::End();
    }

    // show scene hierarchy
    if(bIsShowSceneHierarchy)
    {
        ImGui::Begin("Scene Hierarchy");
    
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

LINK_EDITOR_NAMESPACE_END
