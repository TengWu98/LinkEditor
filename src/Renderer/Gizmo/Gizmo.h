#pragma once

#include "pch.h"
#include "imgui.h"
#include "ImGuizmo.h"

MESH_EDITOR_NAMESPACE_BEGIN

class Camera;

class Gizmo
{
public:
    void Begin();
    void Render(Camera& Camera, glm::mat4& ModelMatrix, float AspectRation, bool& bIsViewChanged, bool& bIsModelChanged);
    void RenderDebug();

public:
    ImGuizmo::OPERATION ActiveOperation = ImGuizmo::TRANSLATE;
    bool bIsShowModelGizmo = false;
};

MESH_EDITOR_NAMESPACE_END