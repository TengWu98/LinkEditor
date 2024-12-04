#pragma once
#include "pch.h"

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include "imgui.h"
#include "imgui_internal.h"

LINK_EDITOR_NAMESPACE_BEGIN

// OpenMesh
inline glm::vec3 ToGlm(const OpenMesh::Vec3f& InVec3) { return {InVec3[0], InVec3[1], InVec3[2]}; }
inline glm::vec4 ToGlm(const OpenMesh::Vec3uc& Color)
{
    const auto cc = OpenMesh::color_cast<OpenMesh::Vec3f>(Color);
    return {cc[0], cc[1], cc[2], 1};
}

// ImGui
inline glm::vec2 ToGlm(const ImVec2& InVec2) { return {InVec2.x, InVec2.y}; }


inline void DecomposeTransform(const glm::mat4 &transform, glm::vec3 &position, glm::vec3 &rotation, glm::vec3 &scale) {
    static glm::vec3 skew;
    static glm::vec4 perspective;
    static glm::quat orientation;
    glm::decompose(transform, scale, orientation, position, skew, perspective);
    rotation = glm::eulerAngles(orientation) * 180.f / glm::pi<float>(); // Convert radians to degrees
}

inline glm::mat4 ComposeTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
{
    glm::quat rotationQuat = glm::quat(glm::radians(rotation));
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
                          glm::mat4_cast(rotationQuat) *
                          glm::scale(glm::mat4(1.0f), scale);
    return transform;
}

LINK_EDITOR_NAMESPACE_END
