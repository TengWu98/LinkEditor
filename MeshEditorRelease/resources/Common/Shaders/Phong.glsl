#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_WorldNormal;

uniform mat4 u_ModelMatrix;
layout(binding = 0) uniform ViewProjectionUBO {
    mat4 ViewMatrix;
    mat4 ProjMatrix;
} ViewProj;

layout(location = 0) out vec4 WorldPosition;
layout(location = 1) out vec3 WorldNormal;

void main()
{
    WorldPosition = ViewProj.ProjMatrix * ViewProj.ViewMatrix * u_ModelMatrix * vec4(a_Position, 1.0);
    WorldNormal = normalize(mat3(u_ModelMatrix) * a_WorldNormal);
    gl_Position = WorldPosition;
}

#type fragment
#version 450

layout(location = 0) in vec4 WorldPosition;
layout(location = 1) in vec3 WorldNormal;

uniform vec4 u_DiffuseColor;
uniform vec4 u_SpecularColor;
uniform float u_Gloss;
layout(binding = 2) uniform LightShaderDataUBO {
    vec4 LightColorAndAmbient;
    vec4 LightDirAndIntensity;
} LightShaderData;

layout(location = 0) out vec4 Color;

void main()
{
    vec3 Normal = normalize(WorldNormal);
    vec3 LightDir = normalize(LightShaderData.LightDirAndIntensity.xyz);
    float NDotL = max(0.0, dot(LightDir, Normal));

    // Ambient
    vec3 MaterialAmbient = vec3(1.0);
    vec3 Ambient = MaterialAmbient * LightShaderData.LightColorAndAmbient.w;

    // Diffuse
    vec3 Diffuse = LightShaderData.LightColorAndAmbient.rgb * u_DiffuseColor.rgb * NDotL;
    
    // Specular
    vec3 ReflectDir = normalize(reflect(-LightDir, Normal));
    vec3 ViewDir = normalize(-WorldPosition.xyz);
    float RDotV = max(0, dot(ReflectDir, ViewDir));
    vec3 Specular = LightShaderData.LightColorAndAmbient.rgb * u_SpecularColor.rgb * pow(RDotV, u_Gloss);

    // Final color
    vec3 FinalColor = Ambient + Diffuse + Specular;
    Color = vec4(FinalColor, 1.0);
}