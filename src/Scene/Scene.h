#pragma once

#include "pch.h"
#include "Renderer/Camera/Camera.h"
#include "Renderer/Renderer.h"
#include "Renderer/Mesh/MeshElement.h"
#include "Renderer/Buffers/VertexBuffer.h"
#include "Renderer/Buffers/UniformBuffer.h"

#include "entt.hpp"
// #include "ImGuizmo.h"

// struct Gizmo;
class Mesh;

struct Model {
    Model(glm::mat4 &&InTransform)
        : Transform{std::move(InTransform)}, InvTransform{glm::transpose(glm::inverse(Transform))} {}

    glm::mat4 Transform{1};
    // `InvTransform` is the _transpose_ of the inverse of `Transform`.
    // Since this rarely changes, we precompute it and send it to the shader.
    glm::mat4 InvTransform{1};
};

struct ViewProj {
    glm::mat4 View{1}, Projection{1};
};

struct ViewProjNearFar {
    glm::mat4 View{1}, Projection{1};
    float Near, Far;
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
    glm::mat4 Transform {1};
    bool bIsSelect = true;
    bool bIsVisible = true;
    bool bIsSubmit = true;
};

using MeshBuffers = std::unordered_map<MeshElement, VertexArray>;
struct MeshGLData
{
    std::unordered_map<entt::entity, MeshBuffers> Main, NormalIndicators;
    std::unordered_map<entt::entity, VertexBuffer> Models;
};

class Scene
{
public:
    Scene();
    ~Scene();

    entt::entity AddMesh(Mesh&& InMesh, MeshCreateInfo InMeshCreateInfo = {});
    entt::entity AddMesh(const fs::path& MeshFilePath, MeshCreateInfo InMeshCreateInfo = {});
    entt::entity GetSelectedEntity() const;
    entt::entity GetParentEntity(entt::entity Entity) const;

    void SetEntityVisible(entt::entity Entity, bool bIsVisible);

    Camera CreateDefaultCamera() const;
    void Render();
    void RenderGizmos();

public:
    Camera Camera;

    glm::vec4 EdgeColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);        // Used for line mode.
    glm::vec4 MeshEdgeColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);  // Used for faces mode.
    
    entt::registry Registry;
    entt::entity SelectedEntity = entt::null;

    std::unique_ptr<Renderer> MainRenderPipeline;
    glm::vec4 BackgroundColor = glm::vec4(0.22f, 0.22f, 0.22f, 1.f);

    // std::unique_ptr<Gizmo> Gizmo;

    SelectionMode SelectionMode = SelectionMode::Object;
    MeshElement SelectionMeshElement = MeshElement::Face;
    MeshElementIndex SelectedElement;

    std::unique_ptr<MeshGLData> MeshGLData;

    // buffers
    std::unique_ptr<UniformBuffer> TransformBuffer;
    std::unique_ptr<UniformBuffer> ViewProjNearFarBuffer;
};