#include "Camera.h"

#include "glm/ext/matrix_transform.hpp"

Camera::Camera(glm::vec3 InPosition, glm::vec3 InUp, float InYaw, float InPitch)
    : Position(InPosition), WorldUp(InUp), Yaw(InYaw), Pitch(InPitch), Front(glm::vec3(0.f, 0.f, -1.f)), MovementSpeed(10.f), MouseSensiticity(0.1f), Zoom(45.f)
{
    UpdateCameraVectors();
}

Camera::Camera(float PosX, float PosY, float PosZ, float UpX, float UpY, float UpZ, float InYaw, float InPitch)
    : Position(glm::vec3(PosX, PosY, PosZ)), WorldUp(glm::vec3(UpX, UpY, UpZ)), Yaw(InYaw), Pitch(InPitch), Front(glm::vec3(0.f, 0.f, -1.f)), MovementSpeed(10.f), MouseSensiticity(0.1f), Zoom(45.f)
{
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
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
    XOffset *= MouseSensiticity;
    YOffset *= MouseSensiticity;

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

void Camera::ProcessMouseScroll(float YOffset)
{
    Zoom -= static_cast<float>(YOffset);
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
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
