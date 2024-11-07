#include "DirectionalLight.h"

MESH_EDITOR_NAMESPACE_BEGIN

DirectionalLight::DirectionalLight(const glm::vec3 InIntensity, const glm::vec3& InDirection)
    : Light(InIntensity), Direction(InDirection)
{
}

DirectionalLight::~DirectionalLight()
{
}

MESH_EDITOR_NAMESPACE_END
