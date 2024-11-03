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
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);

    // Frame buffer
    FramebufferSpecification Spec;
    Spec.Attachments = {FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth};
    FBO = std::make_unique<FrameBuffer>(Spec);
    FBO->Bind();

    // Shader Library
    ShaderLibrary[ShaderPipelineType::Flat] = std::make_shared<Shader>("resources/shaders/Flat.glsl");
    ShaderLibrary[ShaderPipelineType::VertexColor] = std::make_shared<Shader>("resources/shaders/VertexColor.glsl");
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

void Renderer::UpdateShaderData(std::vector<ShaderBindingDescriptor>&& Descriptors)
{
    for (const auto& Descriptor : Descriptors)
    {
        const auto& Shader = ShaderLibrary.at(CurrentShaderPipeline);
        if (Shader)
        {
            if (Descriptor.ScalarData.has_value())
            {
                Shader->UploadUniformFloat(Descriptor.BindingName, Descriptor.ScalarData.value());
            }
            else if (Descriptor.VectorData.has_value())
            {
                Shader->UploadUniformFloat4(Descriptor.BindingName, Descriptor.VectorData.value());
            }
            else if (Descriptor.MatrixData.has_value())
            {
                Shader->UploadUniformMat4(Descriptor.BindingName, Descriptor.MatrixData.value());
            }
        }
    }
}

void Renderer::Render(const std::shared_ptr<VertexArray>& VertexArray, const std::shared_ptr<VertexBuffer> ModelMatrix, uint32_t IndexCount)
{
    auto Shader = ShaderLibrary[CurrentShaderPipeline];
    Shader->Bind();
    DrawIndexed(VertexArray, IndexCount);
}

void Renderer::DrawIndexed(const std::shared_ptr<VertexArray>& VertexArray, uint32_t IndexCount)
{
    VertexArray->Bind();
    uint32_t Count = IndexCount ? IndexCount : VertexArray->GetIndexBuffer()->GetCount();
    glDrawElements(GL_TRIANGLES, Count, GL_UNSIGNED_INT, nullptr);
}

void Renderer::DrawIndexInstanced(const std::shared_ptr<VertexArray>& VertexArray, uint32_t IndexCount,
    uint32_t InstanceCount)
{
    // TODO(WT) Implement this
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