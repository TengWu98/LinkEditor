#include "Input.h"

#include "GLFW/glfw3.h"

LINK_EDITOR_NAMESPACE_BEGIN

bool Input::IsKeyPressed(GLFWwindow* Window, KeyCode KeyCode)
{
    if(!Window)
    {
        return false;
    }

    auto State = glfwGetKey(Window, static_cast<int32_t>(KeyCode));
    return State == GLFW_PRESS;
}

bool Input::IsMouseButtonPressed(GLFWwindow* Window, MouseCode Button)
{
    if(!Window)
    {
        return false;
    }

    auto State = glfwGetMouseButton(Window, static_cast<int32_t>(Button));
    return State == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition(GLFWwindow* Window)
{
    if(!Window)
    {
        return glm::vec2();
    }

    double XPos, YPos;
    glfwGetCursorPos(Window, &XPos, &YPos);
    return {XPos, YPos};
}

float Input::GetMouseX(GLFWwindow* Window)
{
    return GetMousePosition(Window).x;
}

float Input::GetMouseY(GLFWwindow* Window)
{
    return GetMousePosition(Window).y;
}

LINK_EDITOR_NAMESPACE_END
