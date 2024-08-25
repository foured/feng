#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aDiffuse;
layout (location = 4) in vec4 aSpecular;
layout (location = 5) in float aTexIdxs;
// dynamic
layout (location = 6) in vec3 aOffset;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 DiffuseCol;
out vec4 SpecCol;
flat out int d_map_idx;
flat out int s_map_idx;

uniform mat4 model;
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
    DiffuseCol = aDiffuse;
    SpecCol = aSpecular;

    d_map_idx = int(aTexIdxs) & 0xFF;
    s_map_idx = int(aTexIdxs) >> 8;

    FragPos = vec3(model * vec4(aPos + aOffset, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}