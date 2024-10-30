#include "Scene.h"
#include "Renderer/Mesh/Mesh.h"

Scene::Scene() :
    Camera(CreateDefaultCamera()),
    Registry(entt::registry()),
    MeshGLData(std::make_unique<::MeshGLData>())
{
    MainRenderPipeline = std::make_unique<Renderer>();

    // Initialize Uniform buffers
    TransformBuffer = std::make_unique<UniformBuffer>(sizeof(ViewProj), 0);
    ViewProjNearFarBuffer = std::make_unique<UniformBuffer>(sizeof(ViewProjNearFar), 1);
    // TODO(WT) Lights buffer

    // 编译Shader
}

Scene::~Scene()
{
    
}

entt::entity Scene::AddMesh(Mesh&& InMesh, MeshCreateInfo InMeshCreateInfo)
{
    const auto Entity = Registry.create();

    auto Node = Registry.emplace<SceneNode>(Entity);
    Registry.emplace<Model>(Entity, glm::mat4(InMeshCreateInfo.Transform));
    Registry.emplace<std::string>(Entity, InMeshCreateInfo.Name);

    SetEntityVisible(Entity, true);
    if(!InMeshCreateInfo.bIsVisible)
    {
        SetEntityVisible(Entity, false);
    }

    
    return Entity;
}

entt::entity Scene::AddMesh(const fs::path& MeshFilePath, MeshCreateInfo InMeshCreateInfo)
{
    return AddMesh(Mesh(MeshFilePath), std::move(InMeshCreateInfo));
}

void Scene::Render()
{
    MainRenderPipeline->SetViewport(0, 0, 1280, 720);
    MainRenderPipeline->SetClearColor(BackgroundColor);
    MainRenderPipeline->Clear();
}

void Scene::RenderGizmos()
{
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
