#include "Material.h"
#include "Renderer/Shader/Shader.h"

LINK_EDITOR_NAMESPACE_BEGIN

Material::Material(const std::shared_ptr<Shader>& Shader, const std::string& Name)
    : MaterialShader(Shader), MaterialName(Name)
{
}

LINK_EDITOR_NAMESPACE_END
