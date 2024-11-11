#pragma once

#include "pch.h"
#include "Buffers/VertexBuffer.h"
#include "Renderer/RenderContext/RenderContext.h"
#include "Renderer/Buffers/FrameBuffer.h"
#include "Renderer/Buffers/UniformBuffer.h"

MESH_EDITOR_NAMESPACE_BEGIN

class Camera;
class Shader;
class Scene;
class VertexArray;
class Window;
class Model;

enum class ShaderPipelineType {
    Flat,
    VertexColor,
};

struct ShaderBindingDescriptor
{
    ShaderPipelineType PipelineType;
    std::string BindingName;
    std::optional<float> ScalarData = std::nullopt;
    std::optional<glm::vec4> VectorData = std::nullopt;
    std::optional<glm::mat4> MatrixData = std::nullopt;
};

struct ShaderBindingData
{
    // for flat shader
    glm::vec4 FlatColor = {1.0f, 1.0f, 1.0f, 1.0f};
};

struct RenderSpecification
{
    uint32_t Width = 1280;
    uint32_t Height = 720;
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
    void DrawIndexInstanced(const std::shared_ptr<VertexArray>& VertexArray, uint32_t IndexCount = 0, uint32_t InstanceCount = 0);
    
    void UpdateShaderData(std::vector<ShaderBindingDescriptor>&& Descriptors);

    void Render(const std::shared_ptr<VertexArray>& VertexArray, const std::shared_ptr<Model> ModelMatrix, std::optional<uint32_t> ModelIndex = 0);

    std::shared_ptr<FrameBuffer> GetFrameBuffer() const { return FBO; }

public:
    ShaderPipelineType CurrentShaderPipeline = ShaderPipelineType::Flat;
    ShaderBindingData ShaderData;

private:
    RenderSpecification Specification;
    
    std::shared_ptr<FrameBuffer> FBO;
    std::unordered_map<ShaderPipelineType, std::shared_ptr<Shader>> ShaderLibrary;
};

MESH_EDITOR_NAMESPACE_END
