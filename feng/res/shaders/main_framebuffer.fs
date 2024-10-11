#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D quad_texture;

const float exposure = 1.0;

void main()
{
    vec3 hdrCol = texture(quad_texture, TexCoords).rgb;
    vec3 mappedCol = vec3(1.0) - exp(-hdrCol * exposure);
    mappedCol = pow(mappedCol, vec3(1.0 / 2.2));
    FragColor = vec4(mappedCol, 1.0);
} 