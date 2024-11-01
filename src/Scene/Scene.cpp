#include "Scene.h"
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

    MeshGLData->Models.emplace(Entity, VertexBuffer(sizeof(Model)));
    SetEntityVisible(Entity, true);
    if(!InMeshCreateInfo.bIsVisible)
    {
        SetEntityVisible(Entity, false);
    }

    MeshBufferMap MeshBuffers;
    VertexArray VAO;
    VertexBuffer VBO(sizeof(float) * InMesh.GetVertices().size());
    VBO.SetData(InMesh.GetVertices().data(), sizeof(float) * InMesh.GetVertices().size());

    MeshBuffers[/* 填充相应的MeshElement*/] = std::move(VAO);

    
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
