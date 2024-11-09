#pragma once

#include "pch.h"
#include "Renderer/Texture/Texture.h"

MESH_EDITOR_NAMESPACE_BEGIN

class Texture2D : public Texture
{
public:
    Texture2D(const std::string& InPath);
    Texture2D(const TextureSpecification& InSpec);
    virtual ~Texture2D();

    virtual const TextureSpecification& GetSpecification() const override { return Spec; } 
    virtual uint32_t GetRendererID() const override { return RendererID; }
    virtual uint32_t GetWidth() const override { return Width; }
    virtual uint32_t GetHeight() const override { return Height; }
    virtual const std::string& GetPath() const override { return Path; }
    virtual bool IsLoaded() const override { return bIsLoaded; }

    virtual void Bind(uint32_t Slot = 0) const override;
    virtual void SetData(void* Data, uint32_t Size) override;

    virtual bool operator==(const Texture& Other) const override
    {
        return RendererID == Other.GetRendererID();
    }
    
private:
    TextureSpecification Spec;
    uint32_t RendererID;
    
    std::string Path;
    uint32_t Width, Height;

    bool bIsLoaded = false;

    GLenum InternalFormat;
    GLenum DataFormat;
};

MESH_EDITOR_NAMESPACE_END
