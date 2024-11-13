#include "Renderer.h"
#include "Scene/Scene.h"
#include "Core/Window/Window.h"
#include "Renderer/Buffers/VertexArray.h"
#include "Renderer/Buffers/IndexBuffer.h"
#include "Renderer/Shader/Shader.h"

MESH_EDITOR_NAMESPACE_BEGIN

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
    Spec.Width = Specification.Width;
    Spec.Height = Specification.Height;
    Spec.Samples = 0;
    FBO = std::make_shared<FrameBuffer>(Spec);

    // Shader Library
    ShaderLibrary[ShaderPipelineType::Flat] = std::make_shared<Shader>("resources/shaders/Flat.glsl");
    ShaderLibrary[ShaderPipelineType::Depth] = std::make_shared<Shader>("resources/shaders/Depth.glsl");
    ShaderLibrary[ShaderPipelineType::Phong] = std::make_shared<Shader>("resources/shaders/Phong.glsl");
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

void Renderer::Render(const std::shared_ptr<VertexArray>& VertexArray, const std::shared_ptr<Model> ModelMatrix, std::optional<uint32_t> ModelIndex)
{
    auto Shader = ShaderLibrary[CurrentShaderPipeline];
    Shader->Bind();

    const uint32_t IndexCount = VertexArray->GetIndexBuffer()->GetCount();
    const uint32_t InstanceCount = ModelIndex.has_value() ? 1 : 0;
    DrawIndexInstanced(VertexArray, IndexCount, InstanceCount);
}

void Renderer::DrawIndexed(const std::shared_ptr<VertexArray>& VertexArray, uint32_t IndexCount)
{
    VertexArray->Bind();
    switch(Mode)
    {
    case RenderMode::Faces:
        glDrawElements(GL_TRIANGLES, IndexCount, GL_UNSIGNED_INT, nullptr);
        break;
    case RenderMode::Vertices:
        glDrawElements(GL_POINTS, IndexCount, GL_UNSIGNED_INT, nullptr);
        break;
    case RenderMode::Edges: // TODO(WT) 这里有问题
        glLineWidth(0);
        glDrawElements(GL_LINES, IndexCount, GL_UNSIGNED_INT, nullptr);
        break;            
    }
}

void Renderer::DrawIndexInstanced(const std::shared_ptr<VertexArray>& VertexArray, uint32_t IndexCount,
    uint32_t InstanceCount)
{
    VertexArray->Bind();
    switch(Mode)
    {
    case RenderMode::Faces:
        glDrawElementsInstanced(GL_TRIANGLES, IndexCount, GL_UNSIGNED_INT, nullptr, InstanceCount);
        break;
    case RenderMode::Vertices:
        glDrawElementsInstanced(GL_POINTS, IndexCount, GL_UNSIGNED_INT, nullptr, InstanceCount);
        break;
    case RenderMode::Edges:
        glDrawElementsInstanced(GL_LINES, IndexCount, GL_UNSIGNED_INT, nullptr, InstanceCount);
        break;
    }
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

MESH_EDITOR_NAMESPACE_END