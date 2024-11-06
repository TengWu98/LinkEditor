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
    
    float GetCurrentDistance() const;
    void SetTargetDistance(float InTargetDistance);
    void SetPositionFromView(const glm::mat4& ViewMatrix);
    
    void SetDistance(float Distance);
    void Update();

public:
    // Camera Attributes
    glm::vec3 Position; // position of the camera
    glm::vec3 WorldUp;  // world up vector
    glm::vec3 Up;       // up vector
    glm::vec3 Front;    // front vector
    glm::vec3 Right;    // right vector

    float Yaw;
    float Pitch;

    float FieldOfView;   // field of view
    float NearClip;      // near clip plane
    float FarClip;       // far clip plane
    
    // Camera Options
    float MovementSpeed = 10.f;     // how fast the camera moves
    float MouseSensitivity = 0.1f;  // how fast the camera turns
    float ZoomLevel = 45.f;         // how zoomed in the camera is

private:
    float TargetDistance;   // target distance the camera is moving towards
    bool bIsMoving = false; // whether the camera is moving
};
