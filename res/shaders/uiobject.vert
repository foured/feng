#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aOffset;
layout (location = 3) in vec2 aSize;

out vec2 TexCoords;

uniform mat4 projection;

void main(){
	TexCoords = aTexCoords;
    vec3 FragPos = aPos * vec3(aSize, 1.0) + aOffset;
    gl_Position = projection * vec4(FragPos, 1.0);
}