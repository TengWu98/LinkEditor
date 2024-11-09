#include "FrameBuffer.h"

MESH_EDITOR_NAMESPACE_BEGIN

static const uint32_t MaxFramebufferSize = 8192;

static GLenum TextureTarget(bool bIsMultiSample)
{
    return bIsMultiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

static void CreateTextures(bool bIsMultiSample, uint32_t* OutID, uint32_t Count)
{
    glCreateTextures(TextureTarget(bIsMultiSample), Count, OutID);
}

static void BindTexture(bool bIsMultiSample, uint32_t Id)
{
    glBindTexture(TextureTarget(bIsMultiSample), Id);
}

static void  AttachColorTexture(uint32_t Id, int Samples, GLenum InternalFormat, GLenum Format, uint32_t Width, uint32_t Height, int Index)
{
    const bool bIsMultiSample = Samples > 1;
    if (bIsMultiSample)
    {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, InternalFormat, Width, Height, GL_FALSE);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width, Height, 0, Format, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + Index, TextureTarget(bIsMultiSample), Id, 0);
}

static void AttachDepthTexture(uint32_t Id, int Samples, GLenum Format, GLenum AttachmentType, uint32_t Width, uint32_t Height)
{
    const bool bIsMultiSample = Samples > 1;
    if (bIsMultiSample)
    {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, Format, Width, Height, GL_FALSE);
    }
    else
    {
        glTexStorage2D(GL_TEXTURE_2D, 1, Format, Width, Height);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, AttachmentType, TextureTarget(bIsMultiSample), Id, 0);
}

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
        if(!IsDepthFormat(Attachment.TextureFormat))
        {
            ColorAttachmentSpecifications.emplace_back(Attachment);
        }
        else
        {
            DepthAttachmentSpecification = Attachment;
        }
    }

    Invalidate();
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &RendererID);
    glDeleteTextures(ColorAttachments.size(), ColorAttachments.data());
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

void FrameBuffer::Invalidate()
{
    if(RendererID)
    {
        glDeleteFramebuffers(1, &RendererID);
        glDeleteTextures(static_cast<GLsizei>(ColorAttachments.size()), ColorAttachments.data());
        glDeleteTextures(1, &DepthAttachment);

        ColorAttachments.clear();
        DepthAttachment = 0;
    }

    glCreateFramebuffers(1, &RendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, RendererID);

    bool bIsMultiSample = Specification.Samples > 1;

    // Color Attachments
    if(!ColorAttachmentSpecifications.empty())
    {
        ColorAttachments.resize(ColorAttachmentSpecifications.size());
        CreateTextures(bIsMultiSample, ColorAttachments.data(), ColorAttachments.size());

        for(size_t i = 0; i < ColorAttachments.size(); i++)
        {
            BindTexture(bIsMultiSample, ColorAttachments[i]);
            switch (ColorAttachmentSpecifications[i].TextureFormat)
            {
            case FramebufferTextureFormat::RGBA8:
                AttachColorTexture(ColorAttachments[i], Specification.Samples, GL_RGBA8, GL_RGBA, Specification.Width, Specification.Height, i);
                break;
            case FramebufferTextureFormat::RED_INTEGER:
                AttachColorTexture(ColorAttachments[i], Specification.Samples, GL_R32I, GL_RED_INTEGER, Specification.Width, Specification.Height, i);
            }
        }
    }

    // Depth Attachment
    if(DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
    {
        CreateTextures(bIsMultiSample, &DepthAttachment, 1);
        BindTexture(bIsMultiSample, DepthAttachment);
        switch (DepthAttachmentSpecification.TextureFormat)
        {
        case FramebufferTextureFormat::DEPTH24STENCIL8:
            AttachDepthTexture(DepthAttachment, Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, Specification.Width, Specification.Height);
            break;
        }
    }

    if(ColorAttachments.size() > 1)
    {
        MESH_EDITOR_CORE_ASSERT(ColorAttachments.size() <= 4);
        GLenum Buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(ColorAttachments.size(), Buffers);
    }
    else if(ColorAttachments.empty())
    {
        glDrawBuffer(GL_NONE);
    }

    MESH_EDITOR_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
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

    Invalidate();
}

int FrameBuffer::ReadPixel(uint32_t AttachmentIndex, int X, int Y)
{
    glReadBuffer(GL_COLOR_ATTACHMENT0 + AttachmentIndex);

    int PixelData;
    glReadPixels(X, Y, 1, 1, GL_RED_INTEGER, GL_INT, &PixelData);
    return PixelData;
}

void FrameBuffer::ClearAttachment(uint32_t AttachmentIndex, int Value)
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

MESH_EDITOR_NAMESPACE_END
