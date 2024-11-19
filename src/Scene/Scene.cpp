#include "Scene.h"
#include "Renderer/Buffers/IndexBuffer.h"
#include "Renderer/Mesh/Mesh.h"
#include "Renderer/Gizmo/Gizmo.h"

MESH_EDITOR_NAMESPACE_BEGIN

std::string IdString(entt::entity Entity)
{
    std::ostringstream oss;
    oss << std::hex << std::setw(8) << std::setfill('0') << static_cast<uint32_t>(Entity);
    return "0x" + oss.str();
}

Scene::Scene() :
    SceneCamera(CreateDefaultCamera()),
    Registry(entt::registry()),
    SceneMeshGLData(std::make_unique<MeshGLData>())
{
    // Initialize Render pipeline
    RenderSpecification Spec;
    SceneRenderer = std::make_unique<Renderer>(Spec);

    // Initialize ViewProj buffers
    ViewProjBuffer = std::make_unique<UniformBuffer>(sizeof(ViewProj), 0);
    ViewProjNearFarBuffer = std::make_unique<UniformBuffer>(sizeof(ViewProjNearFar), 1);
    UpdateViewProjBuffers();
    
    // TODO(WT) Lights buffer
    Lights.emplace_back(std::make_shared<DirectionalLight>());
    LightsBuffer = std::make_unique<UniformBuffer>(sizeof(LightShaderParameters), 2);
    UpdateLightsBuffer();
    
    SceneGizmo = std::make_unique<Gizmo>();
    SceneGizmo->Init();
}

Scene::~Scene()
{
}

void Scene::SetViewportSize(uint32_t Width, uint32_t Height)
{
    ViewportWidth = Width;
    ViewportHeight = Height;
    SceneRenderer->GetFrameBuffer()->Resize(Width, Height);
}

