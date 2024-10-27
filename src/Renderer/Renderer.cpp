#include "Renderer.h"

std::unique_ptr<SceneData> Renderer::RendererSceneData = std::make_unique<SceneData>();

void Renderer::Render(std::shared_ptr<Shader>& Shader, std::shared_ptr<VertexArray>& VertexArray)
{
}
