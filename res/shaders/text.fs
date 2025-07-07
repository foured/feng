#version 330 core
in vec2 TexCoords;
in float TextureIdx;
in vec3 Color;

out vec4 color;

#external define [MAX_NO_TEXTURE_UNITS]

uniform sampler2D textures[MAX_NO_TEXTURE_UNITS];

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textures[int(TextureIdx)], TexCoords).r);
    color = vec4(Color, 1.0) * sampled;
} 