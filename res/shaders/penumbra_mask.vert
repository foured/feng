#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 7) in vec3 aOffset;
layout (location = 8) in vec3 aSize;

#include<lights_structures.glsl>
#include<defines.glsl>
#include<matrices_layout.glsl>
#include<lights_layout.glsl>

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

out vec4 FragPosLightSpace;

void main(){
	FragPosLightSpace = lightSpaceMatrix * model * vec4(aPos * aSize + aOffset, 1.0);
    gl_Position = projection * view * vec4(vec3(model * vec4(aPos * aSize + aOffset, 1.0)), 1.0);
}