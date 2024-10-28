#pragma once

#include "pch.h"
#include "Renderer/Light/Light.h"

class DirectionalLight : public Light
{
public:
    DirectionalLight(const glm::vec3 InIntensity, const glm::vec3& InDirection);
    ~DirectionalLight();
    
private:
    using Light::Intensity;
    glm::vec3 Direction;
};
