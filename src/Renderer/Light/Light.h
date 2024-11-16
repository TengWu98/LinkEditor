#pragma once

#include "pch.h"

MESH_EDITOR_NAMESPACE_BEGIN

class Light
{
public:
    Light();
    ~Light();

    void SetIntensity(const float InIntensity);
    void SetLightColor(const glm::vec3& InColor);
    
public:
    bool bIsEnabled;
    float Intensity;
    glm::vec3 LightColor;
};

MESH_EDITOR_NAMESPACE_END
