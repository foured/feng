#version 330 core

#include<shadows.glsl>

out float FragColor;

in VS_OUT{
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;
uniform sampler2D inputTexture;

void main() {
    vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float shadow = PCSS(shadowMap, projCoords);
    float value = texture(inputTexture, fs_in.TexCoords).r;

    FragColor = max(shadow, value);
}