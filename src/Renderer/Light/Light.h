#pragma once

#include "pch.h"

MESH_EDITOR_NAMESPACE_BEGIN

class Light
{
public:
    Light(const float InIntensity);
    ~Light();
    
protected:
    float Intensity;
    bool bIsEnabled;
};

MESH_EDITOR_NAMESPACE_END
