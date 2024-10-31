#pragma once

#include "pch.h"
#include "Renderer/RenderContext/RenderContext.h"
#include "Renderer/Buffers/FrameBuffer.h"

class Camera;
class Shader;
class Scene;
class VertexArray;
class Window;

class Renderer
{
public:
    Renderer();
    ~Renderer();

    void Init();

    void SetViewport(uint32_t X, uint32_t Y, uint32_t Width, uint32_t Height);

    void SetClearColor(const glm::vec4& Color);
    void Clear();

    void DrawIndexed(const std::shared_ptr<VertexArray>& VertexArray, uint32_t IndexCount);
    void DrawLines(const std::shared_ptr<VertexArray>& VertexArray, uint32_t VertexCount);
    void SetLineWidth(float Width);

    void CompileShaders();

private:
    std::unique_ptr<FrameBuffer> FBO;
};
