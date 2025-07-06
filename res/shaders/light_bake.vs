#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

layout (location = 7) in vec3 aOffset;
layout (location = 8) in vec3 aSize;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

out VS_OUT{
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

void main() {
    vec4 worldPos = model * vec4(aPos * aSize + aOffset, 1.0);

    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosLightSpace = lightSpaceMatrix * worldPos;

    vec2 pos = aTexCoords * 2 - vec2(1.0);

    gl_Position = vec4(pos, 0.0, 1.0);
}