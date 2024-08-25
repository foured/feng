#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aOffset;

out VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 ViewPos;
} vs_out;

uniform mat4 model;
uniform vec3 viewPos;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{    
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;  
    vs_out.TexCoords = aTexCoords;
    vs_out.ViewPos = viewPos;

    gl_Position = projection * view * vec4(vec3(model * vec4(aPos + aOffset, 1.0)), 1.0);
}