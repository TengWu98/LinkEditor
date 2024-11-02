#include "Scene.h"

#include "Renderer/Buffers/IndexBuffer.h"
#include "Renderer/Mesh/Mesh.h"

Scene::Scene() :
    Camera(CreateDefaultCamera()),
    Registry(entt::registry()),
    MeshGLData(std::make_unique<::MeshGLData>())
{
    // Initialize Render pipeline
    RenderSpecification Spec;
    MainRenderPipeline = std::make_unique<Renderer>(Spec);

    // Initialize Uniform buffers
    TransformBuffer = std::make_unique<UniformBuffer>(sizeof(ViewProj), 0);
    ViewProjNearFarBuffer = std::make_unique<UniformBuffer>(sizeof(ViewProjNearFar), 1);
    // TODO(WT) Lights buffer
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
    Registry.emplace<Model>(Entity, glm::mat4(InMeshCreateInfo.Transform));
    Registry.emplace<std::string>(Entity, InMeshCreateInfo.Name);

    MeshGLData->ModelMatrices.emplace(Entity, std::make_shared<VertexBuffer>(sizeof(Model)));
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

    // Render Meshs
    for(auto PrimaryMesh : MeshGLData->PrimaryMeshs)
    {
        auto Entity = PrimaryMesh.first;
        auto ModelMatrix = MeshGLData->ModelMatrices.at(Entity);
        auto MeshVertexArrayBuffer = PrimaryMesh.second.at(SelectionMeshElementType);
        MainRenderPipeline->Render(MeshVertexArrayBuffer, ModelMatrix);
    }
}

void Scene::RenderGizmos()
{
}

void Scene::CompileShaders()
{
    MainRenderPipeline->CompileShaders();
}

void Scene::UpdateTransformBuffers()
{
    const float AspectRatio = ViewportWidth == 0 || ViewportHeight == 0 ? 1.f : static_cast<float>(ViewportWidth) / static_cast<float>(ViewportHeight);
    const ViewProj ViewProjMat = {Camera.GetViewMatrix(), Camera.GetProjectionMatrix(AspectRatio)};
}

VertexBufferLayout Scene::CreateDefaultVertexLayout()
{
    return {
        {ShaderDataType::Float3, "a_Position"},
        {ShaderDataType::Float3, "a_WorldNormal"},
        {ShaderDataType::Float4, "a_Color"},
        {ShaderDataType::Mat4, "ModelMatrix"},
        {ShaderDataType::Mat4, "InverseModelMatrix"}
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

void Scene::UpdateModelBuffer(entt::entity Entity)
{
    const auto BufferIndex = GetModelBufferIndex(Entity);
    if(BufferIndex)
    {
        const auto& ModelStruct = Registry.get<Model>(GetParentEntity(Entity));
        const auto& ModelMatrixBuffer = MeshGLData->ModelMatrices.at(Entity);

        if(ModelMatrixBuffer)
        {
            ModelMatrixBuffer->SetData(&ModelStruct, sizeof(ModelStruct));
        }
    }
}

Camera Scene::CreateDefaultCamera() const
{
    return {glm::vec3(0, 0, 2), glm::vec3(0, 1, 0), 60, 0.01, 100, -90, -40};
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
    // TODO(WT) 补全功能
}
