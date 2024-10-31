#pragma once

#include "pch.h"
#include "Renderer/RenderContext/RenderContext.h"
#include "Renderer/Buffers/FrameBuffer.h"

class Camera;
class Shader;
class Scene;
class VertexArray;
class Window;

struct RenderSpecification
{
    uint32_t ViewportWidth = 0;
    uint32_t ViewportHeight = 0;
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

    void DrawIndexed(const std::shared_ptr<VertexArray>& VertexArray, uint32_t IndexCount);
    void DrawLines(const std::shared_ptr<VertexArray>& VertexArray, uint32_t VertexCount);
    void SetLineWidth(float Width);

    void CompileShaders();

    uint32_t GetViewportWidth() const { return ViewportWidth; }
    uint32_t GetViewportHeight() const { return ViewportHeight; }

private:
    RenderSpecification Specification;
    
    std::unique_ptr<FrameBuffer> FBO;
    std::unordered_map<std::string, std::shared_ptr<Shader>> Shaders;

    uint32_t ViewportWidth = 0;
    uint32_t ViewportHeight = 0;
};
