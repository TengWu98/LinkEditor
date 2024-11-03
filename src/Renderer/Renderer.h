#pragma once

#include "pch.h"
#include "Buffers/VertexBuffer.h"
#include "Renderer/RenderContext/RenderContext.h"
#include "Renderer/Buffers/FrameBuffer.h"
#include "Renderer/Buffers/UniformBuffer.h"

class Camera;
class Shader;
class Scene;
class VertexArray;
class Window;

enum class ShaderPipelineType {
    Flat,
    VertexColor,
};

struct ShaderBindingDescriptor
{
    ShaderPipelineType PipelineType;
    std::string BindingName;
    std::optional<float> ScalarData;
    std::optional<glm::vec4> VectorData;
    std::optional<glm::mat4> MatrixData;
};

struct RenderSpecification
{
};

class Renderer
{
public:
    Renderer(RenderSpecification InSpecification = RenderSpecification());
    ~Renderer();

    void Init();

    void SetViewport(uint32_t X, uint32_t Y, uint32_t Width, uint32_t Height);

    void SetClearColor(const glm::vec4& Color);
    void Clear();
    
    void DrawLines(const std::shared_ptr<VertexArray>& VertexArray, uint32_t VertexCount);
    void SetLineWidth(float Width);
    void DrawIndexed(const std::shared_ptr<VertexArray>& VertexArray, uint32_t IndexCount = 0);
    void DrawIndexInstanced(const std::shared_ptr<VertexArray>& VertexArray, uint32_t IndexCount, uint32_t InstanceCount);

    void CompileShaders();
    void UpdateShaderData(std::vector<ShaderBindingDescriptor>&& Descriptors);

    void Render(const std::shared_ptr<VertexArray>& VertexArray, const std::shared_ptr<VertexBuffer> ModelMatrix, uint32_t IndexCount = 0);

public:
    inline static ShaderPipelineType CurrentShaderPipeline = ShaderPipelineType::Flat;

private:
    RenderSpecification Specification;
    
    std::unique_ptr<FrameBuffer> FBO;
    std::unordered_map<ShaderPipelineType, std::shared_ptr<Shader>> ShaderLibrary;
};
