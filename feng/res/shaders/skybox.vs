#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout (std430, binding = 1) buffer Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 sb_view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * sb_view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
} 