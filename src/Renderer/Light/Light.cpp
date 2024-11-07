#include "Light.h"

MESH_EDITOR_NAMESPACE_BEGIN

Light::Light(const glm::vec3 InIntensity)
    : Intensity(InIntensity)
{
}

Light::~Light()
{
}

MESH_EDITOR_NAMESPACE_END
