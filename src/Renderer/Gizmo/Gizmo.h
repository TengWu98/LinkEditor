#pragma once

#include "pch.h"

#include "imgui.h"
#include "ImGuizmo.h"

class Camera;

class Gizmo
{
public:
    void Begin() const;
    void Render(Camera& Camera, bool& bIsViewChanged) const;
    void RenderDebug();

public:
    ImGuizmo::OPERATION ActiveOperation = ImGuizmo::TRANSLATE;
};
