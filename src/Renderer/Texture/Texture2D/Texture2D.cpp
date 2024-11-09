#include "Texture2D.h"

MESH_EDITOR_NAMESPACE_BEGIN

static GLenum ImageFormatToGLDataFormat(ImageFormat Format)
{
    switch (Format)
    {
    case ImageFormat::RGB8:  return GL_RGB;
    case ImageFormat::RGBA8: return GL_RGBA;
    }

    MESH_EDITOR_CORE_ASSERT(false)
    return 0;
}

static GLenum ImageFormatToGLInternalFormat(ImageFormat Format)
{
    switch (Format)
    {
    case ImageFormat::RGB8:  return GL_RGB8;
    case ImageFormat::RGBA8: return GL_RGBA8;
    }

    MESH_EDITOR_CORE_ASSERT(false)
    return 0;
}

Texture2D::Texture2D(const std::string& InPath)
    : Path(InPath)
{
    // TODO（WT）
}

Texture2D::Texture2D(const TextureSpecification& InSpec)
    : Spec(InSpec), Width(InSpec.Width), Height(InSpec.Height)
{
    InternalFormat = ImageFormatToGLInternalFormat(Spec.Format);
    DataFormat = ImageFormatToGLDataFormat(Spec.Format);
    
    glCreateTextures(GL_TEXTURE_2D, 1, &RendererID);
    glTextureStorage2D(RendererID, 1, InternalFormat, Width, Height);

    glTextureParameteri(RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureParameteri(RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &RendererID);
}

void Texture2D::Bind(uint32_t Slot) const
{
    glBindTextureUnit(Slot, RendererID);
}

void Texture2D::SetData(void* Data, uint32_t Size)
{
    uint32_t BytePerPixel = DataFormat == GL_RGBA ? 4 : 3;
    MESH_EDITOR_CORE_ASSERT(Size == Width * Height * BytePerPixel, "Data must be entire texture!");
    glTextureSubImage2D(RendererID, 0, 0, 0, Width, Height, DataFormat, GL_UNSIGNED_BYTE, Data);
}

MESH_EDITOR_NAMESPACE_END
