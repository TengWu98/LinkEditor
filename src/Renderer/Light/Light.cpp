#include "Light.h"

MESH_EDITOR_NAMESPACE_BEGIN

Light::Light()
{
}

Light::~Light()
{
}

void Light::SetIntensity(const float InIntensity)
{
    Intensity = InIntensity;
}

void Light::SetLightColor(const glm::vec3& InColor)
{
    LightColor = InColor;
}

MESH_EDITOR_NAMESPACE_END
