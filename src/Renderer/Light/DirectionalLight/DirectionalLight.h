#pragma once

#include "pch.h"
#include "Renderer/Light/Light.h"

MESH_EDITOR_NAMESPACE_BEGIN

class DirectionalLight : public Light
{
public:
    DirectionalLight(const float InIntensity, const glm::vec3& InDirection, const glm::vec3& InColor);
    ~DirectionalLight();
    
private:
    using Light::Intensity;
    glm::vec3 Direction;
    glm::vec3 Color;
};

MESH_EDITOR_NAMESPACE_END
