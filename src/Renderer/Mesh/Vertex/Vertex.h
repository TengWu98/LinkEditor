#pragma once

#include "pch.h"

MESH_EDITOR_NAMESPACE_BEGIN

struct Vertex3D
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec4 Color;
    glm::vec2 TexCoord;
};

MESH_EDITOR_NAMESPACE_END
