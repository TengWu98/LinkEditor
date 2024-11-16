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

enum class RenderMode : uint8_t
{
    None,
    Face = 1 << 0,
    Points = 1 << 1,
    Wireframe = 1 << 2,
};

constexpr RenderMode operator|(RenderMode A, RenderMode B)
{
    return static_cast<RenderMode>(static_cast<uint8_t>(A) | static_cast<uint8_t>(B));
}

constexpr RenderMode operator&(RenderMode A, RenderMode B)
{
    return static_cast<RenderMode>(static_cast<uint8_t>(A) & static_cast<uint8_t>(B));
}

constexpr RenderMode operator^(RenderMode A, RenderMode B)
{
    return static_cast<RenderMode>(static_cast<uint8_t>(A) ^ static_cast<uint8_t>(B));
}

constexpr RenderMode operator~(RenderMode A)
{
    return static_cast<RenderMode>(~static_cast<uint8_t>(A));
}

constexpr RenderMode& operator|=(RenderMode& A, RenderMode B)
{
    A = A | B;
    return A;
}

constexpr RenderMode& operator&=(RenderMode& A, RenderMode B)
{
    A = A & B;
    return A;
}

constexpr RenderMode& operator^=(RenderMode& A, RenderMode B)
{
    A = A ^ B;
    return A;
}

enum class ShaderPipelineType {
    Phong,
    Depth,
    EnvMap,
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
    // for phong
    glm::vec4 Phong_Diffuse = {1.f, 1.f, 1.f, 1.f};
    glm::vec4 Phong_Specular = {1.f, 1.f, 1.f, 1.f};
    float Phong_Gloss = 5.f;

    // for depth
    float Depth_NearPlane = 0.1f;
    float Depth_FarPlane = 100.0f;
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
    
    void UpdateShaderData(std::vector<ShaderBindingDescriptor>&& Descriptors);
    void DrawIndexInstanced(const std::shared_ptr<VertexArray>& VertexArray, uint32_t IndexCount = 0, uint32_t InstanceCount = 0);
    void Render(const std::shared_ptr<VertexArray>& VertexArray, const std::shared_ptr<Model> ModelMatrix, std::optional<uint32_t> ModelIndex = 0);

    std::shared_ptr<FrameBuffer> GetFrameBuffer() const { return FBO; }

public:
    ShaderPipelineType CurrentShaderPipeline = ShaderPipelineType::Phong;
    ShaderBindingData ShaderData;
    RenderMode Mode = RenderMode::Face;

    float PointSize = 1.0f;
    float LineWidth = 1.0f;

private:
    RenderSpecification Specification;
    std::shared_ptr<FrameBuffer> FBO;
    std::unordered_map<ShaderPipelineType, std::shared_ptr<Shader>> ShaderLibrary;
};

MESH_EDITOR_NAMESPACE_END
