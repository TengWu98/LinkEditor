#pragma once
#include "pch.h"

LINK_EDITOR_NAMESPACE_BEGIN

class Shader;

class Material
{
public:
    Material() = default;
    Material(const std::shared_ptr<Shader>& Shader, const std::string& Name = "");
    virtual ~Material() = default;

    virtual std::shared_ptr<Shader> GetShader() const = 0;
    virtual const std::string& GetName() const = 0;

private:
    std::shared_ptr<Shader> MaterialShader;
    std::string MaterialName;
};

LINK_EDITOR_NAMESPACE_END
