#pragma once

#include "pch.h"

LINK_EDITOR_NAMESPACE_BEGIN

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

LINK_EDITOR_NAMESPACE_END
