#pragma once

#include "pch.h"
#include "imgui.h"
#include "ImGuizmo.h"

LINK_EDITOR_NAMESPACE_BEGIN

class Camera;

class Gizmo
{
public:
    void Init();
    void Render(glm::mat4& ModelMatrix);
    void RenderDebug();

public:
    ImGuizmo::OPERATION ActiveOperation = ImGuizmo::TRANSLATE;
};

LINK_EDITOR_NAMESPACE_END