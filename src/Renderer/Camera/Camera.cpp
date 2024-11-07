#include "Camera.h"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

MESH_EDITOR_NAMESPACE_BEGIN

Camera::Camera(glm::vec3 InPosition, glm::vec3 InWorldUp, glm::vec3 InTarget, float InFieldOfView, float InNearClip, float InFarClip) :
    Position(InPosition), WorldUp(InWorldUp), Front(glm::normalize(InTarget - Position)),
    FieldOfView(InFieldOfView), NearClip(InNearClip), FarClip(InFarClip)
{
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, WorldUp);
}

glm::mat4 Camera::GetProjectionMatrix(float AspectRatio)
{
    return glm::perspective(glm::radians(FieldOfView), AspectRatio, NearClip, FarClip);
}

glm::mat4 Camera::GetViewProjectionMatrix(float AspectRatio)
{
    return GetProjectionMatrix(AspectRatio) * GetViewMatrix();
}

glm::mat4 Camera::GetInvViewProjectionMatrix(float AspectRatio)
{
    return glm::inverse(GetViewProjectionMatrix(AspectRatio));
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
        SetDistance(glm::mix(CurrentDistance, TargetDistance, MovementSpeed));
    }
}

void Camera::SetDistance(float Distance)
{
    Position += Front * (1 - Distance);
}

MESH_EDITOR_NAMESPACE_END
