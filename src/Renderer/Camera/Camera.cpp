#include "Camera.h"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

Camera::Camera(glm::vec3 InPosition, glm::vec3 InUp, float InFieldOfView, float InNearClip, float InFarClip, float InYaw, float InPitch) :
    Position(InPosition), WorldUp(InUp),
    FieldOfView(InFieldOfView), NearClip(InNearClip), FarClip(InFarClip),
    Yaw(InYaw), Pitch(InPitch)
{
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
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

void Camera::ProcessKeyboard(CameraMovement Direction, float DeltaTime)
{
    float Velocity = MovementSpeed * DeltaTime;
    if (Direction == FORWARD)
        Position += Front * Velocity;
    if(Direction == BACKWARD)
        Position -= Front * Velocity;
    if (Direction == LEFT)
        Position -= Right * Velocity;
    if (Direction == RIGHT)
        Position += Right * Velocity;
    if (Direction == UP)
        Position += WorldUp * Velocity;
    if (Direction == DOWN)
        Position -= WorldUp * Velocity;
}

void Camera::ProcessMouseMovement(float XOffset, float YOffset, bool ConstrainPitch)
{
    XOffset *= MouseSensitivity;
    YOffset *= MouseSensitivity;

    Yaw += XOffset;
    Pitch += YOffset;

    if(ConstrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 NewFront;
    NewFront.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    NewFront.y = sin(glm::radians(Pitch));
    NewFront.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    
    Front = glm::normalize(NewFront);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
