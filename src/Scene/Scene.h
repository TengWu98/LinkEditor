#pragma once

#include "pch.h"
#include "entt.hpp"

class Mesh;

struct MeshCreateInfo
{
    std::string Name;
    glm::mat4 Transform;
    bool bIsSelect = true;
    bool bIsVisible = true;
    bool bIsSubmit = true;
};

class Scene
{
public:
    Scene();
    ~Scene();

    entt::entity AddMesh(Mesh&& InMesh, MeshCreateInfo InMeshCreateInfo = {});
    entt::entity AddMesh(const fs::path& MeshFilePath, MeshCreateInfo InMeshCreateInfo = {});

public:
    entt::registry Registry;
};
