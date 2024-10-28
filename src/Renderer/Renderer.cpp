#include "Renderer.h"
#include "Scene/Scene.h"
#include "Core/Window/Window.h"

void Renderer::Render(Scene* Scene, Window* Window/* , std::shared_ptr<Shader>& Shader, std::shared_ptr<VertexArray>& VertexArray */)
{
    int display_w, display_h;
    glfwGetFramebufferSize(Window->GetNativeWindow(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    
    glClearColor(Scene->BackgroundColor.x * Scene->BackgroundColor.w,
            Scene->BackgroundColor.y * Scene->BackgroundColor.w,
            Scene->BackgroundColor.z * Scene->BackgroundColor.w,
            Scene->BackgroundColor.w);
    glClear(GL_COLOR_BUFFER_BIT);

    glClear(GL_COLOR_BUFFER_BIT);
}
