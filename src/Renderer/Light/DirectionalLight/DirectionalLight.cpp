#include "DirectionalLight.h"

MESH_EDITOR_NAMESPACE_BEGIN

DirectionalLight::DirectionalLight(const float InIntensity, const glm::vec3& InDirection, const glm::vec3& InColor)
    : Light(InIntensity), Direction(InDirection), Color(InColor)
{
}

DirectionalLight::~DirectionalLight()
{
}

MESH_EDITOR_NAMESPACE_END
