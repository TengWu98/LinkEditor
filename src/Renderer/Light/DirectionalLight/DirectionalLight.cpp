#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const glm::vec3 InIntensity, const glm::vec3& InDirection)
    : Light(InIntensity), Direction(InDirection)
{
}

DirectionalLight::~DirectionalLight()
{
}
