#pragma once
#include "pch.h"

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

#include "imgui.h"
#include "imgui_internal.h"

MESH_EDITOR_NAMESPACE_BEGIN

// OpenMesh
inline glm::vec3 ToGlm(const OpenMesh::Vec3f& InVec3) { return {InVec3[0], InVec3[1], InVec3[2]}; }
inline glm::vec4 ToGlm(const OpenMesh::Vec3uc& Color)
{
    const auto cc = OpenMesh::color_cast<OpenMesh::Vec3f>(Color);
    return {cc[0], cc[1], cc[2], 1};
}

// ImGui
inline glm::vec2 ToGlm(const ImVec2& InVec2) { return {InVec2.x, InVec2.y}; }

MESH_EDITOR_NAMESPACE_END
