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
    Camera(glm::vec3 InPosition, glm::vec3 InWorldUp, glm::vec3 InTarget, float InFieldOfView, float InNearClip, float InFarClip);
    Camera(glm::vec3 InPosition, glm::vec3 InWorldUp, float InYaw, float InPitch, float InFieldOfView, float InNearClip, float InFarClip);

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix(float AspectRatio);
    glm::mat4 GetViewProjectionMatrix(float AspectRatio);
    glm::mat4 GetInvViewProjectionMatrix(float AspectRatio);
    float GetDistance() const;
    void SetPositionFromView(const glm::mat4& ViewMatrix);
    void SetTargetDistance(float Distance);
    void SetDistance(float Distance);
    void Tick();

public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 WorldUp;
    glm::vec3 Up;
    glm::vec3 Front;
    glm::vec3 Right;

    float Yaw;
    float Pitch;

    float FieldOfView;
    float NearClip;
    float FarClip;
    
    // Camera Options
    float MovementSpeed = 10.f;
    float MouseSensitivity = 0.1f;
    float ZoomLevel = 45.f;

private:
    float TargetDistance;
    bool bIsMoving = false;
};
