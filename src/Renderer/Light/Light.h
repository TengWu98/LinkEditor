#pragma once

#include "pch.h"

MESH_EDITOR_NAMESPACE_BEGIN

class Light
{
public:
    Light(const glm::vec3 InIntensity);
    ~Light();
    
protected:
    glm::vec3 Intensity;
    bool bIsEnabled;
};

MESH_EDITOR_NAMESPACE_END
