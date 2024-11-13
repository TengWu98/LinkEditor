#include "Ray.h"

MESH_EDITOR_NAMESPACE_BEGIN

Ray Ray::WorldToLocal(const glm::mat4& ModelMatrix) const
{
    const glm::mat4 InverseModelMatrix = glm::inverse(ModelMatrix);
    const glm::vec4 LocalOrigin = InverseModelMatrix * glm::vec4(Origin, 1.0f);
    const glm::vec3 LocalDirection = glm::normalize(glm::vec3(InverseModelMatrix * glm::vec4(Direction, 0.0f)));
    return {LocalOrigin, LocalDirection};
}

MESH_EDITOR_NAMESPACE_END
