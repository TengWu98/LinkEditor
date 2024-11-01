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
    Spec.ViewportWidth = ViewportWidth;
    Spec.ViewportHeight = ViewportHeight;
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

    MeshGLData->ModelMatrices.emplace(Entity, VertexBuffer(sizeof(Model)));
    SetEntityVisible(Entity, true);
    if(!InMeshCreateInfo.bIsVisible)
    {
        SetEntityVisible(Entity, false);
    }

    MeshBufferMap MeshBuffers;
    for (auto ElementType : AllMeshElementTypes)
    {
        VertexArray VertexArrayBuffer;
        std::vector<Vertex3D> Vertices = InMesh.CreateVertices(ElementType);
        std::vector<uint> Indices = InMesh.CreateIndices(ElementType);

        auto VertexBufferObject = std::make_shared<VertexBuffer>(Vertices.size() * sizeof(Vertex3D));
        VertexBufferObject->SetLayout({
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float3, "a_Normal"},
            {ShaderDataType::Float4, "a_Color"}
        });
        VertexBufferObject->SetData(Vertices.data(), Vertices.size() * sizeof(Vertex3D));
        VertexArrayBuffer.AddVertexBuffer(VertexBufferObject);

        auto IndexBufferObject = std::make_shared<IndexBuffer>(Indices.data(), Indices.size());
        VertexArrayBuffer.SetIndexBuffer(IndexBufferObject);
        MeshBuffers.emplace(ElementType, VertexArrayBuffer);
    }

    Registry.emplace<Mesh>(Entity, std::move(InMesh));
    
    if(InMeshCreateInfo.bIsSelect)
    {
        // SelecteEntity(Entity, false);
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
    MainRenderPipeline->SetClearColor(BackgroundColor);
    MainRenderPipeline->Clear();
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
}

Camera Scene::CreateDefaultCamera() const
{
    return {glm::vec3(0.0f, 20.0f, 30.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90, -40};
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
