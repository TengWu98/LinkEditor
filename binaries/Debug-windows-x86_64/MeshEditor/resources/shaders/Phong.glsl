#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_WorldNormal;

uniform mat4 u_ModelMatrix;
layout(binding = 0) uniform ViewProjectionUBO {
    mat4 ViewMatrix;
    mat4 ProjMatrix;
} ViewProj;

layout(location = 0) out vec4 VertexPosition;
layout(location = 1) out vec3 VertexNormal;

void main()
{
    VertexPosition = ViewProj.ProjMatrix * ViewProj.ViewMatrix * u_ModelMatrix * vec4(a_Position, 1.0);
    VertexNormal = normalize(gl_NormalMatrix * a_WorldNormal);
    gl_Position = VertexPosition;
}

#type fragment
#version 450

layout(location = 0) in vec4 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

uniform float u_Shininess;
layout(binding = 2) uniform LightsUBO {
    vec4 ViewColorAndAmbient;
    vec4 DirectionalColorAndIntensity;
    vec3 Direction;
} Lights;

layout(location = 0) out vec4 Color;

void main()
{
    vec3 Normal = normalize(VertexNormal);
    vec4 Diffuse = vec4(0.0);
    vec4 Specular = vec4(0.0);

    vec4 MaterialAmbient = vec4(1.0);
    vec4 MaterialDiffuse = vec4(1.0);
    vec4 MaterialSpecular = vec4(1.0);

    vec4 Ambient = MaterialAmbient * Lights.ViewColorAndAmbient.w;
    vec4 KD = MaterialDiffuse * gl_LightSource[0].diffuse;
    vec4 KS = MaterialSpecular * gl_LightSource[0].specular;

    vec3 LightDir = normalize(vec3(gl_LightSource[0].position.xyz - VertexPosition.xyz));
    float NDotL = max(dot(Normal, LightDir), 0.0);

    if (NDotL > 0.0)
    {
        Diffuse = KD * NDotL;
    }

    vec3 RVector = normalize(2.0 * Normal * NDotL - LightDir);
    vec3 VVector = normalize(-VertexPosition.xyz);
    float RDotV = max(dot(RVector, VVector), 0.0);

    if (RDotV > 0.0)
    {
        Specular = KS * pow(RDotV, u_Shininess);
    }

    Color = Ambient + Diffuse + Specular;
}