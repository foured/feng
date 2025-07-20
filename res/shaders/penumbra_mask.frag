#version 330 core

out float FragColor;

#include <shadows.glsl>

uniform sampler2D shadowMap;

const float lightSize = 1.0 / 500;

in vec4 FragPosLightSpace;

void main(){
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float gradientNoise = InterleavedGradientNoise(projCoords.xy);
    float avgBlockerDepth = CalculateAvgBlockerDepth(shadowMap, gradientNoise, projCoords, lightSize, int(BLOCKER_SEARCH_NUM_SAMPLES / 2));
    if (avgBlockerDepth == 0){
        FragColor = 1;
    }
    else{
        FragColor = CalculatePenumbra(avgBlockerDepth, lightSize, projCoords.z); 
    }
    //float shadow = CHSS(shadowMap, projCoords, lightSize);
    //FragColor = shadow;
}