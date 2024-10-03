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

layout (std430, binding = 1) buffer Matrices
{
    mat4 projection;
    mat4 view;
};

layout (std430, binding = 2) buffer Lights
{
    DirLight dirLight;
    int noSpotLights;
    SpotLight spotLights[MAX_SPOT_LIGHTS];
    int noPointLights;
    PointLight pointLights[MAX_POINT_LIGHTS];
};

uniform bool useNormalMapping;

void main()
{
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));

    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;  
    vs_out.TexCoords = aTexCoords;
    vs_out.ViewPos = TBN * viewPos;

    vs_out.DirectionalLight = dirLight;
    vs_out.NoPointLights = noPointLights;
    vs_out.PointLights = pointLights;
    vs_out.NoSpotLights = noSpotLights;
    vs_out.SpotLights = spotLights;

    if(useNormalMapping){
        vs_out.FragPos = TBN * vs_out.FragPos;
        vs_out.ViewPos = TBN * viewPos;

        vs_out.DirectionalLight.direction = TBN * vec3(dirLight.direction);
        for(int i = 0; i < MAX_POINT_LIGHTS; i++){
            vs_out.PointLights[i].position = TBN * vec3(pointLights[i].position);
        }
        for(int i = 0; i < MAX_SPOT_LIGHTS; i++){
            vs_out.SpotLights[i].position = TBN * vec3(spotLights[i].position);
            vs_out.SpotLights[i].direction = TBN * vec3(spotLights[i].direction);
        }
    }

    gl_Position = projection * view * vec4(vec3(model * vec4(aPos + aOffset, 1.0)), 1.0);
}