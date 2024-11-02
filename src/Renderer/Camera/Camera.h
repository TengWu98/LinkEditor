#pragma once

#include "pch.h"

enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera
{
public:
    Camera(glm::vec3 InPosition, glm::vec3 InUp, float InFieldOfView, float InNearClip, float InFarClip, float InYaw, float InPitch);

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix(float AspectRatio);
    glm::mat4 GetViewProjectionMatrix(float AspectRatio);
    glm::mat4 GetInvViewProjectionMatrix(float AspectRatio);

    void ProcessKeyboard(CameraMovement Direction, float DeltaTime);
    void ProcessMouseMovement(float XOffset, float YOffset, bool ConstrainPitch = true);
    void ProcessMouseScroll( float YOffset);
private:
    void UpdateCameraVectors();
    
public:
    // Camera Attributes
    glm::vec3 Front = glm::vec3(0.f, 0.f, -1.f);
    glm::vec3 Position;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float FieldOfView;
    float NearClip;
    float FarClip;

    // Euler Angles
    float Yaw;
    float Pitch;

    // Camera Options
    float MovementSpeed = 10.f;
    float MouseSensitivity = 0.1f;
    float ZoomLevel = 45.f;
};
