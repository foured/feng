#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

#include<matrices_layout.glsl>

uniform mat4 sb_view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * sb_view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
} 