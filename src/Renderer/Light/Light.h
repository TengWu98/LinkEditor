#pragma once

#include "pch.h"

class Light
{
public:
    Light(const glm::vec3 InIntensity);
    ~Light();
    
protected:
    glm::vec3 Intensity;
    bool bIsEnabled;
};
