#pragma once

#include "pch.h"
#include "Renderer/Camera/Camera.h"
#include "Renderer/Renderer.h"
#include "Renderer/Mesh/MeshElement.h"
#include "Renderer/Buffers/VertexBuffer.h"
#include "Renderer/Buffers/UniformBuffer.h"
#include "Renderer/Buffers/VertexArray.h"

#include "entt.hpp"
// #include "ImGuizmo.h"

// struct Gizmo;
class Mesh;

struct Visible
{
};

struct Model {
    Model(const glm::mat4& InTransform)
        : Transform{InTransform}, InvTransform{glm::transpose(glm::inverse(Transform))} {}

    glm::mat4 Transform = glm::mat4(1);
    // `InvTransform` is the _transpose_ of the inverse of `Transform`.
    // Since this rarely changes, we precompute it and send it to the shader.
    glm::mat4 InvTransform = glm::mat4(1);
};

struct ViewProj {
    glm::mat4 ViewMatrix = glm::mat4(1);
    glm::mat4 ProjectionMatrix = glm::mat4(1);
};

struct ViewProjNearFar {
    glm::mat4 ViewMatrix = glm::mat4(1);
    glm::mat4 ProjectionMatrix = glm::mat4(1);
    
    float Near;
    float Far;
};

struct SceneNode {
    entt::entity Parent = entt::null;
    std::vector<entt::entity> Children;
    // Maps entities to their index in the models buffer. Includes parent. Only present in parent nodes.
    // This allows for contiguous storage of models in the buffer, with erases but no inserts (only appends, which avoids shuffling memory regions).
    std::unordered_map<entt::entity, unsigned int> ModelIndices;
};

enum class SelectionMode {
    Object,  // Select objects
    Element, // Select individual mesh elements (vertices, edges, faces)
};

struct MeshCreateInfo
{
    std::string Name;
    glm::mat4 Transform = glm::mat4(1);
    bool bIsSelect = true;
    bool bIsVisible = true;
    bool bIsSubmit = true;
};

using MeshBufferMap = std::unordered_map<MeshElementType, std::shared_ptr<VertexArray>>;
struct MeshGLData
{
    std::unordered_map<entt::entity, MeshBufferMap> PrimaryMeshs;
    std::unordered_map<entt::entity, std::shared_ptr<Model>> ModelMatrices;
    // std::unordered_map<entt::entity, MeshBufferMap> NormalIndicators;
};

class Scene
{
public:
    Scene();
    ~Scene();

    void SetViewportSize(uint32_t Width, uint32_t Height);
    uint32_t GetViewportWidth() const { return ViewportWidth; }
    uint32_t GetViewportHeight() const { return ViewportHeight; }

    entt::entity AddMesh(Mesh&& InMesh, MeshCreateInfo InMeshCreateInfo = {});
    entt::entity AddMesh(const fs::path& MeshFilePath, MeshCreateInfo InMeshCreateInfo = {});
    entt::entity GetSelectedEntity() const;
    entt::entity GetParentEntity(entt::entity Entity) const;

    void SetEntityVisible(entt::entity Entity, bool bIsVisible);
    
    void Render();
    void RenderGizmos();

    Camera CreateDefaultCamera() const;
    VertexBufferLayout CreateDefaultVertexLayout();

    std::optional<unsigned int> GetModelBufferIndex(entt::entity Entity);

    void UpdateViewProjBuffers();

public:
    uint32_t ViewportWidth = 0, ViewportHeight = 0;
    
    Camera Camera;
    
    entt::registry Registry;
    entt::entity SelectedEntity = entt::null;

    std::unique_ptr<Renderer> MainRenderPipeline;
    glm::vec4 BackgroundColor = glm::vec4(0.22f, 0.22f, 0.22f, 1.f);

    // std::unique_ptr<Gizmo> Gizmo;

    SelectionMode SelectionMode = SelectionMode::Object;
    MeshElementType SelectionMeshElementType = MeshElementType::Face;
    MeshElementIndex SelectedElement;

    std::unique_ptr<MeshGLData> MeshGLData;

    // buffers
    std::unique_ptr<UniformBuffer> ViewProjBuffer;
    std::unique_ptr<UniformBuffer> ViewProjNearFarBuffer;
    std::unique_ptr<UniformBuffer> LightsBuffer;
};