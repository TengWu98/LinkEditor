#type vertex
#version 450

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ModelMatrix;
layout(binding = 0) uniform ViewProjectionUBO {
    mat4 ViewMatrix;
    mat4 ProjMatrix;
} ViewProj;

layout(location = 0) out vec4 VertexPosition;

void main()
{
    VertexPosition = ViewProj.ProjMatrix * ViewProj.ViewMatrix * u_ModelMatrix * vec4(a_Position, 1.0);
    gl_Position = VertexPosition;
}

#type fragment
#version 450

layout(location = 0) in vec4 VertexPosition;

uniform float u_Near;
uniform float u_Far;

layout(location = 0) out vec4 Color;

void main()
{
    float Gray = (VertexPosition.z - u_Near) / (u_Far - u_Near);
    Color = vec4(vec3(Gray), 1.0);
}