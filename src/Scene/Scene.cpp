#include "Scene.h"

#include "Renderer/Buffers/IndexBuffer.h"
#include "Renderer/Mesh/Mesh.h"

void Gizmo::Begin() const
{
    using namespace ImGui;
    
    const auto ContentRegion = GetContentRegionAvail();
    const auto& WindowPos = GetWindowPos();
    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetRect(WindowPos.x, WindowPos.y + GetTextLineHeightWithSpacing(), ContentRegion.x, ContentRegion.y);
}

void Gizmo::Render(Camera& Camera, bool& bIsViewChanged) const
{
    using namespace ImGui;

    static constexpr float ViewManipulateSize = 128;

    const auto& WindowPos = GetWindowPos();
    const auto ViewManipulatePos = ImVec2{GetWindowContentRegionMax().x - ViewManipulateSize + WindowPos.x, GetWindowContentRegionMin().y + WindowPos.y};
    auto CameraView = Camera.GetViewMatrix();
    const float CameraDistance = Camera.GetDistance();
    bIsViewChanged = ImGuizmo::ViewManipulate(&CameraView[0][0], CameraDistance, ViewManipulatePos, {ViewManipulateSize, ViewManipulateSize}, 0);
    if (bIsViewChanged)
    {
        Camera.SetPositionFromView(CameraView);
    }
}

void Gizmo::RenderDebug()
{
}

Scene::Scene() :
    Camera(CreateDefaultCamera()),
    Registry(entt::registry()),
    MeshGLData(std::make_unique<::MeshGLData>())
{
    // Initialize Render pipeline
    RenderSpecification Spec;
    MainRenderPipeline = std::make_unique<Renderer>(Spec);

    // Initialize ViewProj buffers
    ViewProjBuffer = std::make_unique<UniformBuffer>(sizeof(ViewProj), 0);
    ViewProjNearFarBuffer = std::make_unique<UniformBuffer>(sizeof(ViewProjNearFar), 1);
    UpdateViewProjBuffers();
    
    // TODO(WT) Lights buffer

    SceneGizmo = std::make_unique<Gizmo>();
}

Scene::~Scene()
{
}

void Scene::SetViewportSize(uint32_t Width, uint32_t Height)
{
    ViewportWidth = Width;
    ViewportHeight = Height;
}

entt::entity Scene::AddMesh(Mesh&& InMesh, MeshCreateInfo InMeshCreateInfo)
{
    const auto Entity = Registry.create();

    auto Node = Registry.emplace<SceneNode>(Entity);
    Registry.emplace<std::string>(Entity, InMeshCreateInfo.Name);
    MeshGLData->ModelMatrices.emplace(Entity, std::make_shared<Model>(InMeshCreateInfo.Transform));
    
    SetEntityVisible(Entity, true);
    if(!InMeshCreateInfo.bIsVisible)
    {
        SetEntityVisible(Entity, false);
    }

    MeshBufferMap MeshBuffers;
    for (auto ElementType : AllMeshElementTypes)
    {
        auto VertexArrayBuffer = std::make_shared<VertexArray>();
        std::vector<Vertex3D> Vertices = InMesh.CreateVertices(ElementType);
        std::vector<uint> Indices = InMesh.CreateIndices(ElementType);

        auto VertexBufferObject = std::make_shared<VertexBuffer>(Vertices.size() * sizeof(Vertex3D));
        VertexBufferObject->SetLayout(CreateDefaultVertexLayout());
        VertexBufferObject->SetData(Vertices.data(), Vertices.size() * sizeof(Vertex3D));
        VertexArrayBuffer->AddVertexBuffer(VertexBufferObject);

        auto IndexBufferObject = std::make_shared<IndexBuffer>(Indices.data(), Indices.size());
        VertexArrayBuffer->SetIndexBuffer(IndexBufferObject);
        MeshBuffers.emplace(ElementType, VertexArrayBuffer);
    }
    
    MeshGLData->PrimaryMeshs.emplace(Entity, MeshBuffers);

    Registry.emplace<Mesh>(Entity, std::move(InMesh));
    
    if(InMeshCreateInfo.bIsSelect)
    {
        
    }

    if(InMeshCreateInfo.bIsSubmit)
    {
        
    }
    
    return Entity;
}

entt::entity Scene::AddMesh(const fs::path& MeshFilePath, MeshCreateInfo InMeshCreateInfo)
{
    return AddMesh(Mesh(MeshFilePath), std::move(InMeshCreateInfo));
}

