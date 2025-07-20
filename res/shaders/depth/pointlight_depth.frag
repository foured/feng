#version 330 core
in vec4 FragPos;

uniform float far_plane;
uniform vec3 lightPos;

void main()
{
    float lightDistance = length(FragPos.xyz - lightPos);
    lightDistance = lightDistance / far_plane;
    
    gl_FragDepth = lightDistance;
}