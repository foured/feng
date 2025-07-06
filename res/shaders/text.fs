#version 330 core
in vec2 TexCoords;
in float TextureIdx;
in vec3 Color;

out vec4 color;

uniform sampler2D textures[32];

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textures[int(TextureIdx)], TexCoords).r);
    color = vec4(Color, 1.0) * sampled;
} 