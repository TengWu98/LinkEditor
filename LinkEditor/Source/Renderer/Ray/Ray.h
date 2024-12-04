#pragma once
#include "pch.h"

LINK_EDITOR_NAMESPACE_BEGIN

struct Ray
{
    Ray(const glm::vec3& InOrigin, const glm::vec3& InDirection)
        : Origin(InOrigin)
        , Direction(InDirection)
    {
    }
    
    glm::vec3 operator()(float Time) const
    {
        return Origin + Time * Direction;
    }

    Ray WorldToLocal(const glm::mat4& ModelMatrix) const;

    glm::vec3 Origin;
    glm::vec3 Direction;
};

LINK_EDITOR_NAMESPACE_END