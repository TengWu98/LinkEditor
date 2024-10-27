#pragma once

#include "pch.h"

class Camera;
class Shader;
class VertexArray;

struct SceneData
{
    glm::mat4 ViewProjectionMatrix;
};

class Renderer
{
public:
    void Render(std::shared_ptr<Shader>& Shader, std::shared_ptr<VertexArray>& VertexArray);

    static std::unique_ptr<SceneData> RendererSceneData;
};
