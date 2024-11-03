#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_WorldNormal;
layout(location = 2) in vec3 a_Color;
layout(location = 3) in mat4 ModelMatrix;

layout(binding = 0) uniform ViewProjectionUBO {
    mat4 ViewMatrix;
    mat4 ProjMatrix;
} ViewProj;

void main()
{ 
    // ViewProj.ProjMatrix * ViewProj.ViewMatrix * ModelMatrix *
    gl_Position = ViewProj.ProjMatrix * ViewProj.ViewMatrix * ModelMatrix * vec4(a_Position, 1.0);
}

#type fragment
#version 450

uniform vec4 u_Color;

layout(location = 0) out vec4 Color;

void main()
{
    Color = vec4(1, 0, 0, 1);
}