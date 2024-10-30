#include "RenderContext.h"

RenderContext::RenderContext(GLFWwindow* InWindowHandle)
    : WindowHandle(InWindowHandle)
{
    MESH_EDITOR_CORE_ASSERT(WindowHandle, "Window handle is null!")
}

void RenderContext::Init()
{
    glfwMakeContextCurrent(WindowHandle);
    glfwSwapInterval(1); // Enable vsync

    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!status)
    {
        LOG_ERROR("Failed to initialize GLAD \n");
        return;
    }

    LOG_INFO("OpenGL Info:");
    LOG_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
    LOG_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
    LOG_INFO("  Version: {0}", glGetString(GL_VERSION));

    MESH_EDITOR_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "MeshEditor requires at least OpenGL version 4.5!");
}

void RenderContext::SwapBuffers()
{
    glfwSwapBuffers(WindowHandle);
}
