#pragma once

#include "pch.h"
#include "Renderer/Light/Light.h"

MESH_EDITOR_NAMESPACE_BEGIN

class DirectionalLight : public Light
{
public:
    DirectionalLight();
    ~DirectionalLight();
    
public:
    glm::vec3 Direction;
    float AmbientIntensity = 0,f;
};

MESH_EDITOR_NAMESPACE_END
