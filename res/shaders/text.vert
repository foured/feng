#version 330 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in float aTextureIdx;
layout (location = 3) in vec3 aColor; 

out vec2 TexCoords;
out float TextureIdx;
out vec3 Color;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(aPos, 1.0);
    TexCoords = aTexCoords;
    TextureIdx = aTextureIdx;
    Color = aColor;
}  