void Scene::Render()
{
    if(ViewportWidth == 0 || ViewportHeight == 0)
    {
        return;
    }
    
    MainRenderPipeline->SetViewport(0, 0, ViewportWidth, ViewportHeight);
    MainRenderPipeline->SetClearColor(BackgroundColor);
    MainRenderPipeline->Clear();

    UpdateViewProjBuffers();

    // Render Meshs
    for(auto PrimaryMesh : MeshGLData->PrimaryMeshs)
    {
        auto Entity = PrimaryMesh.first;
        auto ModelStruct = MeshGLData->ModelMatrices.at(Entity);
        auto MeshVertexArrayBuffer = PrimaryMesh.second.at(SelectionMeshElementType);
        
        MainRenderPipeline->UpdateShaderData({
            ShaderBindingDescriptor{ShaderPipelineType::Flat, "u_ModelMatrix", std::nullopt, std::nullopt, ModelStruct->Transform},
            ShaderBindingDescriptor{ShaderPipelineType::VertexColor, "u_ModelMatrix", std::nullopt, std::nullopt, ModelStruct->Transform}
        });
        
        MainRenderPipeline->Render(MeshVertexArrayBuffer, ModelStruct);
    }
}

void Scene::RenderGizmos()
{
    // const float mouse_wheel = ImGui::GetIO().MouseWheel;
    // if (mouse_wheel != 0 && ImGui::IsWindowHovered()) Camera.SetTargetDistance(Camera.GetDistance() * (1.f - mouse_wheel / 16.f));
    // SceneGizmo->Begin();
    //
    // const float AspectRation = float(ViewportWidth) / float(ViewportHeight);
    // if(SelectedEntity != entt::null)
    // {
    //     
    // }
    // else
    // {
    //     bool bIsViewChanged = false;
    //     SceneGizmo->Render(Camera, bIsViewChanged);
    //     Camera.Tick();
    //     if(bIsViewChanged)
    //     {
    //         UpdateViewProjBuffers();
    //     }
    // }
}

void Scene::UpdateViewProjBuffers()
{
    const float AspectRatio = ViewportWidth == 0 || ViewportHeight == 0 ? 1.f : static_cast<float>(ViewportWidth) / static_cast<float>(ViewportHeight);
    const ViewProj ViewProjMat = {Camera.GetViewMatrix(), Camera.GetProjectionMatrix(AspectRatio)};
    ViewProjBuffer->SetData(&ViewProjMat, sizeof(ViewProjMat));

    const ViewProjNearFar ViewProjNearFarMat = {Camera.GetViewMatrix(), Camera.GetProjectionMatrix(AspectRatio), Camera.NearClip, Camera.FarClip};
    ViewProjNearFarBuffer->SetData(&ViewProjNearFarMat, sizeof(ViewProjNearFarMat));
}

VertexBufferLayout Scene::CreateDefaultVertexLayout()
{
    return {
        {ShaderDataType::Float3, "a_Position"},
        {ShaderDataType::Float3, "a_WorldNormal"},
        {ShaderDataType::Float4, "a_Color"},
    };
}

std::optional<unsigned> Scene::GetModelBufferIndex(entt::entity Entity)
{
    if(Entity == entt::null)
    {
        return std::nullopt;
    }

    const auto& ModelIndices = Registry.get<SceneNode>(GetParentEntity(Entity)).ModelIndices;
    const auto Iter = ModelIndices.find(Entity);
    if(Iter != ModelIndices.end())
    {
        return Iter->second;
    }

    return std::nullopt;
}

Camera Scene::CreateDefaultCamera() const
{
    return {glm::vec3(0, 0, 2), glm::vec3(0, 1, 0), glm::vec3(0, 0, 0), 60, 0.01, 100};
}

entt::entity Scene::GetSelectedEntity() const
{
    return SelectedEntity;
}

entt::entity Scene::GetParentEntity(entt::entity Entity) const
{
    if(Entity == entt::null)
    {
        return entt::null;
    }

    const auto& Node = Registry.get<SceneNode>(Entity);
    return Node.Parent == entt::null ? Entity : GetParentEntity(Node.Parent);
}

void Scene::SetEntityVisible(entt::entity Entity, bool bIsVisible)
{
    const bool bIsAlreadyVisible = Registry.has<Visible>(Entity);
    if((bIsVisible && bIsAlreadyVisible) || (!bIsVisible && !bIsAlreadyVisible))
    {
        return;
    }

    const auto Parent = GetParentEntity(Entity);
    auto& ParentNode = Registry.get<SceneNode>(Parent);
    auto& ModelIndices = ParentNode.ModelIndices;
    if(bIsVisible)
    {
        Registry.emplace<Visible>(Entity);
        const auto NewModelIndex = Entity == Parent || ModelIndices.empty() ? 0 : std::max_element(ModelIndices.begin(), ModelIndices.end(), [](const auto& Lhs, const auto& Rhs) { return Lhs.second < Rhs.second; })->second + 1;
        for(auto& [_, ModelIndex] : ModelIndices)
        {
            if(ModelIndex >= NewModelIndex)
            {
                ModelIndex++;
            }
        }
        ModelIndices.emplace(Entity, NewModelIndex);
    }
    else
    {
        Registry.remove<Visible>(Entity);
        const auto OldModelIndex = *GetModelBufferIndex(Entity);
        // TODO(WT) 补全实现
    }
}
