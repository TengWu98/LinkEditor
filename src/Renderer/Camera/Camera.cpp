#include "Camera.h"
#include "Renderer/Ray/Ray.h"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

MESH_EDITOR_NAMESPACE_BEGIN

Camera::Camera(glm::vec3 InPosition, glm::vec3 InWorldUp, glm::vec3 InTarget, float InNearClip, float InFarClip) :
    Position(InPosition), WorldUp(InWorldUp), Front(glm::normalize(InTarget - Position)),
    NearClip(InNearClip), FarClip(InFarClip)
{
    FieldOfView = 90.f;

    OrthoWidth = 1536.0f;
    
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, WorldUp);
}

glm::mat4 Camera::GetProjectionMatrix()
{
    if(ProjectionMode == CameraProjectionMode::Orthographic)
    {
        return glm::ortho(-OrthoWidth * AspectRatio, OrthoWidth * AspectRatio, -OrthoWidth, OrthoWidth, NearClip, FarClip);
    }
    
    return glm::perspective(glm::radians(FieldOfView), AspectRatio, NearClip, FarClip);
}

glm::mat4 Camera::GetViewProjectionMatrix()
{
    return GetProjectionMatrix() * GetViewMatrix();
}

glm::mat4 Camera::GetInvViewProjectionMatrix()
{
    return glm::inverse(GetViewProjectionMatrix());
}

float Camera::GetCurrentDistance() const
{
    return glm::distance(Position, Position + Front);
}

void Camera::SetPositionFromView(const glm::mat4& ViewMatrix)
{
    Position = glm::inverse(ViewMatrix)[3];
    bIsMoving = false;
}

void Camera::SetFieldOfView(float InFieldOfView)
{
    FieldOfView = InFieldOfView;
}

void Camera::SetAspectRatio(float InAspectRatio)
{
    AspectRatio = InAspectRatio;
}

void Camera::SetTargetDistance(float InTargetDistance)
{
    TargetDistance = InTargetDistance;
    bIsMoving = true;
}

void Camera::Update()
{
    if(!bIsMoving)
    {
        return;
    }

    const auto CurrentDistance = GetCurrentDistance();
    if(std::abs(CurrentDistance - TargetDistance) < 0.01f)
    {
        bIsMoving = false;
        SetDistance(TargetDistance);
    }
    else
    {
        SetDistance(glm::mix(CurrentDistance, TargetDistance, CameraSpeed));
    }
}

Ray Camera::ClipPosToWorldRay(const glm::vec2& NDCPos)
{
    const auto InverseViewProjection = glm::inverse(GetViewProjectionMatrix());

    // Perspective divide.
    glm::vec4 NearPoint = InverseViewProjection * glm::vec4(NDCPos.x, NDCPos.y, -1.0f, 1.0f);
    NearPoint /= NearPoint.w;
    glm::vec4 FarPoint = InverseViewProjection * glm::vec4(NDCPos.x, NDCPos.y, 1.0f, 1.0f);
    FarPoint /= FarPoint.w;

    return Ray(glm::vec3(NearPoint), glm::normalize(glm::vec3(FarPoint - NearPoint)));
}

void Camera::SetDistance(float Distance)
{
    Position += Front * (1 - Distance);
}

MESH_EDITOR_NAMESPACE_END
