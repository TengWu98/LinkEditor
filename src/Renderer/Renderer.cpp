#include "Renderer.h"
#include "Scene/Scene.h"
#include "Core/Window/Window.h"
#include "Renderer/Buffers/VertexArray.h"
#include "Renderer/Buffers/IndexBuffer.h"

Renderer::Renderer()
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

    FBO = std::make_unique<FrameBuffer>(FramebufferSpecification());
    FBO->Bind();
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

// void Renderer::Render(Scene* Scene, Window* Window/* , std::shared_ptr<Shader>& Shader, std::shared_ptr<VertexArray>& VertexArray */)
// {
//     int display_w, display_h;
//     glfwGetFramebufferSize(Window->GetNativeWindow(), &display_w, &display_h);
//     glViewport(0, 0, display_w, display_h);
//     
//     glClearColor(Scene->BackgroundColor.x * Scene->BackgroundColor.w,
//             Scene->BackgroundColor.y * Scene->BackgroundColor.w,
//             Scene->BackgroundColor.z * Scene->BackgroundColor.w,
//             Scene->BackgroundColor.w);
//     glClear(GL_COLOR_BUFFER_BIT);
//
//     glClear(GL_COLOR_BUFFER_BIT);
// }
