#pragma once

#include "pch.h"

class Camera;
class Shader;
class Scene;
class VertexArray;
class Window;

class Renderer
{
public:
    static void Render(Scene* Scene, Window* Window/* , std::shared_ptr<Shader>& Shader, std::shared_ptr<VertexArray>& VertexArray */);
};
