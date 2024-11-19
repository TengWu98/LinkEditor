#include "Material.h"
#include "Renderer/Shader/Shader.h"

MESH_EDITOR_NAMESPACE_BEGIN

Material::Material(const std::shared_ptr<Shader>& Shader, const std::string& Name)
    : MaterialShader(Shader), MaterialName(Name)
{
}

MESH_EDITOR_NAMESPACE_END
