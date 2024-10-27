#pragma once

#include "pch.h"

enum class FramebufferTextureFormat
{
    None = 0,

    // Color
    RGBA8,
    RED_INTEGER,

    // Depth / Stencil
    DEPTH24STENCIL8,

    // Defaults
    Depth = DEPTH24STENCIL8
};

struct FramebufferTextureSpecification
{
    FramebufferTextureSpecification() = default;
    FramebufferTextureSpecification(FramebufferTextureFormat Format)
        : TextureFormat(Format) {}

    FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
};

struct FramebufferAttachmentSpecification
{
    FramebufferAttachmentSpecification() = default;
    FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> InAttachments)
        : Attachments(InAttachments) {}

    std::vector<FramebufferTextureSpecification> Attachments;
};

struct FramebufferSpecification
{
    uint32_t Width = 0, Height = 0;
    FramebufferAttachmentSpecification Attachments;
    uint32_t Samples = 1;

    bool SwapChainTarget = false;
};

class FrameBuffer
{
public:
    FrameBuffer(const FramebufferSpecification& Spec);
    ~FrameBuffer();

    void Bind() const;
    void Unbind() const;

    void Resize(uint32_t Width, uint32_t Height);
    int ReadPixel(uint32_t AttachmentIndex, int X, int Y);

    void ClearAttendment(uint32_t AttachmentIndex, int Value);
    uint32_t GetColorAttachmentRendererID(uint32_t Index = 0) const;

    const FramebufferSpecification& GetSpecification() const { return Specification; }
    
private:
    uint32_t RendererID;

    FramebufferSpecification Specification;
    std::vector<FramebufferTextureSpecification> ColorAttachmentSpecifications;
    FramebufferTextureSpecification DepthAttachmentSpecification = FramebufferTextureFormat::None;
    
    std::vector<uint32_t> ColorAttachments;
    uint32_t DepthAttachment = 0;
};
