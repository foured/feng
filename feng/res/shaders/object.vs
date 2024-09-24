#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aOffset;

#define MAX_POINT_LIGHTS 1
#define MAX_SPOT_LIGHTS 1

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

out VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 ViewPos;

    DirLight DirectionalLight;
    flat int NoPointLights;
	PointLight PointLights[MAX_POINT_LIGHTS];
	flat int NoSpotLights;
	SpotLight SpotLights[MAX_SPOT_LIGHTS];
} vs_out;

uniform mat4 model;
uniform vec3 viewPos;

layout (std430, binding = 3) readonly buffer Matrices
{
    mat4 projection;
    mat4 view;
};

layout (std140) uniform Lights
{
    DirLight directionalLight;
    int noPointLights;
	PointLight pointLights[MAX_POINT_LIGHTS];
	int noSpotLights;
	SpotLight spotLights[MAX_SPOT_LIGHTS];
};

layout (std430, binding = 2) readonly buffer DirLightBuffer
{
    DirLight dirLight;
};

void main()
{    
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;  
    vs_out.TexCoords = aTexCoords;
    vs_out.ViewPos = viewPos;

    vs_out.DirectionalLight = directionalLight;
    vs_out.NoPointLights = noPointLights;
    vs_out.PointLights = pointLights;
    vs_out.NoSpotLights = noSpotLights;
    vs_out.SpotLights = spotLights;

    gl_Position = projection * view * vec4(vec3(model * vec4(aPos + aOffset, 1.0)), 1.0);
}