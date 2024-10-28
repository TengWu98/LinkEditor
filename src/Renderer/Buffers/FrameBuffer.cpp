#include "FrameBuffer.h"

static const uint32_t MaxFramebufferSize = 8192;

static bool IsDepthFormat(FramebufferTextureFormat Format)
{
    switch (Format)
    {
    case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
    }

    return false;
}

static GLenum FrameBufferTextureFormatToGL(FramebufferTextureFormat Format)
{
    switch (Format)
    {
    case FramebufferTextureFormat::RGBA8:       return GL_RGBA8;
    case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
    }
    
    return 0;
}

FrameBuffer::FrameBuffer(const FramebufferSpecification& Spec)
    : Specification(Spec)
{
    for(auto Attachment : Specification.Attachments.Attachments)
    {
        if(IsDepthFormat(Attachment.TextureFormat))
        {
            ColorAttachmentSpecifications.emplace_back(Attachment);
        }
        else
        {
            DepthAttachmentSpecification = Attachment;
        }
    }
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &RendererID);
    glDeleteTextures((GLsizei)ColorAttachments.size(), ColorAttachments.data());
    glDeleteTextures(1, &DepthAttachment);
}

void FrameBuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, RendererID);
    glViewport(0, 0, Specification.Width, Specification.Height);
}

void FrameBuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Resize(uint32_t Width, uint32_t Height)
{
    if (Width == 0 || Height == 0 || Width > MaxFramebufferSize || Height > MaxFramebufferSize)
    {
        LOG_WARN("Attempted to rezize framebuffer to {0}, {1}", Width, Height);
        return;
    }
    
    Specification.Width = Width;
    Specification.Height = Height;
}

int FrameBuffer::ReadPixel(uint32_t AttachmentIndex, int X, int Y)
{
    glReadBuffer(GL_COLOR_ATTACHMENT0 + AttachmentIndex);

    int PixelData;
    glReadPixels(X, Y, 1, 1, GL_RED_INTEGER, GL_INT, &PixelData);
    return PixelData;
}

void FrameBuffer::ClearAttendment(uint32_t AttachmentIndex, int Value)
{
    MESH_EDITOR_CORE_ASSERT(AttachmentIndex < ColorAttachments.size())
    
    auto& Spec = ColorAttachmentSpecifications[AttachmentIndex];
    
    glClearTexImage(ColorAttachments[AttachmentIndex], 0,
        FrameBufferTextureFormatToGL(Spec.TextureFormat), GL_INT, &Value);
}

uint32_t FrameBuffer::GetColorAttachmentRendererID(uint32_t Index) const
{
    MESH_EDITOR_CORE_ASSERT(Index < ColorAttachments.size())
    
    return ColorAttachments[Index];
}
