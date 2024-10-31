#include "Renderer.h"
#include "Scene/Scene.h"
#include "Core/Window/Window.h"
#include "Renderer/Buffers/VertexArray.h"
#include "Renderer/Buffers/IndexBuffer.h"
#include "Renderer/Shader/Shader.h"

Renderer::Renderer(RenderSpecification InSpecification)
    : Specification(InSpecification)
{
    Init();
}

Renderer::~Renderer()
{
}

void Renderer::Init()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);

    ViewportWidth = Specification.ViewportWidth;
    ViewportHeight = Specification.ViewportHeight;

    // Frame buffer
    FramebufferSpecification Spec;
    Spec.Width = ViewportWidth;
    Spec.Height = ViewportHeight;
    Spec.Attachments = {FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth};
    FBO = std::make_unique<FrameBuffer>(Spec);
    FBO->Bind();

    // Set the viewport
    SetViewport(0, 0, ViewportWidth, ViewportHeight);

    // TODO(WT) Shader Library
    // Shaders["Fill"] = std::make_shared<Shader>("", "");
    
}

void Renderer::SetViewport(uint32_t X, uint32_t Y, uint32_t Width, uint32_t Height)
{
    glViewport(X, Y, Width, Height);
}

void Renderer::SetClearColor(const glm::vec4& Color)
{
    glClearColor(Color.r, Color.g, Color.b, Color.a);
}

void Renderer::SetLineWidth(float Width)
{
    glLineWidth(Width);
}

void Renderer::CompileShaders()
{
    
}

void Renderer::DrawIndexed(const std::shared_ptr<VertexArray>& VertexArray, uint32_t IndexCount)
{
    VertexArray->Bind();
    uint32_t Count = IndexCount ? IndexCount : VertexArray->GetIndexBuffer()->GetCount();
    glDrawElements(GL_TRIANGLES, Count, GL_UNSIGNED_INT, nullptr);
}

void Renderer::DrawLines(const std::shared_ptr<VertexArray>& VertexArray, uint32_t VertexCount)
{
    VertexArray->Bind();
    glDrawArrays(GL_LINES, 0, VertexCount);
}

void Renderer::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}