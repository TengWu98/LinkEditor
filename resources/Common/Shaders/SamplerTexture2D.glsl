#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 3) in vec2 a_TexCoord;

uniform mat4 u_ModelMatrix;
layout(binding = 0) uniform ViewProjectionUBO {
    mat4 ViewMatrix;
    mat4 ProjMatrix;
} ViewProj;

layout(location = 0) out vec2 VertexTexCoord;

void main()
{ 
    gl_Position = ViewProj.ProjMatrix * ViewProj.ViewMatrix * u_ModelMatrix * vec4(a_Position, 1.0);
    VertexTexCoord = a_TexCoord;
}

#type fragment
#version 450

layout(location = 0) in vec2 VertexTexCoord;
layout(binding = 1) uniform sampler2D u_Texture;

layout(location = 0) out vec4 Color;

void main()
{
    Color = texture(u_Texture, VertexTexCoord);;
}