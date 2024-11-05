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
    Camera(glm::vec3 InPosition, glm::vec3 InUp, glm::vec3 InTarget, float InFieldOfView, float InNearClip, float InFarClip);

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix(float AspectRatio);
    glm::mat4 GetViewProjectionMatrix(float AspectRatio);
    glm::mat4 GetInvViewProjectionMatrix(float AspectRatio);
    
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Up;
    glm::vec3 Target;

    float FieldOfView;
    float NearClip;
    float FarClip;

    // Camera Options
    float MovementSpeed = 10.f;
    float MouseSensitivity = 0.1f;
    float ZoomLevel = 45.f;
};
