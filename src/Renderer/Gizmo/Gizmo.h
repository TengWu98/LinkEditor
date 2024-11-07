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
    void Render(Camera& Camera, bool& bIsViewChanged);
    void RenderDebug();

public:
    ImGuizmo::OPERATION ActiveOperation = ImGuizmo::TRANSLATE;
};

MESH_EDITOR_NAMESPACE_END