entt::entity Scene::AddMesh(Mesh&& InMesh, MeshCreateInfo InMeshCreateInfo)
{
    const auto Entity = Registry.create();

    auto Node = Registry.emplace<SceneNode>(Entity);
    Registry.emplace<Model>(Entity, InMeshCreateInfo.Transform);
    Registry.emplace<std::string>(Entity, InMeshCreateInfo.Name);
    SceneMeshGLData->ModelMatrices.emplace(Entity, std::make_shared<Model>(InMeshCreateInfo.Transform));
    
    SetEntityVisible(Entity, true);
    if(!InMeshCreateInfo.bIsVisible)
    {
        SetEntityVisible(Entity, false);
    }

    MeshBufferMap MeshBuffers;
    for (auto ElementType : AllMeshElementTypes)
    {
        auto VertexArrayBuffer = std::make_shared<VertexArray>();
        std::vector<MeshVertex> Vertices = InMesh.CreateVertices(ElementType);
        std::vector<uint> Indices = InMesh.CreateIndices(ElementType);

        auto VertexBufferObject = std::make_shared<VertexBuffer>(Vertices.size() * sizeof(MeshVertex));
        VertexBufferObject->SetLayout(CreateDefaultVertexLayout());
        VertexBufferObject->SetData(Vertices.data(), Vertices.size() * sizeof(MeshVertex));
        VertexArrayBuffer->AddVertexBuffer(VertexBufferObject);

        auto IndexBufferObject = std::make_shared<IndexBuffer>(Indices.data(), Indices.size());
        VertexArrayBuffer->SetIndexBuffer(IndexBufferObject);
        MeshBuffers.emplace(ElementType, VertexArrayBuffer);
    }
    
    SceneMeshGLData->PrimaryMeshs.emplace(Entity, MeshBuffers);

    Registry.emplace<Mesh>(Entity, std::move(InMesh));
    
    if(InMeshCreateInfo.bIsSelect)
    {
        SelectEntity(Entity);
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

const Mesh& Scene::GetSelectedMesh() const
{
    return Registry.get<Mesh>(SelectedEntity);
}

void Scene::Render()
{
    if(ViewportWidth == 0 || ViewportHeight == 0)
    {
        return;
    }
    
    // Clear
    SceneRenderer->SetClearColor(BackgroundColor);
    SceneRenderer->Clear();
    
    // Update Camera
    SceneCamera.Update();
    UpdateViewProjBuffers();

    // Update Lights
    UpdateLightsBuffer();

    // Render Gizmos

    // Render Meshs
    for(auto PrimaryMesh : SceneMeshGLData->PrimaryMeshs)
    {
        auto Entity = PrimaryMesh.first;
        auto ModelStruct = SceneMeshGLData->ModelMatrices.at(Entity);
        auto MeshVertexArrayBuffer = PrimaryMesh.second.at(SelectionMeshElementType);
        
        SceneRenderer->UpdateShaderData({
            // Phong Shader
            ShaderBindingDescriptor{ShaderPipelineType::Phong, "u_ModelMatrix", std::nullopt, std::nullopt, ModelStruct->Transform},
            ShaderBindingDescriptor{ShaderPipelineType::Phong, "u_DiffuseColor", std::nullopt, SceneRenderer->ShaderData.Phong_Diffuse, std::nullopt},
            ShaderBindingDescriptor{ShaderPipelineType::Phong, "u_SpecularColor", std::nullopt, SceneRenderer->ShaderData.Phong_Specular, std::nullopt},
            ShaderBindingDescriptor{ShaderPipelineType::Phong, "u_Gloss", SceneRenderer->ShaderData.Phong_Gloss, std::nullopt, std::nullopt},

            // Depth Shader
            ShaderBindingDescriptor{ShaderPipelineType::Depth, "u_ModelMatrix", std::nullopt, std::nullopt, ModelStruct->Transform},
            ShaderBindingDescriptor{ShaderPipelineType::Depth, "u_Near", SceneRenderer->ShaderData.Depth_NearPlane, std::nullopt, std::nullopt},
            ShaderBindingDescriptor{ShaderPipelineType::Depth, "u_Far", SceneRenderer->ShaderData.Depth_FarPlane, std::nullopt, std::nullopt},
        });
        
        SceneRenderer->Render(MeshVertexArrayBuffer);
    }
}

void Scene::UpdateViewProjBuffers()
{
    const float AspectRatio = ViewportWidth == 0 || ViewportHeight == 0 ? 1.f : static_cast<float>(ViewportWidth) / static_cast<float>(ViewportHeight);
    SceneCamera.SetAspectRatio(AspectRatio);
    const ViewProj ViewProjMat = {SceneCamera.GetViewMatrix(), SceneCamera.GetProjectionMatrix()};
    ViewProjBuffer->SetData(&ViewProjMat, sizeof(ViewProjMat));

    const ViewProjNearFar ViewProjNearFarMat = {SceneCamera.GetViewMatrix(), SceneCamera.GetProjectionMatrix(), SceneCamera.NearClip, SceneCamera.FarClip};
    ViewProjNearFarBuffer->SetData(&ViewProjNearFarMat, sizeof(ViewProjNearFarMat));
}

void Scene::UpdateLightsBuffer()
{
    LightShaderParameters LightShaderData;
    auto DirLight = std::static_pointer_cast<DirectionalLight>(Lights[0]);
    LightShaderData.LightColorAndAmbient = glm::vec4(glm::vec3(DirLight->LightColor), DirLight->AmbientIntensity);
    LightShaderData.LightDirAndIntensity = glm::vec4(DirLight->Direction, DirLight->Intensity);
    LightsBuffer->SetData(&LightShaderData, sizeof(LightShaderData));
}

void Scene::UpdateRenderBuffers(entt::entity InEntity, MeshElementIndex HighLightElement)
{
    if(InEntity == entt::null)
    {
        return;
    }

    const auto& SelectedMesh = Registry.get<Mesh>(InEntity);
    auto& MeshBuffers = SceneMeshGLData->PrimaryMeshs.at(InEntity);
    const Mesh::ElementIndex HighLight{HighLightElement};
    for(auto ElementType : AllMeshElementTypes)
    {
        auto VertexArrayBuffer = MeshBuffers.at(ElementType);
        std::vector<MeshVertex> Vertices = SelectedMesh.CreateVertices(ElementType, HighLight);

        auto VertexBufferObject = VertexArrayBuffer->GetVertexBuffers()[0];
        VertexBufferObject->SetData(Vertices.data(), Vertices.size() * sizeof(MeshVertex));
    }
}

VertexBufferLayout Scene::CreateDefaultVertexLayout()
{
    return {
        {ShaderDataType::Float3, "a_Position"},
        {ShaderDataType::Float4, "a_Color"},
        {ShaderDataType::Float3, "a_WorldNormal"},
        {ShaderDataType::Float2, "a_TexCoord"},
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
    return {glm::vec3(0, 0, 2), glm::vec3(0, 1, 0), glm::vec3(0, 0, 0), 0.01, 100};
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

std::string Scene::GetEntityName(entt::entity Entity) const
{
    if(Entity == entt::null)
    {
        return "NULL";
    }

    if(const auto *Name = Registry.try_get<std::string>(Entity))
    {
        if(!Name->empty())
        {
            return *Name;
        }
    }

    return IdString(Entity);
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

void Scene::SelectEntity(entt::entity InEntity)
{
    SelectedEntity = InEntity;
}

glm::mat4 Scene::GetModelMatrix(entt::entity InEntity) const
{
    if(InEntity == entt::null)
    {
        return glm::mat4(1);
    }
    
    return Registry.get<Model>(InEntity).Transform;
}

void Scene::SetModelMatrix(entt::entity InEntity, const glm::mat4& InModelMatrix)
{
    if(InEntity == entt::null)
    {
        return;
    }
    
    Registry.replace<Model>(InEntity, InModelMatrix);
    SceneMeshGLData->ModelMatrices.at(InEntity)->Transform = InModelMatrix;
    SceneMeshGLData->ModelMatrices.at(InEntity)->UpdateInvTransform();
}

MESH_EDITOR_NAMESPACE_END
