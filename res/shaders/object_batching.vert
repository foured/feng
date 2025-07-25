#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec4 aDiffuse;
layout (location = 5) in vec4 aSpecular;
layout (location = 6) in float aTexIdxs;
// dynamic
layout (location = 7) in vec3 aOffset;
layout (location = 8) in vec3 aSize;
// rotation
// 4 vec4s to store a 4x4 rotation matrix
layout (location = 9) in vec4 aRotation_1;
layout (location = 10) in vec4 aRotation_2;
layout (location = 11) in vec4 aRotation_3;
layout (location = 12) in vec4 aRotation_4;



#include<defines.glsl>
#include<lights_structures.glsl>
#include<lights_layout.glsl>
#include<matrices_layout.glsl>

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

    flat int d_map_idx;
    flat int s_map_idx;
    flat int n_map_idx;
    vec4 DiffuseCol;
    vec4 SpecCol;

    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 model;
uniform vec3 viewPos;
uniform mat4 lightSpaceMatrix;

void main()
{
    mat4 rotation = mat4(aRotation_1, aRotation_2, aRotation_3, aRotation_4);
    vec4 realPos = vec4(vec3(rotation * vec4(aPos * aSize, 1.0)) + aOffset, 1.0);

    vs_out.DiffuseCol = aDiffuse;
    vs_out.SpecCol = aSpecular;
    vs_out.d_map_idx = int(aTexIdxs) & 0xFF;
    vs_out.s_map_idx = (int(aTexIdxs) >> 8) & 0xFF;
    vs_out.n_map_idx = (int(aTexIdxs) >> 16) & 0xFF;

    vs_out.FragPos = vec3(model * realPos);
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;  
    vs_out.TexCoords = aTexCoords;
    //vs_out.ViewPos = vec3(model * vec4(viewPos, 1.0));
    vs_out.ViewPos = viewPos;

    vs_out.DirectionalLight = dirLight;
    vs_out.NoPointLights = noPointLights;
    vs_out.PointLights = pointLights;
    vs_out.NoSpotLights = noSpotLights;
    vs_out.SpotLights = spotLights;

    vs_out.FragPosLightSpace = lightSpaceMatrix * realPos;

    for(int i = 0; i < MAX_POINT_LIGHTS; i++){
        vs_out.PointLights[i].position = vec3(model * vec4(pointLights[i].position, 1.0));
    }

    if(vs_out.n_map_idx != NULL_TEXTURE_IDX){
        mat3 normalMatrix = transpose(inverse(mat3(model)));
        vec3 T = normalize(normalMatrix * aTangent);
        vec3 N = normalize(normalMatrix * aNormal);
        T = normalize(T - dot(T, N) * N);
        vec3 B = cross(N, T);
        mat3 TBN = transpose(mat3(T, B, N));

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

    gl_Position = projection * view * model * realPos;
}