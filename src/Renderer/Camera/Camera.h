#pragma once

#include "pch.h"

MESH_EDITOR_NAMESPACE_BEGIN

struct Ray;

enum CameraProjectionMode
{
    Perspective,
    Orthographic
};

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
    Camera(glm::vec3 InPosition, glm::vec3 InWorldUp, glm::vec3 InTarget, float InNearClip, float InFarClip);

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
    glm::mat4 GetViewProjectionMatrix();
    glm::mat4 GetInvViewProjectionMatrix();
    
    float GetCurrentDistance() const;
    void SetTargetDistance(float InTargetDistance);
    void SetPositionFromView(const glm::mat4& ViewMatrix);

    void SetFieldOfView(float InFieldOfView);
    void SetAspectRatio(float InAspectRatio);
    
    void SetDistance(float Distance);
    void Update();

    Ray ClipPosToWorldRay(const glm::vec2& ClipPos);

public:
    CameraProjectionMode ProjectionMode = CameraProjectionMode::Perspective;
    
    // Camera Attributes
    glm::vec3 Position; // position of the camera
    glm::vec3 WorldUp;  // world up vector
    glm::vec3 Up;       // up vector
    glm::vec3 Front;    // front vector
    glm::vec3 Right;    // right vector

    float AspectRatio;   // aspect ratio
    float FieldOfView;   // field of view
    float NearClip;      // near clip plane
    float FarClip;       // far clip plane

    float OrthoWidth;
    
    // Camera Options
    float CameraSpeed = 10.f;               // how fast the camera moves
    float MouseScrollCameraSpeed = 5,f;             // how fast the camera zooms
    float MouseSensitivity = 0.1f;          // how fast the camera turns
    float ZoomLevel = 45.f;                 // how zoomed in the camera is

private:
    float TargetDistance;   // target distance the camera is moving towards
    bool bIsMoving = false; // whether the camera is moving
};

MESH_EDITOR_NAMESPACE_END
