#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 7) in vec3 aOffset;
layout (location = 8) in vec3 aSize;

uniform mat4 model;

void main()
{
    gl_Position = vec4(aPos * aSize + aOffset, 1.0);
}  