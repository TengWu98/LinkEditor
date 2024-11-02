#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Color;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

layout(location = 0) out vec4 OutColor;

void main()
{
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
    OutColor = vec4(a_Color, 1.0);
}

#type fragment
#version 450

layout(location = 0) in vec4 OutColor;
layout(location = 0) out vec4 Color;

void main()d
{
    Color = vec4(1, 0, 0, 1);
}