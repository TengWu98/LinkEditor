#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_WorldNormal;
layout(location = 2) in vec4 a_Color;

uniform mat4 u_ModelMatrix;
layout(binding = 0) uniform ViewProjectionUBO {
    mat4 ViewMatrix;
    mat4 ProjMatrix;
} ViewProj;

layout(location = 0) out vec4 OutColor;

void main()
{
    gl_Position = ViewProj.ProjMatrix * ViewProj.ViewMatrix * u_ModelMatrix * vec4(a_Position, 1.0);
    OutColor = a_Color;
}

#type fragment
#version 450

layout(location = 0) in vec4 OutColor;
layout(location = 0) out vec4 Color;

void main()
{
    Color = OutColor;
}