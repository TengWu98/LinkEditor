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
    Camera(glm::vec3 InPosition = glm::vec3(0.f, 0.f, 0.f), glm::vec3 InUp = glm::vec3(0.f, 0.f, 0.f), float InYaw = -90.f, float InPitch = 0.f);
    Camera(float PosX, float PosY, float PosZ, float UpX, float UpY, float UpZ, float InYaw, float InPitch);

    glm::mat4 GetViewMatrix();

    void ProcessKeyboard(CameraMovement Direction, float DeltaTime);
    void ProcessMouseMovement(float XOffset, float YOffset, bool ConstrainPitch = true);
    void ProcessMouseScroll( float YOffset);
private:
    void UpdateCameraVectors();
    
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Euler Angles
    float Yaw;
    float Pitch;

    // Camera Options
    float MovementSpeed;
    float MouseSensiticity;
    float Zoom;
};
